// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include <terminalio/terminal.h>
#include <iostream>
#include <chrono>

#include "debug.h"

//mouse event escape sequences: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
//https://stackoverflow.com/questions/76805544/is-there-any-documentation-for-the-mouse-scroll-event-escape-sequences
//#define ENABLE_MOUSE_TRACKING "\x1b[?1000h"
#define ENABLE_MOUSE_TRACKING "\x1b[?1000h"		//1000-->down, up, scroll; (1002 adds drag and 1003 adds move)
#define ENABLE_MODE_SGR "\x1b[?1006h]"
#define ENABLE_MODE_UTF8 "\x1b[?1005h]"

#define DISABLE_MOUSE_TRACKING "\x1b[?1000l"
#define DISABLE_MODE_SGR "\x1b[?1006l]"
#define DISABLE_MODE_UTF8 "\x1b[?1005l]"
//#define DISABLE_MOUSE_TRACKING "\x1b[?1006l\x1b[?1015l\x1b[?1002l\x1b[?1000l"

#define ENABLE_MOUSE_TRACKING_SZ 8
#define DISABLE_MOUSE_TRACKING_SZ ENABLE_MOUSE_TRACKING_SZ


//\0nnn The eight-bit character whose value is the octal (base-8) value nnn where nnn is 0 to 3 digits
//\xHH The eight-bit character whose value is the hexadecimal (base-16) value HH (one or two digits)
// \x1b = \033 = 27
// "\033[?1000h" # Send Mouse X & Y on button press and release, down, up, scroll
// "\033[?1002h" ... down, up, scroll, drag
// "\033[?1003h" ... down, up, scroll, drag, move
// "\033[?1005h" # Enable UTF-8 Mouse Mode	-->default
// "\033[?1006h" # Enable SGR Mouse Mode, xterm. i.e. --> maybe better than UTF?

terminal *recentTerminal = NULL;

void signal_resize_wrapper( int signal )
{
	if (recentTerminal != NULL)
	{
		recentTerminal->windowResize( signal );
	}
}

terminal::terminal()
{
	DEBUG_MSG("terminal::terminal()");

	//set terminal properties:
	tcgetattr( 0, &old_tio);
	new_tio = old_tio;			 // ~ means NOT
	new_tio.c_lflag &= ~ICANON;  // NOLINT Non canonique terminal.
    new_tio.c_lflag &= ~ECHO;    // NOLINT Do not print after a key press.
//	new_tio.c_lflag &= ~ISIG; 	 // INTR, QUIT, SUSP or DSUSP characters -> signal is generated
		// The INTR (interrupt) character raises a SIGINT signal	--> ctrl - c works
		// The QUIT character raises a SIGQUIT signal
		// The SUSP (suspend) character
		// The DSUSP (suspend) character
	new_tio.c_iflag &= ~INLCR;	//	INLCR Translate NL to CR on input.
	new_tio.c_iflag &= ~ICRNL;	// ICNRL Translate carriage return to newline on input (unless IGNCR is set).
	new_tio.c_iflag &= ~IXON;	// Enable XON/XOFF flow control on output
	new_tio.c_iflag &= ~IXOFF;	// Enable XON/XOFF flow control on input.
	//newtio.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF);

	new_tio.c_cflag &= ~(PARENB);	//Enable parity generation on output and parity checking foinput.
	new_tio.c_cflag = (new_tio.c_cflag & ~CSIZE) | CS8;

	new_tio.c_cc[VMIN] = 1;
    //new_tio.c_cc[VTIME] = 0;
	tcsetattr( 0, TCSANOW, &new_tio);


	//enable mouse tracking:
	write(STDIN_FILENO, ENABLE_MOUSE_TRACKING, ENABLE_MOUSE_TRACKING_SZ);
	write(STDIN_FILENO, ENABLE_MODE_SGR, ENABLE_MOUSE_TRACKING_SZ);


	//init terminal size variables:
	winsize winSize;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize);
	termWidth = winSize.ws_col;
	termHeight = winSize.ws_row;
	cout << "initial terminal size: width=" << termWidth << ", height=" << termHeight << endl;

	//set signal handler for SIGWINCH (size change of terminal):
	signal( SIGWINCH, signal_resize_wrapper );
	recentTerminal = this;
	winResizeCallback = NULL;

	//initialize flags:
	sizeChange = false;
	mainConsole = NULL;
	mainInput = NULL;
}

terminal::~terminal()
{
	DEBUG_MSG("terminal::terminal()");

	//disable mouse tracking:
	write(STDIN_FILENO, DISABLE_MOUSE_TRACKING, DISABLE_MOUSE_TRACKING_SZ);
	write(STDIN_FILENO, DISABLE_MODE_SGR, DISABLE_MOUSE_TRACKING_SZ);

	//reset terminal properties from before
	tcsetattr( 0, TCSANOW, &old_tio);

	if( mainConsole != NULL )
		delete mainConsole;
	if( mainInput != NULL )
		delete mainInput;
}

console *terminal::createMainConsole()		//CREATES AND GETS
{
	if ( mainConsole == NULL)
	{
		mainConsole = new console( termWidth, termHeight );

		using namespace std::placeholders;
		setWinResizeCallback( bind(&console::resize, mainConsole, _1, _2 ) );
	}
	return mainConsole;
}

input *terminal::createMainInput()
{
	if (mainInput == NULL)
	{
		mainInput = new input();
	}
	return mainInput;
}

void terminal::addToInputBuffer( char c )
{
		inputBuffer += c;
}

void terminal::windowResize( int signal )
{
	winsize winSize;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize);
	termHeight = winSize.ws_row;
	termWidth = winSize.ws_col;
	sizeChange = true;

	if ( winResizeCallback != NULL)
	{
		winResizeCallback(termWidth, termHeight);
	}
}

//returns if the terminal has been resized after last check and resets flag afterwards
bool terminal::didResizeWithReset( int *w, int *h )
{
	bool temp = sizeChange;
	sizeChange = false;
	getWindowSize( w, h );

	return temp;
}

void terminal::getWindowSize( int *w, int *h )
{
	if ( w != NULL )
		*w = termWidth;
	if( h != NULL )
		*h = termHeight;
}
