// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 *
 * Copyright (C) 2025 Daniel Wegkamp
 */

#ifndef input_h
#define input_h

#include <functional>
#include <threader/threader.h>

#define MOUSE_MOD_NONE 0
#define MOUSE_MOD_RIGHT 2
#define MOUSE_MOD_SHIFT 4
#define MOUSE_MOD_ALT 8
#define MOUSE_MOD_CTRL 16
#define MOUSE_MOD_SCROLL_DOWN 64
#define MOUSE_MOD_SCROLL_UP 65

#define INPUT_MAX_BUFFER 64

#define INPUT_ARROW_LEFT 0x4400
#define INPUT_ARROW_RIGHT 0x4300
#define INPUT_ARROW_UP 0x4100
#define INPUT_ARROW_DOWN 0x4200

#define INPUT_MOD_NONE 0x5B
#define INPUT_MOD_SHIFT 0x32
#define INPUT_MOD_ALT 0x33
#define INPUT_MOD_CTRL 0x35         //does not seem to work on the mac

//combinations (not sure if there is a logic behin this)
#define INPUT_MOD_SHIFTCTRL 0x35
#define INPUT_MOD_SHIFTALT 0x35
#define INPUT_MOD_CTRLALT 0x35
#define INPUT_MOD_SHIFTCTRLALT 0x35



using namespace std;

class input : public threader
{
public:
    input();
    ~input();

    void setKeyboardInputSimpleCallback( function<void(char*, int)> f )
    { keyboardInputSimpleCallback = f; }
    void setKeyboardInputSpecialCallback( function<void(char*, int)> f )
    { keyboardInputSpecialCallback = f; }
    void setMouseDownCallback( function<void(int, int, char)> f )
    { mouseDownCallback = f; }
    void setMouseUpCallback( function<void(int, int, char)> f )
    { mouseUpCallback = f; }
    void setMouseScrollCallback( function<void(int, int, char)> f )
    { mouseScrollCallback = f; }
    void setActiveCallback( function<bool()> f )
    { activeCallback = f; }

    void start( int myid ); //override?
    void setVerbose( bool v ){ verbose = v; };

private:
    void looped() override;

    void keyboardInput( char *buffer, int length );
    void mouseInput( char *buffer, int length );

    function<void(char*, int)> keyboardInputSimpleCallback;
    function<void(char*, int)> keyboardInputSpecialCallback;
    function<void(int, int, char )> mouseDownCallback;
    function<void(int, int, char )> mouseUpCallback;
    function<void(int, int, char )> mouseScrollCallback;
    function<bool()> activeCallback;

    bool active;
    bool verbose;

    char lastInput;
};


#endif
