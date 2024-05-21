#pragma once
# include "libs.hpp"
# include "Clients.hpp"

class Channel
{
    private:
        const std::string               _chanName;
        std::map<std::string, Clients*>  _connUsers;
    public:
        Channel(const std::string & chanName);
        ~Channel();

        const std::string getChanName();
};
