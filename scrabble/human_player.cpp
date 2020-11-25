#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "place_result.h"
#include "move.h"
#include "exceptions.h"
#include "human_player.h"
#include "tile_kind.h"
#include "formatting.h"
#include "rang.h"

using namespace std;


// This method is fully implemented.
inline string& to_upper(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

Move HumanPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
    //scrabble calls get_move
    //prints out the player's hand
    print_hand(cout);
    // 1. player will enter their move option, which we will call parse_move to decipher
    //    the input string. This will return a Move object
    cout << "Enter your move: ";        //prompt player for move input
    cin >> ws;
    string input;
    getline(cin, input);       //read in input from player
    try{
        Move player_move = parse_move(input);       //create a move object that stores the deciphered
                                                    //version of user's move
        // if the player's move is either a pass or exchange, we directly return the move
        // bc we don't need to place any tiles on the board
        if(player_move.kind == MoveKind::PASS || player_move.kind == MoveKind::EXCHANGE){
            return player_move;
        }

        // 2. test if the returned Move is valid on the board, using test_place in the Board class
        PlaceResult result = board.test_place(player_move);

        // 3. the test_place will return a result in the form of PlaceResult, which would give us 
        //    a vector of all the possible word combinations in this move and total points
        //    (given that if the move is valid; if not, PlaceResult would just return error)
        if(!result.valid){
            //if the move is an invalid one, prompt the user for a new move
            //we can just throw an exception because we have the catch block already ready on the bottom
            throw invalid_argument(result.error);
        }
        // 4. check to see if all the words exist in the dictionary
        for(size_t i=0; i<result.words.size(); i++){
            cout << "Word: " << result.words[i] << endl;
            if(dictionary.is_word(result.words[i]) == false)
                throw CommandException("Word not found on dictionary.");
        }
        // if the move is valid and all the words exist in the dictionary, 
        // display the words and return the move
        cout << "Words formed: ";
        for(size_t i=0; i<result.words.size(); i++){
            cout << result.words[i] << " ";
        }
        cout << endl;
        //if the player uses all hand tiles in this move, get extra 50 points
        /*if(player_move.tiles.size() == get_hand_size()){
            move_points += 50;
        }*/
        return player_move;
    }

    //exception thrown from parse_tiles
    catch(invalid_argument& message){
        cerr << message.what() << "\nPlease enter a new move." << endl;
        // call the get_move function again
        return get_move(board, dictionary);
    }
    
    //exception thrown from parse_move
    catch(CommandException& error){
        cerr << error.what() << "\nPlease enter a new move." << endl;
        // call the get_move function again
        return get_move(board, dictionary);
    }
    //exception thrown from lookup_tile in TileCollection
    catch(out_of_range& error){
        cerr << error.what() << "\nPlease enter a new move." << endl;
        return get_move(board, dictionary);
    }

}

vector<TileKind> HumanPlayer::parse_tiles(string& letters) const{
    
    // n stores the size of letters
    size_t n = letters.size();
    
    // create vector to store parsed tiles
    vector<TileKind> parsed_tiles;
    // create a map to keep track of repeating letters in player hand
    map<char, size_t> repeat_letter;

    for(size_t i=0; i<n; i++){
        try{
            //checking if player is attempting to use more tiles than they have
            map<char, size_t>::iterator it = repeat_letter.find(letters[i]);
            if(it == repeat_letter.end()){      //key not found
                TileKind kind = player_tiles.lookup_tile(letters[i]);
                repeat_letter[letters[i]] = 1;
                if(kind.letter == '?'){
                        kind.assigned = letters[i+1];
                        i++;
                    }
                parsed_tiles.push_back(kind);
            }
            else{
                if(player_tiles.count_tiles(player_tiles.lookup_tile(letters[i])) >= it->second+1){
                    it->second++;
                    TileKind kind = player_tiles.lookup_tile(letters[i]);
                    //check if this is the blank tile
                    if(kind.letter == '?'){
                        kind.assigned = letters[i+1];
                        i++;
                    }
                    parsed_tiles.push_back(kind);
                }
                else{
                    throw out_of_range("Not enough tiles.");
                }
            }
        }
        catch(out_of_range& message){
            // if the tile doesn't exist in the player's hand, throw an exception
            throw message;
        }
    }
    // if the for loop ends successfully, insert all the tiles in vector back to player's hand
    return parsed_tiles;
}

Move HumanPlayer::parse_move(string& move_string) const {
	// read in the entire user input string into ss
    // user input format: PLACE <dir> <row> <column> <string of tiles>
    stringstream ss(move_string);
    // opton stores the player's move (PASS, EXCHANGE, PLACE)
    string option;
    ss >> option;
    option = to_upper(option);    //make player's move all upper case
    // check for matching move
    if(option == "PASS"){
        // call the Move constructor for PASS
        Move m;
        return m;
    }
    else if(option == "PLACE"){
        char d;     // stores direction
        int r, c;   // stores row and columns
        string letters;
        ss >> d >> r >> c >> letters;
        //check to see if input matches expected
        if(ss.fail()){
            throw CommandException("Invalid input.");
        }
        // create Direction struct to store the appropriate direction
        Direction dir;
        if(d == '-'){
            dir = Direction::ACROSS;
        }
        else if(d == '|'){
            dir = Direction::DOWN;
        }
        else {
            // direction not specified
            throw CommandException("Missing direction.");
        }


        // create vector to store the tiles you are playing
        vector<TileKind> play_tile;
        play_tile = parse_tiles(letters);

        // call the Move constructor for PLACE
        Move m(play_tile, r-1, c-1, dir);   //set r and c to start at index 0
        return m;
    }
    else if(option == "EXCHANGE"){
        string letters;     // store the letters the player wants to replace
        ss >> letters;
        // check to see if input match expected
        if(ss.fail()){
            throw CommandException("Invalid move.");
        }
        //create vector to store the tiles you are replacing
        vector<TileKind> replace_tile;
        replace_tile = parse_tiles(letters);
        // call the Move constructor for EXCHANGE
        Move m(replace_tile);
        return m;
    }
    else{
        throw CommandException("Invalid move.");
    }
}


// This function is fully implemented.
void HumanPlayer::print_hand(ostream& out) const {
	const size_t tile_count = player_tiles.count_tiles();
	const size_t empty_tile_count = this->get_hand_size() - tile_count;
	const size_t empty_tile_width = empty_tile_count * (SQUARE_OUTER_WIDTH - 1);

	for(size_t i = 0; i < HAND_TOP_MARGIN - 2; ++i) {
		out << endl;
	}

	out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_HEADING << "Your Hand: " << endl << endl;

    // Draw top line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;

    // Draw middle 3 lines
    for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
        out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD << repeat(SPACE, HAND_LEFT_MARGIN);
        for (auto it = player_tiles.cbegin(); it != player_tiles.cend(); ++it) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_PLAYER_HAND;

            // Print letter
            if (line == 1) {
                out << repeat(SPACE, 2) << FG_COLOR_LETTER << (char)toupper(it->letter) << repeat(SPACE, 2);

            // Print score in bottom right
            } else if (line == SQUARE_INNER_HEIGHT - 1) {
                out << FG_COLOR_SCORE << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << setw(2) << it->points;

            } else {
                out << repeat(SPACE, SQUARE_INNER_WIDTH);
            }
        }
        if (player_tiles.count_tiles() > 0) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
            out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << rang::style::reset << endl;
}
