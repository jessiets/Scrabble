# ScrabbleGame
## About: <br> <i>A two-part class project on creating the game of Scrabble. </i>

 This project is completed in the language of C++ and focuses on the implementation of 
<li> object orientated design </li>
<li> STL classes </li>
<li> graphs: tries </li>
<li> backtracking search </li>
<li> exception error </li>

## Project Description
Scrabble (and its clones like “Words with Friends”) is a game for typically 2-4 players, 
in which the <b>players take turns placing tiles on a board, forming correct words, and gaining points for their words</b>.
However, to add more variety to the game, we expanded the program to include <b>1-8 human players</b>. 
Of course, computer players can also be included under the condition that there must be at least one human player.

## Game Rules
There are 3 kinds of moves a player can make: <strong>place a word, exchange tiles,</strong> or <strong> pass </strong>. <br> Below are a few rules to keep in mind of when playing the game: <br> </br>
If you choose to place a word, <br>
<ul>
  <li>Your tiles, together with tiles that are on the board, must form a single contiguous horizontal or vertical line - no gaps allowed.</li>
  <li>You cannot place a tile on top of another previously placed tile.</li>
  <li>Except for the first move of the game, at least one of the placed tiles must be adjacent (horizontally or vertically) to a previously placed tile.</li>
  <li>For the first move of the game, one of your tiles must be on the start square.</li>
  <li>Each maximal sequence (meaning that on both ends, it is bordered by an empty square or the end of the board) of two or more letters that is formed by your placement of tiles (notice that there could be multiple) must be a legal word according to the dictionary (see below).
Sequences are read left to right, and top to bottom.
Single letters are not considered words.</li>
  <li>The moment you place a word including one or more blank tiles, those blank tiles are assigned letters (by you), which they keep for the rest of the game.</li>
</ul>

<p>Instead of placing a word, you may <strong>exchange anywhere from 1 to <code class="highlighter-rouge">hand_size</code></strong> of your tiles with the bag.
When you do this, you first return the tiles, and then you draw the same number of tiles from the bag to replace them (possibly getting some of the same tiles back).
This ends your move.</p>

<p>Lastly, <strong>you may pass your turn</strong> and not do anything.</p>


## Start, Process, and End of Game
<ul>
  <li>First, the program should ask you about who’s playing. 
  <p>After supplying the player name (e.g. Tommie), the program should ask if that player is a computer (y/n).
    “y” indicates computer.
    “n” indicates human.
    If  users do not specify, the default player should be a human player.</p>
</li>
  <li>Play proceeds in turn by player order.</li>
  <li>The game ends in one of two ways:
    <ul>
      <li>Play finishes once a player runs out of letters when all tiles have already been taken from the bag.</li>
      <li>Play can also finish if every single player passes their turn without any other moves occurring in that span.</li>
    </ul>
  </li>
  <li>When the game ends, each other player subtracts from their scores the sum of points of all tiles that they have in their hands.
If the game ended because a player ran out of tiles, that player gets to add the sum of all other players’ tiles to his/her score.</li>
</ul>

## Syntax Rules for Input
<li>The player can enter one of the three following moves (commands could be in upper or lower case, shown in uppercase):
    <ul>
      <li>
<code class="highlighter-rouge">PASS</code>: the player simply passes his/her turn.</li>
      <li>
<code class="highlighter-rouge">EXCHANGE &lt;string of tiles&gt;</code>: the player wishes to discard these tiles back into the bag, 
drawing the same number of new tiles to replace them.</li>
      <li>
<code class="highlighter-rouge">PLACE &lt;dir&gt; &lt;row&gt; &lt;column&gt; &lt;string of tiles&gt;</code>: the player wishes to place a word on the board.
The first tile will be placed in the given row and column (where counting starts at 1, not 0).
Row 1 is the top row of the board, and column 1 is the leftmost column of the board.
<code class="highlighter-rouge">-</code> indicates that tiles will be placed horizontally, and <code class="highlighter-rouge">|</code> indicates that tiles will be placed vertically.
The string of tiles should be tiles from the player’s hand (no spaces), in the order in which they are to be placed.
If the player wishes to use a blank tile, he/she uses <code class="highlighter-rouge">?</code>, immediately followed by the character to be used for the blank.
Note that if the player wants to start a word using an already placed letter or letters, then the user chooses <code class="highlighter-rouge">&lt;row&gt;</code> and <code class="highlighter-rouge">&lt;column&gt;</code> according to the first letter they place, not the first letter of the word.</li>
    </ul>
  </li>
  <li>Once a player makes a legal move, the word is permanently placed on the board.
  <li>Press <b>return</b> to continue.</li>
</ul>

## Examples of Input Commands
<ul>
  <li>
<code class="highlighter-rouge">PASS</code>: pass the turn.</li>
  <li>
<code class="highlighter-rouge">EXCHANGE aa?</code>: put two tiles of the letter <code class="highlighter-rouge">a</code> and a blank tile back into the bag and draw three new tiles.</li>
  <li>
<code class="highlighter-rouge">PLACE | 3 3 CAKE</code>: put the tiles <code class="highlighter-rouge">C</code>, <code class="highlighter-rouge">A</code>, <code class="highlighter-rouge">K</code>, <code class="highlighter-rouge">E</code> down in order, starting at row 3, column 3, going down.</li>
  <li>
<code class="highlighter-rouge">PLACE - 5 1 POE?MON</code>: imagining that the previous move (CAKE) has been executed, and no other relevant tiles are around:
 place a horizontal word starting in row 5, column 1 (left border of the board), placing the tile <code class="highlighter-rouge">P</code> there, <code class="highlighter-rouge">O</code> in row 5, column 2, <code class="highlighter-rouge">E</code> in row 5, column 4, placing a blank tile that is interpreted as an <code class="highlighter-rouge">M</code> in row 5, column 5, and so on.
 This forms the word <code class="highlighter-rouge">POKEMON</code> with the <code class="highlighter-rouge">K</code> that was already there at row 5, column 3.</li>
</ul>
<p>Notice that when placing tiles, you do <strong>not</strong> specify the tiles that your word will also be using which are already on the board, only the ones you are adding (in order).</p>

## Running the Game
<p>Since this program requires some text files and configuration, there is a folder called <b>config </b> that has all the required data to start the program. 
Simply type the following to start the program! </p>
First type <code class="highlighter-rouge">make</code> <br>
Then <div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code> ./scrabble &lt;configuration-file-name&gt; </code>
</pre></div></div>

<br> </br>
<p> That's it for the instructions, hope you enjoy the game~ </p>

