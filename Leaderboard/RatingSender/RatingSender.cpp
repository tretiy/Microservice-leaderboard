#include "UsersRatingCache.h"
#include <sstream>
#include "../PocoHandler/SimplePocoHandler.h"
#include "amqpcpp/login.h"
#include "amqpcpp/connection.h"
#include "../RatingMessagesManagment/RatingMessageSenderAMQP.h"
#include <atomic>

std::atomic<bool> runThread = true;

void ratingSendRoutine(UsersRatingCache& cache)
{
	while (runThread)
	{
		auto ratings = cache.getMessagesToSend();
		std::cout << "\n\ngot a " << ratings.size() << " rating messages\n" << std::endl;

		std::this_thread::sleep_for(std::chrono::minutes(1));
	}
}

int main()
{
	UsersRatingCache ratingCache;
	std::thread ratingSendThread{ &ratingSendRoutine, std::ref(ratingCache)};
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
		ratingCache.updateNamesCache(id, name);
	};

	auto connectionReceivedCallback = [&channel, &ratingCache](const AMQP::Message &message,
		uint64_t deliveryTag,
		bool redelivered)
	{
		long id;
		bool isConnected;
		std::stringstream ss(message.message());
		ss >> id >> isConnected;
		ratingCache.updateConnectedCache(id, isConnected);
		if(isConnected)
			std::cout << ratingCache.getUserRating(id);
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
			ratingCache.updateRatingCache(id, amount);
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
