#include "board.h"

#include "board_square.h"
#include "exceptions.h"
#include "formatting.h"
#include <fstream>
#include <iomanip>

using namespace std;

bool Board::Position::operator==(const Board::Position& other) const {
    return this->row == other.row && this->column == other.column;
}

bool Board::Position::operator!=(const Board::Position& other) const {
    return this->row != other.row || this->column != other.column;
}

Board::Position Board::Position::translate(Direction direction) const { return this->translate(direction, 1); }

Board::Position Board::Position::translate(Direction direction, ssize_t distance) const {
    if (direction == Direction::DOWN) {
        return Board::Position(this->row + distance, this->column);
    } else {
        return Board::Position(this->row, this->column + distance);
    }
}

Board Board::read(const string& file_path) {
    ifstream file(file_path);
    if (!file) {
        throw FileException("cannot open board file!");
    }

    size_t rows;
    size_t columns;
    size_t starting_row;
    size_t starting_column;
    file >> rows >> columns >> starting_row >> starting_column;
    Board board(rows, columns, starting_row, starting_column);

    // TODO: complete implementation of reading in board from file here.
    // Nested for loop to read in and initialize each board square of Board

    char current_square;
    unsigned int multiplier;

    // PARSER CODE ---------------------------------------
    for (size_t i = 0; i < rows; i++) {
        // cerr << "I " << i << endl;
        // For each row, insert a vector of boardsquares to represent columns
        board.squares.push_back(vector<BoardSquare>());
        // cerr << board.squares.size() << endl;
        for (size_t j = 0; j < columns; j++) {
            // If you can take in an integer, it is a letter multiplier
            // cerr << "J " << j << endl;
            /*if(file>>multiplier){
            //  cerr << "Multiplier" << multiplier << endl;
              board.squares[i].push_back(BoardSquare(multiplier, 1));
            }
            //If you cannot take in a integer, the value is  a character */
            if (file >> current_square) {
                // Normal Square
                //  cerr << "current_square " << current_square << endl;
                if (current_square == '.') {
                    board.squares[i].push_back(BoardSquare(1, 1));
                }
                // Double Word
                else if (current_square == 'd') {
                    board.squares[i].push_back(BoardSquare(1, 2));
                }
                // Triple Word
                else if (current_square == 't') {
                    board.squares[i].push_back(BoardSquare(1, 3));
                } else {
                    multiplier = current_square - '0';
                    if (multiplier < 0 || multiplier > 9) {
                        throw FileException("Invalid Board");
                    }
                    board.squares[i].push_back(BoardSquare(multiplier, 1));
                }
            }
            /*else{
              cerr<< "Cannot seem to read file from stream" << endl;
            }*/
        }
    }
    // END OF PARSER CODE --------------------------------
    return board;
}

size_t Board::get_move_index() const { return this->move_index; }

// Test_place should verify that the move given as an argument can be placed on the board
// It should return a valid PlaceResult object with appropriate words if so
// and an invalid PlaceResult object with error message otherwise.
PlaceResult Board::test_place(const Move& move) const {
    // TODO: complete implementation here
    // print(cout);
    // Create a position struct to represent move start
    bool contains_start = false;
    bool is_adjacent = false;
    Position move_start(move.row, move.column);
    vector<string> words_generated;
    unsigned int points = 0;

    // variable for the word formed from the actual placing
    string placed_word = "";
    unsigned int placed_points = 0;
    unsigned int placed_multiplier = 1;
    size_t tile_counter = 0;

    if (in_bounds_and_has_tile(move_start)) {
        return PlaceResult("Cannot Place Tile -- Overlap Error");
    }

    if (move.tiles.size() == 1) {
        Position move_location = move_start;
        if (!is_in_bounds(move_start)) {
            return PlaceResult("Location is Out of Bounds");
        } else if (move_index == 0 && move_location != start) {
            return PlaceResult("Starting word is not placed over start tile");
        }
        // vertical word
        generate_score_and_word(move_start, move.tiles[0], 'u', words_generated, points);
        // horizontal word
        generate_score_and_word(move_start, move.tiles[0], 'l', words_generated, points);

        if (move_index != 0 && !check_adjacent(move_location)) {
            return PlaceResult("Please put at least one tile adjacent to another tile");
        }

        return PlaceResult(words_generated, points);
    }

    else if (move.direction == Direction::DOWN) {
        Position move_location = move_start;

        // Generate Perpendicular Words
        for (size_t i = 0; i < move.tiles.size(); i++) {
            if (!is_in_bounds(move_location)) {
                return PlaceResult("Location is Out of Bounds");
            }
            while (in_bounds_and_has_tile(move_location)) {
                if (move_location == start) {
                    contains_start = true;
                }
                if (check_adjacent(move_location)) {
                    is_adjacent = true;
                }
                move_location.row += 1;
            }
            // cerr << move_location.row << "  " << move_location.column << "  " << points << endl;
            generate_score_and_word(move_location, move.tiles[i], 'l', words_generated, points);
            // cerr << move_location.row << "  " << move_location.column << "  " << points << endl;
            if (move_location == start) {
                contains_start = true;
            }
            if (check_adjacent(move_location)) {
                is_adjacent = true;
            }
            move_location.row += 1;
        }

        // Generate Words from Actual Placed Tiles;
        // Get the top most locatoin
        Position front = get_top(move_start);
        // cerr<< move_start.row << move_start.column << " " << front.row << " " <<front.column << endl;
        while (is_in_bounds(front) && (in_bounds_and_has_tile(front) || tile_counter < move.tiles.size())) {
            if (in_bounds_and_has_tile(front)) {
                if (at(front).get_tile_kind().letter == '?') {
                    placed_word += at(front).get_tile_kind().assigned;
                } else {
                    placed_word += at(front).get_tile_kind().letter;
                }
                placed_points += at(front).get_tile_kind().points;  // * at(front).letter_multiplier;
            } else {
                // cerr << placed_word << "  " << placed_points << move.tiles[tile_counter].points <<endl;
                // cerr << at(front).letter_multiplier << endl;
                if (move.tiles[tile_counter].letter == '?') {
                    placed_word += move.tiles[tile_counter].assigned;
                } else {
                    placed_word += move.tiles[tile_counter].letter;
                }
                placed_points += move.tiles[tile_counter].points * at(front).letter_multiplier;
                tile_counter++;
                placed_multiplier *= at(front).word_multiplier;
                // cerr << placed_word << "  " << placed_points << endl;
            }

            // cerr << placed_word << "  " << placed_points << endl;
            front.row += 1;
        }
        words_generated.push_back(placed_word);
        // cerr << points << endl;
        placed_points *= placed_multiplier;
        points += placed_points;
        // cerr << points << endl;
        if (move_index == 0 && !contains_start) {
            return PlaceResult("First move must cover start tile");
        }
        if (move_index != 0 && !is_adjacent) {
            return PlaceResult("Please place at least one tile adjacent to tile on the board");
        }

        return PlaceResult(words_generated, points);
    }

    else if (move.direction == Direction::ACROSS) {
        Position move_location = move_start;

        // Generate Perpendicular Words
        for (size_t i = 0; i < move.tiles.size(); i++) {
            if (!is_in_bounds(move_location)) {
                return PlaceResult("Location is Out of Bounds");
            }
            while (in_bounds_and_has_tile(move_location)) {
                if (move_location == start) {
                    contains_start = true;
                }
                if (check_adjacent(move_location)) {
                    is_adjacent = true;
                }
                move_location.column += 1;
            }
            generate_score_and_word(move_location, move.tiles[i], 'u', words_generated, points);
            if (move_location == start) {
                contains_start = true;
            }
            if (check_adjacent(move_location)) {
                is_adjacent = true;
            }
            move_location.column += 1;
        }

        // Generate Words from Actual Placed Tiles;
        // Get the top most locatoin
        Position front = get_left(move_start);
        while (is_in_bounds(front) && (in_bounds_and_has_tile(front) || tile_counter < move.tiles.size())) {
            if (in_bounds_and_has_tile(front)) {
                if (at(front).get_tile_kind().letter == '?') {
                    placed_word += at(front).get_tile_kind().assigned;
                } else {
                    placed_word += at(front).get_tile_kind().letter;
                }
                placed_points += at(front).get_tile_kind().points;  // * at(front).letter_multiplier;
            } else {
                // cerr << placed_word << "  " << placed_points << move.tiles[tile_counter].points <<endl;
                // cerr << at(front).letter_multiplier << endl;
                if (move.tiles[tile_counter].letter == '?') {
                    placed_word += move.tiles[tile_counter].assigned;
                } else {
                    placed_word += move.tiles[tile_counter].letter;
                }
                placed_points += move.tiles[tile_counter].points * at(front).letter_multiplier;
                tile_counter++;
                placed_multiplier *= at(front).word_multiplier;
                // cerr << placed_word << "  " << placed_points << endl;
            }

            front.column += 1;
        }
        words_generated.push_back(placed_word);
        placed_points *= placed_multiplier;

        points += placed_points;

        if (move_index == 0 && !contains_start) {
            return PlaceResult("First move must cover start tile");
        }
        if (move_index != 0 && !is_adjacent) {
            return PlaceResult("Please place at least one tile adjacent to tile on the board");
        }
        return PlaceResult(words_generated, points);
    } else {
        return PlaceResult("No Direction Specified");
    }
}

PlaceResult Board::place(const Move& move) {
    // TODO: Complete implementation here
    PlaceResult result = test_place(move);
    Position move_start(move.row, move.column);
    if (result.valid) {
        if (move.tiles.size() == 1) {
            squares[move.row][move.column].set_tile_kind(move.tiles[0]);
        }

        else if (move.direction == Direction::DOWN) {
            Position move_location = move_start;
            for (size_t i = 0; i < move.tiles.size(); i++) {
                while (in_bounds_and_has_tile(move_location)) {
                    move_location.row += 1;
                }
                squares[move_location.row][move_location.column].set_tile_kind(move.tiles[i]);
                move_location.row += 1;
            }
        } else if (move.direction == Direction::ACROSS) {
            Position move_location = move_start;
            for (size_t i = 0; i < move.tiles.size(); i++) {
                while (in_bounds_and_has_tile(move_location)) {
                    move_location.column += 1;
                }
                squares[move_location.row][move_location.column].set_tile_kind(move.tiles[i]);
                move_location.column += 1;
            }
        }
    }
    move_index++;
    return result;
}

Board::Position Board::get_left(Position move_start) const {
    move_start.column--;
    while (in_bounds_and_has_tile(move_start)) {
        move_start.column -= 1;
    }
    // When while loop exits, we need to add one to get the position of the final tile
    move_start.column += 1;
    return move_start;
}

Board::Position Board::get_top(Position move_start) const {
    move_start.row--;
    while (in_bounds_and_has_tile(move_start)) {
        move_start.row -= 1;
    }
    // When while loop exits, we need to add one to get the position of the final tile
    move_start.row += 1;
    return move_start;
}

void Board::generate_score_and_word(
        Position move_start, TileKind placed_tile, char dir, vector<string>& words, unsigned int& points) const {
    bool tile_used = false;
    string word = "";
    unsigned int word_points = 0;
    unsigned short total_word_multiplier = 1;
    // generate vertical word
    if (dir == 'u') {
        Position front = get_top(move_start);
        while (is_in_bounds(front) && (in_bounds_and_has_tile(front) || !tile_used)) {
            if (in_bounds_and_has_tile(front)) {
                if (at(front).get_tile_kind().letter == '?') {
                    word += at(front).get_tile_kind().assigned;
                } else {
                    word += at(front).get_tile_kind().letter;
                }
                word_points += at(front).get_tile_kind().points;  // * at(front).letter_multiplier;
            } else {
                if (placed_tile.letter == '?') {
                    word += placed_tile.assigned;
                } else {
                    word += placed_tile.letter;
                }
                word_points += placed_tile.points * at(front).letter_multiplier;
                tile_used = true;
                total_word_multiplier *= at(front).word_multiplier;
            }
            front.row += 1;
        }
    }
    // generate horizontal word
    else if (dir == 'l') {
        Position front = get_left(move_start);
        while (is_in_bounds(front) && (in_bounds_and_has_tile(front) || !tile_used)) {
            if (in_bounds_and_has_tile(front)) {
                if (at(front).get_tile_kind().letter == '?') {
                    word += at(front).get_tile_kind().assigned;
                } else {
                    word += at(front).get_tile_kind().letter;
                }
                word_points += at(front).get_tile_kind().points;  //* at(front).letter_multiplier;
            } else {
                if (placed_tile.letter == '?') {
                    word += placed_tile.assigned;
                } else {
                    word += placed_tile.letter;
                }
                word_points += placed_tile.points * at(front).letter_multiplier;
                tile_used = true;
                total_word_multiplier *= at(front).word_multiplier;
            }
            front.column += 1;
        }
    }
    // Add Error Checks for word (just the tile itself)
    if (word.length() == 1) {
        return;
    } else {
        words.push_back(word);
        word_points *= total_word_multiplier;
        points += word_points;
    }
}

bool Board::check_adjacent(Position move_start) const {
    Position adjacent_left(move_start.row, move_start.column - 1);
    Position adjacent_right(move_start.row, move_start.column + 1);
    Position adjacent_up(move_start.row - 1, move_start.column);
    Position adjacent_down(move_start.row + 1, move_start.column);

    return (in_bounds_and_has_tile(adjacent_up) || in_bounds_and_has_tile(adjacent_down)
            || in_bounds_and_has_tile(adjacent_left) || in_bounds_and_has_tile(adjacent_right));
}

//-------------------------HW 5 --------------------------//

char Board::letter_at(Position p) const {
    TileKind temp = at(p).get_tile_kind();
    if (temp.letter == '?') {
        return temp.assigned;
    } else {
        return temp.letter;
    }
}

bool Board::is_anchor_spot(Position p) const {
    if (is_in_bounds(p) && !in_bounds_and_has_tile(p) && (check_adjacent(p) || (p == this->start))) {
        return true;
    } else {
        return false;
    }
}

vector<Board::Anchor> Board::get_anchors() const {
    vector<Anchor> anchors;
    // Iterate through the entire board
    for (size_t i = 0; i < squares.size(); i++) {
        for (size_t j = 0; j < squares[i].size(); j++) {
            // If the current position is a suitable anchor spot, generate the anchor and add it to the list
            Position temp(i, j);
            if (is_anchor_spot(temp)) {
                anchors.push_back(Anchor(temp, Direction::ACROSS, get_across_limit(temp)));
                anchors.push_back(Anchor(temp, Direction::DOWN, get_down_limit(temp)));
            }
        }
    }

    return anchors;
}

size_t Board::get_across_limit(Position temp) const {
    size_t limit = 0;
    temp.column--;
    while (is_in_bounds(temp) && !in_bounds_and_has_tile(temp) && !is_anchor_spot(temp)) {
        limit++;
        temp.column--;  // move left
    }
    return limit;
}

size_t Board::get_down_limit(Position temp) const {
    size_t limit = 0;
    temp.row--;
    while (is_in_bounds(temp) && !in_bounds_and_has_tile(temp) && !is_anchor_spot(temp)) {
        limit++;
        temp.row--;  // move up
    }
    return limit;
}

TileKind Board::get_tile_at_square(Position p) const { return at(p).get_tile_kind(); }

//------------------------- End of HW 5 --------------------------//

// The rest of this file is provided for you. No need to make changes.

BoardSquare& Board::at(const Board::Position& position) { return this->squares.at(position.row).at(position.column); }

const BoardSquare& Board::at(const Board::Position& position) const {
    return this->squares.at(position.row).at(position.column);
}

bool Board::is_in_bounds(const Board::Position& position) const {
    return position.row < this->rows && position.column < this->columns;
}

bool Board::in_bounds_and_has_tile(const Position& position) const {
    return is_in_bounds(position) && at(position).has_tile();
}

void Board::print(ostream& out) const {
    // Draw horizontal number labels
    for (size_t i = 0; i < BOARD_TOP_MARGIN - 2; ++i) {
        out << std::endl;
    }
    out << FG_COLOR_LABEL << repeat(SPACE, BOARD_LEFT_MARGIN);
    const size_t right_number_space = (SQUARE_OUTER_WIDTH - 3) / 2;
    const size_t left_number_space = (SQUARE_OUTER_WIDTH - 3) - right_number_space;
    for (size_t column = 0; column < this->columns; ++column) {
        out << repeat(SPACE, left_number_space) << std::setw(2) << column + 1 << repeat(SPACE, right_number_space);
    }
    out << std::endl;

    // Draw top line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << endl;

    // Draw inner board
    for (size_t row = 0; row < this->rows; ++row) {
        if (row > 0) {
            out << repeat(SPACE, BOARD_LEFT_MARGIN);
            print_horizontal(this->columns, T_RIGHT, PLUS, T_LEFT, out);
            out << endl;
        }

        // Draw insides of squares
        for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
            out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD;

            // Output column number of left padding
            if (line == 1) {
                out << repeat(SPACE, BOARD_LEFT_MARGIN - 3);
                out << std::setw(2) << row + 1;
                out << SPACE;
            } else {
                out << repeat(SPACE, BOARD_LEFT_MARGIN);
            }

            // Iterate columns
            for (size_t column = 0; column < this->columns; ++column) {
                out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
                const BoardSquare& square = this->squares.at(row).at(column);
                bool is_start = this->start.row == row && this->start.column == column;

                // Figure out background color
                if (square.word_multiplier == 2) {
                    out << BG_COLOR_WORD_MULTIPLIER_2X;
                } else if (square.word_multiplier == 3) {
                    out << BG_COLOR_WORD_MULTIPLIER_3X;
                } else if (square.letter_multiplier == 2) {
                    out << BG_COLOR_LETTER_MULTIPLIER_2X;
                } else if (square.letter_multiplier == 3) {
                    out << BG_COLOR_LETTER_MULTIPLIER_3X;
                } else if (is_start) {
                    out << BG_COLOR_START_SQUARE;
                }

                // Text
                if (line == 0 && is_start) {
                    out << "  \u2605  ";
                } else if (line == 0 && square.word_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'W' << std::setw(1)
                        << square.word_multiplier;
                } else if (line == 0 && square.letter_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'L' << std::setw(1)
                        << square.letter_multiplier;
                } else if (line == 1 && square.has_tile()) {
                    char l = square.get_tile_kind().letter == TileKind::BLANK_LETTER ? square.get_tile_kind().assigned
                                                                                     : ' ';
                    out << repeat(SPACE, 2) << FG_COLOR_LETTER << square.get_tile_kind().letter << l
                        << repeat(SPACE, 1);
                } else if (line == SQUARE_INNER_HEIGHT - 1 && square.has_tile()) {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH - 1) << FG_COLOR_SCORE << square.get_points();
                } else {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH);
                }
            }

            // Add vertical line
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_OUTSIDE_BOARD << std::endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << endl << rang::style::reset << std::endl;
}
