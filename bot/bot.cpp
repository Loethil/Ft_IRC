#include "bot.hpp"

bot::bot() {}

bot::~bot() {}

void    bot::fillTab(std::map<int, std::string> funfact)
{
    funfact[0] = "LE SAVAIS TU ??? JE CHIE DES TRONCS D'ARBRE T'AS JAMAIS VU CA\n";
    funfact[1] = "Tape Jordan9320 sur pornhub pour un compte hot++\n";
    funfact[2] = "Tu savais que shrek avait des problemes d'erection ?\n";
    funfact[3] = "La population de la Terre est d'environ 8 milliards de personnes, mais il y a environ 10 quintillions (10 000 000 000 000 000 000) de fourmis sur la planète\n";
    funfact[4] = "La baleine c'est un super-heros ! Elle peut pas avoir de cancer car elle est big, et plus t'es big plus t'as de cellules\nPlus t'as de cellules et plus tu peux regen vite\n";
    funfact[5] = "Le savais tu ? La terre est plate\n";
    funfact[6] = "Naps prefere son urus a ses gosses (c'est un bon quand meme on l'aime)\n";
    funfact[7] = "Kim kardashian est en fait un agent du mossade depuis 2012\n";
    funfact[8] = "League of legends est la premiere cause de suicide\n";
    funfact[9] = "Connaissiez-vous la plus haute montagne de l'univers exploré? C'est l'Olympe Mons sur Mars qui est 2,5 fois plus élevé que la plus haute montagne de la Terre, c'est-à-dire le mont. EVEREST\n";
    funfact[10] = "Les scientifiques ont trouvé une planète mystérieuse dans l'espace entièrement constituée de diamants. Les scientifiques ont donné à cette étonnante planète le nom de 55 Cancri E\n";
    funfact[11] = "Il y a un homme enterré sur la Lune (en vrai c'est ses cendres mais fun)\n";
    funfact[12] = "Il y a un énorme nuage d'eau flottant dans l'espace (Ca chill pepere autour d'un quasar, un trou noir SUPERMASSIF)\n";
    funfact[13] = "Il y a plus d'arbres sur Terre que d'etoiles dans la Voie Lactee (J'ai dit \"VOIE LACTEE\")\n";
    funfact[14] = "Il y a plus de kangourous en Australie que d'habitants au Chili. Si il y avait un fight entre eux, un chilien devrait fumer environ 3 kangourous\n";
    funfact[15] = "Comment fonctionne la maintenance du cerveau ? il s'auto-mange (La phagocytose)\n";
    funfact[16] = "300 types de fromages différents sont produits en France, ce qui en fait le pays qui en produit le plus au monde\n";
    funfact[17] = "Votre estomac est suffisamment acide pour dissoudre du métal\n";
    funfact[18] = "Toi, tous les arbres, tous les animaux et tous les autres glandus vous descendez tous du meme ancetre organique. La seule forme de vie differente est le VIRUS\n";
    funfact[19] = "Votre corps produit de la chaleur équivalente à celle d'une ampoule électrique de 100 watts chaque jour\n";
    funfact[20] = "Vos yeux peuvent percevoir jusqu'à 10 millions de couleurs différentes.\n";
    funfact[21] = "La surface des alvéoles pulmonaires, les minuscules sacs d'air dans vos poumons, est d'environ la taille d'un terrain de football\n";
    funfact[22] = "Si on prend tous vos vaisseaux et qu'on les met bout a bout pour en faire une corde, celle-ci ferait le tour de la planete Terre\n";
    funfact[23] = "Lovecraft avait peur du noir mais a chaque fois qu'il ecrivait un bouquin, c'etait dans le noir total (Il est zinzin lui)\n";
}

void    printer()
{
    srand(time(NULL));

    //faire le bail pour print toute les 1-2 min un des msg de la map funfact
    //donc rand % 24 de la map
}