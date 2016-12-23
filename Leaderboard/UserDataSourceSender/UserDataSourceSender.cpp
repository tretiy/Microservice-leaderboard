// UserDataSourceSender.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RatingMessagesManagment/RatingMessageSender.h"

int main()
{
	RatingMessageSender messageSender;
	std::string command;
	auto times = 0;
	while(command !="exit")
	{
		if (!command.empty())
		{
			std::cout << "entered command:\t" << command << std::endl;
			std::cout << "\nenter how many times:\t";
			std::cin >> times;
		}
		if(command == "reg")
		{
			for (auto i = 0; i < times; ++i)
			{
				messageSender.sendUserRegistered(i, "reg name");
			}
		}
		if (command == "ren")
		{
			for (auto i = 0; i < times; ++i)
			{
				messageSender.sendUserRenamed(i, "new name");
			}
		}
		if (command == "conn")
		{
			for (auto i = 0; i < times; ++i)
			{
				messageSender.sendUserConnected(i);
			}
		}
		if (command == "disc")
		{
			for (auto i = 0; i < times; ++i)
			{
				messageSender.sendUserDisconnected(i);
			}
		}

		if (command == "deal")
		{
			for (auto i = 0; i < times; ++i)
			{
				messageSender.sendUserDeal(i, std::chrono::system_clock::now(), 300);
			}
		}
		if (command == "won")
		{
			for (auto i = 0; i < times; ++i)
			{
				messageSender.sendUserDealWon(i, std::chrono::system_clock::now(), 300);
			}
		}
		std::cout << "\n enter next command \t";
		std::cin >> command;
		times = 1;
	}
	return 0;
}

