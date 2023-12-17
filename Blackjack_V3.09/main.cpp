/*
 * File:   main.cpp
 * Author: Liam Shaw
 *
 * Created on November 11
 * Purpose: The classic card game, Blackjack.
 */

//System Libraries
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm> 
#include <set> 

using namespace std;

class Card {
private:
    int rank;
    string suit;

public:
    Card(int cardRank) : rank(cardRank) {
        if (rank >= 0 && rank <= 3) {
            suit = " ♥";
        } else if (rank >= 4 && rank <= 7) {
            suit = " ♦";
        } else if (rank >= 8 && rank <= 11) {
            suit = " ♣";
        } else {
            suit = " ♠";
        }
    }

    string getSuit() const {
    return suit;
    }
    
inline int getCardValue(int total, bool aceAsEleven) const {
    if (rank == 0) {
        return (total + 11 <= 21 && aceAsEleven) ? 11 : 1;
    } else if (rank >= 10) {
        return (rank == 10) ? 10 : 10;
    } else {
        return rank + 1;
    }
}

inline int getCardValue(int total) const {
    return getCardValue(total, true);
}

    inline void display() const {
    string rankString;
    if (rank == 0) {
        rankString = "A";
    } else if (rank >= 10) {
        rankString = (rank == 10) ? "J" : (rank == 11) ? "Q" : "K";
    } else {
        rankString = to_string(rank + 1);
    }

    cout << " _________  \n";
    cout << "|         |  \n";
    cout << "|" << setw(2) << rankString << "       |  \n";
    cout << "|   " << setw(2) << suit << "   |  \n";
    cout << "|      " << setw(2) << rankString << " |  \n";
    cout << "|_________|  \n";
}


    bool operator==(const Card& other) const {
        return rank == other.rank && suit == other.suit;
    }
};

class Player {
private:
    vector<Card> cards;
    float balance;
    bool doubleDown;

public:
    Player() : balance(0.0), doubleDown(false) {}

    ~Player() {}

    void initialize(int cardCount) {
        cards.clear();
        cards.reserve(cardCount);
    }

    const string suitNames[5] = {" ♥", " ♦", " ♣", " ♠"};

    void dealInitialCards(int cardCount) {
        set<pair<int, string>> usedCards;

        for (int i = 0; i < cardCount; i++) {
            int newRank;
            string newSuit;

            do {
                newRank = rand() % 13;
                newSuit = (newRank / 4 == 0) ? " ♥" : (newRank / 4 == 1) ? " ♦" : (newRank / 4 == 2) ? " ♣" : " ♠";
            } while (usedCards.count({newRank, newSuit}) > 0);

            cards.push_back(Card(newRank));
            usedCards.insert({newRank, newSuit});
        }
    }

    const vector<Card>& getCards() const {
        return cards;
    }
    
    int getTotal() const {
    int total = 0;
    int aceCount = 0;

    for (const auto &card : cards) {
        int cardValue = card.getCardValue(total, true);
        total += cardValue;

        if (cardValue == 11) {
            aceCount++;
        }

        while (total > 21 && aceCount > 0) {
            total -= 10;
            aceCount--;
        }
    }

    return total;
}

    float getBalance() const {
        return balance;
    }

    void setBalance(float newBalance) {
        balance = newBalance;
    }

    bool getDoubleDown() const {
        return doubleDown;
    }

    void setDoubleDown(bool value) {
        doubleDown = value;
    }
    
    int addCard() {
    int newRank;
    string newSuit;

    do {
        newRank = rand() % 13;
        newSuit = (newRank / 4 == 0) ? " ♥" : (newRank / 4 == 1) ? " ♦" : (newRank / 4 == 2) ? " ♣" : " ♠";
    } while (find(cards.begin(), cards.end(), Card(newRank)) != cards.end());

    cards.push_back(Card(newRank));

    int total = getTotal();
    return total;
    }
};

class ExperienceLevel {
private:
    int level;
    int experiencePoints;

public:
    ExperienceLevel() : level(1), experiencePoints(0) {}

    int getLevel() const {
        return level;
    }

    int getExperiencePoints() const {
        return experiencePoints;
    }

    void gainExperience(int points) {
        experiencePoints += points;
        
        if (experiencePoints >= level * 100) {
            experiencePoints -= level * 100;
            level++;
            cout << "   - Level up! New Level: " << level << "! -\n";
        }
    }

float getBettingLimit() const {
    const float levelBettingLimits[] = {50.0, 100.0, 250.0, 500.0, 1000.0, 5000.0};
    int adjustedLevel = min(max(level, 1), static_cast<int>(sizeof(levelBettingLimits) / sizeof(levelBettingLimits[0])));
    return levelBettingLimits[adjustedLevel - 1];
}
    
    void resetXPFile(ExperienceLevel& experienceLevel, const string& filename, int defaultLevel = 1, int defaultXP = 0) {
        ofstream file(filename, ios::binary | ios::out);
        if (file.is_open()) {
            experienceLevel = ExperienceLevel();
            experienceLevel.gainExperience(defaultXP); 
            int level = experienceLevel.getLevel();
            int xp = experienceLevel.getExperiencePoints();
            file.write(reinterpret_cast<const char*>(&level), sizeof(level));
            file.write(reinterpret_cast<const char*>(&xp), sizeof(xp));
            file.close();
            cout << " XP file reset to default values: Level " << defaultLevel << ", XP " << defaultXP << endl;
        } else {
        cout << " Unable to open file for resetting XP." << endl;
    }
}

    
    void saveExperience(const string& filename) const {
        ofstream file(filename, ios::binary | ios::out);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&level), sizeof(level));
            file.write(reinterpret_cast<const char*>(&experiencePoints), sizeof(experiencePoints));
            file.close();
            cout << "XP level saved to " << filename << endl;
        } else {
            cout << "Unable to open file for saving experience level." << endl;
        }
    }

    void loadExperience(const string& filename) {
        ifstream file(filename, ios::binary | ios::in);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(&level), sizeof(level));
            file.read(reinterpret_cast<char*>(&experiencePoints), sizeof(experiencePoints));
            file.close();
            cout << " XP level loaded from " << filename << endl;
        } else {
            cout << "Unable to open file for loading experience level. Using default level." << endl;
        }
    }
};

int addCardToDealer(Player& player, vector<Card>& dealerCards, int& dealerTotal) {
    int newRank;
    string newSuit;

    do {
        newRank = rand() % 13;
        newSuit = (newRank / 4 == 0) ? " ♥" : (newRank / 4 == 1) ? " ♦" : (newRank / 4 == 2) ? " ♣" : " ♠";
    } while (find(dealerCards.begin(), dealerCards.end(), Card(newRank)) != dealerCards.end());

    dealerCards.push_back(Card(newRank));

    int total = dealerTotal + dealerCards.back().getCardValue(dealerTotal);
    if (total > 21) {
        for (const auto& card : dealerCards) {
            if (card.getCardValue(dealerTotal) == 11) {
                total -= 10;
            }
        }
    }

    dealerTotal = total;
    return dealerCards.back().getCardValue(dealerTotal);
    }

void displayCards(const vector<Card>& cards) {
    for (const auto& card : cards) {
        string rankString;
        if (card.getCardValue(0) == 1) {
            rankString = "A";
        } else if (card.getCardValue(0) >= 10) {
            rankString = (card.getCardValue(0) == 10) ? "10" : (card.getCardValue(0) == 11) ? "J" : (card.getCardValue(0) == 12) ? "Q" : "K";
        } else {
            rankString = to_string(card.getCardValue(0));
        }

        cout << " _________  ";
    }
    cout << endl;

    for (const auto& card : cards) {
        string rankString;
        if (card.getCardValue(0) == 1) {
            rankString = "A";
        } else if (card.getCardValue(0) >= 10) {
            rankString = (card.getCardValue(0) == 10) ? "10" : (card.getCardValue(0) == 11) ? "J" : (card.getCardValue(0) == 12) ? "Q" : "K";
        } else {
            rankString = to_string(card.getCardValue(0));
        }

        cout << "|         | ";
    }
    cout << endl;

    for (const auto& card : cards) {
        string rankString;
        if (card.getCardValue(0) == 1) {
            rankString = "A";
        } else if (card.getCardValue(0) >= 10) {
            rankString = (card.getCardValue(0) == 10) ? "10" : (card.getCardValue(0) == 11) ? "J" : (card.getCardValue(0) == 12) ? "Q" : "K";
        } else {
            rankString = to_string(card.getCardValue(0));
        }

        cout << "|" << setw(2) << rankString << "       | ";
    }
    cout << endl;

    for (const auto& card : cards) {
        string rankString;
        if (card.getCardValue(0) == 1) {
            rankString = "A";
        } else if (card.getCardValue(0) >= 10) {
            rankString = (card.getCardValue(0) == 10) ? "10" : (card.getCardValue(0) == 11) ? "J" : (card.getCardValue(0) == 12) ? "Q" : "K";
        } else {
            rankString = to_string(card.getCardValue(0));
        }

        cout << "|   " << setw(2) << card.getSuit() << "   | ";
    }
    cout << endl;

    for (const auto& card : cards) {
        string rankString;
        if (card.getCardValue(0) == 1) {
            rankString = "A";
        } else if (card.getCardValue(0) >= 10) {
            rankString = (card.getCardValue(0) == 10) ? "10" : (card.getCardValue(0) == 11) ? "J" : (card.getCardValue(0) == 12) ? "Q" : "K";
        } else {
            rankString = to_string(card.getCardValue(0));
        }

        cout << "|      " << setw(2) << rankString << " | ";
    }
    cout << endl;

    for (const auto& card : cards) {
        string rankString;
        if (card.getCardValue(0) == 1) {
            rankString = "A";
        } else if (card.getCardValue(0) >= 10) {
            rankString = (card.getCardValue(0) == 10) ? "10" : (card.getCardValue(0) == 11) ? "J" : (card.getCardValue(0) == 12) ? "Q" : "K";
        } else {
            rankString = to_string(card.getCardValue(0));
        }

        cout << "|_________| ";
    }
    cout << endl;
}

int addCardToDealer(Player& player, vector<Card>& dealerCards, int& dealerTotal);

bool playRound(Player& player, ExperienceLevel& experienceLevel) {
    bool bj = false;
    int pcards = 2;

    cout << "---------------------------------\n";
    cout << "   - Your balance is: $" << player.getBalance() << " -\n\n";
    cout << "        Place your bets!\n        Min Bet is $5.00  \n";
    cout << "        Max Bet is $" << fixed << setprecision(2) << experienceLevel.getBettingLimit() << endl;
    cout << "---------------------------------\n";

    float bet = 0;
    cout << "Your bet: $";
    cin >> bet;
    cout << "---------------------------------\n";

    while (bet < 5 || bet > experienceLevel.getBettingLimit()) {
        cout << "Please choose an amount between $5.00 and $" << fixed << setprecision(2) << experienceLevel.getBettingLimit() << "\n";
        cout << "Your bet: $";
        cin >> bet;
        cout << "---------------------------------\n";
        cin.clear();
    }

    cout << "Your cards:\n";
    displayCards(player.getCards());
    cout << "\nTotal: " << player.getTotal() << endl;

    char doubleDownChoice;
    cout << "---------------------------------\n";
    cout << "Do you want to double down? (Y/N)\n";
    cin >> doubleDownChoice;

    bool canHitOrStand = true;
    
    if (doubleDownChoice == 'Y' || doubleDownChoice == 'y') {
        cout << "---------------------------------\n";
        float originalBet = bet;
        bet *= 2;
        player.setBalance(player.getBalance() - originalBet);
        player.setDoubleDown(true);
        player.addCard();
        cout << "Your cards after doubling down:\n";
        displayCards(player.getCards());
        int doubledDownTotal = player.getTotal();
        cout << "\nTotal: " << player.getTotal() << endl;
        
        if (doubledDownTotal == 21) {
        float betadd = (bet * 3);
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "       Blackjack! You win!\n\n";
        cout << "        You made $" << betadd << "!\n";
        player.setBalance((bet * 3) + player.getBalance());
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(20);
        cout << "       - XP +20" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        bj = true;
    } else if (doubledDownTotal > 21) {
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "        Bust! You lose.\n\n";
        cout << "        You lost $" << bet << "\n";
        player.setBalance(player.getBalance() - bet);
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        cout << "        - XP: (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        bj = true;
    } else {
        canHitOrStand = false;
    }
}
    
    if (player.getTotal() == 21) {
        float betadd = (bet * 3);
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << " Lucky you, first try Blackjack!\n";
        cout << "           You win!\n\n";
        cout << "        You made $" << betadd << "!\n";
        player.setBalance((bet * 3) + player.getBalance());
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(20);
        cout << "       - XP +20" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        bj = true;
    }

    char choice;
    bool turn = true;

    while (player.getTotal() < 21 && turn && canHitOrStand == true) {
        cout << "---------------------------------\n";
        cout << "Enter 'H' to hit or 'S' to stay.\n";
        cin >> choice;
        cin.ignore();
        cout << "---------------------------------\n";

        if (choice == 'H' or choice == 'h') {
            pcards++;
            player.addCard();
            cout << "Your cards:\n";
            displayCards(player.getCards());
            cout << "\nTotal: " << player.getTotal() << endl;

            if (player.getTotal() == 21) {
                float betadd = (bet * 3);
                cout << fixed << setprecision(2);
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                cout << "       Blackjack! You win!\n\n";
                cout << fixed << setprecision(2);
                cout << "        You made $" << betadd << "!\n";
                player.setBalance((bet * 3) + player.getBalance());
                cout << "  - Your balance is: $" << player.getBalance() << " -\n";
                experienceLevel.gainExperience(20);
                cout << "       - XP +20" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                bj = true;
                turn = false;
            } else if (player.getTotal() > 21) {
                cout << fixed << setprecision(2);
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                cout << "        Bust! You lose.\n\n";
                cout << "        You lost $" << bet << "\n";
                player.setBalance(player.getBalance() - bet);
                cout << "  - Your balance is: $" << player.getBalance() << " -\n";
                cout << "        - XP: (" << experienceLevel.getExperiencePoints() << "/100) -\n";
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                bj = true;
                turn = false;
            }
        } else if (choice == 'S' or choice == 's') {
            cout << "       You chose to stay.\n";
            turn = false;
        } else {
            cout << "Invalid choice. Please enter 'H' to hit or 'S' to stay.\n";
        }
    }

    int dealerTotal = 0;
    vector<Card> dealerCards;

   while (dealerTotal < 17 && !bj) {
    addCardToDealer(player, dealerCards, dealerTotal);
}

if (!bj) {
    cout << "---------------------------------\n";
    cout << "Dealer's cards:\n";
    displayCards(dealerCards);
    cout << "\nDealer Total: " << dealerTotal << "\n";
    cout << "Player Total: " << player.getTotal() << endl;
    cout << "---------------------------------\n";

    if (dealerTotal > 21) {
        float betadd = (bet * 2);
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "       The dealer busts!\n";
        cout << "   Congratulations, you win!\n\n";
        cout << "        You made $" << betadd << "!\n";
        player.setBalance((bet * 2) + player.getBalance());
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(10);
        cout << "       - XP +10" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    } else if (dealerTotal > player.getTotal()) {
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "        The dealer wins.\n\n";
        cout << "        You lost $" << bet << "\n";
        player.setBalance(player.getBalance() - bet);
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        cout << "        - XP: (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    } else if (dealerTotal == player.getTotal()) {
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "          It's a tie!\n";
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(5);
        cout << "       - XP +5" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    } else {
        float betadd = (bet * 2);
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "   Congratulations, you win!\n\n";
        cout << "        You made $" << betadd << "!\n";
        player.setBalance((bet * 2) + player.getBalance());
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(10);
        cout << "       - XP +10" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    }
}

    cout << "           Play again?\nEnter 'Y' to continue or 'N' to exit.\n";
    cin >> choice;
    cin.ignore();
    cout << "---------------------------------\n";

    while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n') {
        cout << "Invalid choice, Enter 'Y' to continue or 'N' to exit.\n";
        cin >> choice;
        cin.ignore();
    }

    return (choice == 'y' || choice == 'Y');
}

void saveBalance(const Player& player, const string& filename) {
    ofstream file(filename, ios::binary | ios::out);
    if (file.is_open()) {
        float balance = player.getBalance();
        file.write(reinterpret_cast<const char*>(&balance), sizeof(balance));
        file.close();
        cout << "Balance saved to " << filename << endl;
    } else {
        cout << "Unable to open file for saving balance." << endl;
    }
}

void resetBalanceFile(Player& player, const string& filename, float defaultBalance = 100.00) {
    ofstream file(filename, ios::binary | ios::out);
    if (file.is_open()) {
        player.setBalance(defaultBalance);
        float balance = player.getBalance();
        file.write(reinterpret_cast<const char*>(&balance), sizeof(balance));
        file.close();
        cout << " Balance file reset to default value: $" << defaultBalance << endl;
    } else {
        cout << " Unable to open file for resetting balance." << endl;
    }
}

void loadBalance(Player& player, const string& filename) {
    ifstream file(filename, ios::binary | ios::in);
    if (file.is_open()) {
        float balance;
        file.read(reinterpret_cast<char*>(&balance), sizeof(balance));
        player.setBalance(balance);
        file.close();
        cout << " Balance loaded from " << filename << endl;
    } else {
        cout << " Unable to open file for loading balance. Using default balance." << endl;
    }
}

int main() {
    srand(time(0));

    Player player;
    ExperienceLevel experienceLevel;
    player.setBalance(100.00);
    bool again = true;
    
    loadBalance(player, "balance.bin");
    experienceLevel.loadExperience("experience.bin");
    
    cout << "<><><><><><><><><><><><><><><><><>\n";
    cout << "     Welcome to Liam's Casino!\n";
    cout << "\n         - Blackjack -\n             $ $ $\n\n";
    cout << "     Enter any key to begin\n";
    cout << "   OR type 'HELP' for details:\n";
    cout << "<><><><><><><><><><><><><><><><><>\n";
    
    string userInput;
    cin >> userInput;

    if (userInput == "HELP" || userInput == "help") {
        cout << "---------------------------------\n";
        cout << "          - HELP MENU -          \n\n";
        cout << "RULES:\n"
             << "* Dealing Cards:\n"
             << "  - Each player and the dealer are initially dealt two cards.\n"
             << "  - Each card has a numerical value, and face cards count as 10,\n" 
             << "    while an Ace can be valued as 1 or 11, depending on the \n"
             << "    player's choice.\n"
             << "* Hit, Stay, or Double Down:\n"
             << "  - Players then decide whether to 'hit' to receive an\n"
             << "    additional card or 'stand' to keep their current hand.\n"
             << "  - Players also have the option to 'double down,' which\n"
             << "    involves doubling the initial bet, drawing one more card, and\n"
             << "    and skipping the ability to draw further cards, providing\n"
             << "    players an opportunity to capitalize on a strong initial hand\n"
             << "    and potentially increase their winnings.\n"
             << "* Winning/Losing:\n"
             << "  - Winning occurs if a player's hand surpasses the dealer's\n"
             << "    without exceeding 21 or if the dealer busts by going over 21.\n"
             << "  - Achieving a 'BlackJack,' a hand totaling 21, results in\n"
             << "    a 3x payout!\n"
             << "  - Conversely, players lose if their hand exceeds 21, referred\n"
             << "    to as 'busting,' or if the dealer's hand is closer to 21.\n";
        cout << "\nRATES:\n"
             << " $ Standard Win $\n - Payout: 2x Bet\n\n"
             << " $$ Blackjack $$\n - Payout: 3x Bet\n\n"
             << " $$$ Double Down $$$\n - Payout: 4x Bet\n\n"
             << " $$$$$ Double Down + Blackjack $$$$$\n - Payout: 6x Bet\n";
        } 
    
    while (again && player.getBalance() > 5) {
        player.initialize(10);
        player.dealInitialCards(2);
        
        again = playRound(player, experienceLevel);
        saveBalance(player, "balance.bin");
        experienceLevel.saveExperience("experience.bin");

        if (player.getBalance() < 5) {
            cout << "\n!-------------------------------------------------!\n";
            cout << " Sorry! Your balance is lower than the minimum bet.\n";
            cout << " Your balance has been reset.\n";
            resetBalanceFile(player, "balance.bin");
            experienceLevel.resetXPFile(experienceLevel, "experience.bin");
            cout << "!-------------------------------------------------!\n";
        }
    }

    return 0;
}