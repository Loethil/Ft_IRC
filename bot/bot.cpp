#include "bot.hpp"

bot::bot() {}

bot::~bot() {}

std::string    bot::fillTab(std::map<int, std::string> funfact)
{
    funfact[0] = "LE SAVAIS TU ??? JE CHIE DES TRONCS D'ARBRE T'AS JAMAIS VU CA";
    funfact[1] = "Tape Jordan9320 sur pornhub pour un compte hot++";
    funfact[2] = "Tu savais que shrek avait des problemes d'erection ?";
    funfact[3] = "La population de la Terre est d'environ 8 milliards de personnes, mais il y a environ 10 quintillions (10 000 000 000 000 000 000) de fourmis sur la planète";
    funfact[4] = "La baleine c'est un super-heros ! Elle peut pas avoir de cancer car elle est big, et plus t'es big plus t'as de cellules, plus t'as de cellules et plus tu peux regen vite";
    funfact[5] = "Le savais tu ? La terre est plate";
    funfact[6] = "Naps prefere son urus a ses gosses (c'est un bon quand meme on l'aime)";
    funfact[7] = "Kim kardashian est en fait un agent du mossad depuis 2012";
    funfact[8] = "League of legends est la premiere cause de suicide";
    funfact[9] = "Connaissiez-vous la plus haute montagne de l'univers exploré? C'est l'Olympe Mons sur Mars qui est 2,5 fois plus élevé que la plus haute montagne de la Terre, c'est-à-dire le mont. EVEREST";
    funfact[10] = "Les scientifiques ont trouvé une planète mystérieuse dans l'espace entièrement constituée de diamants. Les scientifiques ont donné à cette étonnante planète le nom de 55 Cancri E";
    funfact[11] = "Il y a un homme enterré sur la Lune (en vrai c'est ses cendres mais fun)";
    funfact[12] = "Il y a un énorme nuage d'eau flottant dans l'espace (Ca chill pepere autour d'un quasar, un trou noir SUPERMASSIF)";
    funfact[13] = "Il y a plus d'arbres sur Terre que d'etoiles dans la Voie Lactee (J'ai dit \"VOIE LACTEE\")";
    funfact[14] = "Il y a plus de kangourous en Australie que d'habitants au Chili. Si il y avait un fight entre eux, un chilien devrait fumer environ 3 kangourous";
    funfact[15] = "Comment fonctionne la maintenance du cerveau ? il s'auto-mange (La phagocytose)";
    funfact[16] = "Trois-cents types de fromages différents sont produits en France, ce qui en fait le pays qui en produit le plus au monde";
    funfact[17] = "Votre estomac est suffisamment acide pour dissoudre du métal";
    funfact[18] = "Toi, tous les arbres, tous les animaux et tous les autres glandus vous descendez tous du meme ancetre organique. La seule forme de vie differente est le VIRUS";
    funfact[19] = "Votre corps produit de la chaleur équivalente à celle d'une ampoule électrique de 100 watts chaque jour";
    funfact[20] = "Vos yeux peuvent percevoir jusqu'à 10 millions de couleurs différentes.";
    funfact[21] = "La surface des alvéoles pulmonaires, les minuscules sacs d'air dans vos poumons, est d'environ la taille d'un terrain de football";
    funfact[22] = "Si on prend tous vos vaisseaux et qu'on les met bout a bout pour en faire une corde, celle-ci ferait le tour de la planete Terre";
    funfact[23] = "Lovecraft avait peur du noir mais a chaque fois qu'il ecrivait un bouquin, c'etait dans le noir total (Il est zinzin lui)";
    return (funfact[rand() % 24]);
}

std::map<int, std::string> bot::getFunfact()
{
    return funfact;
}

void bot::flush(const std::string &message)
{
	std::string normalized = message + "\n";
	send(getSock(), normalized.c_str(), normalized.size(), 0);
}

void bot::userInfo()
{
	flush("PASS oui");
	flush("NICK Babidi");
	flush("USER BobidiBot");
}

int bot::getSock()
{
    return _socket;
}

void    bot::setSock(int sock)
{
    _socket = sock;
}

bool    bot::getStop()
{
    return _stop;
}

void    bot::sigInt_hdl(int signo)
{
    if (signo == SIGINT)
    {
        _stop = true;
        std::cout << "\b\b  \b\b" << std::endl;
    }
}
