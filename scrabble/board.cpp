#include "board.h"
#include "board_square.h"
#include "exceptions.h"
#include "formatting.h"
#include <iomanip>
#include <fstream>

using namespace std;


bool Board::Position::operator==(const Board::Position& other) const {
    return this->row == other.row && this->column == other.column;
}

bool Board::Position::operator!=(const Board::Position& other) const {
    return this->row != other.row || this->column != other.column;
}

Board::Position Board::Position::translate(Direction direction) const {
    return this->translate(direction, 1);
}

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
    string r;
    for(size_t i=0; i<board.rows; i++){
    	// read in one full row
    	file >> r;
    	//create a vector of row
    	vector<BoardSquare> row;
    	for(size_t j=0; j<board.columns; j++){
    		// spot goes through each square in the row
    		char spot = r[j];
    		Position temp(i, j);
    		// find matching value of each square on the board
    		//"." means normal square
    		//single digit means letter bonus: multiply letter value by digit value
    		//single char means word bonus: multiply word value by 2(d) or 3(t)
    		if(spot == '.'){
    			row.push_back(BoardSquare(1, 1));
    		}
    		else if(spot == '2'){
    			row.push_back(BoardSquare(2, 1));
    		}
    		else if(spot == '3'){
    			row.push_back(BoardSquare(3, 1));
    		}
    		else if(spot == 'd'){
    			row.push_back(BoardSquare(1, 2));
    		}
    		else if(spot == 't'){
    			row.push_back(BoardSquare(1, 3));
    		}
    		else{
    			//throw an exception if none of the values match the square
    			throw out_of_range("Invalid square.");
    		}
    	}
    	//now that the rows are filled, push them all back onto board
    	board.squares.push_back(row);
    }
    file.close();

    return board;
}

size_t Board::get_move_index() const {
    return this->move_index;
}


// Test_place should verify that the move given as an argument can be placed on the board
// It should return a valid PlaceResult object with appropriate words if so
// and an invalid PlaceResult object with error message otherwise.
PlaceResult Board::test_place(const Move& move) const {
	string extra;
	size_t tile_num = move.tiles.size();	//total number of tiles being placed
	size_t r = move.row;			//starting row position of player's move, set to index 0
	size_t c = move.column;			//starting column position of player's move, set to index 0
	Direction dir = move.direction;	//get the direction of the player's move

	// make sure the starting square is already occupied
	// if the starting square isn't already filled, we need to check if player's move
	// will place a tile on the starting square first
	if(!in_bounds_and_has_tile(start)){
		// keeps track of whether start square is filled
		bool start_filled = false;
		// continue searching for the starting tile placement
		size_t r_bound = r+tile_num;	//the maximum range of r
		size_t c_bound = c+tile_num;	//the maximum range of c
		while((!start_filled) && r < r_bound && c < c_bound){
			//check if the first placement is on the starting square
			if(start.row == r && start.column == c){
				start_filled = true;
			}
			//determine the direction of how the tiles will be placed
			else if(dir == Direction::ACROSS){		//across means: column increments
				c++;
			}
			else{		//down means: row increments
				r++;
			}
		}
		if(start_filled == false)
				return PlaceResult("error - starting square not filled.");
	}

	// make sure the player isn't placing tile on an already existing one
	Position temp(move.row, move.column);	//set temp to be the player's starting position
	if(in_bounds_and_has_tile(temp))
		return PlaceResult("error - cannot place tile over an existing tile.");
	else if(!is_in_bounds(temp))
		return PlaceResult("error - tile out of bound.");
	

	//at this point, the starting square is verified to have a tile and player isn't making a move on existing tile
	//so we can now check for other placement conditions

	// Create a vector to store all possible words after tile placement.
	vector<string> all_words;
	// create a vector to store letters that form a word
	string letter_found = "";
	// Create a bool that makes sure at least one adjacent letter exists.
	bool adj = false;
	// NEED To CHECK:
	//	1. same direction: for each letter, check up/down or check left/right
	//	2. check words that player places
	
	//get opposite direction of player's move
	Direction opp_dir;
	if(dir == Direction::ACROSS)
		opp_dir = Direction::DOWN;
	else
		opp_dir = Direction::ACROSS;
	

	size_t current_r = r;			//current has the row and column of
	size_t current_c = c;				//the start of the move
	unsigned int letter_points = 0;			//store the points of the player's word
	unsigned int total_points = 0;		//store the total points of all the possible words
	unsigned int extra_word_points = 0;
	unsigned int total_extra_word_points = 0;	
	unsigned int total_word_multiplier = 1;		//keep track of the total word multiplier of one word
	unsigned int extra_word_multiplier = 1;		//keep track of word multiplier for extra words

	//going through each letter placed by the player to look for adjacent letters
	size_t tiles_used = 0;
	//first condition checks for player's tiles; second condition checks for tiles around the player's tiles
	while((tiles_used != tile_num && is_in_bounds(temp)) || in_bounds_and_has_tile(temp)){
		
		//check if this position already has a tile on the board
		if(in_bounds_and_has_tile(temp)){
			adj = true;
			//if so, move on to the next position directly
			//increment current_c or current_r to move to the next tile the player places
			if(dir == Direction::ACROSS)
				current_c += 1;
			else
				current_r += 1;

			if(at(temp).get_tile_kind().letter == '?')
				letter_found += at(temp).get_tile_kind().assigned;
			else
				//add found letter to letter_found
				letter_found += at(temp).get_tile_kind().letter;
			//add points
			letter_points += at(temp).get_tile_kind().points;

			//go to the next loop iteration
			temp = Position(current_r, current_c);
			continue;
		}		
		
		
		//add the tile at the current position to letter_found string
		//check for blank tiles
		if(move.tiles[tiles_used].letter == '?')
			//get the assigned letter to the blank tile
			letter_found += move.tiles[tiles_used].assigned;
		else
			//add letter to word if letter is one of player's tiles
			letter_found += move.tiles[tiles_used].letter;

		//add letter points for the main word
		size_t letter_multi = at(temp).letter_multiplier;
		letter_points = letter_points + move.tiles[tiles_used].points * letter_multi;
		//add up word bonus
		total_word_multiplier = total_word_multiplier * at(temp).word_multiplier;

		//by this point, we are on one single letter that the player is playing,
		//we've checked if the letter already exist on the board so we're sure that this letter
		//is one of the player's move tiles
		//now, check for words that form in opposite directions

		string extra_word = "";

		//add the first letter(which is the player's letter) to extra word
		if(move.tiles[tiles_used].letter == '?')
			extra_word = move.tiles[tiles_used].assigned;
		else
			extra_word = move.tiles[tiles_used].letter;
		//assign the player's letter point to extra word (also account for letter bonus)
		extra_word_points = move.tiles[tiles_used].points * letter_multi;
		//add up word bonus
		extra_word_multiplier = extra_word_multiplier * at(temp).word_multiplier;

		//check up (for ACROSS) and left (for DOWN)
		if(dir == Direction::ACROSS)
			temp = temp.translate(Direction::DOWN, -1);
		else
			temp = temp.translate(Direction::ACROSS, -1);

		while(in_bounds_and_has_tile(temp)) {
			//TESTING
			//cout << "check up/left loop." << endl;
			adj = true;
			//check for blank tiles that are already on the board
			if(at(temp).get_tile_kind().letter == '?'){
				//get the assigned letter to the blank tile
				char exist_letter = at(temp).get_tile_kind().assigned;
				//extra_word.insert(0, exist_letter, 1);
				extra_word = exist_letter + extra_word;
			}
			else{
				//get the letter in this position 
				char exist_letter = at(temp).get_tile_kind().letter;
				//concatenate the letter to the word 
				//extra_word.insert(0, exist_letter, 1);
				extra_word = exist_letter + extra_word;
			}
			//add points: since this letter already exist on board, letter_mult doesn't apply
			extra_word_points += at(temp).get_tile_kind().points;
			//TESTING
			//cout << "Extra Word Points(left/up): " << extra_word_points << endl;
			
			//move position to the one above or left of current position
			if(dir == Direction::ACROSS)
				temp = temp.translate(Direction::DOWN, -1);
			else
				temp = temp.translate(Direction::ACROSS, -1);
		}

		//check down (for ACROSS) and right (for DOWN)
		temp = Position(current_r, current_c);
		temp = temp.translate(opp_dir);
		while(in_bounds_and_has_tile(temp)) {
			//TESTING
			//cout << "check down/right loop." << endl;
			adj = true;
			//check for blank tiles that are already on the board
			if(at(temp).get_tile_kind().letter == '?'){
				//get the assigned letter to the blank tile
				char exist_letter = at(temp).get_tile_kind().assigned;
				extra_word = extra_word + exist_letter;
			}
			else{
				//get the letter in this position
				char exist_letter = at(temp).get_tile_kind().letter;
				//push the word onto the letter_found vector
				extra_word = extra_word + exist_letter;
			}
			//add points: since this letter already exist on board, letter_mult doesn't apply
			extra_word_points += at(temp).get_tile_kind().points;
			//move position to one to the right or below the current position
			temp = temp.translate(opp_dir);
		}
		//the extra word for this one player tile has been found
		//push extra word to vector
		if(extra_word.size() > 1){
			all_words.push_back(extra_word);
			//add the bonus for extra word
			extra_word_points *= extra_word_multiplier;
			//sum up the total extra word points
			total_extra_word_points += extra_word_points;
			//TESTING
			//cout << "Extra word points(down/right): " << extra_word_points << endl;
		}
		//reset the value and bonus value for extra word because we're getting a new extra word in the next iteration
		extra_word_multiplier = 1;

		//we have searched for one player tile, now let's move on to next
		tiles_used++;

		//move to next position
		if(dir == Direction::ACROSS)
			current_c+=1;
		else
			current_r+=1;
		//update temp position
		temp = Position(current_r, current_c);
	}
	//player should have used all tiles, if not, it is because one of the tile is out of bound
	if(tiles_used < tile_num){
		//cout << "return statement" << endl;
		return PlaceResult("error - tile out of bound.");
	}


	//at this point, we have found all the extra words associated with the player's move
	//now, check if the player is placing tiles in between existing ones
	
	temp = Position(move.row, move.column);		//temp is at the very beginning of player's move
	temp = temp.translate(dir, -1);				//check to see if there is a letter to the left or above

	while(in_bounds_and_has_tile(temp)){
		adj = true;
		//check for blank tiles
		if(at(temp).get_tile_kind().letter == '?'){
			letter_found = at(temp).get_tile_kind().assigned + letter_found;
		}
		else{
			letter_found = at(temp).get_tile_kind().letter + letter_found;
		}
		//add points: these tiles already exist on the board, so letter mult doesn't apply
		letter_points += at(temp).get_tile_kind().points;

		//update position
		temp = temp.translate(dir, -1);
	}

	all_words.push_back(letter_found);
	total_points = total_extra_word_points + (letter_points * total_word_multiplier);

	//TESTING
	//cout << "Main Word: " << letter_found << " points, " << letter_points << endl;
	//cout << "Extra Word: " << extra << " points, " << total_extra_word_points << endl;
	//cout << "Total points: " << total_points << endl;
	
	//	4. Store the appropriate values on the PlaceResult object and return it
	//		a. false: string& error
	//		b. true: vector<string>& words, unsigned int points	
	//the move is only valid if there is at least one adjacent letter or if it's the first move
	if(adj || (!in_bounds_and_has_tile(start)))
		return PlaceResult(all_words, total_points);
	else
		return PlaceResult("error - a letter must be placed adjacent to a letter on the board.");

}


PlaceResult Board::place(const Move& move) {
    // TODO: place tiles on the board
	
    // make sure to update bonus value on the square to 1 after each player's turn
    // this->at(Position temp) = BoardSquare(1, 1)
	//player's starting position
	size_t starting_row = move.row;
	size_t starting_column = move.column;
	Position cur(starting_row, starting_column);
	//get total number of tiles the player is placing
	size_t n = move.tiles.size();
	//determine the direction
	Direction dir;
	if(move.direction == Direction::ACROSS)
		dir = Direction::ACROSS;
	else
		dir = Direction::DOWN;

	//call test_place again to get the total points
	PlaceResult outcome = test_place(move);

	size_t i = 0;		//keeps track of player move index
	//place player's tiles on the board
	while(i < n){
		//if tile already exists on board
		if(in_bounds_and_has_tile(cur)){
			//we don't need to place any tiles on the board bc they're already there
			//update to next position
			cur = cur.translate(dir);
			continue;
		}
	    //if tile is one of the player's move
	    else{
	  		TileKind player_tile = move.tiles[i];		//get player's tile
	  		//place the tile on board 
	  		at(cur).set_tile_kind(player_tile);	
	  		//update bonus value to 1
	  		at(cur).letter_multiplier = 1;
	  		at(cur).word_multiplier = 1;

	  		i++;		//move to the next letter the player will place
	  		//set to next position
			cur = cur.translate(dir);
	    }
		
	}

	return outcome;
}


/* HW5: IMPLEMENT THIS
Returns the letter at a position. Assumes there is a tile at p. */
char Board::letter_at(Position p) const{
	return at(p).get_tile_kind().letter;
}

/* HW5: IMPLEMENT THIS
Returns bool indicating whether position p is an anchor spot or not.

A position is an anchor spot if it is 
    1) In bounds
    2) Unoccupied
    2) Either adjacent to a placed tile or is the start square
*/
bool Board::is_anchor_spot(Position p) const{
	//check if p is in bounds
	if(is_in_bounds(p)){
		//check if p is not occupied
		if(at(p).has_tile() == false){
			//check if p is the start square
			if(start.row == p.row && start.column == p.column)
				return true;
			//check if p is adjacent to a placed tile
			else{
				bool adj = false;
				//check left
				Position temp(p.row, p.column-1);
				if(in_bounds_and_has_tile(temp))
					adj = true;
				else{
					//check up
					temp = Position(p.row-1, p.column);
					if(in_bounds_and_has_tile(temp))
						adj = true;
					else{
						//check right
						temp = Position(p.row, p.column+1);
						if(in_bounds_and_has_tile(temp))
							adj = true;
						else{
							//check down
							temp = Position(p.row+1, p.column);
							if(in_bounds_and_has_tile(temp))
								adj = true;
						}						
					}
				}
				//return true if an adjacent tile is found
				if(adj)
					return true;
			}

		}
		else{
			//p has a tile, return false
			return false;
		}
	}
	//p is not in bounds, return false
	return false;
} 

/* HW5: IMPLEMENT THIS
Returns a vector of all the Anchors on the board.

For every anchor square on the board, it should include two Anchors in the vector.
    One for ACROSS and one for DOWN
The limit for the Anchor is the number of unoccupied, non-anchor squares preceeding the anchor square in question. 
*/
//used for testing
std::vector<Board::Anchor> Board::get_anchors() const{
	size_t limit_across, limit_down;
	Position temp_across(0,0);
	Position temp_down(0,0);
	vector<Board::Anchor> anchorResult;

	//go through each square on the board
	Position curr(0,0);
	while(is_in_bounds(curr)){
		//if curr is an anchor spot, create an anchor
		if(is_anchor_spot(curr)){
			limit_across = 0;	//reset anchor limit
			limit_down = 0;

			//limit for anchor is "unoccupied, non-anchor squares PRECEEDING the anchor squares"
			if(curr.column > 0){
				//temp is at the position preceeding the anchor
				temp_across = Position(curr.row, curr.column-1);
				//check ACROSS
				while(!in_bounds_and_has_tile(temp_across) && !is_anchor_spot(temp_across)){
					limit_across++;
					if(temp_across.column == 0)		//since position has size_t, it only takes positive value, 
						break;						//so make sure we don't get a negative value after calculation
					
					//update the preceeding position
					temp_across = temp_across.translate(Direction::ACROSS, -1);
				}
			}

			if(curr.row > 0){
				temp_down = Position(curr.row-1, curr.column);
				//check DOWN
				while(!in_bounds_and_has_tile(temp_down) && !is_anchor_spot(temp_down)){
					limit_down++;
					if(temp_down.row == 0)
						break;

					//update the preceeding position
					temp_down = temp_down.translate(Direction::DOWN, -1);
				}
			}

			//TESTING
			//cout << "limit_across: " << limit_across << endl;
			//cout << "limit_down: " << limit_down << endl;

			//create anchor objects
			Anchor an_across(curr, Direction::ACROSS, limit_across);
			Anchor an_down(curr, Direction::DOWN, limit_down);

			//push anchor objects to vector
			anchorResult.push_back(an_across);
			anchorResult.push_back(an_down);
		}

		
		//if we have reached the end of the column, update to go to next row
		if(curr.column == this->columns-1){
			curr = Position(curr.row+1, 0);
		}
		//otherwise, update to go to next column
		else{
			curr = Position(curr.row, curr.column+1);
		}
	}
	return anchorResult;
}



// The rest of this file is provided for you. No need to make changes.

BoardSquare& Board::at(const Board::Position& position) {
    return this->squares.at(position.row).at(position.column);
}

const BoardSquare& Board::at(const Board::Position& position) const {
    return this->squares.at(position.row).at(position.column);
}

bool Board::is_in_bounds(const Board::Position& position) const {
    return position.row < this->rows && position.column < this->columns;
}

bool Board::in_bounds_and_has_tile(const Position& position) const{
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
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'W' << std::setw(1) << square.word_multiplier;
                } else if (line == 0 && square.letter_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'L' << std::setw(1) << square.letter_multiplier;
				} else if (line == 1 && square.has_tile()) {
                    char l = square.get_tile_kind().letter == TileKind::BLANK_LETTER ? square.get_tile_kind().assigned : ' ';
                    out << repeat(SPACE, 2) << FG_COLOR_LETTER << square.get_tile_kind().letter << l << repeat(SPACE, 1);
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
