#include "RatingMessageSender.h"
#include <sstream>
#include <ctime>

const std::string RatingMessageSender::RegQueue = "user_registration";
const std::string RatingMessageSender::DealQueue = "user_deal";
const std::string RatingMessageSender::ConnQueue = "user_connection";

RatingMessageSender::RatingMessageSender()
	:RatingMessageSender{"localhost", 5672}
{
}
RatingMessageSender::RatingMessageSender(std::string host, uint16_t port)
	:	handler{ host, port },
		connection(&handler, AMQP::Login("guest", "guest"), "/"),
		channel(&connection), 
		loopThread{ &SimplePocoHandler::loop, &handler }
{
}

RatingMessageSender::~RatingMessageSender()
{
	handler.quit();
	loopThread.join();
}

void RatingMessageSender::sendEnvelopeToChannel(AMQP::Envelope& env, std::string&& channelName)
{
	channel.startTransaction();
	channel.publish("", channelName, env);
	channel.commitTransaction();
}

void RatingMessageSender::sendUserRegistered(long id, std::string name)
{
	std::stringstream stream;
	stream << id << ' ' << name;

	AMQP::Envelope env(stream.str());
	sendEnvelopeToChannel(env, std::string{ RegQueue });
}

void RatingMessageSender::sendUserRenamed(long id, std::string name)
{
	sendUserRegistered(id, name);
}

void RatingMessageSender::sendUserDeal(long id, std::chrono::system_clock::time_point time, long amount)
{
	std::stringstream stream;
	time_t time_c = std::chrono::system_clock::to_time_t(time);
	stream << id << ' ' << time_c << ' ' << amount;
	AMQP::Envelope env(stream.str());
	sendEnvelopeToChannel(env, std::string{ DealQueue });
}

void RatingMessageSender::sendUserDealWon(long id, std::chrono::system_clock::time_point time, long amount)
{
	std::stringstream stream;
	time_t time_c = std::chrono::system_clock::to_time_t(time);
	stream << id << ' ' << time_c << ' ' << amount << ' ' << true;
	AMQP::Envelope env(stream.str());
	sendEnvelopeToChannel(env, std::string{ DealQueue });
}

void RatingMessageSender::sendUserConnected(long id, bool isConnected)
{
	std::stringstream stream;
	stream << id << ' ' << isConnected;

	AMQP::Envelope env(stream.str());
	sendEnvelopeToChannel(env, std::string{ ConnQueue });
}

void RatingMessageSender::sendUserDisconnected(long id)
{
	sendUserConnected(id, false);
}
