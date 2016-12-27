#include "UsersRatingCache.h"
#include "postgres-exceptions.h"
#include <iostream>
#include <iterator>
#include <algorithm>

UsersRatingCache::UsersRatingCache()
{
	try
	{
		dbConnection.connect("postgresql://postgres:tree328@localhost:5433/ratingDB");
		dbConnection.begin();
		makeRatingCache();
		makeNamesAndConnectedCaches();
		dbConnection.commit();
	}
	catch (db::postgres::ConnectionException& e)
	{
		std::cerr << e.what() << std::endl;
	}
}


UsersRatingCache::~UsersRatingCache()
{
	
}

void UsersRatingCache::invalidateCaches()
{
	// TODO
}

void UsersRatingCache::makeRatingCache()
{
	try
	{
		dbConnection.begin();
		auto&& rows = dbConnection.execute(R"(
			SELECT userid, sum(rating.wonsum) as total
			FROM rating
			WHERE  date > now() - interval '8 days' 
			GROUP BY userid
			ORDER BY total DESC)");
		ratingCache.resize(rows.count());
		ratingPosToUser.reserve(rows.count());
		ratingPosToUser.clear();
		int numberInRating = 1;
		for(auto& row : rows)
		{
			ratingCache.push_back(RatingLine{ numberInRating, row.as<int>(0), row.as<long long>(1) });
			ratingPosToUser[row.as<int>(0)] = numberInRating - 1;
			++numberInRating;
		}
		dbConnection.commit();
	}
	catch (db::postgres::ExecutionException& e)
	{
		dbConnection.rollback();
		std::cerr << e.what() << std::endl;
	}
}

void UsersRatingCache::makeNamesAndConnectedCaches()
{
	try
	{
		dbConnection.begin();
		auto&& rows = dbConnection.execute(R"(
			SELECT userid, username, connected 
			FROM users
			)");
		for(auto& row : rows)
		{
			auto id = row.as<int>(0);
			namesCache[id] = row.as<std::string>(1);
			if (row.as<bool>(2))
				connectedCache.insert(id);
		}
		dbConnection.commit();
	}
	catch (db::postgres::ExecutionException& e)
	{
		dbConnection.rollback();
		std::cerr << e.what() << std::endl;
	}
}

void UsersRatingCache::updateNamesCache(int id, const std::string& newName)
{
	namesCache[id] = newName;
}

void UsersRatingCache::updateConnectedCache(int id, bool isConnected)
{
	if (isConnected)
		connectedCache.insert(id);
	else
		connectedCache.erase(id);
}

void UsersRatingCache::updateRatingCache(int id, int wonAmount)
{
	auto userpoIter = ratingPosToUser.find(id);
	if (userpoIter != ratingPosToUser.end())
	{
		ratingCache[userpoIter->second].rating += wonAmount; // before next getUserRating Cache should be reordered
	}
}

std::vector<std::string> UsersRatingCache::getMessagesToSend()
{
	std::vector<std::string> messagesToSend;
	messagesToSend.reserve(connectedCache.size());
	for(auto& key : connectedCache)
	{
		auto ratingLines = getUserRating(key);
		std::string message{ std::to_string(key) + " " + std::to_string(ratingLines.size()) + " " };
		for (auto& rating : ratingLines)
			message += rating;
		messagesToSend.push_back(message);
	}
	return messagesToSend;
}

std::vector<RatingLine> UsersRatingCache::getUserRating(int id)
{
	std::vector<RatingLine> ratingForUser;
	auto topEndIndex = std::min(topCountToShow, ratingCache.size());
	for (int i = 0; i < topEndIndex; ++i)
		ratingForUser.push_back(ratingCache[i]);
	
	auto userposIter = ratingPosToUser.find(id);
	if(userposIter != ratingPosToUser.end())
	{
		auto userIndex = userposIter->second;
		if (userIndex > topEndIndex) //check if top and neighbors has intersection
			topEndIndex = userIndex;
	}
	for(int i = topEndIndex; i < std::min(topEndIndex + 21, ratingCache.size()); ++i)
		ratingForUser.push_back(ratingCache[i]);

	return ratingForUser;
}

std::string UsersRatingCache::getUserNameString(int id)
{
	try
	{
		//check in cache
		if (namesCache.find(id) != namesCache.end())
			return namesCache[id];
		dbConnection.begin();
		auto&& result = dbConnection.execute(R"(SELECT username FROM users WHERE  userid=$1)", id);
		if(result.count())
		{
			namesCache[id] = result.as<std::string>(0);
			return result.as<std::string>(0);
		}
		dbConnection.commit();
	}
	catch (db::postgres::ExecutionException& e)
	{
		dbConnection.rollback();
		std::cerr << e.what() << std::endl;
	}

	return "";
}
