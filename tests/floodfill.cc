#include <iostream>
#include <string>
#include <queue>

#include "cell.h"

/* class Cell {
private:
    bool northWall;
    bool eastWall;
    bool southWall;
    bool westWall;

    int value;
    bool isVisited;
    
    int posX;
    int posY;

public:
    // Constructor
    Cell() : northWall(false), eastWall(false), southWall(false), westWall(false), value(-1) {}

    // Wall status setter
    void setNorthWall(bool hasWall) { northWall = hasWall; }
    void setEastWall(bool hasWall) { eastWall = hasWall; }
    void setSouthWall(bool hasWall) { southWall = hasWall; }
    void setWestWall(bool hasWall) { westWall = hasWall; }

    // Wall status getter
    bool hasNorthWall() const { return northWall; }
    bool hasEastWall() const { return eastWall; }
    bool hasSouthWall() const { return southWall; }
    bool hasWestWall() const { return westWall; }

    // Cell value getter and setter
    int getValue() { return value; }
    void setValue(int v) { value = v; }

    // Cell position getters and setters
    int getX() { return posX; }
    void setX(int x) { posX = x; }

    int getY() { return posY; }
    void setY(int y) { posY = y; }

    // Function to reset cell state
    void reset() { 
        setValue(-1);
    }
}; */ 

void floodfill(Cell** maze, Cell& goal, int rows, int cols) {
    // 1. Set all cells except goal to “blank state”
    for (int x = 0; x < rows; ++x) {
        for (int y = 0; y < cols; ++y) {
            maze[x][y].reset();
        }
    }

    // 2. Set goal cell(s) value to 0 and add to queue
    goal.setValue(0);
    std::queue<Cell*> cellQueue;
    cellQueue.push(&goal);

    while (!cellQueue.empty()) {
        // 3.1 Take front cell in a queue "out of line" for consideration
        Cell* temp = cellQueue.front();
        cellQueue.pop();
        
        // 3.2 set all blank and accessible neighbours to front cell's value + 1
        if (!temp->hasNorthWall() && temp->getY() + 1 < cols) {
            Cell* currentCell = &maze[temp->getX()][temp->getY() + 1];
            // Check if it's blank
            if (currentCell->getValue() == -1) {
                currentCell->setValue(temp->getValue() + 1);
                
                // 3.3a Add new cell to queue
                cellQueue.push(currentCell);
            }
        }
        if (!temp->hasEastWall() && temp->getX() + 1 < rows) {
            Cell* currentCell = &maze[temp->getX() + 1][temp->getY()];
            if (currentCell->getValue() == -1) {
                currentCell->setValue(temp->getValue() + 1);
            
                // 3.3b Add new cell to queue
                cellQueue.push(currentCell);
            }
        }
        if (!temp->hasSouthWall() && temp->getY() - 1 >= 0) {
            Cell* currentCell = &maze[temp->getX()][temp->getY() - 1];
            if (currentCell->getValue() == -1) {
                currentCell->setValue(temp->getValue() + 1);

                // 3.3c Add new cell to queue
                cellQueue.push(currentCell);
            }
        }
        if (!temp->hasWestWall() && temp->getX() - 1 >= 0) {
            Cell* currentCell = &maze[temp->getX() - 1][temp->getY()];
            if (currentCell->getValue() == -1) {
                currentCell->setValue(temp->getValue() + 1);

                // 3.3d Add new cell to queue
                cellQueue.push(currentCell);
            }
        }
        //std::cout << temp->getValue() << '(' << temp->getX() << " ," << temp->getY() << ')' << std::endl;
    }
}


int main() {
    // Create 2d array of cells (maze model)
    // done automatically but can be done manually for testing.
    const int rows = 5;
    const int cols = 5;
    Cell** maze = new Cell*[rows];
    for (int x = 0; x < rows; ++x) {
        maze[x] = new Cell[cols];
        for (int y = 0; y < cols; ++y) {
            maze[x][y].setX(x);
            maze[x][y].setY(y);

            // Set outer walls
            if (x == 0) { maze[x][y].setWestWall(true); }
            if (y == 0) { maze[x][y].setSouthWall(true); }
            if (x == rows-1) { maze[x][y].setEastWall(true); }
            if (y == cols-1) { maze[x][y].setNorthWall(true); }
        }
    }

    Cell &goal = maze[2][2];
    Cell &start = maze[0][0];

    maze[0][2].setNorthWall(true);
    maze[0][3].setSouthWall(true);

    maze[1][2].setEastWall(true);
    maze[2][2].setWestWall(true);

    maze[0][0].setEastWall(true);
    maze[1][0].setWestWall(true);

    maze[0][1].setEastWall(true);
    maze[1][1].setWestWall(true);

    maze[3][2].setSouthWall(true);
    maze[2][2].setNorthWall(true);
    
    floodfill(maze, goal, rows, cols);

    // print out all values in maze map
     for (int x = 0; x < rows; ++x) {
        std::cout << '\n';
        for (int y = 0; y < cols; ++y) {
            std::cout << maze[x][y].getValue() << " ";
        }
    }
    std::cout << std::endl;
    return 0;
}