#include <iostream>
#include <string>

#include <SFML\Network.hpp>

using namespace sf;

//GameAnalytics

int main()
{

	TcpSocket socket;
	Socket::Status status = socket.connect("10.0.0.100", 5000);

	if (status != Socket::Done)
	{
		throw;
		//error
	}

	std::cout << "Success";

	return 0;
}