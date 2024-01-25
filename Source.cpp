#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "boost/asio.hpp"
#include "boost/asio/ts/buffer.hpp"
#include "boost/asio/ts/internet.hpp"

using namespace boost;

std::vector<char> Buffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket);

int main()
{
	system::error_code ec;

	// Create a "context" - essentially the platform specific interface
	asio::io_context context;

	// Give some fake tasks to asio so the context doesn't finish
	asio::io_context::work idleWork(context);

	// Start the context
	std::thread thrContext = std::thread([&]() { context.run(); });

	// Get the address of somewhere we wish to connect to
	std::string addr_1 = "93.184.216.34";
	std::string addr_2 = "51.38.81.49";
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address(addr_1, ec), 80);

	// Create the socket, the context will deliver the implementation
	asio::ip::tcp::socket socket(context);

	// Tell socket to try and connect
	socket.connect(endpoint, ec);

	if (!ec)
	{
		std::cout << "Connected" << std::endl;
	}
	else
	{
		std::cout << "Faild to connect to address: " << ec.message() << std::endl;
	}

	if (socket.is_open())
	{
		GrabSomeData(socket);

		std::string httpRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(httpRequest.data(), httpRequest.size()), ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);

		context.stop();
		if (thrContext.joinable()) thrContext.join();
	}

	std::system("pause");

	return 0;
}

void GrabSomeData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some(asio::buffer(Buffer.data(), Buffer.size()),
		[&](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "\n\nRead " << length << " bytes\n\n";

				for (int i = 0; i < Buffer.size(); i++)
				{
					std::cout << Buffer[i];
				}

				GrabSomeData(socket);
			}
		}
	);
}
