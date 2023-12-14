#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib> 
#include <fstream>
#include <string>
#include <ctime> 
#include <vector>

class Player {
private:
    int* cards;
    int total;
    float balance;

public:
    Player();
    ~Player();

    float getBalance();
    int generateRandomCard() const;
    int getCardValue(int card, int total, bool aceAsEleven) const;
    int getCardValue(int card, int total) const;
    void saveBalance(const std::string& filename) const;
    void resetBalanceFile(const std::string& filename, float defaultBalance = 100.00);
    void loadBalance(const std::string& filename);
    void displayCards(const int* cardArray, int size, const std::string cards[]) const;
    void initializePlayer(int cardCount);
    void dealInitialCards(int cardCount);
    bool playRound(const std::string* cards, int cardCount);
};