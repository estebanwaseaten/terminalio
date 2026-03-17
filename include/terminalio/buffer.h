// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

 #ifndef buffer_h
#define buffer_h

#include <iostream>
#include <algorithm>
#include <random>
#include <cstring>
#include <string>

using namespace std;

class buffer
{
public:
  buffer();
  buffer( int newWidth, int newHeight );
  ~buffer();

  void makeString( string *screenString );    //--> transforms buffer into string

  int resize( int newWidth, int newHeight );
  void randomize();
  void setAll( char c );
  void clear();

  char getChar( int x, int y );
  void setChar( int x, int y, char c );


private:  //onyl really internal stuff

  int   pixelWidth;
  int   pixelHeight;
  int   totalBufferSize;

  char      *charBuffer;
  uint8_t   *colorBuffer;

  //string *pixelString;
};












#endif
