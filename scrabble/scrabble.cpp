#include "scrabble.h"
#include "formatting.h"
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;


// Given to you. this does not need to be changed
Scrabble::Scrabble(const ScrabbleConfig& config)
    : hand_size(config.hand_size)
    , minimum_word_length(config.minimum_word_length)
    , tile_bag(TileBag::read(config.tile_bag_file_path, config.seed))
    , board(Board::read(config.board_file_path))
    , dictionary(Dictionary::read(config.dictionary_file_path)) {}

//added add_player function
void Scrabble::add_players() {
    size_t player_num = 0;
    string name;
    char player_type;
    cout << "How many players are playing? ";
    cin >> player_num;
    cout << player_num << " players confirmed." << endl;
    //assign the private member with the number of players in the game
    num_player = player_num;
    num_human_player = num_player;

    for(size_t i=0; i<player_num; i++){
        cout << "Enter player " << i+1 << " name: ";
        cin >> ws;
        getline(cin, name);
        cout << "Is " << name << " a computer player? (y/n) ";
        cin >> player_type;
        if(player_type == 'y'){
            shared_ptr<Player> npc = make_shared<ComputerPlayer>(name, hand_size);
            players.push_back(npc);
            cout << "Computer player " << i+1 << " named '" << name << "' has been added." << endl;
            //decremenet count of human player
            num_human_player--;
        }
        else{
            //create a human shared_ptr player object and push it onto player vector
            shared_ptr<Player> newPlayer = make_shared<HumanPlayer>(name, hand_size);
            players.push_back(newPlayer);
            cout << "Player " << i+1 << " named '" << name << "' has been added." << endl;
        }
    }
}

// Game Loop should cycle through players and get and execute that players move
// until the game is over.
void Scrabble::game_loop() {
    // game ends: 
        //1. There's no tiles in TileBag, and one player runs out of tiles
        //2. All players pass the turn
    size_t pass_num;    //number of passes per round
    size_t max_hand = hand_size;
    bool player_out_of_tiles = false;

    while((tile_bag.count_tiles()>0||(!player_out_of_tiles))  &&  pass_num!=num_human_player){
        pass_num = 0;   //set pass to 0 at the beginning of each round
        //each for loop iteration represents one round
        for(size_t i=0; i<num_player; i++){
            //distribute player hands
            size_t hand_count = players[i]->count_tiles();
            //pull random tiles from the tile bag
            vector<TileKind> hand_tiles = tile_bag.remove_random_tiles(max_hand - hand_count);
            //assign these random tiles to player
            players[i]->add_tiles(hand_tiles);

            //after distribution, check if there is any player that runs out of tiles
            if(players[i]->count_tiles() == 0)
                player_out_of_tiles = true;

        //need to show in each round: 
            //1. board
            //2. player's hand tiles
            //3. current score of the game
            //4. ask for player's move
            //5. display the result of their move: words formed, points earned, new letters picked up
            //6. press RETURN to continue to next player
            board.print(cout);      //print board
            cout << "Turn: " << players[i]->get_name() << endl;
           //print player's hand, prompt and get player's move
            Move player_move = players[i]->get_move(board, dictionary);
            //if player chooses to place tiles, show them their score
            if(player_move.kind == MoveKind::PLACE){
                //show the score
                PlaceResult tiles_placed = board.place(player_move);
                //add points to player's points
                players[i]->add_points(tiles_placed.points);
                //get bonus if the player uses all hard tiles in this move
                if(player_move.tiles.size() == max_hand){
                    players[i]->add_points(50);
                }
                cout << "You gained " << SCORE_COLOR << players[i]->get_points() << rang::style::reset << " points!" << endl;
            }
            else if(player_move.kind == MoveKind::EXCHANGE){
                //put player's tiles back in tile_bag
                size_t num_exchange = player_move.tiles.size();     //get total number of exchanged tiles
                for(size_t i=0; i<num_exchange; i++){
                   tile_bag.add_tile(player_move.tiles[i]);         //add tiles back to tile bag
                }
                players[i]->remove_tiles(player_move.tiles);         //remove tiles from player's hand

                //then draw from tile_bag
                vector<TileKind> hand_tiles = tile_bag.remove_random_tiles(num_exchange);
                //assign the newly drawn tiles to player's hand
                players[i]->add_tiles(hand_tiles);
            }
            else{
                //player chooses to pass their turn
                //only increment pass_num if it's human player
                if(players[i]->is_human())
                    pass_num++;
            }
            cout << "Your current score: " << SCORE_COLOR << players[i]->get_points() << rang::style::reset << endl;
            cout << endl << "Press [enter] to continue." << endl;;
            cin.ignore(256, '\n');
        }
    }
    
    //game ends

    // Useful cout expressions with fancy colors. Expressions in curly braces, indicate values you supply.
    // cout << "You gained " << SCORE_COLOR << {points} << rang::style::reset << " points!" << endl;
    // cout << "Your current score: " << SCORE_COLOR << {points} << rang::style::reset << endl;
    // cout << endl << "Press [enter] to continue.";
}

// Performs final score subtraction. Players lose points for each tile in their
// hand. The player who cleared their hand receives all the points lost by the
// other players.
void Scrabble::final_subtraction(vector<shared_ptr<Player>> & plrs) {
    // TODO: implement this method.
    // Do not change the method signature.
    unsigned int hand = 0;      //value of total hand tiles the player has left
    size_t total_sub = 0;       //keeps track of the total points being subtracted
    size_t p_num = plrs.size(); //gets the number of players in the game
    int no_tiles = 0;     //keeps track of the player who runs out of tiles by the their number
    //go through the loop to subtract players' points and find out the winner
    for(size_t i=0; i<p_num; i++){
        //look at the number of player_tiles each player have left
        hand = plrs[i]->get_hand_value();
        //player is the winner
        if(hand == 0){
            no_tiles = i+1;      //no_tiles stores the number the player is associated with (base at index 1)
        }
        //player gets points subtracted
        else{
            //subtract player's points
            (plrs[i])->subtract_points(hand);
            //add to the total subtraction value
            total_sub+=hand;
        }
    }
    //if there is a no_tiles player
    if(no_tiles > 0){
        //add the total_sub points to the no_tiles player
        plrs[no_tiles-1]->add_points(total_sub);
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
		cout << SCORE_COLOR << player->get_points() << rang::style::reset << " | " << PLAYER_NAME_COLOR << player->get_name() << rang::style::reset << endl;
	}
}

// You should not need to change this.
void Scrabble::main() {
    add_players();
    game_loop();
    final_subtraction(this->players);
    print_result();
}
