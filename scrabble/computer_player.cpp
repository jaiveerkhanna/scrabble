
#include "computer_player.h"

#include <memory>
#include <string>
using namespace std;

void ComputerPlayer::left_part(
        Board::Position anchor_pos,
        std::string partial_word,
        Move partial_move,
        std::shared_ptr<Dictionary::TrieNode> node,
        size_t limit,
        TileCollection& remaining_tiles,
        std::vector<Move>& legal_moves,
        const Board& board,
        const Dictionary& dictionary) const {
    // HW5: IMPLEMENT THIS

    // Step 1: is call extend_right
    extend_right(anchor_pos, partial_word, partial_move, node, remaining_tiles, legal_moves, board, dictionary);

    // Step 1.5: Need a Base Case to terminate calls to left part
    if (limit == 0) {
        return;
    }

    // Step 2: iterate through nodes that can be reached from current node
    map<char, std::shared_ptr<Dictionary::TrieNode>>::iterator it = node->nexts.begin();
    while (it != node->nexts.end()) {
        char current_char = it->first;

        // We are going to make two types of recursive calls, depending on if
        // we have the char or a blank tile, and using try catch blocks
        try {
            // Check if we have the current tile in our hand

            TileKind current_tile = remaining_tiles.lookup_tile(current_char);
            // Must be careful not to alter the values within this call

            // Add this tile to the new partial move
            Move new_partial_move = partial_move;
            new_partial_move.tiles.push_back(current_tile);

            // Need to change the start location of the move
            if (new_partial_move.direction == Direction::ACROSS) {
                new_partial_move.column--;
            } else {
                new_partial_move.row--;
            }

            // remove tile from hand before recursion
            remaining_tiles.remove_tile(current_tile);

            // make a recrusive call to left_part
            left_part(
                    anchor_pos,
                    partial_word + current_char,
                    new_partial_move,
                    it->second,
                    limit - 1,
                    remaining_tiles,
                    legal_moves,
                    board,
                    dictionary);

            // add tile back to hand after recursion
            remaining_tiles.add_tile(current_tile);
        } catch (std::out_of_range& e) {
            // no need to do anything, just catch the error
        }

        try {
            // Check if we have a blank tile
            TileKind current_tile = remaining_tiles.lookup_tile('?');
            // Must be careful not to alter the values within this call

            // remove tile from hand before recursion
            remaining_tiles.remove_tile(current_tile);

            // Add this tile to the new partial move
            current_tile.assigned = it->first;
            Move new_partial_move = partial_move;
            new_partial_move.tiles.push_back(current_tile);
            // Need to change the start location of the move
            if (new_partial_move.direction == Direction::ACROSS) {
                new_partial_move.column--;
            } else {
                new_partial_move.row--;
            }

            // make a recursive call to left_part
            left_part(
                    anchor_pos,
                    partial_word + current_char,
                    new_partial_move,
                    it->second,
                    limit - 1,
                    remaining_tiles,
                    legal_moves,
                    board,
                    dictionary);

            // add blank tile back to hand after recursion
            current_tile.assigned = '\0';
            remaining_tiles.add_tile(current_tile);
        } catch (std::out_of_range& e) {
            // no need to do anything, just catch the error
        }

        it++;
    }
}

void ComputerPlayer::extend_right(
        Board::Position square,
        std::string partial_word,
        Move partial_move,
        std::shared_ptr<Dictionary::TrieNode> node,
        TileCollection& remaining_tiles,
        std::vector<Move>& legal_moves,
        const Board& board,
        const Dictionary& dictionary) const {
    // HW5: IMPLEMENT THIS

    // Step 1: Check if the current square is empty or out of bounds
    if ((!board.is_in_bounds(square) || !board.in_bounds_and_has_tile(square))) {
        // If the word generated thus far is valid, add the partial move
        if (dictionary.is_word(partial_word)) {
            legal_moves.push_back(partial_move);
        }
    }

    // Base Case: Position is out of bounds
    if (!board.is_in_bounds(square)) {
        // fill in
        return;
    }

    // Code to determine next square based on direction
    Board::Position next_square = square;
    if (partial_move.tiles.size() == 0) {
        // no need to change the square if are on the anchor square empty case.
    } else if (partial_move.direction == Direction::ACROSS) {
        next_square.column++;
    } else if (partial_move.direction == Direction::DOWN) {
        next_square.row++;
    } else {
        throw out_of_range("Didn't give a valid move direction");
    }

    // Case 1: Next Square is Vacant
    if (board.is_in_bounds(next_square) && !board.in_bounds_and_has_tile(next_square)) {

        // ------- Copy from prefix Left ----//
        map<char, std::shared_ptr<Dictionary::TrieNode>>::iterator it = node->nexts.begin();
        while (it != node->nexts.end()) {
            char current_char = it->first;
            // We are going to make two types of recursive calls, depending on if
            // we have the char or a blank tile, and using try catch blocks
            try {
                // Check if we have the current tile in our hand
                TileKind current_tile = remaining_tiles.lookup_tile(current_char);
                // Must be careful not to alter the values within this call

                // Add this tile to the new partial move
                Move new_partial_move = partial_move;
                if (new_partial_move.tiles.size() == 0) {
                    new_partial_move.row = next_square.row;
                    new_partial_move.column = next_square.column;
                }
                new_partial_move.tiles.push_back(current_tile);

                // remove tile from hand before recursion
                remaining_tiles.remove_tile(current_tile);

                // make a recrusive call to extend _right
                extend_right(
                        next_square,
                        partial_word + current_char,
                        new_partial_move,
                        it->second,
                        remaining_tiles,
                        legal_moves,
                        board,
                        dictionary);

                // add tile back to hand after recursion
                remaining_tiles.add_tile(current_tile);
            } catch (std::out_of_range& e) {
                // no need to do anything, just catch the error
            }

            try {
                // Check if we have a blank tile
                TileKind current_tile = remaining_tiles.lookup_tile('?');
                // Must be careful not to alter the values within this call

                // remove tile from hand before recursion
                remaining_tiles.remove_tile(current_tile);

                // Add this tile to the new partial move
                current_tile.assigned = it->first;
                Move new_partial_move = partial_move;
                if (new_partial_move.tiles.size() == 0) {
                    new_partial_move.row = next_square.row;
                    new_partial_move.column = next_square.column;
                }
                new_partial_move.tiles.push_back(current_tile);

                // make a recrusive call to left_part
                extend_right(
                        next_square,
                        partial_word + current_char,
                        new_partial_move,
                        it->second,
                        remaining_tiles,
                        legal_moves,
                        board,
                        dictionary);

                // add blank tile back to hand after recursion
                current_tile.assigned = '\0';
                remaining_tiles.add_tile(current_tile);
            } catch (std::out_of_range& e) {
                // no need to do anything, just catch the error
            }

            it++;
        }
    }

    // Case 2: Next Square is Not Vacant
    if (board.in_bounds_and_has_tile(next_square)) {
        char current_char = board.letter_at(next_square);

        // Check if you can make a word with that letters

        // Check if .find() works
        map<char, std::shared_ptr<Dictionary::TrieNode>>::iterator it = node->nexts.find(current_char);
        // if we found the current character as a child of current node
        if (it != node->nexts.end()) {
            /// Make recursive call to extend right but use the letter there
            Move new_partial_move = partial_move;
            // TileKind next_tile = board.get_tile_at_square(next_square);
            // new_partial_move.tiles.push_back(next_tile);

            // note no change to remaining tiles since we are using from board
            extend_right(
                    next_square,
                    partial_word + current_char,
                    new_partial_move,
                    it->second,
                    remaining_tiles,
                    legal_moves,
                    board,
                    dictionary);
        }
        // Otherwise, do nothing and continue exploring
    }
}

Move ComputerPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
    std::vector<Move> legal_moves;
    std::vector<Board::Anchor> anchors = board.get_anchors();
    // HW5: IMPLEMENT THIS
    // Iterate through anchors
    TileCollection players_hand = this->tiles;
    for (size_t i = 0; i < anchors.size(); i++) {
        // create partial move based on anchor
        vector<TileKind> move_tiles;
        Board::Position move_start = anchors[i].position.translate(anchors[i].direction, 1);
        Move partial_move(move_tiles, move_start.row, move_start.column, anchors[i].direction);

        // Call left part if limit is greater than 0
        if (anchors[i].limit > 0) {
            // special edge case for start move
            if (anchors[i].position == board.start && !board.in_bounds_and_has_tile(board.start)) {
                left_part(
                        anchors[i].position,
                        "",
                        partial_move,
                        dictionary.get_root(),
                        anchors[i].limit - 1,
                        players_hand,
                        legal_moves,
                        board,
                        dictionary);
            } else {
                left_part(
                        anchors[i].position,
                        "",
                        partial_move,
                        dictionary.get_root(),
                        anchors[i].limit,
                        players_hand,
                        legal_moves,
                        board,
                        dictionary);
            }

        }
        // Account for it here and call right part if limit == 0
        else if (anchors[i].limit == 0) {
            Board::Position temp = anchors[i].position;
            string partial_word;
            temp = temp.translate(anchors[i].direction, -1);
            while (board.in_bounds_and_has_tile(temp)) {
                partial_word = board.letter_at(temp) + partial_word;
                temp = temp.translate(anchors[i].direction, -1);
            }
            // Update partial move
            temp = temp.translate(anchors[i].direction, 1);  // get temp to be first tile
            partial_move.row = temp.row;
            partial_move.column = temp.column;
            std::shared_ptr<Dictionary::TrieNode> start_node = dictionary.find_prefix(partial_word);

            extend_right(
                    anchors[i].position,
                    partial_word,
                    partial_move,
                    start_node,
                    players_hand,
                    legal_moves,
                    board,
                    dictionary);
        }
    }
    // after iterating through we will need to go through all the legal moves

    return get_best_move(legal_moves, board, dictionary);
}

Move ComputerPlayer::get_best_move(
        std::vector<Move> legal_moves, const Board& board, const Dictionary& dictionary) const {
    Move best_move = Move();  // Pass if no move found
                              // HW5: IMPLEMENT THIS
    unsigned int highest_points = 0;
    for (size_t i = 0; i < legal_moves.size(); i++) {
        // Not all moves may actually work once being placed
        try {
            PlaceResult curr_move = board.test_place(legal_moves[i]);

            // check place result is Valid
            if (!curr_move.valid) {
                throw MoveException("Invalid Move");
            }
            // Check that all the words created are Valid:
            for (size_t i = 0; i < curr_move.words.size(); i++) {
                if (!dictionary.is_word(curr_move.words[i])) {
                    throw MoveException("Move generates invalid word");
                }
            }

            // If the current move uses all tiles, add 50 points.
            if (legal_moves[i].tiles.size() == this->get_hand_size()) {
                curr_move.points += 50;
            }
            // If this move generates more points than current best move, make it the new best move
            if (curr_move.points > highest_points) {
                highest_points = curr_move.points;
                best_move = legal_moves[i];
            }
        } catch (...) {
            continue;
        }
    }

    return best_move;
}
