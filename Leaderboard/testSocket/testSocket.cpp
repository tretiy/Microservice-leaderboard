// testSocket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ostream>
#include "../PocoHandler/SimplePocoHandler.h"
#include "../../../AMQP-CPP/amqpcpp/connection.h"
#include "../../../AMQP-CPP/amqpcpp/channel.h"
#include "../../../AMQP-CPP/amqpcpp/login.h"

int main()
{
	SimplePocoHandler handler("localhost", 5672);

	AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

	AMQP::Channel channel(&connection);

	channel.onReady([&] 
	{
		int i = 100000;
		int ii = 0;
		while (i-- > 0)
		{
			if (channel.publish("", "test_queue", "message #" + std::to_string(i)))
				++ii;
		}
		std::cout << "Total messages: " << ii;
	});


	handler.loop();
    return 0;
}

