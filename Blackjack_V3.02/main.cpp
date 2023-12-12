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

    int getCardValue(int total, bool aceAsEleven) const {
        if (rank == 0)
            return (total + 11 <= 21 && aceAsEleven) ? 11 : 1;
        else if (rank >= 9 && rank <= 11)
            return 10;
        else
            return rank + 1;
    }

    int getCardValue(int total) const {
        return getCardValue(total, true);
    }

    void display() const {
        cout << "|" << suit << rank + 1 << suit << "|    ";
    }
};

class Player {
private:
    vector<Card> cards;
    float balance;

public:
    Player() : balance(0.0) {}

    ~Player() {}

    void initialize(int cardCount) {
        cards.clear();
        cards.reserve(cardCount);
    }

    void dealInitialCards(int cardCount) {
        for (int i = 0; i < cardCount; i++) {
            cards.push_back(Card(rand() % 13));
        }
    }

    int getTotal() const {
        int total = 0;
        bool aceAsEleven = true;

        for (const auto &card : cards) {
            total += card.getCardValue(total, aceAsEleven);
            if (total > 21 && aceAsEleven) {
                total -= 10;
                aceAsEleven = false;
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

    void addCard() {
        cards.push_back(Card(rand() % 13));
    }

    const vector<Card> &getCards() const {
        return cards;
    }
};

int generateRandomCard() {
    return rand() % 13;
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

void displayCards(const vector<Card>& cards) {
    for (const auto& card : cards) {
        card.display();
    }
    cout << endl;
}

bool playRound(Player& player, const string* cards, int cardCount) {
    bool bj = false;
    int pcards = 2;

    cout << "---------------------------------\n";
    cout << "   - Your balance is: $" << player.getBalance() << " -\n\n";
    cout << "        Place your bets!\n        Min Bet is $5.00  \n        Max Bet is $50.00  \n";
    cout << "---------------------------------\n";

    float bet = 0;
    cout << "Your bet: $";
    cin >> bet;
    cout << "---------------------------------\n";

    while (bet < 5 or bet > 50) {
        cout << "Please choose an amount that is between $5.00 and $50.00\n";
        cout << "Your bet: $";
        cin >> bet;
        cout << "---------------------------------\n";
        cin.clear();
    }

    cout << "Your cards:\n\n";
    displayCards(player.getCards());
    cout << "\nTotal: " << player.getTotal() << endl;

    if (player.getTotal() == 21) {
        float betadd = (bet * 3);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << " Lucky you, first try Blackjack!\n";
        cout << "           You win!\n\n";
        cout << "         You made $" << betadd << "!\n";
        player.setBalance((bet * 3) + player.getBalance());
        cout << fixed << setprecision(2);
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        bj = true;
    }

    char choice;
    bool turn = true;

    while (player.getTotal() < 21 && turn) {
        cout << "---------------------------------\n";
        cout << "Enter 'H' to hit or 'S' to stay.\n";
        cin >> choice;
        cin.ignore();
        cout << "---------------------------------\n";

        if (choice == 'H' or choice == 'h') {
            pcards++;
            player.addCard();
            cout << "Your cards:\n\n";
            displayCards(player.getCards());
            cout << "\nTotal: " << player.getTotal() << endl;

            if (player.getTotal() == 21) {
                float betadd = (bet * 3);
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                cout << "       Blackjack! You win!\n\n";
                cout << "         You made $" << betadd << "!\n";
                player.setBalance((bet * 3) + player.getBalance());
                cout << fixed << setprecision(2);
                cout << "  - Your balance is: $" << player.getBalance() << " -\n";
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                bj = true;
                turn = false;
            } else if (player.getTotal() > 21) {
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                cout << "        Bust! You lose.\n\n";
                cout << "         You lost $" << bet << "!\n";
                player.setBalance(player.getBalance() - bet);
                cout << fixed << setprecision(2);
                cout << "  - Your balance is: $" << player.getBalance() << " -\n";
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
        pcards++;
        player.addCard();
        dealerCards.push_back(player.getCards().back());
        dealerTotal += dealerCards.back().getCardValue(dealerTotal);
    }

    if (!bj) {
        cout << "---------------------------------\n";
        cout << "Dealer's cards:\n\n";
        displayCards(dealerCards);
        cout << "\nTotal: " << dealerTotal << "\n";
        cout << "---------------------------------\n";

        if (dealerTotal > 21 || (dealerTotal <= 21 && dealerTotal < player.getTotal())) {
            float betadd = (bet * 2);
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "   Congratulations! You win!\n\n";
            cout << "         You made $" << betadd << "!\n";
            player.setBalance((bet * 2) + player.getBalance());
            cout << fixed << setprecision(2);
            cout << "  - Your balance is: $" << player.getBalance() << " -\n";
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        } else if (dealerTotal == player.getTotal()) {
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "          It's a tie!\n";
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        } else {
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "        The dealer wins.\n\n";
            cout << "         You lost $" << bet << "!\n";
            player.setBalance(player.getBalance() - bet);
            cout << fixed << setprecision(2);
            cout << "  - Your balance is: $" << player.getBalance() << " -\n";
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        }
    }

    cout << "           Play again?\nEnter 'Y' to continue or 'N' to exit.\n";
    cin >> choice;
    cin.ignore();

    while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n') {
        cout << "Invalid choice, Enter 'Y' to continue or 'N' to exit.\n";
        cin >> choice;
        cin.ignore();
    }

    return (choice == 'y' || choice == 'Y');
}

int main() {
    srand(time(0));

    Player player;
    player.setBalance(100.00);
    bool again = true;

    loadBalance(player, "balance.bin");

    cout << "<><><><><><><><><><><><><><><><><>\n";
    cout << "     Welcome to Liam's Casino!\n";
    cout << "         - Blackjack -\n             $ $ $\n";
    cout << "     Enter any key to begin:\n";
    cout << "<><><><><><><><><><><><><><><><><>\n";
    cin.get();

    while (again && player.getBalance() > 5) {
        player.initialize(10);
        player.dealInitialCards(2);
        again = playRound(player, nullptr, 10);  // Pass nullptr as cards are not used in this version
        saveBalance(player, "balance.bin");

        if (player.getBalance() < 5) {
            cout << "\n!-------------------------------------------------!\n";
            cout << " Sorry! Your balance is lower than the minimum bet.\n";
            cout << " Your balance has been reset.\n";
            resetBalanceFile(player, "balance.bin");
            cout << "!-------------------------------------------------!\n";
        }
    }

    return 0;
}
