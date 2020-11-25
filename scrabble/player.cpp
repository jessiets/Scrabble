#include "player.h"
#include <stdexcept>

using namespace std;

//private member 
	//string name
	//hand_size
	//points
	//TileCollection player_tiles

// TODO: implement member functions
// Adds points to player's score
void Player::add_points(size_t points){
	this->points += points;
}

// Subtracts points from player's score
void Player::subtract_points(size_t points){
	//check if points will be lower than 0 after subtraction
	if(this->points < points)
		this->points = 0;
	else
		this->points = this->points - points;
}

// return player points
size_t Player::get_points() const{
	return points;
}

//return player's name
const string& Player::get_name() const{
	return name;
}

// Returns the number of tiles in a player's hand.
size_t Player::count_tiles() const{
	// based on the TileCollection composition variable
	return player_tiles.count_tiles();
}

// Removes tiles from player's hand.
void Player::remove_tiles(const std::vector<TileKind>& tiles){
	// n stores the size of the tiles vector
	size_t n = tiles.size();
	for(size_t i=0; i<n; i++)
		player_tiles.remove_tile(tiles[i]);
		
	/*catch(out_of_range& message){
		cout << message.what() << endl;*/
}

// Adds tiles to player's hand.
void Player::add_tiles(const std::vector<TileKind>& tiles){
	size_t n = tiles.size();
	for(size_t i=0; i<n; i++){
		player_tiles.add_tile(tiles[i]);
	}
}

// Checks if player has a matching tile.
bool Player::has_tile(TileKind tile){
	try{
		if(tile == player_tiles.lookup_tile(tile.letter))
			return true;
	}
	catch(out_of_range& message){
		return false;
	}
}

// Returns the total points of all tiles in the players hand.
unsigned int Player::get_hand_value() const{
	return player_tiles.total_points();
}

size_t Player::get_hand_size() const{
	return hand_size;
}



