//
// Created by kwojn on 4/22/2024.
//

#include <vector>
#include <iostream>
#include <string>
#include <stack>

#define BOARD_SIZE 11

enum class Player {
    BLUE,
    RED,
    EMPTY
};

struct Board {
    std::vector<Player> array;
    int size = 0;
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
    board.size = 0;
    char input = 0, prevInput = 0, prevPrevInput = 0;
    int row = 0, col = -1, prevCol = 0;
    while (input != '<')
        std::cin.get(input);
    std::vector<int> buffer;
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
            buffer.push_back(input << 16 | col << 8 | row);
        }
        prevPrevInput = prevInput;
        prevInput = input;
        std::cin.get(input);
    }
    board.array.resize(board.size * board.size);
    std::fill(board.array.begin(), board.array.end(), Player::EMPTY);
    for (auto c: buffer) {
        int row = c & 0xFF;
        int col = (c >> 8) & 0xFF;
        char player = (c >> 16) & 0xFF;
        board.array[translateInputColumn(col, row, board.size)] = player == 'b' ? Player::BLUE : Player::RED;
    }
}

int count(Board& board) {
    int count = 0;
    for (int i = 0; i < board.size * board.size; i++) {
        if (board.array[i] == Player::BLUE || board.array[i] == Player::RED)
            count++;
    }
    return count;
}
int count_of_type(Board& board, Player player) {
    int count = 0;
    for (int i = 0; i < board.size * board.size; i++) {
        if (board.array[i] == player)
            count++;
    }
    return count;
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

bool dfs_to_edge(Board& board, int x, int y, Player player, std::vector<bool>& marked) {
    if (x < 0 || x >= board.size || y < 0 || y >= board.size)
        return false;
    if (marked[getBoardIndex(x, y, board.size)])
        return false;
    std::stack<std::pair<int, int>> current;
    current.emplace(x, y);
    while (!current.empty()) {
        auto it = current.top();
        current.pop();
        if (board.array[getBoardIndex(it.first, it.second, board.size)] != player)
            continue;
        if (player == Player::RED ? it.second == board.size - 1 : it.first == board.size - 1)
            return true;

        marked[getBoardIndex(it.first, it.second, board.size)] = true;
        int newX, newY;
        for (int i = 0; i < 6; i++) {
            newX = it.first, newY = it.second;
            decode_move(i, newX, newY);
            if (newX < 0 || newX >= board.size || newY < 0 || newY >= board.size || marked[getBoardIndex(newX, newY, board.size)])
                continue;
            current.emplace(newX, newY);

        }
    }
    return false;
}

bool is_game_over_red(Board& board) {
    std::vector<bool> marked(board.size * board.size, false);
    for (int x = 0; x < board.size; x++) {
        if (dfs_to_edge(board, x, 0, Player::RED, marked))
            return true;
    }
    return false;
}
bool is_game_over_blue(Board& board) {
    std::vector<bool> marked(board.size * board.size, false);
    for (int y = 0; y < board.size; y++) {
        if (dfs_to_edge(board, 0, y, Player::BLUE, marked))
            return true;
    }
    return false;
}

bool has_many_endings(Board& board, Player player) {
    for (int i = 0; i < board.size * board.size; i++) {
        if (board.array[i] != player)
            continue;
        board.array[i] = Player::EMPTY;
        if (!(player == Player::RED ? is_game_over_red(board) : is_game_over_blue(board)))
            return false;
        board.array[i] = player;
    }
    return true;
}




bool board_is_correct(Board& board) {
    int diff = count_of_type(board, Player::RED) - count_of_type(board, Player::BLUE);
    return diff >= 0 && diff <= 1;
}

int main() {
    Board board;
    board.array.reserve(BOARD_SIZE * BOARD_SIZE);
    std::string command;
    int n = 0;
    while (std::cin >> command) {
        if (command == "---") {
            n++;
            read_board(board);
        } else if (command == "BOARD_SIZE") {
            std::cout << board.size << std::endl;
        } else if (command == "PAWNS_NUMBER") {
            std::cout << count(board) << std::endl;
        } else if (command == "IS_BOARD_CORRECT") {
            std::cout << (board_is_correct(board) ? "YES" : "NO") << std::endl;
        } else if (command == "IS_GAME_OVER") {
            if (!board_is_correct(board)) {
                std::cout << "NO" << std::endl;
                continue;
            }
            if (is_game_over_red(board))
                std::cout << "YES RED" << std::endl;
            else if (is_game_over_blue(board))
                std::cout << "YES BLUE" << std::endl;
            else
                std::cout << "NO" << std::endl;
        } else if (command == "IS_BOARD_POSSIBLE") {
            int diff = count_of_type(board, Player::RED) - count_of_type(board, Player::BLUE);
            bool is_correct =  diff >= 0 && diff <= 1;
            if (!is_correct || (is_game_over_red(board) && (diff == 0 || has_many_endings(board, Player::RED)))  || (is_game_over_blue(board) && (diff == 1 ||  has_many_endings(board, Player::BLUE)))) {

                std::cout << "NO" << std::endl;
                continue;
            }
            std::cout << "YES" << std::endl;
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

}
