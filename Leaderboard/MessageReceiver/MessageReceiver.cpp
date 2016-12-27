// MessageReceiver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ostream>
#include <sstream>
#include "../PocoHandler/SimplePocoHandler.h"
#include "../../../AMQP-CPP/amqpcpp/login.h"
#include "../../../AMQP-CPP/amqpcpp/connection.h"
#include "../RatingMessagesManagment/RatingMessageSenderDB.h"
#include "../RatingMessagesManagment/RatingMessageSenderAMQP.h"

int main()
{
	SimplePocoHandler handler("localhost", 5672);
	RatingMessages::RatingMessageSenderDB dbSender;

	AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

	AMQP::Channel channel(&connection);


	auto registrationReceivedCallback = [&channel, &dbSender](const AMQP::Message &message,
		uint64_t deliveryTag,
		bool redelivered)
	{
		std::cout << " [x] Received registration " << message.message() << std::endl;
		//parse message
		long id;
		std::string name;
		bool isNewUser;
		std::stringstream ss(message.message());
		ss >> id >> name >> isNewUser;
		if (isNewUser)
			dbSender.sendUserRegistered(id, name);
		else
			dbSender.sendUserRenamed(id, name);

	};

	auto connectionReceivedCallback = [&channel, &dbSender](const AMQP::Message &message,
		uint64_t deliveryTag,
		bool redelivered)
	{
		std::cout << " [x] Received connection " << message.message() << std::endl;
		//parse message
		long id;
		bool isConnected;
		std::stringstream ss(message.message());
		ss >> id >> isConnected;
		if (isConnected)
			dbSender.sendUserConnected(id);
		else
			dbSender.sendUserDisconnected(id);
	};

	auto dealReceivedCallback = [&channel, &dbSender](const AMQP::Message &message,
		uint64_t deliveryTag,
		bool redelivered)
	{
		std::stringstream messageStream(message.message());
		long id, amount;
		time_t time;
		bool isWon = false;

		messageStream >> id >> time >> amount >> isWon;

		if (isWon)
		{
			dbSender.sendUserDealWon(id, time, amount);
		}

		std::cout << " [x] Received deal " << (isWon ? "won " : "") << message.message() << std::endl;
	};

	AMQP::QueueCallback callbackRegistration =
		[&](const std::string &name, int msgcount, int consumercount)
	{
		channel.bindQueue(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, name, name);
		channel.consume(name, AMQP::noack).onReceived(registrationReceivedCallback);
	};

	AMQP::QueueCallback callbackConnection =
		[&](const std::string &name, int msgcount, int consumercount)
	{
		channel.bindQueue(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, name, name );
		channel.consume(name, AMQP::noack).onReceived(connectionReceivedCallback);
	};

	AMQP::QueueCallback callbackDeal =
		[&](const std::string &name, int msgcount, int consumercount)
	{
		channel.bindQueue(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, name, name);
		channel.consume(name, AMQP::noack).onReceived(dealReceivedCallback);
	};

	AMQP::SuccessCallback successDeclareExchange = [&]()
	{
		channel.declareQueue(RatingMessages::RatingMessageSenderAMQP::RegQueue, AMQP::noack).onSuccess(callbackRegistration);
		channel.declareQueue(RatingMessages::RatingMessageSenderAMQP::ConnQueue, AMQP::noack).onSuccess(callbackConnection);
		channel.declareQueue(RatingMessages::RatingMessageSenderAMQP::DealQueue, AMQP::noack).onSuccess(callbackDeal);
	};

	channel.declareExchange(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, AMQP::direct).onSuccess(successDeclareExchange);

	std::cout << " [*] Waiting for messages. To exit press CTRL-C\n";
	handler.loop();
	return 0;
}

