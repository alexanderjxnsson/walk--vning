/**
 * DUNGEON: a simple game for the terminal. The objective of the
 * game is that the player ("P") reaches the treasure ("X")
 * avoiding the traps ("T") and the bandits ("B").
 * Bandits move randomly each turn.
 * */

/*
1. updatera walk.cpp till kodstandarden så den blir enklare att läsa
2. Dela upp koden i följande 4 sektioner *DECLARATIONS* *INIT* *LOOP* *FUNCTIONS*
3. Updatera   std::cout << "\x1B[2J\x1B[H"; // Resets terminal med vår clearScreen(void) från vår common_lig.cpp
4. Lägg till char currentToRender[10][10]. Gör en ny drawBoard2 som istället för att först skriva till skärmen först gör sin uppdateringslogik direkt på att skriva till currentToRender. 
    Först sedan när allt är skrivet (och skrivet över varandra) så ritas det som finns här upp på skärmen. Tips dvs först måste hela boarden representerad i currentToRender "tömmas" 
    genom att varje plats fyllas med '.' som representerar att ingen bandit, spelar, skatt finns där.
5. Lägg alla Location, Player, Trap, Bandit etc i en world struct som representerar världen.
---------------------------------
6. Gör en generell funktion för movePlayer och moveBandit. Notera att det är exakt samma värden som sätts. (spendera max 30 min gör sist) (för att göra denna behövs inte nr 5 men det är ett sätt att lösa det på)
*/
#include <climits>
#include <ctime>
#include <iostream>
#include <random>
#include <string>
char currentToRender[10][10];
int NUMBEROFTRAPS = 3;
int NUMBEROFBANDITS = 2;
bool match = false;
int xPos;
int yPos;
int maxTurnos = INT_MAX;
bool squareDrawn = false;

struct Location
{
    int xPosition;                  // Represents a place in the board.
    int yPosition;                  // xPosition is the x-axis index and yPosition is the y-axis index
};
struct Player
{
    Location position;              // Represents the player.
    char symbol = 'P';              // It is guaranteed Player position is in the board.
    std::string name = "player";    // Position is altered through function movePlayer.
};
struct Trap
{
    Location position;            // Represents traps on the board
    char symbol = 'T';            // It is guarateed Trap position is in the board.
};
struct Bandit
{
    Location position;            // Represents Bandits moving around the map.
    char symbol = 'B';            // Position is altered through funtion moveBandit.
};
struct Treasure
{
    Location position;            // Represents the treasure.
    char symbol = 'X';            // The game ends as soon Player.position == Treasure.position
};

struct World
{
    struct Bandit 
    {
        struct Location position;
        char symbol = 'B';
    } Bandit;

    struct Location
    {
        int xPosition;
        int yPosition;
    } Location;

    struct Player 
    {
        struct Location position;
        char symbol = 'P';
        std::string name = "player";
    } Player;

    struct Trap 
    {
        struct Location position;
        char symbol = 'T';
    } Trap;

    struct Treasure 
    {
        struct Location position;
        char symbol = 'X';
    };
    int xDimension;               // Represents the board.
    int yDimension;
} board = {.xDimension = 10, .yDimension = 10};

// Possible directions. WRONG_DIRECTION is used to report incorrect input
enum Direction { RIGHT, LEFT, TOP, BOTTOM, WRONG_DIRECTION };
enum Result { VICTORY, DEFEAT };
void drawBoard(Player, Trap[], Bandit[], Treasure);
void drawBoard2(Player, Trap[], Bandit[], Treasure);
void endGame(Result);
void movePlayer(Player &, Direction);
void moveBandit(Bandit &);
Direction askDirection();

int main()
{
    Player player = {.position = {.xPosition = xPos, .yPosition = yPos}};
    Bandit banditsInMap[NUMBEROFBANDITS];
    Trap trapsInMap[NUMBEROFTRAPS];
    // Treasure position is decided randomly.
    Treasure treasure =
    {
      .position = {.xPosition = std::rand() % board.xDimension,
                   .yPosition = std::rand() % board.yDimension}
    };
    // Traps are placed around the map. It is not guaranteed that traps position doesn't converge.
    // In that case, the second trap can be assumed to not exist.
    //Här skulle man kunna lägga till kod som fixar detta.
    for (int i = 0; i < NUMBEROFTRAPS; i++)
    {
        int xPos = std::rand() % board.xDimension;
        int yPos = std::rand() % board.yDimension;
        Trap trap = {.position = {.xPosition = xPos, .yPosition = yPos}};
        trapsInMap[i] = trap;
    }
    // Bandits are placed around the map. It is not guaranteed that bandits position doesn't converge, but they will move anyway.
    for (int i = 0; i < NUMBEROFBANDITS; i++)
    {
        int xPos = std::rand() % board.xDimension;
        int yPos = std::rand() % board.yDimension;
        Bandit bandit = {.position = {.xPosition = xPos, .yPosition = yPos}};
        banditsInMap[i] = bandit;
    }
    do
    {
        xPos = std::rand() % board.xDimension;
        yPos = std::rand() % board.yDimension;
        for (int i = 0; i < NUMBEROFTRAPS; i++)
        {
    	    if ((xPos == trapsInMap[i].position.xPosition && yPos == trapsInMap[i].position.yPosition) ||
        	    (xPos == banditsInMap[i].position.xPosition && yPos == banditsInMap[i].position.yPosition))
    		    {
    			    match = true;
    		    }
        }
    } while (match);
    // The order of the turn is the following:
    // 1. Board is drawn.
    // 2. User is asked for movement direction.
    // 3. Player moves in the chosen direction.
    // 4. Bandits move.
    for (int i = 0; i <= maxTurnos; i++)
    {
        drawBoard2(player, trapsInMap, banditsInMap, treasure);
        drawBoard(player, trapsInMap, banditsInMap, treasure);
        Direction direction;
        do
        {
            direction = askDirection();
            std::cout << std::endl;
        }
        while (direction == WRONG_DIRECTION);
        movePlayer(player, direction);
        for (int i = 0; i < NUMBEROFBANDITS; i++)
        {
            moveBandit(banditsInMap[i]);
        }
        system("cls");;
    }
}
Direction askDirection()
{

  // Asks the user to input a direction and returns it.
  // Precondition: -
  // Poscondition:
  // Return: a Direction value containing the direction chosen or
  // WRONG_DIRECTION.

  std::cout << "Select [L/A]eft, [R/D]ight, [T/W]op or [B/S]ottom: ";
  char answer;
  std::cin.get(answer);

  Direction chosenDirection;
  switch (std::toupper(answer))
  { // Fixa styling
  case 'L':
  case 'A':
    chosenDirection = LEFT;
    break;
  case 'R':
  case 'D':
    chosenDirection = RIGHT;
    break;
  case 'T':
  case 'W':
    chosenDirection = TOP;
    break;
  case 'B':
  case 'S':
    chosenDirection = BOTTOM;
    break;
  default:
    chosenDirection = WRONG_DIRECTION;
  }
  return chosenDirection;
}

void drawBoard(
    /* in */ Player player,
    /* in */ Trap totalTraps[],
    /* in */ Bandit totalBandits[],
    /* in */ Treasure treasure)

// Draws a (board.xDimension * board.yDimension) grid.
// Elements are drawn using .location.?Dimensions.
// Precondition: 0 <= Player.xPosition <= board.xDimension &&
//      0 <= player.position.yPosition <= board.yDimension  &&
//      board.xDimension > 0  && board.yDimension > 0 &&
// Postcondition: The grid has been drawn.
//      All elements have been drawn.
//      If the player is in the same square than the treasure,
//      the game ends with victory.
//      If the player is in the same square than a bandit or
//      a trap, the game ends with defeat.
{
    
    for (int y = 0; y <= board.yDimension; y++)
    {
        for (int x = 0; x <= board.xDimension; x++)
        {
            // Traps are drawn
            for (int z = 0; z <= NUMBEROFTRAPS; z++)
            {
            Trap trapToDraw = totalTraps[z];
            if (trapToDraw.position.xPosition == x && trapToDraw.position.yPosition == y)
            {
                std::cout << trapToDraw.symbol;
                squareDrawn = true;
            }
            }
            // Bandits are drawn.
            // In case of collision with a trap,
            // only the second is drawn.
            for (int z = 0; z <= NUMBEROFBANDITS; z++)
            {
            Bandit banditToDraw = totalBandits[z];
            if (banditToDraw.position.xPosition == x && banditToDraw.position.yPosition == y && !squareDrawn)
            {
                std::cout << banditToDraw.symbol;
                squareDrawn = true;
            }
            }

            // Treasure is drawn. If position of treasure == position of player
            // game ends with victory
            if (x == treasure.position.xPosition && y == treasure.position.yPosition)
            {
                if (treasure.position.xPosition == player.position.xPosition && treasure.position.yPosition == player.position.yPosition)
                {
                endGame(VICTORY);
                }

                std::cout << "X";
                continue;
            }

            if (x == player.position.xPosition && y == player.position.yPosition)
            {
                if (squareDrawn)
                    endGame(DEFEAT);
                    std::cout << "P";
                    continue;
            }
            // Empty square "." is drawn. It only gets printed if there is nothing
            // on the square.
            if (!squareDrawn)
                std::cout << ".";
                squareDrawn = false;
        }
        std::cout << std::endl;
    }
}

void drawBoard2(
    /* in */ Player player,
    /* in */ Trap totalTraps[],
    /* in */ Bandit totalBandits[],
    /* in */ Treasure treasure)
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            currentToRender[i][j] = '.';
        }
    }
    bool squareDrawn = false;
    for (int y = 0; y <= 10; y++)
    {
    for (int x = 0; x <= 10; x++)
    {
    // Traps are drawn
    for (int z = 0; z <= NUMBEROFTRAPS; z++)
    {
        Trap trapToDraw = totalTraps[z];
        if (trapToDraw.position.xPosition == x &&
            trapToDraw.position.yPosition == y)
        {
        //   std::cout << trapToDraw.symbol;
        currentToRender[y][x] = trapToDraw.symbol;
        squareDrawn = true;
        }
    }
    // Bandits are drawn.
    // In case of collision with a trap,
    // only the second is drawn.
    for (int z = 0; z <= NUMBEROFBANDITS; z++)
    {
        Bandit banditToDraw = totalBandits[z];
        if (banditToDraw.position.xPosition == x &&
            banditToDraw.position.yPosition == y && !squareDrawn) 
        {
        //   std::cout << banditToDraw.symbol;
        currentToRender[y][x] = banditToDraw.symbol;
        squareDrawn = true;
        }
    }

    // Treasure is drawn. If position of treasure == position of player
    // game ends with victory
    if (x == treasure.position.xPosition &&
        y == treasure.position.yPosition) 
    {
        if (treasure.position.xPosition == player.position.xPosition &&
            treasure.position.yPosition == player.position.yPosition)
        {
        endGame(VICTORY);
        }
        

        // std::cout << "X";
        currentToRender[y][x] = 'X';
        continue;
    }

    if (x == player.position.xPosition && y == player.position.yPosition) 
    {
        if (squareDrawn)
        endGame(DEFEAT);
        currentToRender[y][x] = 'P';
        // std::cout << "P";
        continue;
    }
    // Empty square "." is drawn. It only gets printed if there is nothing
    // on the square.
    if (!squareDrawn)
        currentToRender[y][x] = '.';
        // std::cout << ".";
    squareDrawn = false;
    }
    // std::cout << std::endl;
    }
}

void movePlayer(
    /* inout */ Player &player,   // Player of the game
    /* in */ Direction direction) // Direction previously chosen.
                                  // It is represented by a Direction object,
                                  // different from WRONG_DIRECTION.
// Moves player in the chosen direction, by altering its coordinates. If the
// player would finish out of the board, no movement is made.
// Precondition: 0 <= Player.xPosension <= board.xDimension &&
//        0 <= player.position.yPosition <= board.yDimension  &&
//        board.xDimension > 0  && board.yDimension > 0 &&
//        direction in {LEFT; RIGHT; TOP; BOTTOM} &&
// Postcondition: player coordinates have been altered &&
//        player remains inside the board.
{
  switch (direction)
  {
  case RIGHT:
    if (player.position.xPosition < board.xDimension)
        player.position.xPosition += 1;
    break;
  case LEFT:
    if (player.position.xPosition > 0)
        player.position.xPosition -= 1;
    break;
  case TOP:
    if (player.position.yPosition > 0)
        player.position.yPosition -= 1;
    break;
  case BOTTOM:
    if (player.position.yPosition < board.yDimension)
        player.position.yPosition += 1;
    break;
  }
}
void moveBandit(/* inout */ Bandit &bandit) // Player of the game
                                // It is represented by a Direction object,
                                // different from WRONG_DIRECTION.

// Moves player in the chosen direction, by altering its coordinates. If the
// player would finish out of the board, no movement is made.
// Precondition: 0 <= Player.xPosension <= board.xDimension &&
//        0 <= player.position.yPosition <= board.yDimension  &&
//        board.xDimension > 0  && board.yDimension > 0 &&
//        direction in {LEFT; RIGHT; TOP; BOTTOM} &&
// Postcondition: player coordinates have been altered &&
//        player remains inside the board.
{
  int direction = std::rand() % 4;
  switch (direction)
  {
  case 0:
    if (bandit.position.xPosition < board.xDimension)
        bandit.position.xPosition += 1;
    break;
  case 1:
    if (bandit.position.xPosition > 0)
        bandit.position.xPosition -= 1;
    break;
  case 2:
    if (bandit.position.yPosition > 0)
        bandit.position.yPosition -= 1;
    break;
  case 3:
    if (bandit.position.yPosition < board.yDimension)
        bandit.position.yPosition += 1;
    break;
  }
}
void endGame(/* in */ Result result)
// Result of the game.
// It is either VICTORY or DEFEAT
// Cleans screen, prints a good bye message
// and ends the game.
// Precondition: a condition for ending the game has been found.
//              Either player.position == bandit.position ||
//              player.position == trap.position [DEFEAT]
//              or player.position == treasure.position [VICTORY]
// Poscondition: game is ended. Greeting message is printed.
{
  std::string announcement = (result == VICTORY) ? "YOU WIN" : "GAME OVER";
  system("cls");
  std::cout << std::endl << std::endl;
  std::cout << "===========================" << std::endl;
  std::cout << "||\t\t\t||" << std::endl;
  std::cout << "||\t" << announcement << "\t\t||" << std::endl;
  std::cout << "||\t\t\t||" << std::endl;
  std::cout << "===========================" << std::endl;
  exit(1);
}