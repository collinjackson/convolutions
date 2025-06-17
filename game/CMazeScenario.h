// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_Maze_SCENARIO_H_INCLUDED__
#define __C_Maze_SCENARIO_H_INCLUDED__

#include "IScenario.h"
#include "CElevator.h"
#include "CVictoryTriggerLocation.h"

using namespace irr;

class CMazeScenario : public IScenario
{

enum maze_dir
{
	WEST = 0,
	SOUTH,
	EAST,
	NORTH
};

enum maze_data_struct
{
	BACK = 0, //backtrack
	SOL, //solution
	BORD, //border
	WALL
};

public:

	CMazeScenario(CStartupMenu* startupMenu);

	~CMazeScenario()
	{
		delete victory;
	}

	virtual scenario_t getScenarioID() { return SCENARIO_MAZE; }

	// Returns true if the user wants to go back to the startup menu
	// bool run();
	bool isActive();

private:

	void loadSceneData();

	IPerson* drone;			
	scene::ISceneNode* skyboxNode;
	scene::IAnimatedMeshSceneNode* model1;

	void makeWall(scene::ISceneNode* wall);
	bool isClosed(bool *cellWalls);

protected:

	CVictoryTriggerLocation* victory;

};

#endif