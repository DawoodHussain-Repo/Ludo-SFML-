 #include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <algorithm>
#include <chrono>  // For sleep
#include <semaphore.h>  // Required for semaphores
using namespace std;
int mouseX,mouseY;
bool gameOver = false; // Declare in the appropriate place (like in your main loop)
sem_t player_semaphores[4];  // Semaphore array for player turns
bool clicked=false;
std::vector<int> turn_order;  
int current_turn_index=0;
sf::Color getColorFromString(const std::string& colorName) {
    if (colorName == "red") {
        // Use a lighter red (brighter color) for the pawn
        return sf::Color(255, 100, 100);  // Lighter red
    } else if (colorName == "blue") {
        // Brighter blue
        return sf::Color(100, 100, 255);
    } else if (colorName == "green") {
        // Brighter green
        return sf::Color(100, 255, 100);
    } else if (colorName == "yellow") {
        // Brighter yellow
        return sf::Color(255, 255, 100);
    } else if (colorName == "white") {
        return sf::Color::White;
    } else {
        // Default to white if color is not recognized
        return sf::Color::White;
    }
}


class Point {
public:
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
     bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};
std::vector<Point> redPath = {
    {-1, -1}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 5}, {6, 4}, {6, 3},
    {6, 2}, {6, 1}, {6, 0}, {7, 0}, {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5},
    {9, 6}, {10, 6}, {11, 6}, {12, 6}, {13, 6}, {14, 6}, {14, 7}, {14, 8}, {13, 8},
    {12, 8}, {11, 8}, {10, 8}, {9, 8}, {8, 9}, {8, 10}, {8, 11}, {8, 12}, {8, 13},
    {8, 14}, {7, 14}, {6, 14}, {6, 13}, {6, 12}, {6, 11}, {6, 10}, {6, 9}, {5, 8},
    {4, 8}, {3, 8}, {2, 8}, {1, 8}, {0, 8}, {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7},
    {5, 7}, {6, 7}
};

std::vector<Point> bluePath = {
    {-1, -1}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {9, 6}, {10, 6}, {11, 6},
    {12, 6}, {13, 6}, {14, 6}, {14, 7}, {14, 8}, {13, 8}, {12, 8}, {11, 8}, {10, 8},
    {9, 8}, {8, 9}, {8, 10}, {8, 11}, {8, 12}, {8, 13}, {8, 14}, {7, 14}, {6, 14},
    {6, 13}, {6, 12}, {6, 11}, {6, 10}, {6, 9}, {5, 8}, {4, 8}, {3, 8}, {2, 8}, {1, 8},
    {0, 8}, {0, 7}, {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 5}, {6, 4},
    {6, 3}, {6, 2}, {6, 1}, {6, 0}, {7, 0}, {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5},
    {7, 6}
};

std::vector<Point> yellowPath = {
    {-1, -1}, {13, 8}, {12, 8}, {11, 8}, {10, 8}, {9, 8}, {8, 9}, {8, 10}, {8, 11},
    {8, 12}, {8, 13}, {8, 14}, {7, 14}, {6, 14}, {6, 13}, {6, 12}, {6, 11}, {6, 10},
    {6, 9}, {5, 8}, {4, 8}, {3, 8}, {2, 8}, {1, 8}, {0, 8}, {0, 7}, {0, 6}, {1, 6},
    {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 5}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0},
    {7, 0}, {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {9, 6}, {10, 6}, {11, 6},
    {12, 6}, {13, 6}, {14, 6}, {14, 7}, {13, 7}, {12, 7}, {11, 7}, {10, 7}, {9, 7},
    {8, 7}
};

std::vector<Point> greenPath = {
    {-1, -1}, {6, 13}, {6, 12}, {6, 11}, {6, 10}, {6, 9}, {5, 8}, {4, 8}, {3, 8},
    {2, 8}, {1, 8}, {0, 8}, {0, 7}, {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6},
    {6, 5}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}, {7, 0}, {8, 0}, {8, 1}, {8, 2},
    {8, 3}, {8, 4}, {8, 5}, {9, 6}, {10, 6}, {11, 6}, {12, 6}, {13, 6}, {14, 6},
    {14, 7}, {14, 8}, {13, 8}, {12, 8}, {11, 8}, {10, 8}, {9, 8}, {8, 9}, {8, 10},
    {8, 11}, {8, 12}, {8, 13}, {8, 14}, {7, 14}, {7, 13}, {7, 12}, {7, 11}, {7, 10},
    {7, 9}, {7, 8}
};
const int PLAYERCOUNT = 4;
class Pawn
{
public:
    bool isSafe;
    int currentLoc;  // Current position in the pawn's path
    bool isHome;  // Whether the pawn is at home
    int x;  // Current X position on the board
    int y;  // Current Y position on the board
    int HomeX;  // The home position X
    int HomeY;  // The home position Y
    string color;  // Pawn's color (e.g., "red", "blue", etc.)
    vector<Point> pawnPath;  // The path this pawn follows
    bool HasWon;
    // Constructor
    Pawn()
        : currentLoc(0), isSafe(false), isHome(true), x(0), y(0), HomeX(0), HomeY(0), color("none"),HasWon(false)
    {}

    // Get the path of the pawn
    vector<Point> getPath() const {
        return pawnPath;
    }

    // Reset the pawn back to the home position
    void resetTOHome() {
        x = HomeX;
        y = HomeY;
        isHome = true;
        setCurrentLoc(0);  // Reset the current location to the home (start position)
    }

    // Set the current position on the path
    void setCurrentLoc(int a) {
        currentLoc = a;
        if(currentLoc==0)
        {
            x=HomeX;
            y=HomeY;
            return;
        }else
        {
            x=pawnPath[currentLoc].x;

            y=pawnPath[currentLoc].y;

        }
        return;
    }

    // Get the current location of the pawn
    Point getLocation() const {
        if (currentLoc == 0) {
            return Point(HomeX, HomeY);  // Return the home position
        } else {
            return pawnPath[currentLoc];  // Return the position on the path
        }
    }

    // Move the pawn to the next position on the path
    void moveToNextPosition() {
        if (currentLoc < pawnPath.size() - 1) {
            currentLoc++;  // Move the pawn to the next step on the path
            x = pawnPath[currentLoc].x;
            y = pawnPath[currentLoc].y;
        }
    }

    // Check if the pawn has reached the winning block (end of the path)
    bool hasWon() const {
        return currentLoc >= pawnPath.size();
    }

    // Set the color of the pawn
    void setColor(const string& color) {
        this->color = color;
    }

    // Get the color of the pawn
    string getColor() const {
        return color;
    }
};

vector<Pawn*> allPawns;



// Define the enum for different field types
enum FieldType {
    RHOME,               // Red home zone
    BHOME,               // Blue home zone
    GHOME,               // Green home zone
    YHOME,               // Yellow home zone
    SAFE_PATH,           // Safe path cells
    MID_CORNERS,         // Middle corner areas
    STAR_CELL,           // Star cells
    ARROW_UP,            // Upward arrows
    ARROW_DOWN,          // Downward arrows
    ARROW_LEFT,          // Leftward arrows
    ARROW_RIGHT,         // Rightward arrows
    BLUE_START,          // Blue start zone
    RED_START,           // Red start zone
    GREEN_START,         // Green start zone
    YELLOW_START,        // Yellow start zone
    RED_WIN,             // Red winning area
    BLUE_WIN,            // Blue winning area
    GREEN_WIN,           // Green winning area
    YELLOW_WIN           // Yellow winning area
};

const int gridSize = 15; // 15x15 Ludo grid
const int cellSize = 52; // Size of each cell

// Ludo board array with FieldType values
FieldType board[15][15] = {
    {RHOME, RHOME, RHOME, RHOME, RHOME, RHOME, SAFE_PATH, ARROW_DOWN, SAFE_PATH, BHOME, BHOME, BHOME, BHOME, BHOME, BHOME},
    {RHOME, RHOME, RHOME, RHOME, RHOME, RHOME, SAFE_PATH, SAFE_PATH, BLUE_START, BHOME, BHOME, BHOME, BHOME, BHOME, BHOME},
    {RHOME, RHOME, RHOME, RHOME, RHOME, RHOME, STAR_CELL, SAFE_PATH, SAFE_PATH, BHOME, BHOME, BHOME, BHOME, BHOME, BHOME},
    {RHOME, RHOME, RHOME, RHOME, RHOME, RHOME, SAFE_PATH, SAFE_PATH, SAFE_PATH, BHOME, BHOME, BHOME, BHOME, BHOME, BHOME},
    {RHOME, RHOME, RHOME, RHOME, RHOME, RHOME, SAFE_PATH, SAFE_PATH, SAFE_PATH, BHOME, BHOME, BHOME, BHOME, BHOME, BHOME},
    {RHOME, RHOME, RHOME, RHOME, RHOME, RHOME, SAFE_PATH, SAFE_PATH, SAFE_PATH, BHOME, BHOME, BHOME, BHOME, BHOME, BHOME},
    {SAFE_PATH, RED_START, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, MID_CORNERS, BLUE_WIN, MID_CORNERS, SAFE_PATH, SAFE_PATH, SAFE_PATH, STAR_CELL, SAFE_PATH, SAFE_PATH},
    {ARROW_RIGHT, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, RED_WIN, MID_CORNERS, YELLOW_WIN, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, ARROW_LEFT},
    {SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, MID_CORNERS, GREEN_WIN, MID_CORNERS, SAFE_PATH, SAFE_PATH, SAFE_PATH, SAFE_PATH, YELLOW_START, SAFE_PATH},
    {GHOME, GHOME, GHOME, GHOME, GHOME, GHOME, SAFE_PATH, SAFE_PATH, SAFE_PATH, YHOME, YHOME, YHOME, YHOME, YHOME, YHOME},
    {GHOME, GHOME, GHOME, GHOME, GHOME, GHOME, SAFE_PATH, SAFE_PATH, SAFE_PATH, YHOME, YHOME, YHOME, YHOME, YHOME, YHOME},
    {GHOME, GHOME, GHOME, GHOME, GHOME, GHOME, SAFE_PATH, SAFE_PATH, SAFE_PATH, YHOME, YHOME, YHOME, YHOME, YHOME, YHOME},
    {GHOME, GHOME, GHOME, GHOME, GHOME, GHOME, SAFE_PATH, SAFE_PATH, STAR_CELL, YHOME, YHOME, YHOME, YHOME, YHOME, YHOME},
    {GHOME, GHOME, GHOME, GHOME, GHOME, GHOME, GREEN_START, SAFE_PATH, SAFE_PATH, YHOME, YHOME, YHOME, YHOME, YHOME, YHOME},
    {GHOME, GHOME, GHOME, GHOME, GHOME, GHOME, SAFE_PATH, ARROW_UP, SAFE_PATH, YHOME, YHOME, YHOME, YHOME, YHOME, YHOME}
};

void UpdateBoard(sf::RenderWindow &window) {
    for (const auto& pawn : allPawns) {
        if (pawn->HasWon) {
            continue;
        }

        // Main pawn size and color
        float pawnSize = cellSize * 0.7f;  // Larger size for visual prominence
        sf::CircleShape pawnShape(pawnSize / 2);
        pawnShape.setFillColor(getColorFromString(pawn->color));  // Pawn's main color
        pawnShape.setOutlineColor(sf::Color::Black);  // Black border for better contrast
        pawnShape.setOutlineThickness(4);  // Thicker border

        // Shadow for depth
        sf::CircleShape shadow(pawnSize / 2);
        shadow.setFillColor(sf::Color(30, 30, 30, 180));  // Dark semi-transparent shadow
        shadow.setScale(1.05f, 1.05f);  // Slightly larger than the main shape
        shadow.setPosition(pawn->getLocation().x * cellSize + 5, pawn->getLocation().y * cellSize + 5);

        // Pure white gradient overlay for a polished effect
        sf::CircleShape gradient(pawnSize / 2);
        gradient.setPointCount(100);  // Smoother circle
        gradient.setFillColor(sf::Color(255, 255, 255, 100));  // Pure white with semi-transparency
        gradient.setScale(0.8f, 0.8f);  // Slightly smaller than the pawn
        gradient.setPosition(pawn->getLocation().x * cellSize + (cellSize - pawnSize) / 2,
                             pawn->getLocation().y * cellSize + (cellSize - pawnSize) / 2);

        // Light reflection for a glossy look
        sf::CircleShape lightReflection(pawnSize / 4);
        lightReflection.setFillColor(sf::Color(255, 255, 255, 150));  // Brighter semi-transparent white
        lightReflection.setPosition(pawn->getLocation().x * cellSize + (cellSize - pawnSize) / 4,
                                    pawn->getLocation().y * cellSize + (cellSize - pawnSize) / 4);

        // Position the main pawn
        Point p = pawn->getLocation();
        pawnShape.setPosition(p.x * cellSize + (cellSize - pawnSize) / 2, p.y * cellSize + (cellSize - pawnSize) / 2);

        // Draw the layers in order
        window.draw(shadow);          // Draw shadow first
        window.draw(pawnShape);       // Draw main pawn
        window.draw(gradient);        // Draw pure white gradient
        window.draw(lightReflection); // Draw light reflection
    }
}




class Player {
public:
    int ID;
    string name;
    Pawn Pawns[4];
    bool isKiller=false;
    vector<int> Turns;
void InitializePawns() {
    // Hardcoded locations for the pawns and setting isHome to true
    for (int i = 0; i < 4; ++i) {
        Pawns[i].isHome = true;

        // Set hardcoded locations for the pawns, each pawn will have a dedicated position
      switch (ID) {
    case 0:  // Red Player
        Pawns[i].pawnPath = redPath;
        switch (i) {
            case 0:
                Pawns[i].x = 2;  Pawns[i].y = 1;
                Pawns[i].HomeX = 2;  Pawns[i].HomeY = 1;
                break;
            case 1:
                Pawns[i].x = 3;  Pawns[i].y = 1;
                Pawns[i].HomeX = 3;  Pawns[i].HomeY = 1;
                break;
            case 2:
                Pawns[i].x = 2;  Pawns[i].y = 2;
                Pawns[i].HomeX = 2;  Pawns[i].HomeY = 2;
                break;
            case 3:
                Pawns[i].x = 3;  Pawns[i].y = 2;
                Pawns[i].HomeX = 3;  Pawns[i].HomeY = 2;
                break;
        }
        Pawns[i].color = "red";
        break;

    case 1:  // Blue Player
        Pawns[i].pawnPath = bluePath;
        switch (i) {
            case 0:
                Pawns[i].x = 12; Pawns[i].y = 1;
                Pawns[i].HomeX = 12; Pawns[i].HomeY = 1;
                break;
            case 1:
                Pawns[i].x = 13; Pawns[i].y = 1;
                Pawns[i].HomeX = 13; Pawns[i].HomeY = 1;
                break;
            case 2:
                Pawns[i].x = 12; Pawns[i].y = 2;
                Pawns[i].HomeX = 12; Pawns[i].HomeY = 2;
                break;
            case 3:
                Pawns[i].x = 13; Pawns[i].y = 2;
                Pawns[i].HomeX = 13; Pawns[i].HomeY = 2;
                break;
        }
        Pawns[i].color = "blue";
        break;

    case 2:  // Green Player
        Pawns[i].pawnPath = greenPath;
        switch (i) {
            case 0:
                Pawns[i].x = 2;  Pawns[i].y = 12;
                Pawns[i].HomeX = 2;  Pawns[i].HomeY = 12;
                break;
            case 1:
                Pawns[i].x = 3;  Pawns[i].y = 12;
                Pawns[i].HomeX = 3;  Pawns[i].HomeY = 12;
                break;
            case 2:
                Pawns[i].x = 2;  Pawns[i].y = 13;
                Pawns[i].HomeX = 2;  Pawns[i].HomeY = 13;
                break;
            case 3:
                Pawns[i].x = 3;  Pawns[i].y = 13;
                Pawns[i].HomeX = 3;  Pawns[i].HomeY = 13;
                break;
        }
        Pawns[i].color = "green";
        break;

    case 3:  // Yellow Player
        Pawns[i].pawnPath = yellowPath;
        switch (i) {
            case 0:
                Pawns[i].x = 12; Pawns[i].y = 13;
                Pawns[i].HomeX = 12; Pawns[i].HomeY = 13;
                break;
            case 1:
                Pawns[i].x = 13; Pawns[i].y = 13;
                Pawns[i].HomeX = 13; Pawns[i].HomeY = 13;
                break;
            case 2:
                Pawns[i].x = 12; Pawns[i].y = 12;
                Pawns[i].HomeX = 12; Pawns[i].HomeY = 12;
                break;
            case 3:
                Pawns[i].x = 13; Pawns[i].y = 12;
                Pawns[i].HomeX = 13; Pawns[i].HomeY = 12;
                break;
        }
        Pawns[i].color = "yellow";
        break;
}


        // Add each pawn to the global list of all pawns
        allPawns.push_back(&Pawns[i]);
    }
}

};
Player players[PLAYERCOUNT];  
Player* activePlayer;
class LudoBoard : public sf::Drawable {
public:
    LudoBoard() {
        initializeBoard();
    }

protected:
 virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // Draw each cell
        for (const auto& cell : cells) {
            target.draw(cell, states);  // Draw each cell
        }

        // Draw each pawn
        for (const auto& pawn : pawns) {
            target.draw(pawn, states);  // Draw each pawn
        }
    }
public:
void toggleSafe(Pawn* pawn) {
    // Check if the pawn is currently in a safe zone
    if (board[pawn->y][pawn->x] == SAFE_PATH || board[pawn->y][pawn->x] ==ARROW_RIGHT || board[pawn->y][pawn->x] ==ARROW_LEFT || board[pawn->y][pawn->x] ==ARROW_UP || board[pawn->y][pawn->x] == ARROW_DOWN) {
        // If the pawn is already in a safe zone, move it out of the safe zone
        std::cout<<pawn->x<<pawn->y<<std::endl;
        std::cout << "Not Safe" << std::endl;
        pawn->isSafe=false;
    } else {
        // If the pawn is not in a safe zone, move it to the safe zone
        std::cout << "Moving to safe" << std::endl;
pawn->isSafe=true;
        // Optionally, change the pawn's location on the board if you want to represent the safe zone visually
    }
}
void CheckCollisions(Pawn* pawn) {
    if(pawn->currentLoc==pawn->pawnPath.size()-1)
    {
        pawn->HasWon=true;
        std::cout<<"Win"<<std::endl;
    }

    // Iterate through all pawns in the global allPawns vector
    for (auto& otherPawn : allPawns) {
        // Skip checking against itself
        if (pawn == otherPawn) continue;
        if(otherPawn->isSafe) continue;
        // Check if the other pawn has the same coordinates (x, y)
        if (pawn->x == otherPawn->x && pawn->y == otherPawn->y) {
            // Check if the colors are different
            if ((pawn->color != otherPawn->color )&&(otherPawn->isSafe==false)) {
                std::cout << "Collision detected! Pawn of color " << pawn->color 
                          << " collides with pawn of color " << otherPawn->color << std::endl;

                // "Kill" the other pawn by resetting its location or any other behavior you'd like
                // Example: Reset the location of the other pawn to its starting position or remove it
                otherPawn->isHome = true;  // Assuming you set the pawn back to the home position
                otherPawn->resetTOHome();  // Assuming 0 represents the home position
                   Player* currentPlayer = &players[current_turn_index];  // Assuming current_turn_index tracks the player's turn
     currentPlayer->isKiller=true;
            }
        }
    }
}

void handleMouseClick(sf::RenderWindow& window, sf::Event event) {
    // Check if a mouse button was pressed
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);  // Get the mouse position
        sf::Vector2f mousePosFloat(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        // Get the current player
        Player* currentPlayer = &players[current_turn_index];  // Assuming current_turn_index tracks the player's turn
        
        // Check if a pawn is clicked and belongs to the current player
        for (auto& pawn : currentPlayer->Pawns) {
            // Create an expanded clickable area for more intuitive selection
            float expandedRadius = cellSize * 0.4f;  // Expand the detection area to make clicks easier
            sf::CircleShape detectionShape(expandedRadius);
            detectionShape.setPosition(pawn.x * cellSize, pawn.y * cellSize);  // Use pawn's x, y for its position
            detectionShape.setOrigin(detectionShape.getRadius(), detectionShape.getRadius());  // Center the shape

            // Check if the mouse click was inside the clickable area (pawn)
            if (detectionShape.getGlobalBounds().contains(mousePosFloat)) {
                std::cout << "Pawn belongs to current player" << std::endl;

                bool allHome = true;
                for (const Pawn& p : currentPlayer->Pawns) {
                    if (!p.isHome) {
                        allHome = false;
                        break;
                    }
                }

                // Check if the player has a 6 in their turns
                if (std::find(currentPlayer->Turns.begin(), currentPlayer->Turns.end(), 6) != currentPlayer->Turns.end()) {
                    if (pawn.isHome) {
                        // If the pawn is home, move it forward by 1
                        pawn.isHome = false;
                        pawn.setCurrentLoc(pawn.currentLoc += 1);
                        toggleSafe(&pawn);
                    } else {
                        // If the pawn is not home, move it forward by 6
                        if (6 + pawn.currentLoc > pawn.pawnPath.size() - 1) {
                            std::cout << "Cannot move" << std::endl;
                            return;
                        }
                        pawn.setCurrentLoc(pawn.currentLoc += 6);
                        toggleSafe(&pawn);
                        CheckCollisions(&pawn);
                    }

                    // Remove one occurrence of 6 from the turns vector
                    auto it = std::find(currentPlayer->Turns.begin(), currentPlayer->Turns.end(), 6);
                    if (it != currentPlayer->Turns.end()) {
                        currentPlayer->Turns.erase(it);  // Remove the found 6
                    }
                } else {
                    // If the player doesn't have a 6, show the message
                    if (allHome) {
                        // If all pawns are home, discard the turn
                        currentPlayer->Turns.clear();
                        std::cout << "Discarded" << std::endl;
                    } else {
                        if (!pawn.isHome) {
                            // Move pawn by the first roll in the turns
                            if (currentPlayer->Turns.front() + pawn.currentLoc > pawn.pawnPath.size() - 1) {
                                std::cout << "Cannot move" << std::endl;
                                return;
                            }
                            pawn.setCurrentLoc(pawn.currentLoc += currentPlayer->Turns.front());

                            // Remove the first occurrence of the roll from the turns
                            auto it = std::find(currentPlayer->Turns.begin(), currentPlayer->Turns.end(), currentPlayer->Turns.front());
                            if (it != currentPlayer->Turns.end()) {
                                currentPlayer->Turns.erase(it);  // Remove the found number
                            }

                            toggleSafe(&pawn);
                            CheckCollisions(&pawn);
                            currentPlayer->Turns.clear();
                        } else {
                            std::cout << "You need a Six" << std::endl;
                        }
                    }
                }
                return;  // Exit the loop after handling the click
            }
        }
    }
}


private:
    std::vector<sf::RectangleShape> cells;
    std::vector<sf::CircleShape> pawns; // Vector to hold the pawns

    void initializeBoard() {
        // Create the grid based on the board array
        for (int row = 0; row < gridSize; ++row) {
            for (int col = 0; col < gridSize; ++col) {
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(col * cellSize, row * cellSize);

                // Map FieldType to color
                switch (board[row][col]) {
                    case RHOME:
                        cell.setFillColor(sf::Color::Red);
                        break;
                    case BHOME:
                        cell.setFillColor(sf::Color::Blue);
                        break;
                    case GHOME:
                        cell.setFillColor(sf::Color::Green);
                        break;
                    case YHOME:
                        cell.setFillColor(sf::Color::Yellow);
                        break;
                    case SAFE_PATH:
                        cell.setFillColor(sf::Color::White);  // White for safe path
                        cell.setOutlineColor(sf::Color::Black);  // Black border for safe path
                        cell.setOutlineThickness(2);  // Border thickness
                        break;
                    case STAR_CELL:
                        cell.setFillColor(sf::Color(255, 215, 0));  // Gold for star cell
                        break;
                    case ARROW_UP:
                    case ARROW_DOWN:
                    case ARROW_LEFT:
                    case ARROW_RIGHT:
                        cell.setFillColor(sf::Color(200, 200, 200)); // Light gray for arrows
                        break;
                    case BLUE_START:
                        cell.setFillColor(sf::Color(0, 0, 255)); // Dark blue for Blue start
                        break;
                    case RED_START:
                        cell.setFillColor(sf::Color(255, 0, 0)); // Dark red for Red start
                        break;
                    case GREEN_START:
                        cell.setFillColor(sf::Color(0, 255, 0)); // Dark green for Green start
                        break;
                    case YELLOW_START:
                        cell.setFillColor(sf::Color(255, 255, 0)); // Dark yellow for Yellow start
                        break;
                    case RED_WIN:
                        cell.setFillColor(sf::Color(255, 69, 0));  // Red-orange for Red win
                        break;
                    case BLUE_WIN:
                        cell.setFillColor(sf::Color(0, 0, 139));  // Dark blue for Blue win
                        break;
                    case GREEN_WIN:
                        cell.setFillColor(sf::Color(34, 139, 34));  // Forest green for Green win
                        break;
                    case YELLOW_WIN:
                        cell.setFillColor(sf::Color(255, 215, 0));  // Gold for Yellow win
                        break;
                    default:
                        cell.setFillColor(sf::Color::White);  // Default color (unused cells)
                }
   // Change the entire middle row (index 7)
                if (row == 7) {  // Middle row (index 7)
                    if (col < 7) {
                        if (board[row][col] == SAFE_PATH) cell.setFillColor(sf::Color::Red);  // Left half red
                    } else {
                        if (board[row][col] == SAFE_PATH) cell.setFillColor(sf::Color::Yellow);  // Right half yellow
                    }
                }

                // Change the entire middle column (index 7)
                if (col == 7) {  // Middle column (index 7)
                    if (row < 7) {
                        if (board[row][col] == SAFE_PATH) cell.setFillColor(sf::Color::Blue);  // Top half blue
                    } else {
                        if (board[row][col] == SAFE_PATH) cell.setFillColor(sf::Color::Green);  // Bottom half green
                    }
                }
                cells.push_back(cell);
            }
        }
    }
    
void createPawn(int x, int y, sf::Color color) {
    float pawnSize = cellSize * 0.95f;  // Almost the entire cell size for maximum visibility

    // Create the main pawn
    sf::CircleShape pawn(pawnSize / 2);
    pawn.setFillColor(color);  // Base color for the pawn
    pawn.setOutlineColor(sf::Color::White);  // Bright outline for a polished look
    pawn.setOutlineThickness(5);  // Thicker outline for better visibility

    // **Gradient Effect: Simulate by overlaying a radial gradient**
    sf::CircleShape gradient(pawnSize / 2);
    gradient.setPointCount(100);  // Smooth circle
    gradient.setFillColor(sf::Color(255, 255, 255, 50));  // Light overlay for gradient effect
    gradient.setScale(0.8f, 0.8f);  // Slightly smaller to create a center glow effect
    gradient.setPosition(x * cellSize + (cellSize - pawnSize) / 2, y * cellSize + (cellSize - pawnSize) / 2);

    // Positioning the main pawn
    pawn.setPosition(x * cellSize + (cellSize - pawnSize) / 2, y * cellSize + (cellSize - pawnSize) / 2);

    // **Shadow Effect: Stronger and more dynamic**
    sf::CircleShape shadow(pawnSize / 2);
    shadow.setFillColor(sf::Color(30, 30, 30, 180));  // Semi-transparent dark shadow
    shadow.setScale(1.05f, 1.05f);  // Slightly larger shadow to enhance depth
    shadow.setPosition(pawn.getPosition().x + 5, pawn.getPosition().y + 5);  // Offset for depth

    // **Lighting Effect: Simulate reflection on the pawn**
    sf::CircleShape lightReflection(pawnSize / 4);
    lightReflection.setFillColor(sf::Color(255, 255, 255, 100));  // White, semi-transparent
    lightReflection.setPosition(pawn.getPosition().x + pawnSize / 4, pawn.getPosition().y + pawnSize / 4);

    // Push shapes to render in the correct order
    pawns.push_back(shadow);  // Shadow at the bottom
    pawns.push_back(pawn);    // Main pawn
    pawns.push_back(gradient); // Gradient overlay
    pawns.push_back(lightReflection); // Light reflection
}


};

struct thread_data {
    pthread_t thread_id;
    Player* player;
    int thread_index;  // To store the player's index if needed
};


void GetCoordinates(sf::RenderWindow &window, sf::Event event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        // Dice position and size (60x60, centered with offset)
        float diceX = (800.f - 60.f) / 2 - 10;  // Assuming windowWidth = 800, dice width = 60
        float diceY = (800.f - 60.f) / 2 - 10;  // Assuming windowHeight = 800, dice height = 60
        float diceWidth = 60.f;
        float diceHeight = 60.f;

        // Check if the click is within the dice bounds
        if (mouseX >= diceX && mouseX <= diceX + diceWidth &&
            mouseY >= diceY && mouseY <= diceY + diceHeight) {
            
            std::cout << "Dice clicked at (" << mouseX << ", " << mouseY << ")" << std::endl;
            clicked=true;
            // You can then update the dice dots or any related logic here based on rolledNumber
        }
    }
}
void setActivePlayer(sf::RenderTarget& target)
{
    Player* player= &players[current_turn_index]; // Set the active player (pointer)
    
    // Create a black circle to represent the arrow
    sf::CircleShape arrow(10.0f);  // Create a circle with radius 10
    arrow.setFillColor(sf::Color::Black);  // Set the color of the arrow to black
    
    // Manually calculate position based on player ID (you can adjust these values as needed)
    sf::Vector2f position;

    if(player->ID == 0) {
        position = sf::Vector2f(6 * cellSize + 15, 6 * cellSize + 15);  
    } 
    else if(player->ID == 1) {
        position = sf::Vector2f(8 * cellSize + 15, 6 * cellSize + 15);  
    }
    else if(player->ID == 2) {
        position = sf::Vector2f(6 * cellSize + 15, 8 * cellSize + 15);  
    } 
    else if(player->ID == 3) {
        position = sf::Vector2f(8 * cellSize + 15, 8 * cellSize + 15);  
    }

    // Set the position of the arrow based on Player ID
    arrow.setPosition(position);  
    
    // Optional: Draw the arrow on the target
    target.draw(arrow);
}

class Dice : public sf::Drawable {
public:
    int rolledNumber;  // The rolled number (1-6)
    sf::RectangleShape diceShape;  // The dice shape
    std::vector<sf::CircleShape> dots;  // Dots on the dice

    Dice() {
        // Set up the dice shape (smaller dice size now)
        diceShape.setSize(sf::Vector2f(60.f, 60.f));  // Dice size 60x60
        diceShape.setFillColor(sf::Color::White);  // Dice color
        diceShape.setOutlineColor(sf::Color::Black);  // Outline color
        diceShape.setOutlineThickness(2.f);

        // Centering the dice in the middle of the window
        float windowWidth = 800.f;  // Assuming window width is 800
        float windowHeight = 800.f; // Assuming window height is 800
        diceShape.setPosition(((windowWidth - diceShape.getSize().x) / 2)-10, ((windowHeight - diceShape.getSize().y) / 2)-10);

        rolledNumber = 1;  // Default to 1 to start with
        createDots();
    }

void roll() {
    // Generate a random number between 1 and 6
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 6);
    rolledNumber = dis(gen);  // Store the rolled number

    // Ensure activePlayer is valid before accessing it
    if (activePlayer) {
        // Add the rolled number to the player's Turns vector
        activePlayer->Turns.push_back(rolledNumber);

        // Print the contents of the Turns vector (i.e., all the rolls)
        std::cout << activePlayer->name << "'s Rolls: ";
        for (int t : activePlayer->Turns) {
            std::cout << t << " ";
        }
        std::cout << std::endl;
    }
createDots();

}


void createDots() {
    // Clear previous dots
    dots.clear();

    // Get the dice's position and size for relative placement of dots
    float dicePosX = diceShape.getPosition().x;
    float dicePosY = diceShape.getPosition().y;
    float diceSizeX = diceShape.getSize().x;
    float diceSizeY = diceShape.getSize().y;

    // Set up dot positions relative to the dice size
    float dotRadius = 6.f;  // Reduced dot radius for smaller dots
    float centerX = dicePosX + diceSizeX / 2.f;
    float centerY = dicePosY + diceSizeY / 2.f;

    // Positions for the dots (relative to the dice center)
    if (rolledNumber == 1) {
        dots.push_back(createDot(centerX, centerY));  // Center
    } else if (rolledNumber == 2) {
        dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + dotRadius + 5.f));  // Top-left
        dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-right
    } else if (rolledNumber == 3) {
        dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + dotRadius + 5.f));  // Top-left
        dots.push_back(createDot(centerX, centerY));  // Center
        dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-right
    } else if (rolledNumber == 4) {
        dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + dotRadius + 5.f));  // Top-left
        dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + dotRadius + 5.f));  // Top-right
        dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-left
        dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-right
    } else if (rolledNumber == 5) {
        dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + dotRadius + 5.f));  // Top-left
        dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + dotRadius + 5.f));  // Top-right
        dots.push_back(createDot(centerX, centerY));  // Center
        dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-left
        dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-right
    } else if (rolledNumber == 6) {
    // Adjusted positions for a more balanced 6
// Existing dots
dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + dotRadius + 5.f));  // Top-left
dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + dotRadius + 5.f));  // Top-right
dots.push_back(createDot(dicePosX + dotRadius + 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-left
dots.push_back(createDot(dicePosX + diceSizeX - dotRadius - 5.f, dicePosY + diceSizeY - dotRadius - 5.f));  // Bottom-right

// Midpoints (middle of the dots)
float midX1 = (dicePosX + dotRadius + 5.f + dicePosX + diceSizeX - dotRadius - 5.f) / 2; // Midpoint X between top-left and bottom-right
float midY1 = (dicePosY + dotRadius + 5.f + dicePosY + diceSizeY - dotRadius - 5.f) / 2; // Midpoint Y between top-left and bottom-right
midX1 -= 19.f; // Adjust the first midpoint slightly to the left
dots.push_back(createDot(midX1, midY1));  // Middle dot between top-left and bottom-right (adjusted left)

float midX2 = (dicePosX + dotRadius + 5.f + dicePosX + diceSizeX - dotRadius - 5.f) / 2; // Midpoint X between top-right and bottom-left
float midY2 = (dicePosY + diceSizeY - dotRadius - 5.f + dicePosY + dotRadius + 5.f) / 2; // Midpoint Y between top-right and bottom-left
midX2 += 19.f; // Adjust the second midpoint slightly to the right
dots.push_back(createDot(midX2, midY2));  // Middle dot between top-right and bottom-left (adjusted right)

}

}



sf::CircleShape createDot(float x, float y) {
    sf::CircleShape dot(6.f);  // Reduced dot radius to 6
    dot.setFillColor(sf::Color::Black);  // Dot color
    dot.setPosition(x - dot.getRadius(), y - dot.getRadius());  // Position the dot (centered)
    return dot;
}

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(diceShape, states);  // Draw the dice shape
        for (const auto& dot : dots) {
            target.draw(dot, states);  // Draw each dot
        }
    }
};

Dice dice;

void* playerThreadFunction(void* arg) {
    thread_data* data = static_cast<thread_data*>(arg);
    Player* player = data->player;
    player->InitializePawns();  // Initialize player's pawns
   
    while (!gameOver) {
        // Display the thread ID and Player ID before waiting
        std::cout << "Thread ID: " << pthread_self() << " is running Player ID: " << player->ID << std::endl;

        // Wait for the player's turn
        sem_wait(&player_semaphores[player->ID]);

        // Roll the dice once after clicking
        while (!clicked);  // Wait until a click is detected
        
        int sixCount = 0;  // Initialize a counter for the number of 6s rolled
        player->Turns.clear();  // Clear any previous rolls

        while (sixCount < 3) {  // Allow rerolling until 3 sixes are rolled
            dice.roll();
            player->Turns.push_back(dice.rolledNumber);
            std::cout << "Player " << player->ID << " rolled " << dice.rolledNumber << std::endl;

            // Sleep for a short time to allow the player to see the dice result
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 1 second pause

            if (dice.rolledNumber == 6) {
                sixCount++;  // Increment the 6s counter if a 6 is rolled
                std::cout << "Player " << player->ID << " rolled a 6! Total sixes: " << sixCount << std::endl;

                // Sleep for a short time to allow the player to see the 6 roll
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 1 second pause

                // If 3 sixes are rolled, clear the turns and move forward
                if (sixCount == 3) {
                    std::cout << "Player " << player->ID << " rolled 3 sixes! Moving forward." << std::endl;
                    player->Turns.clear();  // Clear the player's turns
                    break;  // Exit the loop after 3 sixes
                }
            }

            // If the player doesn't roll a 6, break out of the loop
            if (dice.rolledNumber != 6) {
                break;
            }
        }
 
        clicked = false;  // Reset click status after handling the roll

        while (!player->Turns.empty());
        // Signal next player's semaphore
        int nextPlayer = (player->ID + 1) % 4;
        current_turn_index = nextPlayer;
        sem_post(&player_semaphores[nextPlayer]);
    }

    return nullptr;
}

void checkWinner()
{
    int count=0;
for(Player player:players)
{

    int count=0;
    for(int i=0;i<4;i++)
    {

if(player.Pawns[0].HasWon==true)
{
    count++;
}
    }
    if(count==4)
    {
        gameOver=true;
        std::cout<<player.ID<<" Wins"<<std::endl;
    }
}
}

int main() {
    // Initialize semaphores, player 0 starts
    for (int i = 0; i < 4; ++i) {
        sem_init(&player_semaphores[i], 0, (i == 0) ? 1 : 0);
    }

    // Initialize players
    players[0].ID = 0; players[0].name = "Alice";
    players[1].ID = 1; players[1].name = "Bob";
    players[2].ID = 2; players[2].name = "Jack";
    players[3].ID = 3; players[3].name = "Riddler";

    LudoBoard board;

    pthread_t playerthread[4];
for (int i = 0; i < 4; ++i) {
    thread_data* data = new thread_data{.player = &players[i], .thread_index = i};
    pthread_create(&playerthread[i], NULL, playerThreadFunction, (void*)data);
}

    sf::RenderWindow window(sf::VideoMode(800, 800), "Ludo Board");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            GetCoordinates(window, event);
            board.handleMouseClick(window, event);
        }

        window.clear();
        window.draw(board);
        window.draw(dice);
        UpdateBoard(window);
        setActivePlayer(window);
        checkWinner();
        window.display();
    }

    // Join threads
    for (int i = 0; i < 4; ++i) pthread_join(playerthread[i], nullptr);

    // Destroy semaphores
    for (int i = 0; i < 4; ++i) sem_destroy(&player_semaphores[i]);

    return 0;
}
