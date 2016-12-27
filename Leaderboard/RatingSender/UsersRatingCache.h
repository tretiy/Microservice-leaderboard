#pragma once
#include "postgres-connection.h"
#include <unordered_map>
#include <set>
#include "Rating.h"

class UsersRatingCache
{
	static const unsigned int topCountToShow = 10;
	static const unsigned int neighborsToShow = 10;
	db::postgres::Connection dbConnection;
	std::unordered_map<int, std::string> namesCache;
	std::set<int> connectedCache;
	std::unordered_map<int, int> ratingPosToUser;
	std::vector<RatingLine> ratingCache;

	void makeRatingCache();
	void makeNamesAndConnectedCaches();
	std::string getUserNameString(int id);
	std::vector<RatingLine> getUserRating(int id);
public:
	UsersRatingCache();
	~UsersRatingCache();

	void invalidateCaches();
	void updateNamesCache(int id, const std::string& newName);
	void updateConnectedCache(int id, bool isConnected);
	void updateRatingCache(int id, int wonAmount);
	std::vector<std::string> getMessagesToSend();
};

