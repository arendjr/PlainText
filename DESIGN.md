DESIGN
======

Hierarchy
---------

The source code is split into four main directories:

 - **engine** - Contains all the sources for the core game engine.
 - **interface** - Contains interfaces (telnet, WebSockets) for interacting with
                   the engine.
 - **tests** - Contains all the tests, including unit, functional and
               performance.
 - **utils** - Some command-line utilities, mainly for historical use.


Thread Model
------------

The engine uses four threads:

 - **main thread** - This is running the main Qt event loop where all events 
                     from the interfaces and timers initially enter.
 - **game thread** - The game thread processes events that manipulate the core
                     game data. Any generated output is passed back through Qt
                     signals back to the interfaces. When an object is modified,
                     an event is posted to the sync thread for synchronizing
                     the changes to disk.
 - **sync thread** - The sync thread writes modified objects back to disk. This
                     work is separated from the game thread to make sure the
                     player experience is not affected by the relatively slow
                     disk accesses, especially when a game event affects a
                     large amount of objects at once.
 - **log thread** - The log thread writes all log messages (including
                    statistics) to disk. In addition, when statistics are
                    requested through the admin interface, this thread is used
                    for aggregating the results. This work is separated from
                    the game thread to shield the player experience from slow
                    downs in disk accesses as well as from the spike in load
                    that may be triggered when an admin requests statistics.

Note: It is important to note that all game objects are assigned to the main
      thread even though all manipulation (after the initial loading) is done
      by the game thread.
