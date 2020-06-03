#ifndef GAMESESSION_H
#define GAMESESSION_H

#include "WordLibrary.h"

#include <iostream> 
#include <string>

//This class manages a game of guessing word. A game lasts as long as the player wants
//to continue playing and keeps asking the user to guess words according to some
//clues.
class GameSession {
  private:  
    WordLibrary *wordBank;
    string currentWord;
    string currentClue;
    int score; // number of correct guesses since the beginning of the session
    int currentStreak; // current streak of correct guesses by the player
	int bestStreak; // greatest streak of correct guesses in the current game session
	int status; //1 if the game is ongoing, 0 if the client decides to quit

    void selectWord();
    bool isCommand(const string& );
	string handleCommand(const string& );
	
	string promptWord();
	string getHint();
	string displayScore();
	void updateBestStreak();
	string checkGuess(const string&);
	string displayCommands();

  public:
    GameSession(vector<string>*);
	string startSession() ;
	string handleUserInput(const string& );
	bool getStatus();
	void setStatus(int s);
	int getScore();
	int getBestStreak();
	
	static bool isAMatch(const string& , const string& );
	
};

#endif //GAMESESSION_H