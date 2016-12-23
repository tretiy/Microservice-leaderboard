#pragma once
#include "../PocoHandler/SimplePocoHandler.h"
#include <chrono>
#include <thread>
#include <atomic>

class RatingMessageSender
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
	RatingMessageSender();
	explicit RatingMessageSender(std::string host, uint16_t port = 5672);
	virtual ~RatingMessageSender();

	void sendEnvelopeToChannel(AMQP::Envelope& env, std::string&& channelName);

	void sendUserRegistered(long id, std::string name);
	void sendUserRenamed(long id, std::string name);
	
	void sendUserDeal(long id, std::chrono::system_clock::time_point time, long amount);
	void sendUserDealWon(long id, std::chrono::system_clock::time_point time, long amount);
	
	void sendUserConnected(long id, bool isConnected = true);
	void sendUserDisconnected(long id);
};

