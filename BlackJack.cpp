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

#include <fcntl.h>
#include <io.h>

#include "BlackJack.h"

#define SPADE L"\u2660"
#define HEART L"\u2665"
#define DIAMOND L"\u2666"
#define CLUB L"\u2663"

enum SUIT {spade = 1, heart, diamond, club};

using namespace std;

// Functions
int DEALCARD(vector<int>& check);
void displayPlayerHand(vector<string> playerHand, int playerTotal, vector<int> discard, bool split, int hitSplit, int playerSplit1, int playerSplit2);
void displayPlayerHand(vector<string> playerHand, int playerTotal, vector<int> discard, bool split, int hitSplit, int playerSplit1, int playerSplit2, 
	vector<string> playerSplitHand1, vector<string> playerSplitHand2);
void displayDealerHand(vector<string> dealerHand, int dealerTotal, int dealerInitTotal, int playerTotal, string hit);
void delay(void);
void aceCheck(int x, int& y);
void displayBankroll(int bankroll);
void displaySuit(char temp);
void getRunningCount(vector<int> discard, int cardValues[]);

void printSuit(int suitToSelect);

// Known issues:
// Double down on one split pair will change the bet for both pairs - probably need to new variables to keep track of bets on each pair respectively



int main()
{
	string cardNames[52]
	{
		"Ad", "Ac", "Ah", "As","2d", "2c", "2h", "2s", "3d", "3c", "3h", "3s",
		"4d", "4c", "4h", "4s", "5d", "5c", "5h", "5s", "6d", "6c", "6h", "6s",
		"7d", "7c", "7h", "7s", "8d", "8c", "8h", "8s", "9d", "9c", "9h", "9s",
		"10d", "10c", "10h", "10s", "Jd", "Jc", "Jh", "Js", "Qd", "Qc", "Qh", "Qs",
		"Kd", "Kc", "Kh", "Ks",
	};
	int cardValues[52]
	{
		11, 11, 11, 11, 2, 2, 2, 2, 3, 3, 3, 3,
		4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6,
		7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,
		10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10,
	};

	srand(time(0));
	int x = 0;
	int bankroll = 5;
	int round = 0;
	vector<int> discard = {};
	int runningCount = 0;

	// ---------------------------- START ROUND LOOP ------------------------------------------------------
	while (bankroll != 0)
	{
		round++;
		cout << "        ROUND " << round << endl;
		displayBankroll(bankroll);
		getRunningCount(discard, cardValues);
		vector<string> playerHand = {};
		vector<string> playerSplitHand1 = {};
		vector<string> playerSplitHand2 = {};
		vector<string> dealerHand = {};
		string hit = "h";
		int dealerTotal = 0;
		int dealerAces = 0;
		int playerTotal = 0;
		int playerSplit1 = 0;
		int playerAces = 0;
		bool split = false;
		int hitSplit = 0;

		int bet = 0;
		do {
			cin.clear();
			cin.ignore(256, '\n');
			cout << "Place your bet: $";
			cin >> bet;
			if (bet > bankroll)
				cout << "You Don't have that much money to bet\n";
			if (bet == 0 && !cin.fail())
				cout << "Ah sitting out are we? Try Again\n";
			if (cin.fail())
				cout << "Invalid Input\n";
		} while (bet > bankroll || bet == 0);
		
		//------------------------------------------------------------
		for (int i = 0; i < 2; i++) {								//
			x = DEALCARD(discard);									//
			discard.push_back(x);									// Deal initial hands
			playerHand.push_back(cardNames[x]);
			aceCheck(x, playerAces);								// player > dealer > player > dealer
			playerTotal = playerTotal + cardValues[x];	
			playerSplit1 = cardValues[x];							//
																	//
			x = DEALCARD(discard);									//
			discard.push_back(x);									//
			dealerHand.push_back(cardNames[x]);	
			aceCheck(x, dealerAces);								//
			dealerTotal = dealerTotal + cardValues[x];				//
		}															//
		//------------------------------------------------------------
		int dealerInitTotal = cardValues[x];
		int playerSplit2 = playerSplit1;

		displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
		displayPlayerHand(playerHand, playerTotal, discard, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);

																														// Check for pair to split !!!!!!!!!!!!!!!!!!!!!!!

		string splitInput;

		if (playerHand[0].at(0) == playerHand[1].at(0)) {
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
			displayPlayerHand(playerHand, playerTotal, discard, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
																								// Possible add to display the additional bet being placed
		}

		
		while (playerTotal < 21 && hit == "h") {						// Ask player for hit or stay loop until hit = s	
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
				x = DEALCARD(discard);
				discard.push_back(x);
				playerHand.push_back(cardNames[x]);
				aceCheck(x, playerAces);
				playerTotal = playerTotal + cardValues[x];

				if (split) {
					if (hitSplit == 1)
						playerSplitHand1.push_back(cardNames[x]);
					else if (hitSplit == 2)
						playerSplitHand2.push_back(cardNames[x]);
				}

				if (playerTotal > 21 && playerAces > 0) {
					playerTotal = playerTotal - 10;
					playerAces--;
				}
				if (playerTotal < 21 && !split && hit != "s") {
					displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
					displayPlayerHand(playerHand, playerTotal, discard, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
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
					displayPlayerHand(playerHand, playerTotal, discard, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
				}
			}
		}
	

				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ END HIT STAY OR DOUBLE DOWN LOOP ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

																						// Deal cards to Dealer, stand on 17
		while (dealerTotal < 17 && playerTotal <= 21) {
			x = DEALCARD(discard);
			discard.push_back(x);
			dealerHand.push_back(cardNames[x]);
			dealerTotal = dealerTotal + cardValues[x];

			if (dealerTotal > 21 && dealerAces > 0) {
				dealerTotal = dealerTotal - 10;
				dealerAces = dealerAces - 1;
			}

		}
		if (playerTotal != 21 || playerHand.size() > 2) {
			displayDealerHand(dealerHand, dealerTotal, dealerInitTotal, playerTotal, hit);
			displayPlayerHand(playerHand, playerTotal, discard, split, hitSplit, playerSplit1, playerSplit2, playerSplitHand1, playerSplitHand2);
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

	cout << "You ran out of money. Back to work for you\n";

	return 0;
}

int DEALCARD(vector<int>& check) {
	int x = rand() % 51;
	if (check.size() > 0 && check.size() < 52) {
		for (int i = 0; i <= check.size() - 1; i++)
		{
			if (x == check[i]) {
				x = rand() % 51;
				i = 0;
			}
		}
		return x;
	}
	else if (check.size() == 52) {
		check = {};
		return x;
	}
	else
		return x;
}
void getRunningCount(vector<int> discard, int cardValues[]) {
	int runningCount = 0;
	for (int i = 0; i < discard.size(); i++) {
		int temp = cardValues[discard[i]];
		if (temp > 1 && temp < 7)
			runningCount++;
		else if (temp > 9)
			runningCount--;
	}
	cout << "Running Count: " << runningCount << endl;

}

void printSuit(int suitToSelect) {
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

void displayPlayerHand(vector<string> playerHand, int playerTotal, vector<int> discard, bool split, int hitSplit, int playerSplit1, int playerSplit2, 
	vector<string> playerSplitHand1, vector<string> playerSplitHand2) {
	int lastChar = 0;
	char temp = 'q';
	if (split) 
	{
		int lastChar = 0;
		char temp = 'q';
		cout << "Player Split 1: ";
		
		for (int i = 0; i < playerSplitHand1.size(); i++) {
			lastChar = playerSplitHand1[i].size() - 1;
			temp = playerSplitHand1[i].at(lastChar);
			cout << playerSplitHand1[i].erase(lastChar);
			displaySuit(temp);
			cout << " ";
		}
		if (hitSplit == 1)
			cout << " <-";
		cout << "\nTotal: " << playerSplit1 << "\n\n";

		cout << "Player Split 2: ";
		for (int i = 0; i < playerSplitHand2.size(); i++) {
			lastChar = playerSplitHand2[i].size() - 1;
			temp = playerSplitHand2[i].at(lastChar);
			cout << playerSplitHand2[i].erase(playerSplitHand2[i].size() - 1);
			displaySuit(temp);
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
			 lastChar = playerHand[i].size() - 1;
			 temp = playerHand[i].at(lastChar);
			 cout << playerHand[i].erase(playerHand[i].size() - 1);
			 displaySuit(temp);
			 cout << " ";
		 }
	 cout << endl;
	 printf("Total: %i \n", playerTotal);
	 cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
	}
}

void displayDealerHand(vector<string> dealerHand, int dealerTotal, int dealerInitTotal, int playerTotal, string hit) {
	int lastChar = 0;
	char temp = 'q';

	cout << "-------------------------\n";
	cout << "Dealer: ";
	if (dealerHand.size() > 2 || playerTotal >= 21 || hit == "s") {
		for (int i = 0; i < dealerHand.size(); i++) {
			lastChar = dealerHand[i].size() - 1;
			temp = dealerHand[i].at(lastChar);
			cout << dealerHand[i].erase(dealerHand[i].size() - 1);
			displaySuit(temp);
			cout << " ";
		}
		cout << "\nTotal: " << dealerTotal << endl;
	}
	else {
		cout << "?? ";
		lastChar = dealerHand[1].size() - 1;
		temp = dealerHand[1].at(lastChar);
		cout << dealerHand[1].erase(dealerHand[1].size() - 1);
		displaySuit(temp);
		cout << "\nTotal: " << dealerInitTotal << endl;
	}
	cout << endl;
}

void delay(void) {
	this_thread::sleep_for(chrono::milliseconds(200));
}

void aceCheck(int x, int& y) {
	if (x < 4)
		y++;
}

void displayBankroll(int bankroll) {
	cout << "Bankroll: ";
	for (int i = 0; i < bankroll; i++)
		cout << "$";
	cout << " (" << bankroll << ")" << endl;
}

void displaySuit(char temp) {
	if (temp == 's')
		printSuit(spade);
	else if (temp == 'h')
		printSuit(heart);
	else if (temp == 'd')
		printSuit(diamond);
	else
		printSuit(club);
}