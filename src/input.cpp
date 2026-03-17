// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include "debug.h"

#include <terminalio/input.h>
#include <unistd.h>


input::input()
{
    //subclass threader --> get a name:
    className = "input";

    active = true;

    keyboardInputSimpleCallback = NULL;
    keyboardInputSpecialCallback = NULL;
    mouseDownCallback = NULL;
    mouseUpCallback = NULL;
    mouseScrollCallback = NULL;
}

input::~input()
{

}

//calls callback function if set
void input::keyboardInput( char *buffer, int length )
{
  //DEBUG_MSG("keyboard Input: " << dec << buffer[0]+1);

    //if( buffer[0] == 13 )
    //  DEBUG_MSG("enter key");

    if( buffer[0] == 0x1B )     //27 --> escape char: multi value code follows maaaybe
    {
        if( keyboardInputSpecialCallback != NULL )
            keyboardInputSpecialCallback( buffer, length );
        else
            DEBUG_MSG("keyboardInputSpecialCallback not defined.");
    }
    else
    {
        if( keyboardInputSimpleCallback )
        {
            keyboardInputSimpleCallback( buffer, length );
        }
        else
            DEBUG_MSG("keyboardInputSimpleCallback not defined.");
    }
}

void input::mouseInput( char *buffer, int length )
{
    //1. disentangle mouse input
    int values[3];          //scroll, x and y
    bool newNumber = true;
    int j = 0;

    if( buffer[0] == 0x1B && buffer[1] == 0x5B && buffer[2] == 0x3C )   //mouse event in SGR mode - to be on the safe side
    {
        for( int i = 3; i < length; i++)    //read the 3 decimal numbers in SGR mode: mouse down: -- CSI --  <    0    ;    1    ;    1    M
        {                                                                                     //  0x1B 0x5B 0x3c 0x30 0x3b 0x31 0x3b 0x31 0x4d
            if( newNumber )
            {
                values[j] = atoi(&buffer[i]);
                newNumber = false;
                j++;
            }
            if(buffer[i] == 0x3B)   //0x3B == ';'
            {
                newNumber = true;
            }
        }
    }

    //values[0] is modifier:
    // 0 --> simple click
    // 2 --> right click
    // 4 --> shift click
    // 8 --> alt
    // 16 --> ctrl click
    // 64 --> scroll down
    // 65 --> scroll up

    //UTF8 mode:
    //mouse down: 0x1B 0x5B 0x4D 0x20
    //mouse up: 0x1B 0x5B 0x4D 0x20 y x

    //SGR mode:   -- CSI --  <    0    ;    1    ;    1    M
    //mouse down: 0x1B 0x5B 0x3c 0x30 0x3b 0x31 0x3b 0x31 0x4d
    //mouse up:   0x1B 0x5B 0x3c 0x30 0x3b 0x31 0x3b 0x31 0x6d

    //cout << "scroll: " << values[0] << " x: " << values[1] << " y: " << values[2] << endl;
    if ( values[0] < 64)    //mouse Down or Up (SCROLL IS 64 or 65, there is also the value of two four two finger click)
    {
        if( buffer[length-1] == 'M' )    //mousedown
        {
            if( mouseDownCallback != NULL )
                    mouseDownCallback( values[1], values[2], values[0] );
            else
                DEBUG_MSG("mouseDownCallback not defined.");
        }
        else if( buffer[length-1] == 'm' )
        {
            if( mouseUpCallback != NULL )
                    mouseUpCallback( values[1], values[2], values[0] );
            else
                DEBUG_MSG("mouseUpCallback not defined.");
        }
    }
    else    //scroll
    {
        if( mouseScrollCallback != NULL )
            mouseScrollCallback(  values[1], values[2], values[0] ); //values[0] == 65 --> up, 64 --> down
        else
            DEBUG_MSG("mouseScrollCallback not defined.");
    }
}

void input::start( int myid )
{
  DEBUG_MSG( "input::start()" );

  if (activeCallback !=NULL)
  {
    threader::start( myid );
  }
  else
  {
    DEBUG_MSG( "input::start(): Please define activeCallback first before starting thread (threader::start())");
  }
}


//control sequences:
//https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
void input::looped()        //the input loop waits for/monitors input
{
    //sleep(50);

    char    buffer[INPUT_MAX_BUFFER]; //not sure how long this is at max?
    int     numChars;
    char    singleBuffer;

    int     i;
    bool    stop = false;

    if( active )		//makes sure loop does not get stuck in case myController->keyboardInput( cmd ); returns false
  	{
          // read 9 bytes: min length of a mouse event in SGR mode (see terminal.h)
          // if 2 mouse events are in the buffer, we will only ever fetch the first one:
          numChars = read( STDIN_FILENO, buffer, INPUT_MAX_BUFFER );       //hangs only if there is nothing in the pipe

          if( verbose )
          {
              cout << "raw hex input: " << hex;
              for (size_t i = 0; i < numChars; i++)
              {
                  cout << "0x" << (int)buffer[i] << " ";
              }
              cout << endl;
          }

          if( numChars < 9 )  //9 is the smallest mouse event --> must be keyboard
          {
              keyboardInput( buffer, numChars ); //could still be 3 bytes or so...
          }
          else    //must be mouse!
          {
              if(numChars == 9 && !( buffer[8] == 'M' || buffer[8] == 'm'))  //there might be more bytes in the buffer -- unnecessary
              {
                  i = 9;
                  while( !stop && i < INPUT_MAX_BUFFER )    //at INPUT_MAX_BUFFER we have to stop, otherwise the buffer overruns
                  {
                      numChars = read( STDIN_FILENO, &singleBuffer, 1 );
                      if( numChars == 0 || singleBuffer == 'M' || singleBuffer == 'm' )   //stop at an m/M (end of mouse sequence in SGR mode)
                          stop = true;

                      buffer[i] = singleBuffer; //add new byte to buffer
                      i++;
                  }
                  //the complete sequence should be read!
                  numChars = i;
              }

              mouseInput( buffer, numChars );
          }

          //have to replace this by callback!!!
          if ( activeCallback != NULL)
            active = activeCallback();
          else
            DEBUG_MSG("activeCallback not defined.");
  	}
}
