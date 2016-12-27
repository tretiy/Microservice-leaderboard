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
	public:

		static const std::string RegQueue;
		static const std::string DealQueue;
		static const std::string ConnQueue;
		static const std::string ExchangePoint;

		RatingMessageSenderAMQP();
		explicit RatingMessageSenderAMQP(const std::string& host, uint16_t port = 5672);
		virtual ~RatingMessageSenderAMQP();

		void sendEnvelopeToChannel(const AMQP::Envelope& env, const std::string& channelName, const std::string& exchangeName = std::string{ "" });

		void sendUserRegistered(int id, const std::string& name) override;
		void sendUserRenamed(int id, const std::string& name) override;

		void sendUserDeal(int id, time_t time, int amount) override;
		void sendUserDealWon(int id, time_t time, int amount) override;

		void sendUserConnected(int id, bool isConnected = true) override;
	};
}

