// BlackJack.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <list>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <random>

#include <fcntl.h>
#include <io.h>


#define SPADE L"\u2660"
#define HEART L"\u2665"
#define DIAMOND L"\u2666"
#define CLUB L"\u2663"

enum SUIT {spade = 1, heart, diamond, club};

using namespace std;

// Structures
struct Card {
	string name;
	int value;
};

// Functions
vector<Card> initializeDeck(void);
void dealCard(vector<Card>& deck, int& deckIndex, vector<Card>& hand);
int handValue(vector<Card> hand, int &aces, int total);
void aceCheck(vector<Card> hand, int& aces);
void displayPlayerHand(vector<Card> playerHand, int playerTotal, bool split, int hitSplit, int playerSplit1, int playerSplit2, 
	vector<Card> playerSplitHand1, vector<Card> playerSplitHand2);
void displayDealerHand(vector<Card> dealerHand, int dealerTotal, int dealerInitTotal, int playerTotal, string hit);
void delay(void);
void displayBankroll(int bankroll);
void getRunningCount(vector<Card> deck, int deckIndex);
void printCard(Card card);
void basicStrategy(int playerTotal, int dealerInitTotal);

// KNOWN ISSUES
// Double down on one split pair will change the bet for both pairs - probably need to new variables to keep track of bets on each pair respectively



int main()
{
	vector<Card> deck = initializeDeck();
	int bankroll = 5;
	int round = 0;
	int deckIndex = 0;
	int runningCount = 0;
	int bet = 0;

	// ---------------------------- START ROUND LOOP ------------------------------------------------------
	while (bankroll > 0)
	{
		round++;
		cout << "        ROUND " << round << endl;
		displayBankroll(bankroll);
		int numOfDecks = ceil((deck.size()-deckIndex)/52);
		cout << "\nNumber of Decks Remaining: " << numOfDecks << endl;
		
			
		getRunningCount(deck, deckIndex);
		vector<Card> playerHand = {};
		vector<Card> playerSplitHand1 = {};
		vector<Card> playerSplitHand2 = {};
		vector<Card> dealerHand = {};
		string hit = "h";
		int dealerTotal = 0;
		int dealerAces = 0;
		int playerTotal = 0;
		int playerSplit1 = 0;
		int playerAces = 0;
		bool split = false;
		int hitSplit = 0;
		

		do {
			cin.clear();
			cin.ignore(256, '\n');
			cout << "Place your bet: $";
			cin >> bet;
			if (bet > bankroll)
				cout << "You Don't have that much money to bet\n";
			if (cin.fail())
				cout << "Invalid Input\n";
		} while (bet > bankroll);

		if (bet == 0) {
			bankroll = 0;
			break;
		}
		
			dealCard(deck, deckIndex, playerHand);	// deal initial hands
			dealCard(deck, deckIndex, dealerHand);
			dealCard(deck, deckIndex, playerHand);
			dealCard(deck, deckIndex, dealerHand);	// deals 2 cards to player and 2 cards to dealer
			aceCheck(playerHand, playerAces);
			aceCheck(dealerHand, dealerAces);	// check each card for value 11, add 1 to aces variable
			dealerTotal = handValue(dealerHand, dealerAces, dealerTotal);
			playerTotal = handValue(playerHand, playerAces, playerTotal);
				
		int dealerInitTotal = dealerHand[1].value;
		playerSplit1 = playerHand[0].value;
		int playerSplit2 = playerSplit1;

		displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
		displayPlayerHand(playerHand, playerTotal, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);

																														// Check for pair to split !!!!!!!!!!!!!!!!!!!!!!!

		string splitInput;

		if (playerHand[0].name.at(0) == playerHand[1].name.at(0)) {
			split = true;
			playerSplitHand1.push_back(playerHand[0]);
			playerSplitHand2.push_back(playerHand[1]);
		}

		if (split) {
			cout << "Would you like to split? (y/n) -> ";
			cin >> splitInput;
			cout << endl;
			while (splitInput != "y" && splitInput != "n") {
				cout << "Invalid input try again ->";
				cin >> splitInput;
				cout << endl;
			}
			if (splitInput == "y" && bankroll < bet * 2) {
				cout << "You don't have enough money to split\n";
				split = false;
			}

			if (splitInput == "y") {
				playerTotal = playerSplit1;
				hitSplit = 1;
			}
			else
				split = false;

			displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
			displayPlayerHand(playerHand, playerTotal, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
																								// Possible add to display the additional bet being placed
		}

		
		while (playerTotal < 21 && hit == "h") {	
			basicStrategy(playerTotal, dealerInitTotal);
								// Ask player for hit or stay loop until hit = s	
			cout << "Hit, Stay, or Double Down? (h/s/dd) -> ";
			cin >> hit;
			cout << endl;
			while (bet * 2 > bankroll && hit == "dd") {					// Check if player has enough money to double down
				cout << "You don't have enough money to double down\n";
				cout << "Hit or Stay? (h/s) -> ";
				cin >> hit;
			}
			while (hit != "h" && hit != "s" && hit != "dd") {			// Throw error if input is invalid
				cout << "Invalid input try again -> ";
				cin >> hit;
				cout << endl;
			}

			if (hit == "h" || hit == "dd")	// ---------- ONLY ENTER IF PLAYER WANTS HIT (h or dd) -----------------------------------
			{
				if (hit == "dd") {
					if (bet * 2 > bankroll) {
						cout << "You don't have enough money to double down\n";
						hit = "s";
					}
					else {
						bet = bet * 2;
						hit = "s";
					}
				}

				if (split) {
					if (hitSplit == 1) {
						dealCard(deck, deckIndex, playerSplitHand1);
						aceCheck(playerSplitHand1, playerAces);
						playerSplit1 = handValue(playerSplitHand1, playerAces, playerSplit1);
						playerTotal = playerSplit1;
					}
					else if (hitSplit == 2) {
						dealCard(deck, deckIndex, playerSplitHand2);
						aceCheck(playerSplitHand2, playerAces);
						playerSplit2 = handValue(playerSplitHand2, playerAces, playerSplit2);		// could run into error where unused A in one hand affects total of the other
						playerTotal = playerSplit2;
					}
				} else {
					dealCard(deck, deckIndex, playerHand);
					aceCheck(playerHand, playerAces);
					playerTotal = handValue(playerHand, playerAces, playerTotal);
				}

				if (playerTotal < 21 && !split && hit != "s") {
					displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
					displayPlayerHand(playerHand, playerTotal, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
				}
			} // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ END HIT AND DOUBLE DOWN ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (split) {
				if (hitSplit == 1) {
					playerSplit1 = playerTotal;
					if (playerSplit1 >= 21 || hit == "s") {
						playerTotal = playerSplit2;
						hitSplit = 2;
						hit = "h";
					}
				}
				else if (hitSplit == 2) {
					playerSplit2 = playerTotal;
					if (playerSplit2 >= 21 || hit == "s") {
						hit = "s";
						hitSplit = 0;
					}
				}	
				if (hitSplit != 0) {
					displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
					displayPlayerHand(playerHand, playerTotal, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
				}
			}
		}

		// Deal cards to Dealer, stand on 17
		while (dealerTotal < 17 && playerTotal <= 21) {
			dealCard(deck, deckIndex, dealerHand);
			aceCheck(dealerHand, dealerAces);
			dealerTotal = handValue(dealerHand, dealerAces, dealerTotal);

		}
		if (playerTotal != 21 || playerHand.size() > 2) {
			displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
			displayPlayerHand(playerHand, playerTotal, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
		}

		// see if player beat the dealer
		if (split) {
			cout << "Split Pair 1: ";
			if (playerSplit1 == dealerTotal)
				cout << "Push\n";
			else if (dealerTotal > playerSplit1 && dealerTotal <= 21) {
				cout << "Less than dealer. Lost ($" << bet << ")\n";
				bankroll = bankroll - bet;
			}
			else if (playerSplit1 > 21) {
				cout << "Bust. You lost ($" << bet << ")\n";
				bankroll = bankroll - bet;
			}
			else {
				cout << "Won $" << bet << "\n";
				bankroll = bankroll + bet;
			}
			cout << "\nSplit Pair 2: ";
			if (playerSplit2 == dealerTotal)
				cout << "Push\n";
			else if (dealerTotal > playerSplit2 && dealerTotal <= 21) {
				cout << "Less than dealer. Lost ($" << bet << ")\n";
				bankroll = bankroll - bet;
			}
			else if (playerSplit2 > 21) {
				cout << "Bust. You lost ($" << bet << ")\n";
				bankroll = bankroll - bet;
			}
			else {
				cout << "Won $" << bet << "\n";
				bankroll = bankroll + bet;
			}
		}
		else {
			if (dealerTotal == playerTotal) {
				delay();
				cout << "Push \n";
			}
			else if (dealerTotal > playerTotal && dealerTotal <= 21) {
				delay();
				cout << "Dealer Wins. You lost ($" << bet << ")\n";
				bankroll = bankroll - bet;
			}
			else if (playerTotal > 21) {
				delay();
				cout << "Bust. Bummer you lose your bet ($" << bet << ")\n";
				bankroll = bankroll - bet;
			}
			else if (playerTotal == 21 && playerHand.size() == 2) {
				delay();
				cout << "B"; delay(); cout << "L"; delay(); cout << "A"; delay(); cout << "C"; delay(); cout << "K"; delay();
				cout << "J"; delay(); cout << "A"; delay(); cout << "C"; delay(); cout << "K"; delay(); cout << "!"; delay();
				cout << "\nYou win double your bet $" << bet * 2 << "\n";
				bankroll = bankroll + (bet * 2);
			}
			else
			{
				delay();
				cout << "Players Wins! $" << bet << "\n";
				bankroll = bankroll + bet;
			}
		}

		cout << endl; system("pause"); cout << endl;
	} // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ END ROUND WHILE LOOP ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	if (bet == 0)
		cout << "Player quit\n";
	else 
		cout << "You ran out of money. Back to work for you\n";

	return 0;
}

vector<Card> initializeDeck(void) {
	vector<string> faces = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
	vector<string> suits = {"S","H","D","C"};
	vector<Card> deck = {};

	for (string face : faces) {
		for (string suit : suits) {
			Card card;	// Create a card of struct type Card
			card.name = face + suit;	// Combine face and suit, string concatenation

			if (face == "J" || face == "Q" || face == "K")
				card.value = 10;
			else if (face == "A")
				card.value = 11;
			else
				card.value = stoi(face);	// if face is number use string to integer to get value

			for (int i = 0; i < 5; i++)		// 5 = number of decks
				deck.push_back(card);	// Place each face/suit combo (card) at end of deck vector
		}
	}
	shuffle(deck.begin(), deck.end(), default_random_engine(time(nullptr)));
	return deck;
}

void dealCard(vector<Card>& deck, int& deckIndex, vector<Card>& hand) {
	hand.push_back(deck[deckIndex++]);
	if (deckIndex == 52*5) {	// 52 * number of decks
		deck = initializeDeck();
		deckIndex = 0;
	}
}

int handValue(vector<Card> hand, int &aces, int total) {
	total = 0;
	for (int i = 0; i < hand.size(); i++) {
		total += hand[i].value;
		if (total > 21 && aces > 0) {
				total -= 10;
				aces--;
		}
	}
	return total;
}

void aceCheck(vector<Card> hand, int& aces) {
	for (int i = 0; i < hand.size(); i++) {
		if (hand[i].value == 11)
			aces++;
		
	}
}

void getRunningCount(vector<Card> deck, int deckIndex) {
	int runningCount = 0;
	int trueCount = 0;
	int numOfDecksRemaining = 5;
	for (int i = 0; i < deckIndex; i++) {
		int temp = deck[i].value;
		if (temp > 1 && temp < 7)
			runningCount++;
		else if (temp > 9)
			runningCount--;
	}
	if (deckIndex > 52 && deckIndex < 52*2)
		numOfDecksRemaining = 4;
	else if (deckIndex > 52*2 && deckIndex < 52*3)
		numOfDecksRemaining = 3;
	else if (deckIndex > 52*3 && deckIndex < 52*4)
		numOfDecksRemaining = 2;
	else if (deckIndex > 52*4 && deckIndex < 52*5)
		numOfDecksRemaining = 1;

	if (runningCount > numOfDecksRemaining || runningCount < numOfDecksRemaining * -1)
		trueCount = round(runningCount/numOfDecksRemaining);
	else
		trueCount = runningCount;

	cout << "Running Count: " << runningCount << endl;
	cout << "True Count: " << trueCount << endl;

}

void displayPlayerHand(vector<Card> playerHand, int playerTotal, bool split, int hitSplit, int playerSplit1, int playerSplit2, 
	vector<Card> playerSplitHand1, vector<Card> playerSplitHand2) {
	if (split) 
	{
		cout << "Player Split 1: ";
		
		for (int i = 0; i < playerSplitHand1.size(); i++) {
			printCard(playerSplitHand1[i]);
			cout << " ";
		}
		if (hitSplit == 1)
			cout << " <-";
		cout << "\nTotal: " << playerSplit1 << "\n\n";

		cout << "Player Split 2: ";
		for (int i = 0; i < playerSplitHand2.size(); i++) {
			printCard(playerSplitHand2[i]);
			cout << " ";
		}
		if (hitSplit == 2)
			cout << " <-";
		cout << "\nTotal: " << playerSplit2 << "\n";
		cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^\n";

	} else {
		 cout << "Player: ";
		 for (int i = 0; i < playerHand.size(); i++)
		 {
			 printCard(playerHand[i]);
			 cout << " ";
		 }
	 cout << endl;
	 printf("Total: %i \n", playerTotal);
	 cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
	}
}

void displayDealerHand(vector<Card> dealerHand, int dealerTotal, int dealerInitTotal, int playerTotal, string hit) {

	cout << "-------------------------\n";
	cout << "Dealer: ";
	if (dealerHand.size() > 2 || playerTotal >= 21 || hit == "s") {
		for (int i = 0; i < dealerHand.size(); i++) {
			printCard(dealerHand[i]);
			cout << " ";
		}
		cout << "\nTotal: " << dealerTotal << endl;
	}
	else {
		cout << "?? ";
		printCard(dealerHand[1]);
		cout << "\nTotal: " << dealerInitTotal << endl;
	}
	cout << endl;
}

void delay(void) {
	this_thread::sleep_for(chrono::milliseconds(200));
}



void displayBankroll(int bankroll) {
	cout << "Bankroll: ";
	for (int i = 0; i < bankroll; i++)
		cout << "$";
	cout << " (" << bankroll << ")" << endl;
}

void printCard(Card card) {
    int lastChar = card.name.size() - 1;
    char temp = card.name.at(lastChar);
    SUIT suitToSelect = spade;

    card.name.erase(lastChar);
    cout << card.name;

    if (temp == 'S')
        suitToSelect = spade;
	else if (temp == 'H')
		suitToSelect = heart;
	else if (temp == 'D')
		suitToSelect = diamond;
	else
		suitToSelect = club;


	_setmode(_fileno(stdout), _O_U16TEXT);
	switch (suitToSelect) {
	case spade:
		wcout << SPADE;
		break;
	case heart:
		wcout << HEART;
		break;
	case diamond:
		wcout << DIAMOND;
		break;
	case club:
		wcout << CLUB;
		break;
	}
	_setmode(_fileno(stdout), _O_TEXT);
}

void basicStrategy(int playerTotal, int dealerInitTotal) {
	if (playerTotal > 1 && playerTotal < 9)
		cout << "Perfect Basic Strategy: HIT\n";
	else if (playerTotal == 9) {
		if (dealerInitTotal < 3 || dealerInitTotal > 6)
			cout << "Perfect Basic Strategy: HIT\n";
		else
			cout << "Perfect Basic Strategy: DOUBLE DOWN\n";
	}
	else if (playerTotal == 10) {
		if (dealerInitTotal > 9)
			cout << "Perfect Basic Strategy: HIT\n";
		else
			cout << "Perfect Basic Strategy: DOUBLE DOWN\n";
	}
	else if (playerTotal == 11) {
		if (dealerInitTotal < 10)
			cout << "Perfect Basic Strategy: HIT\n";
		else
			cout << "Perfect Basic Strategy: DOUBLE DOWN\n";
	}
	else if (playerTotal == 12) {
		if (dealerInitTotal < 4 || dealerInitTotal > 6)
			cout << "Perfect Basic Strategy: HIT\n";
		else
			cout << "Perfect Basic Strategy: STAY\n";
	}
	else if (playerTotal > 12 && playerTotal < 17) {
		if (dealerInitTotal > 6)
			cout << "Perfect Basic Strategy: HIT\n";
		else
			cout << "Perfect Basic Strategy: STAY\n";
	}
	else 
		cout << "Perfect Basic Strategy: STAY\n";
	
	
}