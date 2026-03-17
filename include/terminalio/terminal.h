// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #ifndef terminal_h
#define terminal_h


#include <termios.h>	//terminal stuff (linux)
#include <iostream>
#include <vector>
#include <sys/ioctl.h>	//to get the terminal size
#include <csignal>
#include <terminalio/input.h>
#include <terminalio/console.h>

using namespace std;

class terminal
{
public:
	terminal();
	~terminal();

	void addToInputBuffer( char c );					//could put the input Buffer functionality into a parent class?

	bool didResizeWithReset( int *w, int *h );			//for manual checks of window resize action
	void getWindowSize( int *w, int *h );
	void windowResize( int signal );

	console *createMainConsole();
	input 	*createMainInput();

private:

	struct termios old_tio, new_tio;

	inline static int 	termWidth;				//inline --> no need to declare this "global" variable in the .cpp file
	inline static int 	termHeight;
	inline static bool	sizeChange;

	console 	*mainConsole;
	input 		*mainInput;

	string inputBuffer;
	vector<string> inputHistory;

	function<void( int, int )> winResizeCallback;
	void setWinResizeCallback( function<void(int, int)> f )		//for automatic checks
	{ winResizeCallback = f; }

};






#endif
