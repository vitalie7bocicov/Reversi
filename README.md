# Reversi

Reversi is a strategy board game for two players, played on an 8×8 board.

This game was implemented using the client server architecture, the communication was done via TCP/IP, in C and C++.

![Alt text](imgs/reversi.png?raw=true "Reversi board")

Technologies used:

-SQLite (to save a ranking of the top N best players)

-Simple and Fast Multimedia Library (SFML) - for the user interface

-threads (the server will create a thread for each game so that the games can run in parallel).

Features:

-In case a player leaves the game, the remaining player will have the option to
continue to play against the computer.

![Alt text](imgs/computer.png?raw=true "Option to play with the computer")

-If you win you can save your name for the leaderboard

![Alt text](imgs/leaderboard.png?raw=true "Reversi board")

\*check the documentation for details.
