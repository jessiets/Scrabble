
#include <memory>
#include <iostream>
#include "computer_player.h"
#include <string>

#include "place_result.h"


using namespace std;

/* 	Looks for all possible "prefixes"
	Doesn't directly add to legal_moves
	Call extend_right then recurse
	Look left all the way until anchor.limit
*/
void ComputerPlayer::left_part(Board::Position anchor_pos, 
                               std::string partial_word,
                               Move partial_move, 
                               std::shared_ptr<Dictionary::TrieNode> node, 
                               size_t limit,
                               TileCollection& remaining_tiles,
                               std::vector<Move>& legal_moves,
                               const Board& board) const {
    // HW5: IMPLEMENT THIS
   
   //recurse current tile
	extend_right(anchor_pos, partial_word, partial_move, 
				 node, remaining_tiles, legal_moves, board);

    //base case: look left up till anchor.limit
	if(limit == 0)
		return;
	//recurse with actual tile
	map<char, shared_ptr<Dictionary::TrieNode>>::iterator it;
	//look through each edge(children) of the node
	for(it = node->nexts.begin(); it != node->nexts.end(); it++){
		try{
			//only try building words if we have this tile in hand
			TileKind kind = remaining_tiles.lookup_tile(it->first);

			//remove current tile
			remaining_tiles.remove_tile(kind);
			//update new partial move with updated position and tiles
			partial_move.tiles.push_back(kind);
			
			if(partial_move.direction == Direction::ACROSS)
				partial_move.column -= 1;
			else
				partial_move.row -= 1;
			
			//recurse further to the left
			left_part(anchor_pos, kind.letter+partial_word, partial_move, 
					  it->second, limit-1, remaining_tiles, legal_moves, board);

			//add current tile back in hand
			remaining_tiles.add_tile(kind);
			partial_move.tiles.pop_back();
			if(partial_move.direction == Direction::ACROSS)
				partial_move.column += 1;
			else
				partial_move.row += 1;
		}

		catch(out_of_range& message) {
		}

		//recurse blank tile
		try{
			TileKind blank = remaining_tiles.lookup_tile('?');
			
			//remove current tile
			remaining_tiles.remove_tile(blank);
			//assign the blank tile before passing to recursive call
			blank.assigned = it->first;

			//update partial move with updated position 
			partial_move.tiles.push_back(blank);

			if(partial_move.direction == Direction::ACROSS)
				partial_move.column -= 1;
			else
				partial_move.row -= 1;

			//recurse further to the left
			left_part(anchor_pos, blank.assigned+partial_word, partial_move,
					  it->second, limit-1, remaining_tiles, legal_moves, board);

			//add blank tile back in hand
			remaining_tiles.add_tile(blank);
			partial_move.tiles.pop_back();
			if(partial_move.direction == Direction::ACROSS)
				partial_move.column += 1;
			else
				partial_move.row += 1;

		}
		catch(out_of_range& message) {
		}
	}
	
	
}


/*	Finish building possible moves
	Adds finished moves to legal_moves
	Must account for tiles in the way (middle tiles)
	Recursive call
*/
void ComputerPlayer::extend_right(Board::Position square,
                                  std::string partial_word,
                                  Move partial_move, 
                                  std::shared_ptr<Dictionary::TrieNode> node,
                                  TileCollection& remaining_tiles,
                                  std::vector<Move>& legal_moves,
                                  const Board& board) const {
    // HW5: IMPLEMENT THIS
    // backtracking: 
    /* 	remaining_tiles.remove_tiles(x);	pick one tiles we want to build a word on
    	recurse(remaining_tiles);			try building the word
    	remaining_tiles.add_tiles(x);		add the picked tiles back, and pick the next tile to build a word on
    */

    //base case: position is out of bounds
    /*if(!board.is_in_bounds(square)){
    	return;
    }*/

    //case 1: the next square is vacant
    if(!board.in_bounds_and_has_tile(square)){
	    // if partial word is a valid word, push this move to legal_moves
	    if(node->is_final)
	    	legal_moves.push_back(partial_move);
	    // similar logic in left_part
	    map<char, shared_ptr<Dictionary::TrieNode>>::iterator it;
	    //iterate though each edge(children) of the node 
	    for(it = node->nexts.begin(); it != node->nexts.end(); it++){
	    	try{
		    	//if it->first is a letter in hand, 
		    	TileKind kind = remaining_tiles.lookup_tile(it->first);

	    		//update remaining tiles
	    		remaining_tiles.remove_tile(kind);
	    		//update partial move:
	    		partial_move.tiles.push_back(kind);
				
	    		//recurse to next square position
    			extend_right(square.translate(partial_move.direction), 
    						 partial_word+kind.letter, 
    						 partial_move, it->second, remaining_tiles, legal_moves, board);
    			//revert to orignal state
    			remaining_tiles.add_tile(kind);
    			partial_move.tiles.pop_back();
	    	
		    }
		    catch(out_of_range& message){
		    }

		    //recurse blank tiles
		    try{
				TileKind blank = remaining_tiles.lookup_tile('?');

				//remove current tile
				remaining_tiles.remove_tile(blank);
				//assign the blank tile before passing to recursive call
				blank.assigned = it->first;

				//update partial move 
				partial_move.tiles.push_back(blank);
				
				//recurse to next square position
				extend_right(square.translate(partial_move.direction), partial_word+blank.assigned, partial_move,
						  it->second, remaining_tiles, legal_moves, board);

				//add blank tile back in hand
				remaining_tiles.add_tile(blank);
				partial_move.tiles.pop_back();
			}
			catch(out_of_range& message) {
			}

	    }
	}
    //case 2: the next square has an existing tile
    //look in dictionary to see if a word can be made with the existing tile
	else{
		char exist_letter = board.letter_at(square);
		map<char, shared_ptr<Dictionary::TrieNode>>::iterator it;
		it = node->nexts.find(exist_letter);

		//means that letter is not found on dictionary
		if(it == node->nexts.end())	
			return;
		//means that letter is found on dictionary, recurse
		else{
			extend_right(square.translate(partial_move.direction), 
						 partial_word+exist_letter, 
						 partial_move, it->second, remaining_tiles, legal_moves, board);
			//partial_move is unchanged because this tile already exists on the board
		}
	}
}

Move ComputerPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
	//cout <<"Enter cpu get_move." << endl;
	
	std::vector<Move> legal_moves;
    std::vector<Board::Anchor> anchors = board.get_anchors();
    // HW5: IMPLEMENT THIS
    //get access to computer player's hand tiles
    TileCollection cpu_hand(player_tiles);


    size_t i=0;
    while(i < anchors.size()){
    	// if limit==0, accounts for tiles directly left and call extend_right()
    	//Two cases: 
    	//	1. left tile is out of bounds or an anchor square, thus empty string
    	//	2. left tile is an existing tile
    	if(anchors[i].limit == 0){
    		//the position to the left/up is out of bounds or another anchor square: pass in empty string
    		std::string partialWord = "";
    		//the position to the left/up is an existing tile: pass in all the letters
    		//takes in every letter to the left
    		Board::Position cur_position(0,0);
    		cur_position = anchors[i].position.translate(anchors[i].direction, -1);
    		while(board.in_bounds_and_has_tile(cur_position)){
    			//cout << "curposition " << cur_position.row << " " << cur_position.column << endl;
    			partialWord = board.letter_at(cur_position) + partialWord;
    			//update cur_position
    			cur_position = cur_position.translate(anchors[i].direction, -1);
    		}

    		//create a partial Move object
    		vector<TileKind> t;
    		Move partialMove(t, anchors[i].position.row, anchors[i].position.column, anchors[i].direction);

    		//find the starting node on the dictionary trie
    		shared_ptr<Dictionary::TrieNode> start_node = dictionary.find_prefix(partialWord);
    		//if the prefix is invalid, continue to the next iteration
    		if(start_node != nullptr)
		    	extend_right(anchors[i].position, partialWord, partialMove, 
	    				 	 start_node, cpu_hand, legal_moves, board);
    	}


    	// if limit>0, calls left_part()
    	else if(anchors[i].limit > 0){
    		//partial word is an empty string
    		string partialWord = "";
    		//create partial move: no tiles are placed at this point
    		vector<TileKind> t;
    		Move partialMove(t, anchors[i].position.row, anchors[i].position.column, anchors[i].direction);
    		//find the root node of dictionary trie
    		shared_ptr<Dictionary::TrieNode> start_node = dictionary.find_prefix(partialWord);

    		left_part(anchors[i].position, partialWord, partialMove, start_node, 
    				  anchors[i].limit, cpu_hand, legal_moves, board);
    	}
    	//update to next anchor position on board
    	i++;
    }
	
	return get_best_move(legal_moves, board, dictionary);
}

Move ComputerPlayer::get_best_move(std::vector<Move> legal_moves, const Board& board, const Dictionary& dictionary) const {
	//cout << "Enter get_best_move" << endl;
    Move best_move = Move(); // Pass if no move found	
	// HW5: IMPLEMENT THIS

	// if there is no valid move, computer player PASS its turn
	if(legal_moves.size() == 0){
		return best_move;
	}

	// pick the move that generates highest points
	int highest_points = 0;
	size_t i = 0;
	while(i < legal_moves.size()){
		
		bool valid_words = true;
		PlaceResult valid_move = board.test_place(legal_moves[i]);	//get points for current move

		//TESTING: print out the main word
/*		if(legal_moves[i].row == 4 && legal_moves[i].column == 6){
			cout << "word made at (4,6) is: ";
			for(int m = 0; m< legal_moves[i].tiles.size(); m++)
				cout << legal_moves[i].tiles[m].letter << " ";
			cout << endl;
			//TESTING: prints out all words found at this move (including all extra words)
			for(int m = 0; m<valid_move.words.size(); m++){
				cout << valid_move.words[m] << ",  ";
			}
			cout << endl;
		}*/


		//check if the words in valid move are all in dictionary
		for(int k = 0; k < valid_move.words.size(); k++){
			if(!dictionary.is_word(valid_move.words[k]))
				valid_words = false;
		}
		if(valid_words && valid_move.valid){
			unsigned int update_points = valid_move.points;
			// make sure to take into account bonus 50 points: when player uses all hand tiles
			// if(playerMove.tiles.size() == max_hand) pick this move;
			if(legal_moves[i].tiles.size() == get_hand_size()){
				update_points += 50;
			}
			if(update_points > highest_points){
				highest_points = update_points;
				best_move = legal_moves[i];
			}
			
		}
		//go to next move in legal_moves vector
		i++;
	}

	//TESTING
	/*cout << "Final output: " << endl;
	for(int k=0; k<best_move.tiles.size(); k++){
		cout << best_move.tiles[k].letter << " ";
	}
	cout << "  has points: " << highest_points << endl;*/

	return best_move;	
}
