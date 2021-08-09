#include "player.h"

using namespace std;

// TODO: implement member functions

// Adds points to player's score
void Player::add_points(size_t points) { this->points += points; }

// Subtracts points from player's score
void Player::subtract_points(size_t points) { this->points -= points; }

size_t Player::get_points() const { return this->points; }

const std::string& Player::get_name() const { return this->name; }

size_t Player::count_tiles() const { return tiles.count_tiles(); }

void Player::remove_tiles(const std::vector<TileKind>& tiles) {
    for (size_t i = 0; i < tiles.size(); i++) {
        Player::tiles.remove_tile(tiles[i]);
    }
}

void Player::add_tiles(const std::vector<TileKind>& tiles) {
    for (size_t i = 0; i < tiles.size(); i++) {
        Player::tiles.add_tile(tiles[i]);
    }
}

// This function will use the letter of tile and the lookup_tile function to
// search for tile in the tiles. If it catches the error, return false
bool Player::has_tile(TileKind tile) {
    try {
        tiles.lookup_tile(tile.letter);
        return true;  // Code will execute if line above works
    } catch (...) {
        return false;
    }
}

unsigned int Player::get_hand_value() const { return tiles.total_points(); }

size_t Player::get_hand_size() const { return this->hand_size; }
