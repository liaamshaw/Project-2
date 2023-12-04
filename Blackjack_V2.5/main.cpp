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

// Structure to represent a player in the Blackjack game
struct Player {
    int* cards; // Array to store player's cards
    int total; // Sum of the values of player's cards
    float balance; // Player's balance
}; 

// Function to generate a random card index from 0 to 12 (representing A to K)
int generateRandomCard() {
    return rand() % 13;
}

// Function to get the value of a card
int getCardValue(int card, int total, bool aceAsEleven) {
    if (card == 0)
        return (total + 11 <= 21 && aceAsEleven) ? 11 : 1; 
// Return 11 if adding 11 doesn't bust the total (exceed 21) and aceAsEleven is true, otherwise, return 1.
    else if (card >= 9 && card <= 12)
        return 10;
// Return 10, as these cards have a fixed value of 10 in Blackjack.
    else
        return card + 1;
// Return the card value increased by 1, as the card values in the game are 1 more than their numeric representations.
}

// Overload the getCardValue function to use the default behavior (Ace as 11)
int getCardValue(int card, int total) {
    return getCardValue(card, total, true);
}

// Function to save the player's balance to a binary file
void saveBalance(const Player& player, const string& filename) {
    // Open the file in binary output mode
    ofstream file(filename, ios::binary | ios::out);
    // Check if the file is successfully opened
    if (file.is_open()) {
        // Write the binary representation of the player's balance to the file
        file.write(reinterpret_cast<const char*>(&player.balance), sizeof(player.balance));
        // Close the file
        file.close();  
        // Print a success message
        cout << "Balance saved to " << filename << endl;
    } else {
        // Print an error message if the file cannot be opened
        cout << "Unable to open file for saving balance." << endl;
    }
}

// Function to reset the balance file to a default value
void resetBalanceFile(Player& player, const string& filename, float defaultBalance = 100.00) {
    // Open the file in binary output mode
    ofstream file(filename, ios::binary | ios::out);
    // Check if the file is successfully opened
    if (file.is_open()) {
        // Set the player's balance to the default value
        player.balance = defaultBalance;
        // Write the binary representation of the new balance to the file
        file.write(reinterpret_cast<const char*>(&player.balance), sizeof(player.balance));
        // Close the file
        file.close();
        // Print a success message
        cout << " Balance file reset to default value: $" << defaultBalance << endl;
    } else {
        // Print an error message if the file cannot be opened
        cout << " Unable to open file for resetting balance." << endl;
    }
}

// Function to load the player's balance from a binary file
void loadBalance(Player& player, const string& filename) {
    // Open the file in binary input mode
    ifstream file(filename, ios::binary | ios::in);
    // Check if the file is successfully opened
    if (file.is_open()) {
        // Read the binary representation of the player's balance from the file
        file.read(reinterpret_cast<char*>(&player.balance), sizeof(player.balance));
        // Close the file
        file.close();
        // Print a success message
        cout << " Balance loaded from " << filename << endl;
    } else {
        // Print an error message and use the default balance if the file cannot be opened
        cout << " Unable to open file for loading balance. Using default balance." << endl;
    }
}

void displayCards(const int* cardArray, int size, const string cards[]) {
    // Iterate through the card indices
    for (int i = 0; i < size; ++i) {
        // Calculate the card value (1-indexed)
        int cardValue = cardArray[i] + 1;
        
        // Print the corresponding card representation
        if (cardValue == 1) {
            cout << "|A| "; // Print 'A' for Ace
        } else if (cardValue == 11) {
            cout << "|J| "; // Print 'J' for Jack
        } else if (cardValue == 12) {
            cout << "|Q| "; // Print 'Q' for Queen
        } else if (cardValue == 13) {
            cout << "|K| "; // Print 'K' for King
        } else {
            cout << "|" << cards[cardArray[i]] << "| "; // Print numeric card value
        }
    }
    // Add a newline for formatting
    cout << endl;
}

void initializePlayer(Player& player, int cardCount) {
    // Dynamically allocate an array of integers for the player's cards
    player.cards = new int[cardCount];
    //Initialize the total value of the cards to zero
    player.total = 0;
}

// Function to deal initial cards to the player
void dealInitialCards(Player& player, int cardCount) {
    // Loop to deal the specified number of initial cards
    for (int i = 0; i < cardCount; i++) {
        // Generate a random card index and assign it to the player's card array
        player.cards[i] = generateRandomCard();
        // Update the player's total card value based on the dealt card
        player.total += getCardValue(player.cards[i], player.total);
    }
}

// Function to play a single round of Blackjack
bool playRound(Player& player, const string* cards, int cardCount) {
    bool bj = false;
    int pcards = 2;  // Set pcards to 2 for the initial two cards

    // Display initial balance and get the player's bet
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

    // Display the player's cards
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

    int dealerTotal = 0;  // Initialize dealer's total to 0
    vector<int> dealerCards;

while (dealerTotal < 17 && !bj) {
    // Increment the count of cards held by the player
    pcards++;
    // Generate a random card and add it to the player's card array
    player.cards[pcards - 1] = generateRandomCard();
    // Add the drawn card to the dealer's card vector
    dealerCards.push_back(player.cards[pcards - 1]);
    // Update the dealer's total by adding the value of the drawn card
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
    srand(time(0));  // Move the srand call here

    Player player;
    player.balance = 100.00;
    bool again = true;

    string* cards = new string[13];
    for (int i = 0; i < 13; i++) {
        cards[i] = to_string(i + 1);
    }

    // Load the player's balance from a binary file (if available)
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

        // Save the player's balance to a binary file
        saveBalance(player, "balance.bin");

        if (player.balance < 5) {
    // Display a warning message about the low balance
    cout << "\n!-------------------------------------------------!\n";
    cout << " Sorry! Your balance is lower than the minimum bet.\n";
    // Perform a balance reset, including resetting the balance file
    cout << " Your balance has been reset.\n";
    resetBalanceFile(player, "balance.bin");
    // Print a closing separator for better visual presentation
    cout << "!-------------------------------------------------!\n";
    }

// Delete dynamically allocated memory for the player's cards
delete[] player.cards;
    }
// Delete dynamically allocated memory for the cards array used in the game
delete[] cards;

    return 0;
}