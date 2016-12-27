// UserDataSourceSender.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <chrono>
#include "../RatingMessagesManagment/RatingMessageSenderAMQP.h"


using namespace std::chrono;
int main()
{
	RatingMessages::RatingMessageSenderAMQP messageSender;
	std::string command;
	auto startValue = 0;
	auto endValue = 0;
	while(command !="exit")
	{
		if (!command.empty())
		{
			std::cout << "entered command:\t" << command << std::endl;
			std::cout << "\nenter start:\t";
			std::cin >> startValue;

			std::cout << "\nenter end:\t";
			std::cin >> endValue;
		}
		if(command == "reg")
		{
			for (auto i = startValue; i < endValue; ++i)
			{
				messageSender.sendUserRegistered(i, "user#" + std::to_string(i));
			}
		}
		if (command == "ren")
		{
			for (auto i = startValue; i < endValue; ++i)
			{
				messageSender.sendUserRenamed(i, "userRenamed#" + std::to_string(i));
			}
		}
		if (command == "conn")
		{
			for (auto i = startValue; i < endValue; ++i)
			{
				messageSender.sendUserConnected(i);
			}
		}
		if (command == "disc")
		{
			for (auto i = startValue; i < endValue; ++i)
			{
				messageSender.sendUserDisconnected(i);
			}
		}

		if (command == "deal")
		{
			for (auto i = startValue; i < endValue; ++i)
			{
				messageSender.sendUserDeal(i,  system_clock::to_time_t(system_clock::now()) , 300);
			}
		}
		if (command == "won")
		{
			for (int j = 0; j < 6; ++j)
			{
				for (auto i = startValue; i < endValue; ++i)
				{
					messageSender.sendUserDealWon(i, system_clock::to_time_t(system_clock::now() - hours(24 * j)), 300 * j);
				}
			}
		}
		std::cout << "\n enter next command \t";
		std::cin >> command;
		startValue = endValue = 0;
	}
	return 0;
}

