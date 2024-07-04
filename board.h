#pragma once
#include <array>	
#include <vector>
#include <utility>
#include <string>

constexpr size_t ROWS = 9;
constexpr size_t COLS = 9;

constexpr size_t SUB_ROW = 3;
constexpr size_t SUB_COL = 3;

using Board = std::array<std::array<short, ROWS>, COLS>;
using Move  = std::pair<size_t, size_t>;
using Moves = std::vector<std::pair<Move, short>>;

Board board_create(void);
Board board_load(const std::string& file);
void board_print(const Board& board);
bool board_can_insert(const Board& board, Move move, short value);
void board_insert(Board& board, Move move, short value);
Moves board_get_valid_moves(const Board& board);
Moves board_get_valid_values(const Board& board, Move move);
float board_get_move_entropy(const Board& board, Move move, short value);
bool board_solve(Board& board);
bool board_is_solved(const Board& board);