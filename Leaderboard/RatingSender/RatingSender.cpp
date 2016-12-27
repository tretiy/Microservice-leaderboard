


#include "UsersRatingCache.h"
#include <sstream>
#include "../PocoHandler/SimplePocoHandler.h"
#include "amqpcpp/login.h"
#include "amqpcpp/connection.h"
#include "../RatingMessagesManagment/RatingMessageSenderAMQP.h"
#include <atomic>

std::atomic<bool> runThread = true;

void ratingSendRoutine(UsersRatingCache* cache)
{
	while (runThread)
	{
		if (cache != nullptr)
		{
			cache->invalidateCaches();
			auto ratings = cache->getMessagesToSend();
			std::cout << "got a " << ratings.size() << " rating messages" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::minutes(1));
	}
}

int main()
{
	UsersRatingCache* ratingCache = new UsersRatingCache();
	std::thread ratingSendThread{ ratingSendRoutine, ratingCache};
	SimplePocoHandler handler("localhost", 5672);

	AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

	AMQP::Channel channel(&connection);


	auto registrationReceivedCallback = [&channel, &ratingCache](const AMQP::Message &message,
		uint64_t deliveryTag,
		bool redelivered)
	{
		long id;
		std::string name;
		std::stringstream ss(message.message());
		ss >> id >> name;
		ratingCache->updateNamesCache(id, name);
	};

	auto connectionReceivedCallback = [&channel, &ratingCache](const AMQP::Message &message,
		uint64_t deliveryTag,
		bool redelivered)
	{
		long id;
		bool isConnected;
		std::stringstream ss(message.message());
		ss >> id >> isConnected;
		ratingCache->updateConnectedCache(id, isConnected);
	};

	auto dealReceivedCallback = [&channel, &ratingCache](const AMQP::Message &message,
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
			ratingCache->updateRatingCache(id, amount);
		}
	};

	AMQP::QueueCallback callbackRegistration =
		[&](const std::string &name, int msgcount, int consumercount)
	{
		channel.bindQueue(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, name, RatingMessages::RatingMessageSenderAMQP::RegQueue);
		channel.consume(name, AMQP::noack).onReceived(registrationReceivedCallback);
	};

	AMQP::QueueCallback callbackConnection =
		[&](const std::string &name, int msgcount, int consumercount)
	{
		channel.bindQueue(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, name, RatingMessages::RatingMessageSenderAMQP::ConnQueue);
		channel.consume(name, AMQP::noack).onReceived(connectionReceivedCallback);
	};

	AMQP::QueueCallback callbackDeal =
		[&](const std::string &name, int msgcount, int consumercount)
	{
		channel.bindQueue(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, name, RatingMessages::RatingMessageSenderAMQP::DealQueue);
		channel.consume(name, AMQP::noack).onReceived(dealReceivedCallback);
	};

	AMQP::SuccessCallback successDeclareExchange = [&]()
	{
		channel.declareQueue(AMQP::exclusive).onSuccess(callbackRegistration);
		channel.declareQueue(AMQP::exclusive).onSuccess(callbackConnection);
		channel.declareQueue(AMQP::exclusive).onSuccess(callbackDeal);
	};

	channel.declareExchange(RatingMessages::RatingMessageSenderAMQP::ExchangePoint, AMQP::direct).onSuccess(successDeclareExchange);

	handler.loop();
	runThread = false;
	ratingSendThread.join();
	return 0;
}
