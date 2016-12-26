#pragma once
#include "postgres-connection.h"
#include "IMessageSender.h"
namespace RatingMessages
{
	class RatingMessageSenderDB : public IMessageSender
	{
		db::postgres::Connection dbConnection;
	public:
		RatingMessageSenderDB();
		virtual ~RatingMessageSenderDB() {}

		void sendUserRegistered(int id, std::string name) override;
		void sendUserRenamed(int id, std::string name) override;

		void sendUserDeal(int id, time_t time, int amount) override;
		void sendUserDealWon(int id, time_t time, int amount) override;

		void sendUserConnected(int id, bool isConnected = true) override;
	};
}

