// MessageReceiver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "../PocoHandler/SimplePocoHandler.h"
#include "../../../AMQP-CPP/amqpcpp/login.h"
#include "../../../AMQP-CPP/amqpcpp/connection.h"

int main()
{
	SimplePocoHandler handler("localhost", 5672);

	AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

	AMQP::Channel channel(&connection);
	channel.declareQueue("user_registration");
	channel.consume("user_registration", AMQP::noack).onReceived(
		[&channel](const AMQP::Message &message,
			uint64_t deliveryTag,
			bool redelivered)
	{
		std::cout << " [x] Received registration " << message.message() << std::endl;
	});

	channel.declareQueue("user_connection");
	channel.consume("user_connection", AMQP::noack).onReceived(
		[&channel](const AMQP::Message &message,
			uint64_t deliveryTag,
			bool redelivered)
	{
		std::cout << " [x] Received connection " << message.message() << std::endl;
	});

	channel.declareQueue("user_deal");
	channel.consume("user_deal", AMQP::noack).onReceived(
		[&channel](const AMQP::Message &message,
			uint64_t deliveryTag,
			bool redelivered)
	{
		std::stringstream messageStream(message.message());
		long id, amount;
		time_t time;
		bool isWon = false;

		messageStream >> id >> time >> amount >> isWon;

		tm time_tm;
		localtime_s(&time_tm, &time);

		std::cout << " [x] Received deal " << (isWon ? "won " : "") << message.message() << "\t" << std::put_time(&time_tm, "%c %Z") << std::endl;
	});

	std::cout << " [*] Waiting for messages. To exit press CTRL-C\n";
	handler.loop();
	return 0;
}

