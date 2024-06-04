
#pragma once

# include <fcntl.h>
# include <iostream>
# include <string>
# include <vector>
# include <map>

class bot
{
    public :

        bot();
        ~bot();
        void    fillTab(std::vector<std::string> funfact);

    private :

        std::vector<std::string> funfact;
};