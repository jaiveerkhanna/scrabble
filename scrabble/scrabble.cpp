#include "scrabble.h"

#include "formatting.h"
#include <iomanip>
#include <iostream>
#include <map>

using namespace std;

// Given to you. this does not need to be changed
Scrabble::Scrabble(const ScrabbleConfig& config)
        : hand_size(config.hand_size),
          minimum_word_length(config.minimum_word_length),
          tile_bag(TileBag::read(config.tile_bag_file_path, config.seed)),
          board(Board::read(config.board_file_path)),
          dictionary(Dictionary::read(config.dictionary_file_path)) {}

// Game Loop should cycle through players and get and execute that players move
// until the game is over.
void Scrabble::game_loop() {
    // TODO: implement this.
    bool game_on = true;
    size_t move_count = 0;
    size_t passes_in_row = 0;
    // Below is code to keep iterating through the turns
    while (game_on) {
        // figure out which player it is
        int curr_player = move_count % players.size();

        if (!players[curr_player]->is_human()) {
            cout << "Computer Player " << players[curr_player]->get_name() << "'s move" << endl;
        }

        // Catch all the exception here, display error message, and then retry input
        Move curr_move = players[curr_player]->get_move(board, dictionary);

        // What to be done if the player Passes
        if (curr_move.kind == MoveKind::PASS) {
            // only count human player passes
            if (players[curr_player]->is_human()) {
                passes_in_row++;
            }
            if (passes_in_row == num_human_players) {
                game_on = false;
            }
            move_count += 1;
            cout << "Your current score: " << SCORE_COLOR << players[curr_player]->get_points() << rang::style::reset
                 << endl;
            cout << endl << "Press [enter] to continue.";
            cin.ignore();
            continue;
        } else {
            // only reset the passes if a human does not pass
            if (players[curr_player]->is_human()) {
                passes_in_row = 0;
            }
        }

        // Update board, remove tiles, add points generated
        if (curr_move.kind == MoveKind::PLACE) {
            PlaceResult result = board.place(curr_move);
            players[curr_player]->remove_tiles(curr_move.tiles);
            players[curr_player]->add_points(result.points);
            // If player places a full hand of tiles)
            if (curr_move.tiles.size() == hand_size) {
                players[curr_player]->add_points(50);
            }
            cout << "You gained " << SCORE_COLOR << result.points << rang::style::reset << " points!" << endl;
        }
        // Remove tiles from player, add back to TileBag
        else if (curr_move.kind == MoveKind::EXCHANGE) {
            players[curr_player]->remove_tiles(curr_move.tiles);
            for (size_t i = 0; i < curr_move.tiles.size(); i++) {
                // curr_move.tiles[i].assigned = '\0'; // Handles the returning blank tiles
                tile_bag.add_tile(curr_move.tiles[i]);
            }
        }

        // This will ensure player draws correct amount of tiles randomly from tilebag
        size_t tiles_used = curr_move.tiles.size();
        // If this move uses up more tiles than remaining, game should be over
        if (tiles_used >= tile_bag.count_tiles()) {
            players[curr_player]->add_tiles(tile_bag.remove_random_tiles(tile_bag.count_tiles()));
        } else {
            players[curr_player]->add_tiles(tile_bag.remove_random_tiles(tiles_used));
        }
        // If player has no tiles after he has had the opportunity to draw ... game_over
        if (players[curr_player]->count_tiles() == 0 && tile_bag.count_tiles() == 0) {
            game_on = false;
            continue;
        }
        move_count += 1;
        cout << "Your score: " << SCORE_COLOR << players[curr_player]->get_points() << rang::style::reset << endl
             << endl;

        if (players[curr_player]->is_human()) {
          cout << endl << "Press [enter] to continue.";
          cin.ignore();
        }

    }
    // Useful cout expressions with fancy colors. Expressions in curly braces, indicate values you supply.
    // cout << "You gained " << SCORE_COLOR << {points} << rang::style::reset << " points!" << endl;
    // cout << "Your current score: " << SCORE_COLOR << {points} << rang::style::reset << endl;
    // cout << endl << "Press [enter] to continue.";
}

// Performs final score subtraction. Players lose points for each tile in their
// hand. The player who cleared their hand receives all the points lost by the
// other players.
void Scrabble::final_subtraction(vector<shared_ptr<Player>>& plrs) {
    // TODO: implement this method.
    // Do not change the method signature.
    size_t running_total = 0;
    shared_ptr<Player> emptied_hand = nullptr;
    for (size_t i = 0; i < plrs.size(); i++) {
        // If this is the player who emptied their hand
        if (plrs[i]->count_tiles() == 0) {
            emptied_hand = plrs[i];
        }

        // if hand is worth more than points, set points to 0
        if (plrs[i]->get_points() < plrs[i]->get_hand_value()) {
            running_total += plrs[i]->get_points();
            plrs[i]->subtract_points(plrs[i]->get_points());
        } else {
            size_t hand_value = plrs[i]->get_hand_value();
            plrs[i]->subtract_points(hand_value);
            running_total += hand_value;
        }
    }
    // Add all subtracted points to the one who emptied hand
    if (emptied_hand != nullptr) {
        emptied_hand->add_points(running_total);
    }
}

// You should not need to change this function.
void Scrabble::print_result() {
    // Determine highest score
    size_t max_points = 0;
    for (auto player : this->players) {
        if (player->get_points() > max_points) {
            max_points = player->get_points();
        }
    }

    // Determine the winner(s) indexes
    vector<shared_ptr<Player>> winners;
    for (auto player : this->players) {
        if (player->get_points() >= max_points) {
            winners.push_back(player);
        }
    }

    cout << (winners.size() == 1 ? "Winner:" : "Winners: ");
    for (auto player : winners) {
        cout << SPACE << PLAYER_NAME_COLOR << player->get_name();
    }
    cout << rang::style::reset << endl;

    // now print score table
    cout << "Scores: " << endl;
    cout << "---------------------------------" << endl;

    // Justify all integers printed to have the same amount of character as the high score, left-padding with spaces
    cout << setw(static_cast<uint32_t>(floor(log10(max_points) + 1)));

    for (auto player : this->players) {
        cout << SCORE_COLOR << player->get_points() << rang::style::reset << " | " << PLAYER_NAME_COLOR
             << player->get_name() << rang::style::reset << endl;
    }
}

// You should not need to change this.
void Scrabble::main() {
    add_players();
    game_loop();
    final_subtraction(this->players);
    print_result();
}

// Query and add players
void Scrabble::add_players() {
    int num_players = 0;
    // initialize human players counter
    num_human_players = 0;
    string player_name = "";
    cout << "Please enter the number of players:  ";
    cin >> num_players;
    if (num_players < 1 || num_players > 8) {
        throw FileException("Invalid number of players");
    }
    cin.ignore();
    cout << num_players << " players confirmed." << endl;
    // cerr << num_players << endl; -> this is working

    // For loop to take player name and instantiate player, then add to players vector
    for (int i = 0; i < num_players; i++) {
        cout << "Please enter a name for player " << i + 1 << ":  " << endl;
        getline(cin, player_name);

        string computer_player;

        // query user to see if it is a human or computer
        cout << "Is this player a computer (y/n)?" << endl;
        getline(cin, computer_player);

        // Create either computer player or human player depending on input
        shared_ptr<Player> temp_player = nullptr;
        if (computer_player == "y") {
            temp_player = make_shared<ComputerPlayer>(player_name, hand_size);
        } else {
            num_human_players++;
            temp_player = make_shared<HumanPlayer>(player_name, hand_size);  // Create new player
        }

        temp_player->add_tiles(tile_bag.remove_random_tiles(hand_size));
        this->players.push_back(temp_player);  // track the new player
        cout << "Player " << i + 1 << ", named " << player_name << ", has been added." << endl;
    }
}
