#include "RatingMessageSenderDB.h"
#include "postgres-exceptions.h"
#include <iostream>
#include <ctime>
#include <iomanip>


namespace RatingMessages
{
	RatingMessageSenderDB::RatingMessageSenderDB()
	{
		dbConnection.connect("postgresql://postgres:tree328@localhost:5433/ratingDB");
	}

	void RatingMessageSenderDB::sendUserRegistered(int id, std::string name)
	{
		dbConnection.execute(R"(
		INSERT INTO users(userid,username,connected)
		VALUES ($1,$2, 'false')
		)", id, name);
	}

	void RatingMessageSenderDB::sendUserRenamed(int id, std::string name)
	{
		dbConnection.execute(R"(
		UPDATE users
		SET username=$2
		WHERE userid=$1
		)", id, name);
	}

	void RatingMessageSenderDB::sendUserDeal(int id, time_t time, int amount)
	{
	}

	void RatingMessageSenderDB::sendUserDealWon(int id, time_t time, int amount)
	{
		try
		{
			char timeStr[100];
			tm tm;
			localtime_s(&tm, &time);
			std::strftime(timeStr, sizeof(timeStr), "%F %T%z", &tm);
			dbConnection.execute(R"(
		INSERT INTO transactions(userid,amount,date)
		VALUES ($1,$2, $3::timestamptz)
		)", id, amount, std::string(timeStr));
		}
		catch (db::postgres::ExecutionException& e)
		{
			std::cout << e.what();
		}
	}

	void RatingMessageSenderDB::sendUserConnected(int id, bool isConnected)
	{
		dbConnection.execute(R"(
		UPDATE users
		SET connected=$2
		WHERE userid=$1
		)", id, isConnected);
	}
}

