#include <iostream>
#include <string>
#include <queue>

#include "cell.h"

class Maze {
private:
    Cell** maze;
    Cell* goal;
    int rows;
    int cols;

public:
    // Constructor
    Maze(int rows, int cols, int goalX, int goalY) : rows(rows), cols(cols), goal(nullptr) {
        maze = new Cell*[rows];
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

        goal = &maze[goalX][goalY];
    }

    void floodfill() {
        // 1. Set all cells except goal to “blank state”
        for (int x = 0; x < rows; ++x) {
            for (int y = 0; y < cols; ++y) {
                maze[x][y].reset();
            }
        }

        // 2. Set goal cell(s) value to 0 and add to queue
        goal->setValue(0);
        std::queue<Cell*> cellQueue;
        cellQueue.push(goal);

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

    // Print out all values in maze map
    void printMaze() {
        for (int x = 0; x < rows; ++x) {
            std::cout << '\n';
            for (int y = 0; y < cols; ++y) {
                std::cout << maze[x][y].getValue() << " ";
            }
        }
        std::cout << std::endl;
    }
};

int main() {
    Maze maze(8,8, 2,2);
    maze.floodfill();
    maze.printMaze();
    return 0;
}