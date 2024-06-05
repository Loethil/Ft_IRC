#include "bot.hpp"


int main()
{
    int sockfd;
    struct sockaddr_in addr;
    bot Bot;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	{
    	std::cout << "blempro hermano" << std::endl;
	}
	Bot.setSock(sockfd);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(6000);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
    	std::cout << "Problemo couzino" << std::endl;
	}
	Bot.userInfo();
	while(true)
	{
		std::string buffer = Bot.fillTab(Bot.getFunfact()) + "\r\n";
		int bytesSent = send(sockfd, buffer.c_str(), buffer.size(), 0);
		if (bytesSent < 0)
			std::cout << "Galere le frere" << std::endl;
		sleep(5);
	}

    close(sockfd);

    return 0;
}