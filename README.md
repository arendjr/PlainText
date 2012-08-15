PlainText
=========

PlainText is a text-based MUD game engine.

Technology
----------

 * The engine is written in C++, using the Qt toolkit.
 * A JavaScript engine is embedded for providing scripting to game
   objects.
 * The game is accessible over telnet, but also hosts a web interface
   over HTTP which connects to the game using WebSockets.

Running your own server
-----------------------

### Supported platforms ###

 * Linux (tested on Gentoo Linux)
 * Mac OS X 10.7 and up (recently only tested on OS X 10.8)

### Dependencies ###

 * Clang compiler version 3.1 or higher*
 * Qt framework 4.7 or higher

### Compiling ###

Run the following commands from the src/ directory:

 * qmake PlainText.pro
 * make

### Running ###

 * Set the PT_DATA_DIR environment variable to point to the data/ directory.
 * Run your compiled PlainText executable.

\* *Due to usage of new C++11 features. You might get lucky with GCC 4.7 too,
    but you will need to update the PlainText.pro file yourself.*

License
-------

This software is licensed under GPLv3.

For details, see LICENSE.GPL.txt.
