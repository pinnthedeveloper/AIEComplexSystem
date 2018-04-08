#include "GameAnalytics.h"

#include <iostream>
#include <SFML/Network.hpp>


//constructor
GameAnalytics::GameAnalytics()
{

}

//destructor
GameAnalytics::~GameAnalytics()
{

}

//singleton instance retrieval
GameAnalytics* GameAnalytics::getInstance()
{
	static GameAnalytics* singleton = nullptr;

	//create the singleton if it doesn't exist already
	if (singleton == nullptr)
	{
		singleton = new GameAnalytics();
	}

	return singleton;
}

//connection attempt
bool GameAnalytics::connect()
{
	//get the local host
	sf::IpAddress local = sf::IpAddress::getLocalAddress();

	client = new sf::TcpSocket();
	client->setBlocking(true);

	//return the result of the connection attempt
	if (client->connect(local, COMMUNICATION_PORT) != sf::Socket::Done)
	{
		return false;
	}

	//infinite loop to login until successful
	while (true)
	{
		string mode, username, password;

		std::cout << "Mode: ";
		std::cin >> mode;

		//sanitise the input
		if (mode != "login" && mode != "create")
		{
			continue;
		}

		std::cout << "Username: ";
		std::cin >> username;

		std::cout << "Password: ";
		std::cin >> password;

		std::cout << "Connecting...";

		string packet = "@" + mode + "," + username + ',' + password;
		client->send(packet.c_str(), packet.size());

		char rev_packet[COMMUNICATION_SIZE];
		size_t rev_s;

		//wait for the response
		if (client->receive(rev_packet, (size_t)COMMUNICATION_SIZE, rev_s) == sf::Socket::Done)
		{
			std::string rev_packet_s = std::string(rev_packet);

			//examine the response
			if (rev_packet_s == "@success")
			{
				break;
			}
			else if (rev_packet_s == "@failure")
			{
				std::cout << "Connection rejected.\n";
				continue;
			}
		}

		std::cout << "Connection failed.\n";
	}

	std::cout << "Connected.\n";

	return true;
}

//attempts to send a data update to the server
void GameAnalytics::updateData(string name, void * data, EVariableType dataType)
{
	string packet = name;

	if (dataType == EVariableType::INT)
	{
		int d = *((int*)data);
		packet += ",";
		packet += d;
	}
	else if (dataType == EVariableType::FLOAT)
	{
		float d = *((float*)data);
		packet += ",";
		packet += d;
	}
	else if (dataType == EVariableType::BOOL)
	{
		bool d = *((bool*)data);;
		packet += ",";
		packet += d;
	}
	else if (dataType == EVariableType::STRING)
	{
		string d = *((string*)data);
		packet += ",";
		packet += d;
	}

	packet += ",";
}