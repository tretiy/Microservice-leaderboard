#include "RatingMessageSenderAMQP.h"
#include <sstream>
#include <ctime>

namespace RatingMessages
{
	const std::string RatingMessageSenderAMQP::RegQueue = "user_registration";
	const std::string RatingMessageSenderAMQP::DealQueue = "user_deal";
	const std::string RatingMessageSenderAMQP::ConnQueue = "user_connection";

	RatingMessageSenderAMQP::RatingMessageSenderAMQP()
		:RatingMessageSenderAMQP{ "localhost", 5672 }
	{
	}
	RatingMessageSenderAMQP::RatingMessageSenderAMQP(std::string host, uint16_t port)
		: handler{ host, port },
		connection(&handler, AMQP::Login("guest", "guest"), "/"),
		channel(&connection),
		loopThread{ &SimplePocoHandler::loop, &handler }
	{
	}

	RatingMessageSenderAMQP::~RatingMessageSenderAMQP()
	{
		handler.quit();
		loopThread.join();
	}

	void RatingMessageSenderAMQP::sendEnvelopeToChannel(AMQP::Envelope& env, std::string&& channelName)
	{
		channel.startTransaction();
		channel.publish("", channelName, env);
		channel.commitTransaction();
	}

	void RatingMessageSenderAMQP::sendUserRegistered(int id, std::string name)
	{
		std::stringstream stream;
		stream << id << ' ' << name << ' ' << true; // id name isNew

		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, std::string{ RegQueue });
	}

	void RatingMessageSenderAMQP::sendUserRenamed(int id, std::string name)
	{
		std::stringstream stream;
		stream << id << ' ' << name << ' ' << false; // id name isNew

		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, std::string{ RegQueue });
	}

	void RatingMessageSenderAMQP::sendUserDeal(int id, time_t time, int amount)
	{
		std::stringstream stream;
		stream << id << ' ' << time << ' ' << amount;
		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, std::string{ DealQueue });
	}

	void RatingMessageSenderAMQP::sendUserDealWon(int id, time_t time, int amount)
	{
		std::stringstream stream;
		stream << id << ' ' << time << ' ' << amount << ' ' << true;
		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, std::string{ DealQueue });
	}

	void RatingMessageSenderAMQP::sendUserConnected(int id, bool isConnected)
	{
		std::stringstream stream;
		stream << id << ' ' << isConnected;

		AMQP::Envelope env(stream.str());
		sendEnvelopeToChannel(env, std::string{ ConnQueue });
	}
}