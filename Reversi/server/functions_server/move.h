#include "game_util.h"

int move_player_1(int **board, int *score, int desc1, int desc2)
{
    int mv1;
    mv1 = 99;
    msg_score(desc1, score);
    msg_board(board, desc1);
    msg_player_1(desc1, 2); // insert your move
    msg_player_2(desc2, 3); // wait for player 1
    mv1 = rcv_move(board, desc1, desc2);
    if (mv1 == -2 || mv1 == -3 || mv1 == -1)
        return mv1;
    while (!is_playable(board, mv1 / 10, mv1 % 10))
    {
        msg_player_1(desc1, 4); // player 1 : invalid move
        mv1 = rcv_move(board, desc1, desc2);
        if (mv1 == -2 || mv1 == -3)
            return mv1;
    }

    update_board(board, mv1, 1);

    remove_possible_moves(board);

    capture_pieces(board, score, mv1 / 10, mv1 % 10, 1);

    msg_score(desc1, score);
    msg_board(board, desc1);

    mark_possible_moves(board, 2);

    if (check_endgame(board) == 1)
        return 1;
    return 0;
}

int move_player_2(int **board, int *score, int desc1, int desc2)
{
    int mv2;
    msg_player_1(desc1, 3); // wait for player 2
    msg_score(desc2, score);
    msg_board(board, desc2);
    msg_player_2(desc2, 2); // insert move
    mv2 = rcv_move(board, desc2, desc1);
    if (mv2 == -2 || mv2 == -3 || mv2 == -1)
        return mv2;
    while (!is_playable(board, mv2 / 10, mv2 % 10))
    {
        msg_player_2(desc2, 4); // invalid move
        mv2 = rcv_move(board, desc2, desc1);
        if (mv2 == -2 || mv2 == -3)
            return mv2;
    }

    update_board(board, mv2, 2);

    remove_possible_moves(board);

    capture_pieces(board, score, mv2 / 10, mv2 % 10, 2);

    msg_score(desc2, score);
    msg_board(board, desc2);

    mark_possible_moves(board, 1);

    if (check_endgame(board) == 1)
        return 1;
    return 0;
}

int move_pc(int **board, int *score, int desc, int player)
{
    int opp = get_opponent(player);

    int mv;
    mv = rcv_move_auto(board, player);
    sleep(1);
    update_board(board, mv, player);
    remove_possible_moves(board);
    capture_pieces(board, score, mv / 10, mv % 10, player);
    mark_possible_moves(board, opp);
    if (check_endgame(board) == 1)
        return 1;
    return 0;
}

int move_player(int **board, int *score, int desc, int player)
{
    int opp = get_opponent(player);

    int mv;
    mv = 99;
    msg_score(desc, score);
    msg_board(board, desc);
    if (player == 1)
        msg_player_1(desc, 2); // insert move
    else
        msg_player_2(desc, 2); // insert move
    mv = rcv_move(board, desc, desc);
    if (mv == -1)
        return -1;
    while (!is_playable(board, mv / 10, mv % 10))
    {
        if (player == 1)
            msg_player_1(desc, 4); //  invalid move
        else
            msg_player_2(desc, 4); // invalid move
        mv = rcv_move(board, desc, desc);
        if (mv == -1)
            return -1;
    }

    update_board(board, mv, player);
    remove_possible_moves(board);
    capture_pieces(board, score, mv / 10, mv % 10, player);

    msg_score(desc, score);
    msg_board(board, desc);

    mark_possible_moves(board, opp);

    if (check_endgame(board) == 1)
        return 1;
    return 0;
}
