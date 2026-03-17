// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include "debug.h"

#include <terminalio/console.h>




console::console()
{
	DEBUG_MSG( "console::console()" );
}

console::console( int w, int h )
{
	DEBUG_MSG( "console::console( int w, int h )" );

	width = w;
	height = h-1;						//one less than full terminal height because on windows the prompt still needs place below

	mainBuffer = new buffer( width, height );
	mainBuffer->setAll( '.' );
	//mainBuffer->randomize();

	drawnBuffer = new buffer( width, height );

}

console::~console()
{
	DEBUG_MSG( "console::~console()" );

	if (mainBuffer != nullptr)
	{
		delete mainBuffer;
	}
	if (drawnBuffer != nullptr)
	{
		delete drawnBuffer;
	}
}

void console::clearScreen()
{
		cout << "\033[2J\n" ;
		cout << "\033[3J\n" ;
		cout << "\033[2J\n" ;
		//cout << "screen clear" << endl;
}


/*
	printf ( "\033[25;1H");//move cursor to row 25 col 1
	printf ( "\033[9;1H");//move cursor to row 9 col 1
*/
void console::setCursorXY( int x, int y )		//does not seem to work on windows terminal
{
	string cmd = "\033[";
	cmd += to_string(y);
	cmd += ";";
	cmd += to_string(x);
	cmd += "H";		//H or f
	cout << cmd;
	//cout << "\033[12;15Hxyz";

}

void console::drawScreenBuffer()
{
	char c;



	for(int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			c = mainBuffer->getChar( x, y );

			//cout << c;
			if( c != drawnBuffer->getChar( x, y ) )
			{
				//1. go there with cursor
				setCursorXY( x, y );
				//2. write
				cout << c;
				//3. copy into drawnBuffer
				drawnBuffer->setChar( x, y, c );
			}
		}
		//cout << endl;
	}

	//https://stackoverflow.com/questions/34842526/update-console-without-flickering-c

	//cout << screen;																						//6,57s for 1000, linux: 0.075s
	//fwrite( screen, 1, pixelWidth*pixelHeight + pixelHeight + 1, stdout );		//6.33s for 1000, linux: 0.08s

	//this does not work on windows:
	//	std::cout << "\033c";		//remove residual
	//	std::cout << "\x1B[2J\x1B[H";
	//mainBuffer->randomize();
	//mainBuffer->makeString( &screenString );
	//printf( "%s", screenString );
}

void console::resize( int w, int h )
{
	cout << "resize test" << endl;

	width = w;
	height = h-1;

	mainBuffer->resize(width, height);

	//pixelWidth = w;
	//pixelHeight = h-1;

//	bufferSize = pixelWidth*pixelHeight + pixelHeight + 1;

	//should change the buffer...
}


void console::frame()
{
/*	for( int i = 0; i < pixelWidth; i++ )
	{
		screen[i] = '-';
		screen[(pixelHeight-1)*(pixelWidth+1) + i] = '-';
	}
	for( int i = 0; i < pixelHeight; i++ )
	{
		screen[i*(pixelWidth+1)] = '|';
		screen[i*(pixelWidth+1) + pixelWidth -1] = '|';
	}*/
}

void console::setPixel( int x, int y, char c )
{
/*	if (x < pixelWidth && y < pixelHeight && x >= 0 && y >= 0 )
	{
		screen[ y*(pixelWidth+1) + x] = c;
	}*/
}

void console::test()
{
	mainBuffer->randomize();
}
