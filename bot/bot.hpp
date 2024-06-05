
#pragma once

#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>

class bot
{
    public :

        bot();
        ~bot();
        std::string	                fillTab(std::map<int, std::string> funfact);
        std::map<int, std::string>  getFunfact();
        void                        userInfo();
        void                        flush(const std::string &message);
        int                         getSock();
        void                        setSock(int sock);

    private :

        std::map<int, std::string> funfact;
        int _socket;
};