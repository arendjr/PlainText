= DESIGN =

== Hierarchy ==

The source code is split into two main directories:

 - __engine__    Contains all the sources for the core game engine.
 - __interface__ Contains interfaces for interacting with the engine.
 - __tests__     Contains all the tests, both functional and performance.
 - __utils__     Some command-line utilities, only for historical use.


== Thread Model ==

The engine uses four threads:

 - __main thread__ This is running the main Qt event loop where all events from
                   the interfaces and timers initially enter. Every action that
                   needs to be done on the game data is posted as an event on
                   the game thread.
 - __game thread__ The game thread processes events that manipulate the core game
                   data. Any generated output is passed back through Qt signals
                   back to the interfaces. When an object is modified, an event
                   is posted to the sync thread for synchronizing the changes to
                   disk.
 - __sync thread__ The sync thread writes modified objects back to disk.
 - __log thread__  The log thread writes all log messages (including statistics)
                   to disk. In addition, when statistics are requested through
                   the admin interface, it is used for aggregating the results.

Note: It is important to note that all game objects are assigned to the main
      thread even though all manipulation (after the initial loading) is done
      by the game thread.
