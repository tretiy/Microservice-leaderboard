#include "RatingMessageSenderDB.h"
#include "postgres-exceptions.h"
#include <iostream>
#include <ctime>

namespace RatingMessages
{
	RatingMessageSenderDB::RatingMessageSenderDB()
	{
		try
		{
			dbConnection.connect("postgresql://postgres:tree328@localhost:5433/ratingDB");
		}
		catch(db::postgres::ConnectionException& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	void RatingMessageSenderDB::sendUserRegistered(int id, const std::string& name)
	{
		try
		{
			dbConnection.begin();
			dbConnection.execute(R"(
		INSERT INTO users(userid,username,connected)
		VALUES ($1,$2, 'false')
		)", id, name);
			dbConnection.commit();
		}
		catch (db::postgres::ExecutionException& e)
		{
			dbConnection.rollback();
			std::cerr << e.what() << std::endl;
		}
	}

	void RatingMessageSenderDB::sendUserRenamed(int id, const std::string& name)
	{	
		try
		{
			dbConnection.begin();
			dbConnection.execute(R"(
									UPDATE users
									SET username=$2
									WHERE userid=$1
									)", id, name);
			dbConnection.commit();
		}
		catch(db::postgres::ExecutionException& e)
		{
			dbConnection.rollback();
			std::cerr << e.what() << std::endl;
		}
	}

	void RatingMessageSenderDB::sendUserDeal(int id, time_t time, int amount)
	{
	}

	void RatingMessageSenderDB::sendUserDealWon(int id, time_t time, int amount)
	{
		try
		{
			dbConnection.commit();
			dbConnection.execute(R"(
								INSERT INTO transactions(userid,amount,date)
								VALUES ($1,$2, $3::timestamptz)
								)", id, amount, makeTimeString(time));
			dbConnection.commit();
		}
		catch (db::postgres::ExecutionException& e)
		{
			dbConnection.rollback();
			std::cerr << e.what() << std::endl;
		}
	}

	void RatingMessageSenderDB::sendUserConnected(int id, bool isConnected)
	{
		try
		{
			dbConnection.commit();
			dbConnection.execute(R"(
								UPDATE users
								SET connected=$2
								WHERE userid=$1
								)", id, isConnected);
			dbConnection.commit();
		}
		catch (db::postgres::ExecutionException& e)
		{
			dbConnection.rollback();
			std::cerr << e.what() << std::endl;
		}
	}
	std::string RatingMessageSenderDB::makeTimeString(const time_t& time)
	{
		char timeStr[100];
		tm tm;
		localtime_s(&tm, &time);
		strftime(timeStr, sizeof(timeStr), "%F %T%z", &tm);
		return std::string{ timeStr };
	}
}

