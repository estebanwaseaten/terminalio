// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #include <terminalio/buffer.h>
#include "debug.h"

using namespace std;

buffer::buffer()
{
  DEBUG_MSG( "buffer::buffer()" );
  charBuffer = nullptr;
  colorBuffer = nullptr;
}

buffer::buffer( int newWidth, int newHeight )
{
  DEBUG_MSG( "buffer::buffer( int newWidth, int newHeight )" );
  charBuffer = nullptr;
  colorBuffer = nullptr;
  resize(newWidth, newHeight);
  setAll( ' ' );
}

buffer::~buffer()
{
  if( charBuffer != nullptr )
    delete charBuffer;

  if( colorBuffer != nullptr )
    delete colorBuffer;
}


char buffer::getChar( int x, int y )
{
    return charBuffer[ y* pixelWidth + x];
}

void buffer::setChar( int x, int y, char c )
{
    charBuffer[y * pixelWidth + x] = c;
}

void buffer::makeString( string *screenString )
{
  //*screenString = "";

  uint8_t recentModifier = 37;
  uint8_t currentModifier;

  //*screenString += "\x1b[37";

  for( int y = 0; y < pixelHeight; y++ )
  {
    for( int x = 0; x < pixelWidth; x++ )
    {
      currentModifier = 30 + colorBuffer[y*pixelWidth + x];

      if (currentModifier != recentModifier)
      {
        //*screenString += "\033[";
        //*screenString += to_string( currentModifier );
        //*screenString += "m";
        recentModifier = currentModifier;
      }
      *screenString += charBuffer[y*pixelWidth + x];
    }
    *screenString += "\n";
  }
}

void buffer::randomize()
{
  for( int i = 0; i < totalBufferSize; i++ )
  {
    if (rand()%4 > 1)
      charBuffer[i] = '#';
    else
      charBuffer[i] = '#';

    colorBuffer[i] = (uint8_t)rand()%8;
  }
}

void buffer::setAll( char c )
{
  for( int i = 0; i < totalBufferSize; i++ )
  {
    charBuffer[i] = c;
  }
}

void buffer::clear()
{

}


int buffer::resize( int newWidth, int newHeight )
{
  int newLength = newWidth * newHeight;
  //int minWidth = min( newWidth, pixelWidth );     //padding could be negative --> cant ignore it
  //int minHeight = min( newHeight, pixelHeight );

  char      *newCharBuffer = new char[newLength];
  uint8_t   *newColorBuffer = new uint8_t[newLength];

  for( int i = 0; i < newLength; i++)
  {
      newCharBuffer[i] = ' ';
      newColorBuffer[i] = 7;
  }

  pixelWidth = newWidth;
  pixelHeight = newHeight;
  totalBufferSize = newLength;

  char      *oldBuffer = charBuffer;
  uint8_t   *oldColorBuffer = colorBuffer;

  if( oldBuffer != nullptr )
  {
    //try to convert... or not
  }

  charBuffer = newCharBuffer;
  colorBuffer = newColorBuffer;

  if( oldBuffer != nullptr)
  {
      delete oldBuffer;
  }

  if( oldColorBuffer != nullptr )
  {
      delete oldColorBuffer;
  }

  return 0;
}
