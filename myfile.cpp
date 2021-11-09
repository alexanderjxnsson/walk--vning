#include <climits>
#include <ctime>
#include <iostream>
#include <random>
#include <string>
/* Declarations */
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
struct World
{
    struct Bandit {struct Location position; char symbol = 'B';};
    struct Location{ int xPosition; int yPosition;};
    struct Player {struct Location position; char symbol = 'P'; std::string name = "player";};
    struct Trap {struct Location position; char symbol = 'T';};
    struct Treasure {struct Location position; char symbol = 'X';};
    int xDimension;               // Represents the board.
    int yDimension;
} board = {.xDimension = 10, .yDimension = 10};
enum Direction { RIGHT, LEFT, TOP, BOTTOM, WRONG_DIRECTION };
enum Result { VICTORY, DEFEAT };
void drawBoard(World::Player, World::Trap[], World::Bandit[], World::Treasure);
void drawBoard2(World::Player, World::Trap[], World::Bandit[], World::World::Treasure);
void endGame(Result);
void moveCharacter(char cCharacter, World::Player &player, World::Bandit &bandit, Direction direction);
void movePlayer(World::Player &, Direction);
void moveBandit(World::Bandit &);
Direction askDirection();

/* Delarations end */
int main()
{
    /* Init start */
    std::srand(std::time(0));
    
    
    // Treasure position is decided randomly.
    World::Treasure treasure =
    {
      .position = {.xPosition = std::rand() % board.xDimension,
                   .yPosition = std::rand() % board.yDimension}
    };
    // Traps are placed around the map. It is not guaranteed that traps position doesn't converge.
    // In that case, the second trap can be assumed to not exist.
    //Här skulle man kunna lägga till kod som fixar detta
    World::Trap trapsInMap[NUMBEROFTRAPS];
    for (int i = 0; i < NUMBEROFTRAPS; i++)
    {
        int xPos = std::rand() % board.xDimension;
        int yPos = std::rand() % board.yDimension;
        World::Trap trap = {.position = {.xPosition = xPos, .yPosition = yPos}};
        trapsInMap[i] = trap;
    }
    // Bandits are placed around the map. It is not guaranteed that bandits position doesn't converge, but they will move anyway.
    World::Bandit banditsInMap[NUMBEROFBANDITS];
    for (int i = 0; i < NUMBEROFBANDITS; i++)
    {
        int xPos = std::rand() % board.xDimension;
        int yPos = std::rand() % board.yDimension;
        World::Bandit bandit = {.position = {.xPosition = xPos, .yPosition = yPos}};
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

    World::Player player = {.position = {.xPosition = xPos, .yPosition = yPos}};
    for (int i = 0; i <= maxTurnos; i++)
    {
        drawBoard2(player, trapsInMap, banditsInMap, treasure);
        for (size_t i = 0; i < 10; i++)
        {
            for (size_t j = 0; j < 10; j++)
            {
                std::cout << currentToRender[i][j];
            }
            std::cout<<std::endl;
        }
        Direction direction;
        do
        {
            direction = askDirection();
            std::cout << std::endl;
        }
        while (direction == WRONG_DIRECTION);
        int k = 0;
        
        moveCharacter('P',player, banditsInMap[k], direction);
        for (int i = 0; i < NUMBEROFBANDITS; i++)
        {
            moveCharacter('B',player, banditsInMap[i], direction);
        }
        system("cls");;
    }
    /* Loop end */
}
/* Function start */
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

void drawBoard2(World::Player player, World::Trap totalTraps[], World::Bandit totalBandits[], World::Treasure treasure)
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
        World::Trap trapToDraw = totalTraps[z];
        if (trapToDraw.position.xPosition == x &&
            trapToDraw.position.yPosition == y)
        {
            currentToRender[y][x] = trapToDraw.symbol;
            squareDrawn = true;
        }
    }
    // Bandits are drawn.
    // In case of collision with a trap,
    // only the second is drawn.
    for (int z = 0; z <= NUMBEROFBANDITS; z++)
    {
        World::Bandit banditToDraw = totalBandits[z];
        if (banditToDraw.position.xPosition == x &&
            banditToDraw.position.yPosition == y && !squareDrawn) 
        {
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
        currentToRender[y][x] = 'X';
        continue;
    }

    if (x == player.position.xPosition && y == player.position.yPosition) 
    {
        if (squareDrawn)
        {
            endGame(DEFEAT);
        }
        currentToRender[y][x] = 'P';
        continue;
    }
    // Empty square "." is drawn. It only gets printed if there is nothing
    // on the square.
    if (!squareDrawn)
    {
        currentToRender[y][x] = '.';
    }
    squareDrawn = false;
    }
    // std::cout << std::endl;
    }
}

void moveCharacter(char cCharacter, World::Player &player, World::Bandit &bandit, Direction direction)
{
    int banditMove = std::rand() % 4;
     switch (cCharacter)
     {
     case 'B':
        switch (banditMove)
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
        break;
     case 'P':
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
        break;
     }
}

void endGame(Result result)
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
/* Functions end */