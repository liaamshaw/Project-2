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
#include <cmath>
#include <cstdlib> 
#include <fstream>
#include <string>
#include <ctime> 
#include <vector>

using namespace std;

struct Player {
    int* cards;
    int total;
    float balance;
}; 

int generateRandomCard() {
    return rand() % 13;
}

int getCardValue(int card, int total, bool aceAsEleven) {
    if (card == 0)
        return (total + 11 <= 21 && aceAsEleven) ? 11 : 1; 
    else if (card >= 9 && card <= 12)
        return 10;
    else
        return card + 1;
}

int getCardValue(int card, int total) {
    return getCardValue(card, total, true);
}

void saveBalance(const Player& player, const string& filename) {
    ofstream file(filename, ios::binary | ios::out);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&player.balance), sizeof(player.balance));
        file.close();  
        cout << "Balance saved to " << filename << endl;
    } else {
        cout << "Unable to open file for saving balance." << endl;
    }
}

void resetBalanceFile(Player& player, const string& filename, float defaultBalance = 100.00) {
    ofstream file(filename, ios::binary | ios::out);
    if (file.is_open()) {
        player.balance = defaultBalance;
        file.write(reinterpret_cast<const char*>(&player.balance), sizeof(player.balance));
        file.close();
        cout << " Balance file reset to default value: $" << defaultBalance << endl;
    } else {
        cout << " Unable to open file for resetting balance." << endl;
    }
}

void loadBalance(Player& player, const string& filename) {
    ifstream file(filename, ios::binary | ios::in);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&player.balance), sizeof(player.balance));
        file.close();
        cout << " Balance loaded from " << filename << endl;
    } else {
        cout << " Unable to open file for loading balance. Using default balance." << endl;
    }
}

void displayCards(const int* cardArray, int size, const string cards[]) {
    for (int i = 0; i < size; ++i) {
        int cardValue = cardArray[i] + 1;
        
        if (cardValue == 1) {
            cout << "|A| ";
        } else if (cardValue == 11) {
            cout << "|J| ";
        } else if (cardValue == 12) {
            cout << "|Q| ";
        } else if (cardValue == 13) {
            cout << "|K| ";
        } else {
            cout << "|" << cards[cardArray[i]] << "| ";
        }
    }
    cout << endl;
}

void initializePlayer(Player& player, int cardCount) {
    player.cards = new int[cardCount];
    player.total = 0;
}

void dealInitialCards(Player& player, int cardCount) {
    for (int i = 0; i < cardCount; i++) {
        player.cards[i] = generateRandomCard();
        player.total += getCardValue(player.cards[i], player.total);
    }
}

bool playRound(Player& player, const string* cards, int cardCount) {
    bool bj = false;
    int pcards = 2; 

    cout << "---------------------------------\n";
    cout << "   - Your balance is: $" << player.balance << " -\n\n";
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
    displayCards(player.cards, pcards, cards);
    cout << "\nTotal: " << player.total << endl;

    if (player.total == 21) {
        float betadd = (bet * 3);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << " Lucky you, first try Blackjack!\n";
        cout << "           You win!\n\n";
        cout << "         You made $" << betadd << "!\n";
        player.balance = ((bet * 3) + player.balance);
        cout << fixed << setprecision(2);
        cout << "  - Your balance is: $" << player.balance << " -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        bj = true;
    }

    char choice;
    bool turn = true;

    while (player.total < 21 && turn) {
        cout << "---------------------------------\n";
        cout << "Enter 'H' to hit or 'S' to stay.\n";
        cin >> choice;
        cin.ignore();
        cout << "---------------------------------\n";

        if (choice == 'H' or choice == 'h') {
            pcards++;
            player.cards[pcards - 1] = generateRandomCard();
            cout << "Your cards:\n\n";
            displayCards(player.cards, pcards, cards);
            player.total += getCardValue(player.cards[pcards - 1], player.total);
            cout << "\nTotal: " << player.total << endl;

            if (player.total == 21) {
                float betadd = (bet * 3);
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                cout << "       Blackjack! You win!\n\n";
                cout << "         You made $" << betadd << "!\n";
                player.balance = ((bet * 3) + player.balance);
                cout << fixed << setprecision(2);
                cout << "  - Your balance is: $" << player.balance << " -\n";
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                bj = true;
                turn = false;
            } else if (player.total > 21) {
                cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
                cout << "        Bust! You lose.\n\n";
                cout << "         You lost $" << bet << "!\n";
                player.balance = (player.balance - bet);
                cout << fixed << setprecision(2);
                cout << "  - Your balance is: $" << player.balance << " -\n";
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
    vector<int> dealerCards;

while (dealerTotal < 17 && !bj) {
    pcards++;
    player.cards[pcards - 1] = generateRandomCard();
    dealerCards.push_back(player.cards[pcards - 1]);
    dealerTotal += getCardValue(player.cards[pcards - 1], dealerTotal);
}

    if (!bj) {
        cout << "---------------------------------\n";
        cout << "Dealer's cards:\n\n";
        displayCards(dealerCards.data(), dealerCards.size(), cards);
        cout << "\nTotal: " << dealerTotal << "\n";
        cout << "---------------------------------\n";

        if (dealerTotal > 21 || (dealerTotal <= 21 && dealerTotal < player.total)) {
            float betadd = (bet * 2);
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "   Congratulations! You win!\n\n";
            cout << "         You made $" << betadd << "!\n";
            player.balance = ((bet * 2) + player.balance);
            cout << fixed << setprecision(2);
            cout << "  - Your balance is: $" << player.balance << " -\n";
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        } else if (dealerTotal == player.total) {
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "          It's a tie!\n";
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        } else {
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "        The dealer wins.\n\n";
            cout << "         You lost $" << bet << "!\n";
            player.balance = (player.balance - bet);
            cout << fixed << setprecision(2);
            cout << "  - Your balance is: $" << player.balance << " -\n";
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
    player.balance = 100.00;
    bool again = true;

    string* cards = new string[13];
    for (int i = 0; i < 13; i++) {
        cards[i] = to_string(i + 1);
    }

    loadBalance(player, "balance.bin");

    cout << "<><><><><><><><><><><><><><><><><>\n";
    cout << "     Welcome to Liam's Casino!\n";
    cout << "         - Blackjack -\n             $ $ $\n";
    cout << "     Enter any key to begin:\n";
    cout << "<><><><><><><><><><><><><><><><><>\n";
    cin.get();

    while (again && player.balance > 5) {
        initializePlayer(player, 10);
        dealInitialCards(player, 2);  
        again = playRound(player, cards, 10);
        saveBalance(player, "balance.bin");

        if (player.balance < 5) {
    cout << "\n!-------------------------------------------------!\n";
    cout << " Sorry! Your balance is lower than the minimum bet.\n";
    cout << " Your balance has been reset.\n";
    resetBalanceFile(player, "balance.bin");
    cout << "!-------------------------------------------------!\n";
    }

delete[] player.cards;
    }
delete[] cards;

    return 0;
}