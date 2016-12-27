#pragma once
#include <string>

namespace RatingMessages
{
	struct IMessageSender
	{
		virtual ~IMessageSender() {}
		virtual void sendUserRegistered(int id, const std::string& name) = 0;
		virtual void sendUserRenamed(int id, const std::string& name) = 0;

		virtual void sendUserDeal(int id, time_t time, int amount) = 0;
		virtual void sendUserDealWon(int id, time_t time, int amount) = 0;

		virtual void sendUserConnected(int id, bool isConnected = true) = 0;
		virtual void sendUserDisconnected(int id)
		{
			sendUserConnected(id, false);
		}
	};
}
