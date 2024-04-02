#include <iostream>
#include <string>
#include <queue>
#include "cell.h"

class Cell {
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
};

int main() {
    return 0;
}