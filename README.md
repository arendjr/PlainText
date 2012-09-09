PlainText
=========

PlainText is a text-based MUD game engine.

Technology
----------

 * The engine is written in C++, using the Qt toolkit.
 * A JavaScript engine is embedded for providing scripting to game
   objects.
 * The game is accessible over telnet (with support for ANSI colors and the MCCP
   and MSSP extension protocols), but also hosts a web interface over HTTP which
   connects to the game using WebSockets.

Running your own server
-----------------------

### Supported platforms ###

 * Linux (tested on Gentoo Linux and openSUSE 12.2)
 * Mac OS X 10.7 and up (recently only tested on OS X 10.8)

### Dependencies ###

 * Clang 3.1 or higher, or GCC 4.7 or higher
 * Qt framework 4.7 or higher
 * Zlib development package

### Compiling ###

Run the following commands from the src/ directory:

 * qmake PlainText.pro
 * make

### Running ###

 * Set the PT_DATA_DIR environment variable to point to the data/ directory.
 * If you want to enable logging, set the PT_LOG_DIR variable to the directory
   where you want your logs to be stored.
 * Run your compiled PlainText executable.

Playing the game
----------------

**Note**: If you want to play the game using your webbrowser, you should have a
modern standards compliant browser. These are the browsers that *should* work:
 * Chrome
 * Firefox 4 or higher
 * Internet Explorer 10
 * Safari

### Connect to your own server ###

If you have just compiled your own server, you can connect to it through telnet
or using your webbrowser. The ports to connect to are displayed on the console
when you start your server.

The first character you register on your own server will automatically become an
admin.

### Connect to the demo server ###

If you do not have your own server, feel free to connect to the demo server:

 * Point your webbrowser to http://mud.yunocloud.com:8080/ *or*
 * Connect with telnet to mud.yunocloud.com on port 4801.

License
-------

This software is licensed under GPLv3.

For details, see LICENSE.GPL.txt.
