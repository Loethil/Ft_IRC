#pragma once
#include <fcntl.h>
#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

class bot
{
    private :
        std::map<int, std::string>  funfact;
        int                         _socket;
        static bool                 _stop;
    public :
        bot();
        ~bot();
        std::string	                fillTab(std::map<int, std::string> funfact);
        std::map<int, std::string>  getFunfact();
        void                        userInfo();
        void                        flush(const std::string &message);
        int                         getSock();
        void                        setSock(int sock);
        static void                 sigInt_hdl(int signo);
        bool                        getStop();
};