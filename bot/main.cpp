#include "bot.hpp"

bool bot::_stop = false;

int main(int ac, char **av)
{
	if (ac == 2)
	{
		int sockfd;
		struct sockaddr_in addr;
		srand(time(NULL));
		bot Bot;

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			std::cerr << "Error: Socket failed\n" << std::endl;
		Bot.setSock(sockfd);

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(atoi(av[1]));

		if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
			std::cerr << "Error: Connect failed" << std::endl;
		Bot.userInfo();
		signal(SIGINT, bot::sigInt_hdl);
		while(Bot.getStop() == false)
		{
			std::string buffer = Bot.fillTab(Bot.getFunfact()) + "\r\n";
			int bytesSent = send(sockfd, buffer.c_str(), buffer.size(), 0);
			if (bytesSent < 0)
				std::cerr << "Error: Send failed" << std::endl;
			sleep(60);
		}
		// close(sockfd);
	}
	else
		std::cerr << "Error: To execute use: [./ircbot <server_port>]";

    return 0;
}