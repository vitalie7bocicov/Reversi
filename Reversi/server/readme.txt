Instructions for Linux:


Instal gcc:
  $sudo apt-get install gcc

Install SQLite:
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

    $tar xvfz sqlite-autoconf-*.tar.gz
    $cd sqlite-autoconf-*.tar.gz
    $./configure --prefix=/usr/local
    $make
    $make install



Start the server:
    Run the following commands:

    $make
    $make run

