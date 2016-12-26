#pragma once
#include "../PocoHandler/SimplePocoHandler.h"
#include <thread>
#include "IMessageSender.h"
namespace RatingMessages
{
	class RatingMessageSenderAMQP : public IMessageSender
	{
		SimplePocoHandler handler;
		AMQP::Connection connection;
		AMQP::Channel channel;
		std::thread loopThread;
		std::thread messageThread;

		static const std::string RegQueue;
		static const std::string DealQueue;
		static const std::string ConnQueue;
	public:
		RatingMessageSenderAMQP();
		explicit RatingMessageSenderAMQP(std::string host, uint16_t port = 5672);
		virtual ~RatingMessageSenderAMQP();

		void sendEnvelopeToChannel(AMQP::Envelope& env, std::string&& channelName);

		void sendUserRegistered(int id, std::string name) override;
		void sendUserRenamed(int id, std::string name) override;

		void sendUserDeal(int id, time_t time, int amount) override;
		void sendUserDealWon(int id, time_t time, int amount) override;

		void sendUserConnected(int id, bool isConnected = true) override;
	};
}

