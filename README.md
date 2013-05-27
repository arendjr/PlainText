PlainText
=========

PlainText is a text-based MUD game engine.

 * **[Features](#features)**
 * **[Community](#community)**
 * **[Documentation](#documentation)**
 * **[Running your own server](#running-your-own-server)**
   * [Supported platforms](#supported-platforms)
   * [Dependencies](#dependencies)
   * [Compiling](#compiling)
   * [Running](#running)
 * **[Playing the game](#playing-the-game)**
   * [Connect to your own server](#connect-to-your-own-server)
   * [Connect to the demo server](#connect-to-the-demo-server)
 * **[License](#license)**
 

<a id="features"></a>
Features
--------

 * Support for both telnet (including ANSI colors, MCCP, MSDP and MSSP
   extensions) and HTML5 (WebSockets) 
 * Command-line OLC
 * Graphical HTML5 map editor, with perspective view for designing 3D areas
 * Game events propagate through rooms
   * Hear sounds and see visuals as they happen in nearby rooms
   * Sounds and visuals lose strength and become increasingly blurry over distance
   * Semi-realistic line-of-sight algorithm
 * Dynamic room descriptions
   * Reflects positions and directions for items, exits and characters
   * Complements, not replaces, hand-written descriptions
 * Scripting support using JavaScript
   * Commands may be implemented in JavaScript
   * Customizable combat
   * Scriptable NPCs with triggers
   * Game objects may be extended
 * Logging of sessions (including IPs) and player commands
 * Statistics tracking and visualization in the map editor
 * Basic MUD commands
   * Movement and orientation (go, look, enter, examine, open, close)
   * Conversations (say, talk, tell, gtalk)
   * Buying and handling items (buy, give, drop, get)
   * Container items (put ... into, get ... from)
   * Player groups (follow, lose, disband, gtalk)
   * Basic combat (kill, attack, wield, remove)
 * Designed for minimal latency
   * High-performance C++ core
   * Multi-threaded design
   * All game objects are in memory
   * Changes are synced to disk in separate thread

<a id="community"></a>
Community
---------

Follow updates on the official PlainText blog: http://plaintext-mud.blogspot.com/

If you have questions, or just want to discuss anything related to the PlainText
engine, please join us at one of the following forums:

 * The [PlainText core](https://groups.google.com/forum/?fromgroups#!forum/plaintext-core)
forum is meant for technical discussions regarding use of the PlainText engine.
 * The [PlainText gameplay](https://groups.google.com/forum/?fromgroups#!forum/plaintext-gameplay)
forum is used to discuss gameplay mechanics (including code snippets).
 * The [PlainText tooling](https://groups.google.com/forum/?fromgroups#!forum/plaintext-tooling)
forum is for discussion regarding tooling (IDEs, room building, compilers, etc.).

Bugs can be reported through [GitHub's issue tracker](https://github.com/arendjr/PlainText/issues).

<a id="documentation"></a>
Documentation
-------------

There's documentation available in the
[PlainText Wiki](https://github.com/arendjr/PlainText/wiki).

You may want to jump straight to the
[Frequently Asked Questions](https://github.com/arendjr/PlainText/wiki/Frequently-Asked-Questions).

<a id="running-your-own-server"></a>
Running your own server
-----------------------

<a id="supported-platforms"></a>
### Supported platforms ###

 * Linux (tested on Gentoo Linux, openSUSE 12.2 and Ubuntu 12.10)
 * Mac OS X 10.7 and up (recently only tested on OS X 10.8)
 * Windows (tested on Windows 7)

<a id="dependencies"></a>
### Dependencies ###

 * Clang 3.1 or higher, or GCC 4.7 or higher
 * Qt framework 4.7 or higher, or Qt framework 5.x
 * Zlib development package

If you are using Ubuntu Linux, you can use the following command to install
the necessary dependencies:

    $ sudo apt-get install libqt4-dev libz-dev

If you are using Windows, download the Qt package for Windows that comes bundled
with MinGW.

<a id="compiling"></a>
### Compiling ###

If you're using Linux or OS X, run the following commands from the project
directory:

    $ qmake PlainText.pro
    $ make

If you're on Windows, you need to add the MinGW/bin/ directories to your PATH
environment variable. Assuming you installed Qt 5.0.2 under C:\Qt, these are the
following two directories:

    C:\Qt\Qt5.0.2\5.0.2\mingw47_32\bin
    C:\Qt\Qt5.0.2\Tools\MinGW\bin

After you have properly set your PATH variable, you can run the following
commands from the project directory:

    $ qmake PlainText.pro -r -spec win32-g++
    $ mingw32-make

<a id="running"></a>
### Running ###

 * Set the PT_DATA_DIR environment variable to point to the data/ directory.
 * If you want to enable logging, set the PT_LOG_DIR variable to the directory
   where you want your logs to be stored.
 * Run your compiled PlainText executable from the project directory.

<a id="playing-the-game"></a>
Playing the game
----------------

**Note**: If you want to play the game using your webbrowser, you should have a
modern standards compliant browser. These are the browsers that *should* work:
 * Chrome
 * Firefox 4 or higher
 * Internet Explorer 10
 * Safari

<a id="connect-to-your-own-server"></a>
### Connect to your own server ###

If you have just compiled your own server, you can connect to it through telnet
or using your webbrowser. The ports to connect to are displayed on the console
when you start your server.

The first character you register on your own server will automatically become an
admin.

<a id="connect-to-the-demo-server"></a>
### Connect to the demo server ###

If you do not have your own server, feel free to connect to the demo server:

 * Point your webbrowser to http://mud.yunocloud.com:8080/ *or*
 * Connect with telnet to mud.yunocloud.com on port 4801.

<a id="license"></a>
License
-------

This software is licensed under GPLv3.

For details, see LICENSE.GPL.txt.
