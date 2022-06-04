Install SQLite on Linux:
    check if you already have SQLite installed on your machine:

    $sqlite3
    SQLite version 3.7.15.2 2013-01-09 11:53:05
    Enter ".help" for instructions
    Enter SQL statements terminated with a ";"
    sqlite>
    If you do not see the above result, then it means you do not have SQLite installed on your Linux machine.
    Following are the following steps to install SQLite −

    Step 1 − Go to SQLite download page and download sqlite-autoconf-*.tar.gz from source code section.

    Step 2 − Run the following command −

    $tar xvfz sqlite-autoconf-3071502.tar.gz
    $cd sqlite-autoconf-3071502
    $./configure --prefix=/usr/local
    $make
    $make install

Install SFML:
    Run the following command −

    $sudo apt-get install libsfml-dev


Start the server:
    Run the following command in the /Reversi folder −

    $make

Connect a player:
    Run the following command in the /Reversi folder −

    $make run

Enjoy!
