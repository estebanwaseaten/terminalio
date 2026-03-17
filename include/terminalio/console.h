// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #ifndef console_h
#define console_h

#include <terminalio/buffer.h>
#include <string>
#include <iostream>
#include <cstring> //memset
#include <cstdio>
#include <cmath>



using namespace std;

/********** display
//
// manages all the buffers... a vector of them?
//
//
*/


class console       //display is also a buffer! --> in display we need to implement additionally the functions to draw the buffer to the console.
{
public:
    console();
    console( int w, int h );
    ~console();

    void clearScreen();
    void setPixel( int x, int y, char c );
    void setCursorXY( int x, int y );

    void frame();
    void test();


    //print screen buffer
    void drawScreenBuffer();
    void resize( int w, int h );

private:
    buffer *mainBuffer;
    buffer *drawnBuffer;

    int width;
    int height;

    //string screenString;
    //sstring blankString;




};



#endif
