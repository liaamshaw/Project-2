/*
 * File:   main.cpp
 * Author: Liam Shaw
 *
 * Created on December 17
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

// Class representing a playing card with a rank and suit
class Card {
protected:
    int rank;     // Numeric value representing the card's rank (0-12)
    string suit;  // String representing the card's suit (♥, ♦, ♣, ♠)

public:
    // Constructor: Initializes a card with the given rank and determines its suit based on the rank
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

    // Copy constructor
    Card(const Card &other) : rank(other.rank), suit(other.suit) {}
    
    // Getter method for retrieving the card's suit
    string getSuit() const {
        return suit;
    }

    // Calculates the value of the card in a game, considering the total points and the option to treat an Ace as 11
    inline int getCardValue(int total, bool aceAsEleven) const {
        if (rank == 0) {
            return (total + 11 <= 21 && aceAsEleven) ? 11 : 1;
        } else if (rank >= 10 && rank <= 12) {
            return 10;
        } else {
            return rank + 1;
        }
    }

    // Overloaded method with a default parameter to simplify calling code
    inline int getCardValue(int total) const {
        return getCardValue(total, true);
    }

    // Displays a textual representation of the card in a simple ASCII art format
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

    // Overloaded equality operator to compare two cards
    bool operator==(const Card& other) const {
        return rank == other.rank && suit == other.suit;
    }
};

// Class representing a player in a card game
class Player {
private:
    vector<Card> cards;            // Player's current hand of cards
    float balance;                  // Player's balance or money
    bool doubleDown;                // Flag indicating whether the player has chosen to double down
    float xpMultiplier;             // Experience multiplier for the player
    float betMultiplier;            // Bet multiplier for the player
    int privateMember;

public:
    // Constructor
    Player() : balance(0.0), doubleDown(false), xpMultiplier(1.0), betMultiplier(1.0) {}

    // Destructor
    ~Player() {}

    // Friend function to add a card to the dealer's hand during a Blackjack game.
    friend int addCardToDealer(Player& player, vector<Card>& dealerCards, int& dealerTotal);
    
    // Getter function for experience multiplier
    float getXPMultiplier() const {
        return xpMultiplier;
    }

    // Getter function for bet multiplier
    float getBetMultiplier() const {
        return betMultiplier;
    }

    // Setter function for experience multiplier
    void setXPMultiplier(float multiplier) {
        xpMultiplier = multiplier;
    }

    // Setter function for bet multiplier
    void setBetMultiplier(float multiplier) {
        betMultiplier = multiplier;
    }
    
    // Initializes the player's hand with a specified number of cards
    void initialize(int cardCount) {
        cards.clear();
        cards.reserve(cardCount);
    }

    // Array of suit names for card display
    const string suitNames[5] = {" ♥", " ♦", " ♣", " ♠"};

    // Deals a specified number of initial cards to the player
    void dealInitialCards(int cardCount) {
        set<pair<int, string>> usedCards;

        for (int i = 0; i < cardCount; i++) {
            int newRank;
            string newSuit;

            // Generate a random card that has not been used before
            do {
                newRank = rand() % 13;
                newSuit = (newRank / 4 == 0) ? " ♥" : (newRank / 4 == 1) ? " ♦" : (newRank / 4 == 2) ? " ♣" : " ♠";
            } while (usedCards.count({newRank, newSuit}) > 0);

            cards.push_back(Card(newRank));
            usedCards.insert({newRank, newSuit});
        }
    }

    // Getter function for the player's current hand
    const vector<Card>& getCards() const {
        return cards;
    }
    
    // Calculates the total value of the player's hand considering aces
    int getTotal() const {
        int total = 0;
        int aceCount = 0;

        for (const auto &card : cards) {
            int cardValue = card.getCardValue(total, true);
            total += cardValue;

            if (cardValue == 11) {
                aceCount++;
            }

            // Handle aces as 1 if needed to avoid busting
            while (total > 21 && aceCount > 0) {
                total -= 10;
                aceCount--;
            }
        }
        return total;
    }

    // Getter function for the player's balance
    float getBalance() const {
        return balance;
    }

    // Setter function for the player's balance
    void setBalance(float newBalance) {
        balance = newBalance;
    }

    // Getter function for the double down flag
    bool getDoubleDown() const {
        return doubleDown;
    }

    // Setter function for the double down flag
    void setDoubleDown(bool value) {
        doubleDown = value;
    }
    
    // Adds a new card to the player's hand, ensuring it has not been used before
    int addCard() {
        int newRank;
        string newSuit;

        // Generate a random card that has not been used before
        do {
            newRank = rand() % 13;
            newSuit = (newRank / 4 == 0) ? " ♥" : (newRank / 4 == 1) ? " ♦" : (newRank / 4 == 2) ? " ♣" : " ♠";
        } while (find(cards.begin(), cards.end(), Card(newRank)) != cards.end());

        cards.push_back(Card(newRank));

        // Return the updated total value of the player's hand
        return getTotal();
    }
};

//Tracks the XP level of the player
class ExperienceLevel {
private:
    int level;               // Current level of the player
    int experiencePoints;    // Total experience points accumulated by the player
    float xpMultiplier;      // Multiplier affecting experience point gain

public:
    // Default constructor initializes the level to 1 and experiencePoints to 0
    ExperienceLevel() : level(1), experiencePoints(0) {}

    // Getter method to retrieve the experience point multiplier
    float getXPMultiplier() const {
        return xpMultiplier;
    }
    
    // Getter method to retrieve the current level
    int getLevel() const {
        return level;
    }

    // Getter method to retrieve the total experience points
    int getExperiencePoints() const {
        return experiencePoints;
    }

    // Method to simulate gaining experience points and handle level-ups
    void gainExperience(int points) {
        experiencePoints += points;
        
        if (experiencePoints >= level * 100) {
            experiencePoints -= level * 100;
            level++;
            cout << "   - Level up! New Level: " << level << "! -\n";
        }
    }

    // Getter method to retrieve the betting limit based on the player's level
    float getBettingLimit() const {
        const float levelBettingLimits[] = {50.0, 100.0, 250.0, 500.0, 1000.0};
        int adjustedLevel = min(max(level, 1), static_cast<int>(sizeof(levelBettingLimits) / sizeof(levelBettingLimits[0])));
        return levelBettingLimits[adjustedLevel - 1];
    }
    
    // Method to reset the experience level and save it to a binary file
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
            cout << " XP file reset to: Level: " << defaultLevel << endl;
        } else {
        cout << " Unable to open file for resetting XP." << endl;
    }
}
    
    // Method to save the current experience level to a binary file
    void saveExperience(const string& filename) const {
        ofstream file(filename, ios::binary | ios::out);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&level), sizeof(level));
            file.write(reinterpret_cast<const char*>(&experiencePoints), sizeof(experiencePoints));
            file.close();
        } else {
            cout << " Unable to open file for saving experience level." << endl;
        }
    }

    // Method to load the experience level from a binary file
    void loadExperience(const string& filename) {
        ifstream file(filename, ios::binary | ios::in);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(&level), sizeof(level));
            file.read(reinterpret_cast<char*>(&experiencePoints), sizeof(experiencePoints));
            file.close();
            cout << " XP level loaded from " << filename << endl;
        } else {
            cout << " Unable to open file for loading experience level. Using default level." << endl;
        }
    }
};

// Class definition for a Shop that offers XP and Bet multipliers for purchase
class Shop {
private:
    // Arrays to store the prices of different XP and Bet multipliers
    float xpMultiplierPrices[3] = {30.0, 75.0, 150.0};
    float betMultiplierPrices[3] = {100.0, 200.0, 300.0};

public:
    // Function to display the items available in the shop along with their prices
    void displayShop() {
        cout << "---------------------------------\n";
        cout << "       Welcome to the Shop!\n\n";
        cout << "  - XP Multipliers:\n";
        cout << "  1. 1.5x XP Multiplier:  $" << xpMultiplierPrices[0] << "\n";
        cout << "  2. 2x XP Multiplier:    $" << xpMultiplierPrices[1] << "\n";
        cout << "  3. 3x XP Multiplier:    $" << xpMultiplierPrices[2] << "\n";
        cout << "  - Bet Multipliers:\n";
        cout << "  4. 1.5x Bet Multiplier: $" << betMultiplierPrices[0] << "\n";
        cout << "  5. 2x Bet Multiplier:   $" << betMultiplierPrices[1] << "\n";
        cout << "  6. 3x Bet Multiplier:   $" << betMultiplierPrices[2] << "\n";
        cout << "---------------------------------\n";
    }

    // Function to handle the purchase of XP Multiplier by a player
    bool purchaseXPMultiplier(Player& player, int multiplier) {
        float price = xpMultiplierPrices[multiplier - 1];
        // Check if the player has sufficient funds to make the purchase
        if (player.getBalance() >= price) {
            // Deduct the price from the player's balance and update the XP Multiplier
            player.setBalance(player.getBalance() - price);
            player.setXPMultiplier(player.getXPMultiplier() * static_cast<float>(multiplier) / 2.0);
            return true; // Purchase successful
        } else {
            cout << "Insufficient funds to purchase XP multiplier.\n";
            return false; // Purchase failed due to insufficient funds
        }
    }

    // Function to handle the purchase of Bet Multiplier by a player
    bool purchaseBetMultiplier(Player& player, int multiplier) {
        float price = betMultiplierPrices[multiplier - 1];
        // Check if the player has sufficient funds to make the purchase
        if (player.getBalance() >= price) {
            // Deduct the price from the player's balance and update the Bet Multiplier
            player.setBalance(player.getBalance() - price);
            player.setBetMultiplier(static_cast<float>(multiplier) / 2.0);
            return true; // Purchase successful
        } else {
            cout << "Insufficient funds to purchase bet multiplier.\n";
            return false; // Purchase failed due to insufficient funds
        }
    }
};

bool promptForPurchase(Player& player, Shop& shop, int option);
// Function to prompt the player to visit the shop and make a purchase
bool promptForShop(Player& player, Shop& shop) {
    cout << "Would you like to visit the shop?\nEnter 'Y' to continue or 'N' to exit.\n";
    char choice;
    cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        shop.displayShop();
        cout << "Enter the option number to make a purchase (0 to leave):\n";
        int option;
        cin >> option;
        cout << "---------------------------------\n";
        
        // Process the player's choice
        switch (option) {
            case 0:
                cout << "You chose to skip the shop.\n";
                return true;
            // Cases 1-3 represent XP multipliers, and cases 4-6 represent Bet multipliers
            case 1:
                if (promptForPurchase(player, shop, 1)) {
                    cout << "You purchased a 1.5x XP multiplier!\n";
                    cout << "You spent: $30.00\n";
                    cout << "---------------------------------\n";
                }
                break;
            case 2:
                if (promptForPurchase(player, shop, 2)) {
                    cout << "You purchased a 2x XP multiplier!\n";
                    cout << "You spent: $75.00\n";
                    cout << "---------------------------------\n";
                }
                break;
            case 3:
                if (promptForPurchase(player, shop, 3)) {
                    cout << "You purchased a 3x XP multiplier!\n";
                    cout << "You spent: $150.00\n";
                    cout << "---------------------------------\n";
                }
                break;
            case 4:
                if (promptForPurchase(player, shop, 4)) {
                    cout << "You purchased a 1.5x Bet multiplier!\n";
                    cout << "You spent: $100.00\n";
                    cout << "---------------------------------\n";
                }
                break;
            case 5:
                if (promptForPurchase(player, shop, 5)) {
                    cout << "You purchased a 2x Bet multiplier!\n";
                    cout << "You spent: $200.00\n";
                    cout << "---------------------------------\n";
                }
                break;
            case 6:
                if (promptForPurchase(player, shop, 6)) {
                    cout << "You purchased a 3x Bet multiplier!\n";
                    cout << "You spent: $300.00\n";
                    cout << "---------------------------------\n";
                }
                break;
            default:
                cout << "Invalid option. Returning to the game.\n";
                break;
        }
        return true;
    } else {
        return true;
    }
}

// Function to process the purchase based on the selected option
bool promptForPurchase(Player& player, Shop& shop, int option) {
    switch (option) {
        case 1:
        case 2:
        case 3:
            return shop.purchaseXPMultiplier(player, option);
        case 4:
        case 5:
        case 6:
            return shop.purchaseBetMultiplier(player, option - 3);
        default:
            return false;
    }
}

// Function to add a card to the dealer's hand
int addCardToDealer(Player& player, vector<Card>& dealerCards, int& dealerTotal) {
    // Variables to store the new card's rank and suit
    int newRank;
    string newSuit;

    // Generate a random rank for the new card and determine its suit based on the rank
    do {
        newRank = rand() % 13;
        newSuit = (newRank / 4 == 0) ? " ♥" : (newRank / 4 == 1) ? " ♦" : (newRank / 4 == 2) ? " ♣" : " ♠";
    } while (find(dealerCards.begin(), dealerCards.end(), Card(newRank)) != dealerCards.end());

    // Add the new card to the dealer's hand
    dealerCards.push_back(Card(newRank));

    // Calculate the new total value of the dealer's hand, adjusting for Ace value if necessary
    int total = dealerTotal + dealerCards.back().getCardValue(dealerTotal);
    if (total > 21) {
        // Check for Aces and adjust total if necessary
        for (const auto& card : dealerCards) {
            if (card.getCardValue(dealerTotal) == 11) {
                total -= 10;
            }
        }
    }

    // Update the dealer's total value
    dealerTotal = total;
    
    // Return the value of the last added card
    return dealerCards.back().getCardValue(dealerTotal);
}

// Function to display the cards in a visually appealing format
void displayCards(const vector<Card>& cards) {
    // Loop through each card and print its representation in a row
    for (const auto& card : cards) {
        string rankString;
        // Determine the string representation of the card's rank
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

// Function to add a card to the dealer's hand and update the dealer's total
int addCardToDealer(Player& player, vector<Card>& dealerCards, int& dealerTotal);

// Function to execute a single round of the blackjack game
// Takes player information, experience level, and shop as parameters
bool playRound(Player& player, ExperienceLevel& experienceLevel, Shop& shop) {
    // Flag to check if a blackjack has occurred during the round
    bool bj = false;

    // Initial number of cards dealt to the player
    int pcards = 2;

    // Display player information and betting options
    cout << "---------------------------------\n";
    cout << "   - Your balance is: $" << player.getBalance() << " -\n";
    cout << "     - Your level is: " << experienceLevel.getLevel() << " -\n\n";
    cout << "        Place your bets!\n        Min Bet is $5.00  \n";
    cout << "        Max Bet is $" << fixed << setprecision(2) << experienceLevel.getBettingLimit() << endl;
    cout << "---------------------------------\n";

    // Get the player's bet amount.
    float bet = 0;
    cout << "Your bet: $";
    cin >> bet;
    cout << "---------------------------------\n";

    // Validate the bet amount to be within the allowed range
    while (bet < 5 || bet > experienceLevel.getBettingLimit()) {
        cout << "Please choose an amount between $5.00 and $" << fixed << setprecision(2) << experienceLevel.getBettingLimit() << "\n";
        cout << "Your bet: $";
        cin >> bet;
        cout << "---------------------------------\n";
        cin.clear();
    }

    // Display the player's initial cards and total
    cout << "Your cards:\n";
    displayCards(player.getCards());
    cout << "\nTotal: " << player.getTotal() << endl;

    // Variable to store the player's choice for doubling down
    char doubleDownChoice;

    // Flag to determine if the player can hit or stand
    bool canHitOrStand = true;

    // Flag to track if it is the first attempt to achieve blackjack
    bool firstTryBlackjack = false;
    
// This loop prompts the player for a decision on whether to double down in the game
while (true) {
    cout << "---------------------------------\n";
    cout << "Do you want to double down?\nEnter 'Y' to continue or 'N' to exit.\n";
    cin >> doubleDownChoice;

    // If the player chooses to double down
    if (doubleDownChoice == 'Y' || doubleDownChoice == 'y') {
        cout << "---------------------------------\n";
        
        // Store the original bet, double the current bet, and update player balance.
        float originalBet = bet;
        bet *= 2;
        player.setBalance(player.getBalance() - originalBet);
        player.setDoubleDown(true);
        
        // Draw an additional card for the player after doubling down.
        player.addCard();
        cout << "Your cards after doubling down:\n";
        displayCards(player.getCards());
        int doubledDownTotal = player.getTotal();
        cout << "\nTotal: " << player.getTotal() << endl;

        // Check for a blackjack after doubling down
        if (doubledDownTotal == 21) {
           float betMultiplier = player.getBetMultiplier();
           float xpMultiplier = experienceLevel.getXPMultiplier();
           // Calculate winnings, update player balance, and award experience points
           float betadd = (bet * 3) * betMultiplier;
           cout << fixed << setprecision(2);
           cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
           cout << "       Blackjack! You win!\n\n";
           cout << "        You made $" << betadd << "!\n";
           player.setBalance((bet * 3 * betMultiplier) + player.getBalance());
           cout << "  - Your balance is: $" << player.getBalance() << " -\n";
           experienceLevel.gainExperience(40 * xpMultiplier);
           cout << "       - XP +40" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
           cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
           bj = true;
           
           if (!player.getDoubleDown()) {
                    firstTryBlackjack = true;
                }
        // Display a message indicating a bust, deduct the bet, and update experience points
        } else if (doubledDownTotal > 21) {
            cout << fixed << setprecision(2);
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "        Bust! You lose.\n\n";
            cout << "        You lost $" << bet << "\n";
            player.setBalance(player.getBalance() - bet);
            cout << "  - Your balance is: $" << player.getBalance() << " -\n";
            experienceLevel.gainExperience(-10);
            cout << "        - XP: -10 (" << experienceLevel.getExperiencePoints() << "/100) -\n";
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            bj = true;

        // If no immediate win or loss, disable further hits or stands
        } else {
            canHitOrStand = false;
        }

        break;
    // If the player chooses not to double down, enable further hits or stands and exit the loop
    } else if (doubleDownChoice == 'N' || doubleDownChoice == 'n') {
        canHitOrStand = true;
        break;
    } else {
        cout << "Invalid choice. Please enter 'Y' or 'N'.\n";
    }
}
    
// Check if the player has a Blackjack on the first try
if (player.getTotal() == 21 && !firstTryBlackjack) {
    // Retrieve bet and experience multipliers
    float betMultiplier = player.getBetMultiplier();
    float xpMultiplier = experienceLevel.getXPMultiplier();

    // Calculate the winnings and update player's balance
    float betadd = (bet * 3) * betMultiplier;
    player.setBalance((bet * 3 * betMultiplier) + player.getBalance());

    // Display the result and update experience points
    cout << fixed << setprecision(2);
    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    cout << " Lucky you, first try Blackjack!\n";
    cout << "           You win!\n\n";
    cout << "        You made $" << betadd << "!\n";
    cout << "  - Your balance is: $" << player.getBalance() << " -\n";
    experienceLevel.gainExperience(20 * xpMultiplier);
    cout << "       - XP +20" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";

    // Set the flag to indicate that the player achieved Blackjack on the first try
    bj = true;
}

// Player's turn to decide whether to hit or stand
char choice;
bool turn = true;

// This loop manages the player's turn in the blackjack game
while (player.getTotal() < 21 && turn && canHitOrStand == true) {
    cout << "---------------------------------\n";
    cout << "Enter 'H' to hit or 'S' to stay.\n";
    cin >> choice;
    cin.ignore();
    cout << "---------------------------------\n";

    // If the player chooses to hit
    if (choice == 'H' or choice == 'h') {
        pcards++;
        player.addCard();
        cout << "Your cards:\n";
        displayCards(player.getCards());
        cout << "\nTotal: " << player.getTotal() << endl;

        // Calculate multipliers for bet and experience points
        float betMultiplier = player.getBetMultiplier();
        float xpMultiplier = experienceLevel.getXPMultiplier();

        // If the player gets a blackjack (total equals 21)
        if (player.getTotal() == 21) {
            float betadd = (bet * 3) * betMultiplier;
            cout << fixed << setprecision(2);
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "       Blackjack! You win!\n\n";
            cout << fixed << setprecision(2);
            cout << "        You made $" << betadd << "!\n";
            player.setBalance((bet * 3 * betMultiplier) + player.getBalance());
            cout << "  - Your balance is: $" << player.getBalance() << " -\n";
            experienceLevel.gainExperience(20 * xpMultiplier);
            cout << "       - XP +20" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            bj = true;
            turn = false;
        }
        // If the player goes over 21 (busts)
        else if (player.getTotal() > 21) {
            float betLost = bet;
            cout << fixed << setprecision(2);
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
            cout << "        Bust! You lose.\n\n";
            cout << "        You lost $" << bet << "\n";
            player.setBalance(player.getBalance() - bet);
            cout << "  - Your balance is: $" << player.getBalance() << " -\n";
            experienceLevel.gainExperience(-5);
            cout << "        - XP: -5 (" << experienceLevel.getExperiencePoints() << "/100) -\n";
            cout << "-=-=-=-=-=-c=-=-=-=-=-=-=-=-=-=-=-\n";
            bj = true;
            turn = false;
        }
    } 
    // If the player chooses to stay
    else if (choice == 'S' or choice == 's') {
        cout << "       You chose to stay.\n";
        turn = false;
    } 
    // If the player enters an invalid choice
    else {
        cout << "Invalid choice. Please enter 'H' to hit or 'S' to stay.\n";
    }
}

// This loop manages the dealer's turn in the blackjack game
int dealerTotal = 0;
vector<Card> dealerCards;

// Dealer draws cards until their total is at least 17
while (dealerTotal < 17 && !bj) {
    addCardToDealer(player, dealerCards, dealerTotal);
}


// Check if the dealer (bj) has not won, indicating that the game is still in progress
if (!bj) {
    // Display the dealer's cards and game information
    cout << "---------------------------------\n";
    cout << "Dealer's cards:\n";
    displayCards(dealerCards);
    cout << "\nDealer Total: " << dealerTotal << "\n";
    cout << "Player Total: " << player.getTotal() << endl;
    cout << "---------------------------------\n";

    // Retrieve multipliers for bet and experience points based on player and experience level
    float betMultiplier = player.getBetMultiplier();
    float xpMultiplier = experienceLevel.getXPMultiplier();

    // Check if the dealer has busted (total over 21)
    if (dealerTotal > 21) {
        // Calculate winnings, update player's balance, and provide feedback
        float betadd = (bet * 2) * betMultiplier;
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "       The dealer busts!\n";
        cout << "   Congratulations, you win!\n\n";
        cout << "        You made $" << betadd << "!\n";
        player.setBalance((bet * 2 * betMultiplier) + player.getBalance());
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(10 * xpMultiplier);
        cout << "       - XP +10 (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    } else if (dealerTotal > player.getTotal()) {
        // Handle the case where the dealer wins, deduct bet, and update player's balance
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "        The dealer wins.\n\n";
        cout << "        You lost $" << bet << "\n";
        player.setBalance(player.getBalance() - bet);
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(-5);
        cout << "        - XP: -5 (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    } else if (dealerTotal == player.getTotal()) {
        // Handle the case of a tie, update player's balance, and provide feedback
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "          It's a tie!\n";
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(5 * xpMultiplier);
        cout << "       - XP +5 (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    } else {
        // Handle the case where the player wins, calculate winnings, update balance, and provide feedback
        float betWon = bet * 2 * betMultiplier;
        cout << fixed << setprecision(2);
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        cout << "   Congratulations, you win!\n\n";
        cout << "        You made $" << betWon << "!\n";
        player.setBalance(betWon + player.getBalance());
        cout << "  - Your balance is: $" << player.getBalance() << " -\n";
        experienceLevel.gainExperience(10 * xpMultiplier);
        cout << "       - XP +10" << " (" << experienceLevel.getExperiencePoints() << "/100) -\n";
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    }
}
// Prompt the user for input to play again
cout << "           Play again?\nEnter 'Y' to continue or 'N' to exit.\n";
cin >> choice;
cin.ignore();
cout << "---------------------------------\n";

// Validate user input; loop until a valid choice is entered
while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n') {
    cout << "Invalid choice, Enter 'Y' to continue or 'N' to exit.\n";
    cin >> choice;  // Get user input again if the choice is invalid
    cin.ignore();
}

// If the user chooses to play again, prompt for a shop visit and return the result
if (choice == 'y' || choice == 'Y') {
    bool visitedShop = promptForShop(player, shop);
    return visitedShop;
} else {
    // If the user chooses not to play again, return false
    return false;
}

// Alternatively, return the result of the play again choice
return (choice == 'y' || choice == 'Y');
}

// Function to save the player's balance to a binary file
void saveBalance(const Player& player, const string& filename) {
    ofstream file(filename, ios::binary | ios::out);

    // Check if the file is successfully opened
    if (file.is_open()) {
        // Retrieve the player's balance and write it to the file
        float balance = player.getBalance();
        file.write(reinterpret_cast<const char*>(&balance), sizeof(balance));
        file.close();
    } else {
        // Display an error message if the file couldn't be opened
        cout << " Unable to open file for saving balance." << endl;
    }
}

// Function to reset the player's balance file to a specified default balance
void resetBalanceFile(Player& player, const string& filename, float defaultBalance = 100.00) {
    ofstream file(filename, ios::binary | ios::out);

    // Check if the file is successfully opened
    if (file.is_open()) {
        // Set the player's balance to the default value and write it to the file
        player.setBalance(defaultBalance);
        float balance = player.getBalance();
        file.write(reinterpret_cast<const char*>(&balance), sizeof(balance));
        file.close();

        // Display a message indicating the successful reset and the new default balance
        cout << " Balance file reset to: $" << defaultBalance << endl;
    } else {
        // Display an error message if the file couldn't be opened for resetting
        cout << " Unable to open file for resetting balance." << endl;
    }
}

// Function to load the player's balance from a binary file
void loadBalance(Player& player, const string& filename) {
    ifstream file(filename, ios::binary | ios::in);

    // Check if the file is successfully opened
    if (file.is_open()) {
        // Read the balance from the file and set it for the player
        float balance;
        file.read(reinterpret_cast<char*>(&balance), sizeof(balance));
        player.setBalance(balance);
        file.close();

        // Display a message indicating the successful loading of the balance
        cout << " Balance loaded from " << filename << endl;
    } else {
        // Display an error message and use the default balance if the file couldn't be opened
        cout << " Unable to open file for loading balance. Using default balance." << endl;
    }
}

int main() {
    // Initialize random seed
    srand(time(0));

    // Create instances of Player, ExperienceLevel, and Shop
    Player player;
    ExperienceLevel experienceLevel;
    Shop shop;

    // Set initial balance for the player
    player.setBalance(100.00);

    // Flag to control game continuation
    bool again = true;

    // Load player's balance and experience level from files
    loadBalance(player, "balance.bin");
    experienceLevel.loadExperience("experience.bin");

    // Display welcome message and instructions
    cout << "<><><><><><><><><><><><><><><><><>\n";
    cout << "     Welcome to Liam's Casino!\n";
    cout << "\n         - Blackjack -\n             $ $ $\n\n";
    cout << "     Enter any key to begin\n";
    cout << "   OR type 'HELP' for details:\n";
    cout << "<><><><><><><><><><><><><><><><><>\n";
    
    // Receive user input to start the game or display help menu
    string userInput;
    cin >> userInput;

    // Display help menu if requested
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
             << "  - $ Standard Win $\n    Payout: 2x Bet\n"
             << "  - $$ Blackjack $$\n    Payout: 3x Bet\n"
             << "  - $$$ Double Down $$$\n    Payout: 4x Bet\n"
             << "  - $$$$$ Double Down + Blackjack $$$$$\n    Payout: 6x Bet\n";
        cout << "\nLEVELS:\n"
             << "* In this game, each level you gain increases your betting limit,\n"
             << "  allowing for riskier but potentially higher payouts.\n"
             << "  - Winning a game grants +10 XP.\n"
             << "  - Getting a Blackjack grants +20 XP.\n"
             << "  - Getting a tie grants +5 XP.\n"
             << "  - Losing a game loses -5 XP.\n";
        cout << "\nBETTING LIMIT:\n"
             << "  - Level 1: Betting Limit: $50\n"
             << "  - Level 2: Betting Limit: $100\n"
             << "  - Level 3: Betting Limit: $250\n"
             << "  - Level 4: Betting Limit: $500\n"
             << "  - Level 5: Betting Limit: $1000\n\n";
        } 
    
    // Main game loop
    while (again && player.getBalance() > 5) {
    // Initialize player, deal initial cards, and play a round
    player.initialize(10);
    player.dealInitialCards(2);

    // Play a round and update player balance and experience level
    again = playRound(player, experienceLevel, shop);
    saveBalance(player, "balance.bin");
    experienceLevel.saveExperience("experience.bin");

    // Check if the player's balance is below the minimum bet
    if (player.getBalance() < 5) {
        // Display a message and reset the player's balance
        cout << "\n!-------------------------------------------------!\n";
        cout << " Sorry! Your balance is lower than the minimum bet.\n";
        cout << " Your balance has been reset.\n";
        resetBalanceFile(player, "balance.bin");
        experienceLevel.resetXPFile(experienceLevel, "experience.bin");
        cout << "!-------------------------------------------------!\n";
        // Exit the loop to end the game
        break;
    }

    // Prompt the player for shop interactions
    promptForShop(player, shop);
    }

// End of the program
return 0;
}