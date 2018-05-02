#include "ServerProcessor.h"

#include <iostream>
#include "DataConverter.h"
#include "StringHelper.h"
#include "AccountManager.h"

using namespace std;

//constructor, starts the server
ServerProcessor::ServerProcessor()
{
	cout << "Binding server to port " << COMMUNICATION_PORT << "...\n";

	//bind the server
	if (listener.listen(COMMUNICATION_PORT) != Socket::Done)
	{
		//failed to bind the server
		throw;
	}

	cout << "Binding succeeded.\n";

	listener.setBlocking(false);
}

//destructor, ends the server
ServerProcessor::~ServerProcessor()
{
	listener.close();
}

//runs the loop that controls the server
void ServerProcessor::run()
{
	while (true)
	{
		TcpSocket* s = new TcpSocket();

		//check if there is a new client joining
		if (listener.accept(*s) == Socket::Done)
		{
			sockets.push_back(s);
			s->setBlocking(false);

			loggedAccounts.push_back(nullptr);

			cout << "Client connected.\n";
			
		}
		else
		{
			delete s;
		}

		size_t size = sockets.size();

		//check each socket for messages
		for (int i = 0; i < size; i++)
		{
			TcpSocket* socket = sockets[i];

			sf::Packet rv;

			Socket::Status s = socket->receive(rv);

			//reception code
			if (s == Socket::Partial || s == Socket::Done)
			{
				size_t rs = rv.getDataSize();
				string packet = (char*)rv.getData();
				packet.resize(rs);

				//attempt to recieve all of the data from the client
				while (s != Socket::Done)
				{
					s = socket->receive(rv);

					rs = rv.getDataSize();
					string append = (char*)rv.getData();

					append.resize(rs);

					packet += append;
				}

				processRequest(packet, i);
			}
			else if (s == Socket::Disconnected)
			{
				//remove the socket as it is disconnected
				sockets.erase(sockets.begin() + i);

				AccountInfo* account = loggedAccounts[i];

				//close the account if it is open
				if (account != nullptr)
				{
					std::cout << "Client: " << loggedAccounts[i]->name << " disconnected.\n";
				}

				loggedAccounts.erase(loggedAccounts.begin() + i);

				i--;
				size = sockets.size();
				
			}
		}
	}
}

//processes the string as a request
void ServerProcessor::processRequest(string data, int i)
{

	//grab the socket and account that sent the request
	TcpSocket* socket = sockets[i];
	AccountInfo* info = loggedAccounts[i];

	vector<string> parts = split(data, ',');

	string command = parts[0];

	if (info == nullptr)
	{
		//sanitise the input
		if (parts.size() == 3)
		{
			//check that the account exists
			AccountInfo* search = AM->searchAccount(parts[1], parts[2]);

			if (search == nullptr)
			{
				if (command == "@login") //login and the account wasn't found = FAILURE
				{
					string packetStr = "@failure";
					sf::Packet packet;
					packet.append(packetStr.c_str(), packetStr.length());

					sf::Socket::Status status = socket->send(packet);

					//keep sending the packet until it is done or stop if the socket disconnects
					while (status != sf::Socket::Done && status != sf::Socket::Disconnected)
					{
						status = socket->send(packet);
					}

					std::cout << "Failed login attempt: " << parts[1] << ", " << parts[2] << ".\n";
				}
				else if (command == "@create") //creation and the account wasn't found = SUCCESS
				{
					string packetStr = "@success";
					sf::Packet packet;
					packet.append(packetStr.c_str(), packetStr.length());

					sf::Socket::Status status = socket->send(packet);

					//keep sending the packet until it is done or stop if the socket disconnects
					while (status != sf::Socket::Done && status != sf::Socket::Disconnected)
					{
						status = socket->send(packet);
					}

					//create the account and retrieve it
					AM->createAccount(parts[1], parts[2]);
					AccountInfo* n = AM->searchAccount(parts[1], parts[2]);

					loggedAccounts[i] = n;

					std::cout << "New account created: " << parts[1] << ", " << parts[2] << ".\n";
				}
			}
			else
			{
				if (command == "@login") //login and the account was found = SUCCESS
				{
					loggedAccounts[i] = search;

					string packetStr = "@success";
					sf::Packet packet;
					packet.append(packetStr.c_str(), packetStr.length());

					sf::Socket::Status status = socket->send(packet);

					//keep sending the packet until it is done or stop if the socket disconnects
					while (status != sf::Socket::Done && status != sf::Socket::Disconnected)
					{
						status = socket->send(packet);
					}

					std::cout << "Successful login attempt: " << parts[1] << ", " << parts[2] << ".\n";
				}
				else if (command == "@create") //creation and the account wasn't found = FAILURE
				{
					string packetStr = "@failure";
					sf::Packet packet;
					packet.append(packetStr.c_str(), packetStr.length());

					sf::Socket::Status status = socket->send(packet);

					//keep sending the packet until it is done or stop if the socket disconnects
					while (status != sf::Socket::Done && status != sf::Socket::Disconnected)
					{
						status = socket->send(packet);
					}

					std::cout << "Failed account creation: " << parts[1] << ", " << parts[2] << ".\n";
				}
			}
		}
	}
	else
	{
		//sanitise the input
		if (parts.size() == 3)
		{
			if (command == "@overwrite")
			{
				string data = parts[1] + "," + parts[2];
				
				BaseData* bd = DC->deserialise(data);

				info->overwriteData(bd);

				AM->save();

				std::cout << "Data overwitten: " << parts[1] << ": " << parts[2] << "\n";
			}
			else if (command == "@offset")
			{
				string data = parts[1] + "," + parts[2];

				BaseData* bd = DC->deserialise(data);

				info->offsetData(bd);

				AM->save();

				std::cout << "Data offset: " << parts[1] << ": " << parts[2] << "\n";
			}
		}
	}
}
