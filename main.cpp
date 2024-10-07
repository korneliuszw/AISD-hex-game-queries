//
// Created by kwojn on 4/22/2024.
//

#include <vector>
#include <iostream>
#include <string>
#include <stack>

#define BOARD_SIZE 11

bool marked[BOARD_SIZE * BOARD_SIZE];
int read_buffer[BOARD_SIZE * BOARD_SIZE];
enum class Player {
    BLUE,
    RED,
    EMPTY
};

struct Board {
    Player array[BOARD_SIZE * BOARD_SIZE];
    int size = 0;
    // 0 - no response, 1 - ok, 2 - no
    int ok_board_response = 0;
    int empty_spaces = 0;
    int red_pawns = 0;
    int blue_pawns = 0;
    int empty_spaces_array[BOARD_SIZE * BOARD_SIZE];
};

inline int getBoardIndex(int x, int y, int size) {
    return y * size + x;
}

inline int translateInputColumn(int col, int row, int size) {
    if (row >= size)
        return getBoardIndex(col, col + (row - size + 1), size);
    return getBoardIndex(size - row + col -1 , col, size);
}


void read_board(Board& board) {
    board = Board();
    char input = 0, prevInput = 0, prevPrevInput = 0;
    int row = 0, col = -1, prevCol = 0;
    while (input != '<')
        std::cin.get(input);
    int top = 0;
    while (input != '-' || prevInput != '-' || prevPrevInput != '-') {
        if (input == '\n') {
            if (prevCol <= col)
                board.size++;
            row++;
            prevCol = col;
            col = -1;
        } else if (input == '<') {
            col++;
        }
        else if (input == 'b' || input == 'r') {
            read_buffer[top++] = input << 16 | col << 8 | row;
        }
        prevPrevInput = prevInput;
        prevInput = input;
        std::cin.get(input);
    }
    for (int i = 0; i < board.size * board.size; i++)
        board.array[i] = Player::EMPTY;
    for (int i = 0; i < top; i++) {
        auto c = read_buffer[i];
        int row = c & 0xFF;
        int col = (c >> 8) & 0xFF;
        char player = (c >> 16) & 0xFF;
        if (player == 'b') {
            board.blue_pawns++;
            board.array[translateInputColumn(col, row, board.size)] = Player::BLUE;
        } else {
            board.red_pawns++;
            board.array[translateInputColumn(col, row, board.size)] = Player::RED;
        }
    }
    board.empty_spaces = board.size * board.size - board.red_pawns - board.blue_pawns;
}

inline void decode_move(int move, int& x, int &y) {
    switch (move) {
        case 0:
            y -= 1;
            break;
        case 1:
            x += 1;
            y -= 1;
            break;
        case 2:
            x -= 1;
            break;
        case 3:
            x += 1;
            break;
        case 4:
            y += 1;
            break;
        case 5:
            x -= 1;
            y += 1;
            break;
    }
}



bool dfs_to_edge(Board& board, int x, int y, Player player, bool marked[BOARD_SIZE * BOARD_SIZE]) {
    if (x < 0 || x >= board.size || y < 0 || y >= board.size)
        return false;
    if (marked[getBoardIndex(x, y, board.size)])
        return false;
    int current[BOARD_SIZE * BOARD_SIZE];
    int stack_top = 0;
    int newX, newY, curX, curY;
    current[stack_top++] = getBoardIndex(x, y, board.size);
    while (stack_top != 0) {
        int it = current[--stack_top];
        if (board.array[it] != player)
            continue;
        curX = it % board.size, curY = it / board.size;
        if (player == Player::RED ? curY == board.size - 1 : curX == board.size - 1)
            return true;
        marked[it] = true;
        for (int i = 0; i < 6; i++) {
            newX = curX, newY = curY;
            decode_move(i, newX, newY);
            int index = getBoardIndex(newX, newY, board.size);
            if (newX < 0 || newX >= board.size || newY < 0 || newY >= board.size || marked[index])
                continue;
            current[stack_top++] = index;

        }
    }
    return false;
}


bool is_game_over(Board& board, Player player) {
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
        marked[i] = false;
    }
    for (int x = 0; x < board.size; x++) {
        if (player == Player::RED ? dfs_to_edge(board, x, 0, Player::RED, marked) : dfs_to_edge(board, 0, x, Player::BLUE, marked)) {
            return true;
        }
    }
    return false;
}
bool has_many_endings(Board& board, Player player) {
    for (int i = 0; i < board.size * board.size; i++) {
        if (board.array[i] != player)
            continue;
        board.array[i] = Player::EMPTY;
        if (!is_game_over(board, player)) {
            board.array[i] = player;
            return false;
        }
        board.array[i] = player;
    }
    return true;
}

int getPawnsDifference(Board &board) {
    return board.red_pawns - board.blue_pawns;
}
bool board_is_correct(Board& board) {
    int diff = getPawnsDifference(board);
    return diff >= 0 && diff <= 1;
}


bool is_board_impossible(Board& board) {
    int diff = getPawnsDifference(board);
    bool is_correct =  diff >= 0 && diff <= 1;
    return !is_correct || (is_game_over(board, Player::RED) && (diff == 0 || has_many_endings(board, Player::RED)))  || (is_game_over(board, Player::BLUE) && (diff == 1 ||  has_many_endings(board, Player::BLUE)));

}

bool can_win_in_n_moves_inside(Board &board, Player player, int moves, int diff) {
    int enemy_moves = moves;
    if ((diff == 0 && player == Player::RED) || (diff == 1 && player == Player::BLUE) )
        enemy_moves--;
    if (board.empty_spaces - enemy_moves < moves)
        return false;
    for (int i = 0; i < board.empty_spaces; i++) {
        board.array[board.empty_spaces_array[i]] = player;
        bool is_over = is_game_over(board, player);
        if (is_over && moves == 1) {
            board.array[board.empty_spaces_array[i]] = Player::EMPTY;
            return true;
        }
        else if (!is_over && moves == 2) {
            for (int j = 0; j < board.empty_spaces; j++) {
                board.array[board.empty_spaces_array[j]] = player;
                if (is_game_over(board, player)) {
                    board.array[board.empty_spaces_array[j]] = Player::EMPTY;
                    return true;
                }
                board.array[board.empty_spaces_array[j]] = Player::EMPTY;
            }
        }
        board.array[board.empty_spaces_array[i]] = Player::EMPTY;
    }
    return false;
}

int winning_moves_perfect[BOARD_SIZE * BOARD_SIZE];

#include <map>
#include <set>

int count_unique_moves(Board& board, Player player) {
    std::set<int> marked;
    for (int i = 0; i < board.empty_spaces; i++) {
        if (board.array[board.empty_spaces_array[i]] != Player::EMPTY) continue;
        board.array[board.empty_spaces_array[i]] = player;
        if (is_game_over(board, player))
            marked.insert(i);
        board.array[board.empty_spaces_array[i]] = Player::EMPTY;
    }
    return marked.size();
}

bool can_win_in_n_moves_inside_perfect(Board& board, Player player, int moves, int diff) {
    int enemy_moves = moves;
    if ((diff == 0 && player == Player::RED) || (diff == 1 && player == Player::BLUE) )
        enemy_moves--;
    if (board.empty_spaces - enemy_moves < moves)
        return false;

    if (enemy_moves == 0 && moves == 1 ) {
        return count_unique_moves(board, player) > 0;
    } else if (enemy_moves == 1 && moves == 1) {
        return count_unique_moves(board, player) > 1;
    } else if (enemy_moves == 1 && moves == 2) {
        for (int i = 0; i < board.empty_spaces; i++) {
            board.array[board.empty_spaces_array[i]] = player;
            if (!is_game_over(board, player) && count_unique_moves(board, player) > 1) {
                board.array[board.empty_spaces_array[i]] = Player::EMPTY;
                return true;
            }
            board.array[board.empty_spaces_array[i]] = Player::EMPTY;
        }
    } else {
        for (int j = 0; j < board.empty_spaces; j++) {
            board.array[board.empty_spaces_array[j]] = player == Player::RED ? Player::BLUE : Player::RED;
            int unique_wins = 0;
            for (int i = 0; i < board.empty_spaces; i++) {
                if (i == j) continue;
                board.array[board.empty_spaces_array[i]] = player;
                if (!is_game_over(board, player) && count_unique_moves(board, player) > 1) {
                    unique_wins++;
                }
                board.array[board.empty_spaces_array[i]] = Player::EMPTY;
            }
            if (unique_wins < 1) {
                board.array[board.empty_spaces_array[j]] = Player::EMPTY;
                return false;
            }
            board.array[board.empty_spaces_array[j]] = Player::EMPTY;
        }
        return true;
    }
    return false;
}

void find_empty_spaces(Board& board) {
    int top = 0;
    for (int i = 0; i < board.size * board.size; i++) {
        if (board.array[i] == Player::EMPTY)
            board.empty_spaces_array[top++] = i;
    }
}

void can_win_in_n_moves(Board& board, Player player, int moves, bool naive = true) {
    int diff = getPawnsDifference(board);
    // check and memorize correctness of board
    if (board.ok_board_response == 0) {
        if (diff < 0 || diff > 1 ||
            is_game_over(board, Player::RED) || is_game_over(board, Player::BLUE) || is_board_impossible(board)) {
            board.ok_board_response = 2;
        } else {
            find_empty_spaces(board);
            board.ok_board_response = 1;
        }
    }
    if (board.ok_board_response == 2) {
        std::cout << "NO" << std::endl;
        return;
    }
    if (naive && can_win_in_n_moves_inside(board, player, moves, diff)) {
        std::cout << "YES" << std::endl;
    } else if (!naive && can_win_in_n_moves_inside_perfect(board, player, moves, diff))
        std::cout << "YES" << std::endl;
    else {
        std::cout << "NO" << std::endl;
    }
}



int main() {
    Board board;
    std::string command;
    while (std::cin >> command) {
        if (command == "---") {
            read_board(board);
            std::cout << std::endl;
        } else if (command == "BOARD_SIZE") {
            std::cout << board.size << std::endl;
        } else if (command == "PAWNS_NUMBER") {
            std::cout << board.red_pawns + board.blue_pawns << std::endl;
        } else if (command == "IS_BOARD_CORRECT") {
            std::cout << (board_is_correct(board) ? "YES" : "NO") << std::endl;
        } else if (command == "IS_GAME_OVER") {
            if (!board_is_correct(board)) {
                std::cout << "NO" << std::endl;
                continue;
            }
            if (is_game_over(board, Player::RED))
                std::cout << "YES RED" << std::endl;
            else if (is_game_over(board, Player::BLUE))
                std::cout << "YES BLUE" << std::endl;
            else
                std::cout << "NO" << std::endl;
        } else if (command == "IS_BOARD_POSSIBLE") {
            if (is_board_impossible(board))
                std::cout << "NO" << std::endl;
            else
                std::cout << "YES" << std::endl;
        } else if (command == "CAN_RED_WIN_IN_2_MOVES_WITH_NAIVE_OPPONENT") {
            can_win_in_n_moves(board, Player::RED, 2);
        } else if (command == "CAN_RED_WIN_IN_1_MOVE_WITH_NAIVE_OPPONENT") {
            can_win_in_n_moves(board, Player::RED, 1);
        } else if (command == "CAN_BLUE_WIN_IN_2_MOVES_WITH_NAIVE_OPPONENT") {
            can_win_in_n_moves(board, Player::BLUE, 2);
        } else if (command == "CAN_BLUE_WIN_IN_1_MOVE_WITH_NAIVE_OPPONENT") {
            can_win_in_n_moves(board, Player::BLUE, 1);
        } else if (command == "CAN_RED_WIN_IN_2_MOVES_WITH_PERFECT_OPPONENT") {
        can_win_in_n_moves(board, Player::RED, 2, false);
    } else if (command == "CAN_RED_WIN_IN_1_MOVE_WITH_PERFECT_OPPONENT") {
        can_win_in_n_moves(board, Player::RED, 1, false);
    } else if (command == "CAN_BLUE_WIN_IN_2_MOVES_WITH_PERFECT_OPPONENT") {
        can_win_in_n_moves(board, Player::BLUE, 2, false);
    } else if (command == "CAN_BLUE_WIN_IN_1_MOVE_WITH_PERFECT_OPPONENT") {
        can_win_in_n_moves(board, Player::BLUE, 1, false);
    }
        else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

}
