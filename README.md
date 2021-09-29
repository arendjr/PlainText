PlainText
=========

PlainText is a text-based MUD game engine.

 * **[Features](#features)**
 * **[Documentation](#documentation)**
 * **[Running your own server](#running-your-own-server)**
   * [Connecting](#connecting)
 * **[License](#license)**
 

Features
--------

**Disclaimer:** Some of these are only available in the `legacy` branch. My
intention is to get everything below reimplemented in the current Rust port.

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
   * Written in Rust
   * Lock-free, multi-threaded design
   * All game objects are in memory
   * Changes are synced to disk in separate thread

Documentation
-------------

There's documentation available in the
[PlainText Wiki](https://github.com/arendjr/PlainText/wiki).

You may want to jump straight to the
[Frequently Asked Questions](https://github.com/arendjr/PlainText/wiki/Frequently-Asked-Questions).

Running your own server
-----------------------

Make sure you have the latest stable Rust release installed and cloned this
repository, then from the command line, run:

    $ cargo run

You can use the PT_DATA_DIR and PT_LOG_DIR environment variables to control
where data files are loaded from and logs are written to, respectively.

### Connecting ###

Once your own server is running, you can connect to it through telnet
or using your webbrowser. The ports to connect to are displayed on the console
when you start your server.

The first character you register on your own server will automatically become an
admin.

License
-------

This software is licensed under AGPLv3.

For details, see [LICENSE](https://github.com/arendjr/PlainText/blob/main/LICENSE).
