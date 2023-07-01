#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <malloc.h>
#include <string.h>

int N = 10; // leaderboard top N

void msg_invalid_username(int desc);
char *msg_rcv_username(int desc);

void init_db()
{
    sqlite3 *db;
    char *msg_err = 0;

    int ok = sqlite3_open("db/leaderboard.db", &db);

    if (ok != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
    }

    char *sql = "DROP TABLE IF EXISTS Leaderboard;"
                "CREATE TABLE Leaderboard(Score INT, Name TEXT UNIQUE);";
    ok = sqlite3_exec(db, sql, 0, 0, &msg_err);
    if (ok != SQLITE_OK)
    {
        printf("SQL CREATE error: %s\n", msg_err);
        sqlite3_free(msg_err);
    }

    sqlite3_close(db);
}

void insert_db(int desc, int score, char *winner)
{
    sqlite3 *db;
    char *msg_err = 0;

    int ok = sqlite3_open("db/leaderboard.db", &db);

    if (ok != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
    }
    do
    {
        char *query = sqlite3_mprintf("INSERT into Leaderboard VALUES (%d,'%q');", score, winner);
        ok = sqlite3_exec(db, query, 0, 0, &msg_err);
        sqlite3_free(query);
        if (ok != SQLITE_OK)
        {
            printf("SQL INSERT error: %s\n", msg_err);
            sqlite3_free(msg_err);
            msg_invalid_username(desc);
            winner = msg_rcv_username(desc);
            query = sqlite3_mprintf("INSERT into Leaderboard VALUES (%d,'%q');", score, winner);
            ok = sqlite3_exec(db, query, 0, 0, &msg_err);
            sqlite3_free(query);
        }
    } while (ok != SQLITE_OK);

    free(winner);
    sqlite3_close(db);
}

void select_db(char *ldboard)
{

    sqlite3 *db;

    int ok = sqlite3_open("db/leaderboard.db", &db);

    if (ok != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_stmt *stmt;

    char *sql = "SELECT * FROM Leaderboard ORDER BY 1 DESC";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("error: %s", sqlite3_errmsg(db));
        return;
    }
    int rows = 1;
    while (rows++ <= N && (rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {

        char *line = malloc(sizeof(char) * 100);
        line[0] = '\0';
        strcat(line, "|");

        const unsigned char *score = sqlite3_column_text(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);

        strcat(line, (const char *)score);
        strcat(line, "   |");
        strcat(line, (const char *)name);
        strcat(line, "\n");

        strcat(ldboard, line);
        free(line);
    }

    sqlite3_finalize(stmt);

    sqlite3_close(db);
}
