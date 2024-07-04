#include "board.h"

#include <iostream>
#include <cassert>

static constexpr short  BLANK_TILE            = 0;
static constexpr char   BLANK_TILE_CHAR       = '|';
static constexpr size_t MOVE_VALUES_LENGTH    = 9;
static constexpr float  TOTAL_NUMBER_OF_MOVES = ROWS * COLS * MOVE_VALUES_LENGTH;

static const std::array<short, MOVE_VALUES_LENGTH> MOVE_VALUES = {
	1, 2, 3, 4, 5, 6, 7, 8, 9
};

Board board_create(void) {
	Board board = {};

	for (auto& row : board) {
		for (short& tile : row) {
			tile = BLANK_TILE;
		}
	}

	return board;
}

Board board_load(const std::string& file) {
	return Board();
}

void board_print(const Board& board) {
	size_t i = 0;

	for (auto const& row : board) {
		size_t j = 0;

		for (auto const tile : row) {
			if (tile == BLANK_TILE) {
				std::cout << BLANK_TILE_CHAR << ' ';
			} else {
				std::cout << tile << ' ';
			}

			if (++j == SUB_COL) {
				std::cout << ' ';
				j = 0;
			}
		}

		std::cout << '\n';

		if (++i == SUB_ROW) {
			std::cout << '\n';
			i = 0;
		}
	}
}

bool board_can_insert(const Board& board, Move move, short value) {
	auto [i, j] = move;
	assert(MOVE_VALUES.front() <= value && value <= MOVE_VALUES.back());
	
	if (board[i][j] != BLANK_TILE) {
		return false;
	}

	for (size_t k = 0; k < COLS; k++) {
		if (board[i][k] == value) {
			return false;
		}
	}

	for (size_t k = 0; k < ROWS; k++) {
		if (board[k][j] == value) {
			return false;
		}
	}

	for (size_t k = i - i % SUB_ROW; k < i - i % SUB_ROW + SUB_ROW; k++) {
		for (size_t l = j - j % SUB_COL; l < j - j % SUB_COL + SUB_COL; l++) {
			if (board[k][l] == value) {
				return false;
			}
		}
	}

	return true;
}

void board_insert(Board& board, Move move, short value) {
	auto [i, j] = move;
	assert(MOVE_VALUES.front() <= value && value <= MOVE_VALUES.back());

	board[i][j] = value;
}

Moves board_get_valid_moves(const Board& board) {
	Moves moves;

	for (short move : MOVE_VALUES) {
		for (size_t i = 0; i < ROWS; i++) {
			for (size_t j = 0; j < COLS; j++) {
				if (board_can_insert(board, { i, j }, move)) {
					moves.push_back({ {i, j}, move });
				}
			}
		}
	}

	return moves;
}
	
Moves board_get_valid_values(const Board& board, Move move) {
	Moves moves;
	
	for (short value : MOVE_VALUES) {
		if (board_can_insert(board, move, value)) {
			moves.push_back({ move, value });
		}
	}

	return moves;
}

float board_get_move_entropy(const Board& board, Move move, short value) {
	assert(MOVE_VALUES.front() <= value && value <= MOVE_VALUES.back());
	assert(board_can_insert(board, move, value));

	Board board_next = board;
	board_insert(board_next, move, value);

	bool accumulated = false;
	float entropy = 0.f;

	for (size_t i = 0; i < ROWS; i++) {
		for (size_t j = 0; j < COLS; j++) {
			if (board_next[i][j] == BLANK_TILE) {
				accumulated = true;
				entropy += board_get_valid_values(board_next, { i, j }).size();
			}
		}
	}

	entropy = 1.f - entropy / TOTAL_NUMBER_OF_MOVES;
	return accumulated ? entropy : 1.f;
}

static void board_solve_recursive(Board& board) {
	Moves valid_moves = board_get_valid_moves(board);

	if (!valid_moves.size()) {
		return;
	}

	auto best_move = std::min_element(valid_moves.begin(), valid_moves.end(), [&board](auto a, auto b)->bool {
		return board_get_move_entropy(board, a.first, a.second) < board_get_move_entropy(board, b.first, b.second);
	});

	board_insert(board, best_move->first, best_move->second);

	board_solve_recursive(board);
}

bool board_solve(Board& board) {
	Moves valid_moves = board_get_valid_moves(board);
	Board old = board;

	for (auto const& [move, value] : valid_moves) {
		board_insert(board, move, value);
		board_solve_recursive(board);

		if (board_is_solved(board)) {
			return true;
		} else {
			board = old;
		}
	}

	return false;
}

bool board_is_solved(const Board& board) {
	for (size_t i = 0; i < ROWS; i++) {
		for (size_t j = 0; j < COLS; j++) {
			if (board[i][j] == BLANK_TILE) {
				return false;
			}

			Board copy = board;
			copy[i][j] = BLANK_TILE;
			if (!board_can_insert(copy, { i, j }, board[i][j])) {
				return false;
			}
		}
	}

	return true;
}
