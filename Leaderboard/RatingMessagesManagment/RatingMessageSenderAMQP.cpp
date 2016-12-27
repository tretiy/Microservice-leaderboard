#include "RatingMessageSenderAMQP.h"
#include <sstream>
#include <ctime>

namespace RatingMessages
{
	const std::string RatingMessageSenderAMQP::RegQueue = "user_registration";
	const std::string RatingMessageSenderAMQP::DealQueue = "user_deal";
	const std::string RatingMessageSenderAMQP::ConnQueue = "user_connection";
	const std::string RatingMessageSenderAMQP::ExchangePoint = "rating_exchange";

	RatingMessageSenderAMQP::RatingMessageSenderAMQP()
		:RatingMessageSenderAMQP{ "localhost", 5672 }
	{
	}
	RatingMessageSenderAMQP::RatingMessageSenderAMQP(const std::string& host, uint16_t port)
		: handler{ host, port },
		connection(&handler, AMQP::Login("guest", "guest"), "/"),
		channel(&connection),
		loopThread{ &SimplePocoHandler::loop, &handler }
	{
		channel.declareExchange(ExchangePoint, AMQP::direct);
	}

	RatingMessageSenderAMQP::~RatingMessageSenderAMQP()
	{
		handler.quit();
		loopThread.join();
	}

	void RatingMessageSenderAMQP::sendEnvelopeToChannel(const AMQP::Envelope& env, const std::string& channelName, const std::string& exchangeName)
	{
		channel.startTransaction();
		channel.publish(exchangeName, channelName, env);
		channel.commitTransaction();
	}

	void RatingMessageSenderAMQP::sendUserRegistered(int id, const std::string& name)
	{
		std::stringstream stream;
		stream << id << ' ' << name << ' ' << true; // id name isNew

		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, RegQueue, ExchangePoint);
	}

	void RatingMessageSenderAMQP::sendUserRenamed(int id, const std::string& name)
	{
		std::stringstream stream;
		stream << id << ' ' << name << ' ' << false; // id name isNew

		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, RegQueue, ExchangePoint);
	}

	void RatingMessageSenderAMQP::sendUserDeal(int id, time_t time, int amount)
	{
		std::stringstream stream;
		stream << id << ' ' << time << ' ' << amount;
		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env,  DealQueue, ExchangePoint);
	}

	void RatingMessageSenderAMQP::sendUserDealWon(int id, time_t time, int amount)
	{
		std::stringstream stream;
		stream << id << ' ' << time << ' ' << amount << ' ' << true;
		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, DealQueue, ExchangePoint);
	}

	void RatingMessageSenderAMQP::sendUserConnected(int id, bool isConnected)
	{
		std::stringstream stream;
		stream << id << ' ' << isConnected;

		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env,  ConnQueue, ExchangePoint);
	}
}