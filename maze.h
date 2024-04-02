#ifndef MAZE_H
#define MAZE_H

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
            // Serial.println(temp->getValue() + temp->getX() + temp->getY());
        }
    }

    // Add a wall to the maze and to the corrresponding adjacent cell
    void recordWall(int currentX, int currentY, int dir) {
        // Record north wall
        if (dir == 1) {
            maze[currentX][currentY].setNorthWall(true);
            
            // Set adajent wall
            if (currentY + 1 < rows) {
                maze[currentX][currentY + 1].setSouthWall(true);
            }
        }

        // Record east wall
        if (dir == 2) {
            maze[currentX][currentY].setEastWall(true);
            
            // Set adajent wall
            if (currentX + 1 < cols) {
                maze[currentX + 1][currentY].setWestWall(true);
            }
        }

        // Record south wall
        if (dir == 3) {
            maze[currentX][currentY].setSouthWall(true);
            
            // Set adajent wall
            if (currentY - 1 >= 0) {
                maze[currentX][currentY - 1].setNorthWall(true);
            }
        }

        // Record west wall
        if (dir == 4) {
            maze[currentX][currentY].setWestWall(true);

            // Set adajent wall
            if (currentX - 1 >= 0) {
                maze[currentX - 1][currentY].setEastWall(true);
            }
        }

    }

    Cell getNorthCell(int currentX, int currentY) {
        if (currentY + 1 < cols) {
            return maze[currentX][currentY + 1];
        }

        // Return default cell (value = -1) if out of range
        return Cell();
    }

    Cell getEastCell(int currentX, int currentY) {
        if (currentX + 1 < rows) {
            return maze[currentX + 1][currentY];    
        }
    
        // Return default cell (value = -1) if out of range
        return Cell();
    }

    Cell getSouthCell(int currentX, int currentY) {
        if (currentY - 1 >= 0) {
            return maze[currentX][currentY - 1];
        }

        // Return default cell (value = -1) if out of range
        return Cell();
    }

    Cell getWestCell(int currentX, int currentY) {
        if (currentX - 1 >= 0) {
            return maze[currentX - 1][currentY];
        }

        // Return default cell (value = -1) if out of range
        return Cell();
    }

    Cell getCurrentCell(int currentX, int currentY) {
        return maze[currentX][currentY]
    }

    // Print out all values in maze map
    // North is to the east but coordinates still make sense
    void printMaze() {
        for (int x = 0; x < rows; ++x) {
            Serial.println();
            for (int y = 0; y < cols; ++y) {
                Serial.print(maze[x][y].getValue() + " ");
            }
        }
    }
};

#endif