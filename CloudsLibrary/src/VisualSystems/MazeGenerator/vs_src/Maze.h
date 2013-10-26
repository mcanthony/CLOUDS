//
//  Maze.h
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#ifndef __MazeGenerator__Maze__
#define __MazeGenerator__Maze__

#include <ofMain.h>
#include "MazeCamera.h"
#include "MazeCell.h"
#include "ParamManager.h"

#define NUM_CELLS_X 31
#define NUM_CELLS_Y 1000

class Maze
{
public:
    float cellSize;
    float wallThickness;
    float wallHeight;
    
    Maze(float cSize, float wThick, float wHeight);
    ~Maze();
    void generate();
    
    void draw(ofCamera *cam);
    
    float getWidth();
    
    
protected:
    void generateStep();
    
    MazeCell* cells[NUM_CELLS_X][NUM_CELLS_Y];
    std::stack<MazeCell*> cellStack;
    int step;
    MazeCell* currentCell;
    MazeCell* exitCell;

    bool finishedGenerating;
    bool finishedSolving;
};

#endif /* defined(__MazeGenerator__Maze__) */
