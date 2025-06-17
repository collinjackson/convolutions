// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#include "CMazeScenario.h"
#include "CElevator.h"
#include "stereogram.h"

#include "CCivilianPerson.h"
#include "CDronePerson.h"
#include "CAllyPerson.h"

#include <stdio.h>
#include <vector>
#include <stack>
#include <deque>
#include <string>
#include <iostream>
#include <time.h>

#include <matrix4.h> 
#include "CStartupMenu.h"
#include "CMainMenu.h"
#include "ISkyBoxFaderSceneNode.h"

CMazeScenario::CMazeScenario(CStartupMenu* startupMenu)
	: IScenario(startupMenu),
	  model1(0), skyboxNode(0), victory(0)
{
  introAnimStartPos = core::vector3df(-480,15,-20000); 
  introAnimEndPos = core::vector3df(30,0,30);

  backColor.set(255,0,0,0);

}



void CMazeScenario::loadSceneData()
{
	// Create octtree, set up blending, skybox fader, etc.
	IScenario::loadSceneData();
	addMap("maze.bsp", core::vector3df(-1300,-70,-1249), false);

	addResultImage("media/text/maze_text.jpg", 1000);

	core::aabbox3d<f32> victorybox(core::vector3df(-5245.924805f,-100000.0f, -2155.608154f), 
		core::vector3df(-4845.924805f,100000.000f, -1755.608154f) ); // = model1->getBoundingBox();
	victory	= new CVictoryTriggerLocation(this, operative->getModel(), victorybox);

	// load quake level

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* sm = device->getSceneManager();

	// load sydney model and create 2 instances

	scene::IAnimatedMesh* mesh = 0;
	
	mesh = sm->getMesh("media\\models\\hobgoblin.md2");
	//mesh = sm->getMesh(startupMenu->getModelFilename(startupMenu->getCurrentOperatorModel()));
	if (mesh)
	{
		int j = 0, i = 0; 
		//for(j = 0; j<10; j++){for(i = 0; i<10; i++){
		
		model1 = sm->addAnimatedMeshSceneNode(mesh);
		if (model1 )
			//&& i==0 && j==0)
		{
			model1->setMaterialTexture(0, driver->getTexture("media\\models\\hobgoblin.jpg"));
	//		model1->setMaterialTexture(0, driver->getTexture(startupMenu->getModelTexture(startupMenu->getCurrentOperatorModel()));
			model1->setScale(core::vector3df(2,2,2));
			model1->setMD2Animation(scene::EMAT_SALUTE);
			model1->setMaterialFlag(video::EMF_LIGHTING, false);
			model1->setMaterialType(video::EMT_SPHERE_MAP);
			model1->addShadowVolumeSceneNode();
		}
		model1->setPosition(core::vector3df(-1480.0f + j * 50.0f,0.0f,-80.0f + i * 50.0f));
		//}}

		//sm->getMesh("media\\sydney.md2");
	}

	scene::ISceneNodeAnimator* anim = 0;

	// create sky box

/*
	skyboxNode = sm->addSkyBoxSceneNode(
		driver->getTexture("media\\skybox\\irrlicht2_up.bmp"),
		driver->getTexture("media\\skybox\\irrlicht2_dn.bmp"),
		driver->getTexture("media\\skybox\\irrlicht2_lf.bmp"),
		driver->getTexture("media\\skybox\\irrlicht2_rt.bmp"),
		driver->getTexture("media\\skybox\\irrlicht2_ft.bmp"),
		driver->getTexture("media\\skybox\\irrlicht2_bk.bmp"));
*/
	// create walk-between-portals animation
	
	CCivilianPerson *civilian = new CCivilianPerson(this, operative, "media/models/civi/tris.md2", "media/models/civi/civi.pcx");
	civilian->setPosition(core::vector3df(50,20,230));
	civilian->setPanic(true);
	animatableRegistry.add(civilian);

	drone = new CDronePerson(this, operative, "media/models/sas/tris.md2", "media/models/sas/sas.pcx");
	drone->setPosition(core::vector3df(50,-20,330));	

	//CAllyPerson *ally = new CAllyPerson(this, operative, "media/models/civi/tris.md2", "media/models/civi/civi_agent.bmp");
	//ally->setPosition(core::vector3df(50,-20,30));
	//ally->addEnemy(drone);
	
	//animatableRegistry.add(ally);
	animatableRegistry.add(drone);

	core::vector3df waypoint[2];
	waypoint[0].set(-150,40,100);
	waypoint[1].set(350,40,100);


	scene::ILightSceneNode* light = device->getSceneManager()->addLightSceneNode(0,	core::vector3df(0,0,0),	video::SColorf(1.0f, 0.5f, 0.5f, 1.0f), 1500);



	// create animation for portals;

	core::array<video::ITexture*> textures;
	for (s32 g=1; g<8; ++g)
	{
		char tmp[64];
		sprintf(tmp, "media\\portal%d.bmp", g);
		video::ITexture* t = driver->getTexture(tmp);
		textures.push_back(t);
	}
	
	anim = sm->createTextureAnimator(textures, 100);
	
	// create portals

	scene::IBillboardSceneNode* bill = 0;

	for (int r=0; r<2; ++r)
	{
		bill = sm->addBillboardSceneNode(0, core::dimension2d<f32>(100,100),
			waypoint[r]+ core::vector3df(0,20,0));
		bill->setMaterialFlag(video::EMF_LIGHTING, false);
		bill->setMaterialTexture(0, driver->getTexture("media\\portal1.bmp"));
		bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		bill->addAnimator(anim);
	}

	anim->drop();

	// create cirlce flying dynamic light with transparent billboard attached

	light = 0;

	light = device->getSceneManager()->addLightSceneNode(0,
		core::vector3df(0,0,0),	video::SColorf(1.0f, 0.5f, 0.5f, 1.0f), 1500);

	anim = device->getSceneManager()->createFlyCircleAnimator(
		core::vector3df(100,150,80), 80.0f, 0.0005f);

	light->addAnimator(anim);
	anim->drop();

	bill = device->getSceneManager()->addBillboardSceneNode(
		light, core::dimension2d<f32>(40,40));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialTexture(0, driver->getTexture("media\\particlewhite.bmp"));
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	// create camp fire

	scene::IParticleEmitter* em = getSmokeParticleSystem()->createBoxEmitter(
		core::aabbox3d<f32>(-7,0,-7,7,1,7), 
		core::vector3df(0.0f,0.03f,0.0f),
		80,100, video::SColor(0,255,255,255),video::SColor(0,255,255,255), 800,2000);

	getSmokeParticleSystem()->setEmitter(em);
	em->drop();


	//maze part
	
	
	//these will be defines once I abstract into my own class . . . 
	int MAX_X = 20;
	int MAX_Y = 10;

	core::vector2di stack[20*10];
	int stack_index = 0;

	bool cells[20][10][4][4];
	int i, j, k, newX, newY;
	int xScale, yScale, zScale;
	xScale = 300;
	yScale = 300;
	zScale = 300;

	core::vector3df init = core::vector3df(-4800, 0, -1500);
	//init maze
	
	int total =  MAX_X * MAX_Y;
	int visited = 1;

	for(i=0; i<MAX_X; i++){
		for(j=0; j<MAX_Y; j++){
			for(k=0;k<4;k++){
				cells[i][j][BORD][NORTH] = j==MAX_Y-1 ? true : false;
				cells[i][j][BORD][SOUTH] = j==0 ? true : false;
				cells[i][j][BORD][WEST]  = i==0 ? true : false;
				cells[i][j][BORD][WEST]  = i==MAX_X -1 ? true : false;

				cells[i][j][SOL][k] = false;

				cells[i][j][WALL][k] = true;
				cells[i][j][BACK][k] = false;

			}
		}
	}

	core::vector2di cur;

	

	srand(time(0));
	cur.X = rand() % MAX_X;
	cur.Y = rand() % MAX_Y;
	
	
	int whichSide;
	

	int test[4];


	
	while(visited < total){
		newX = -1;
		printf("Hi: %d, %d\n", cur.X, cur.Y);
		
		for(k=0;k<4;k++){
			whichSide = k==0 ? rand() % 4 : (whichSide + 1) % 4;	
			switch(whichSide){
			case WEST :
				if(cur.X - 1 > -1 && isClosed(cells[cur.X-1][cur.Y][WALL])){
					cells[cur.X][cur.Y][WALL][WEST] = false;
					cells[cur.X-1][cur.Y][WALL][EAST] = false;
					newX = cur.X-1; newY=cur.Y;
				}
				
				break;
			case SOUTH :
				if(cur.Y -1 > -1 && isClosed(cells[cur.X][cur.Y-1][WALL])){
					cells[cur.X][cur.Y][WALL][SOUTH] = false;
					cells[cur.X][cur.Y-1][WALL][NORTH] = false;
					newX = cur.X; newY=cur.Y-1;
				}
				
				break;
			case EAST :
				if(cur.X + 1 < MAX_X && isClosed(cells[cur.X+1][cur.Y][WALL])){
					cells[cur.X][cur.Y][WALL][EAST] = false;
					cells[cur.X+1][cur.Y][WALL][WEST] = false;
					newX = cur.X+1; newY=cur.Y;
				}
				
				break;
			case NORTH :
				if(cur.Y + 1 < MAX_Y && isClosed(cells[cur.X][cur.Y+1][WALL])){
					cells[cur.X][cur.Y][WALL][NORTH] = false;
					cells[cur.X][cur.Y+1][WALL][SOUTH] = false;
					newX = cur.X; newY=cur.Y+1;
				}
				
				break;
			default: printf("Something very bad happened.\n"); break;
			}
			if(newX != -1) break;
		}
		
		test[0] = isClosed(cells[newX][newY][WALL]) ? 1 : 0;
		printf("newX = %d; isClosed = %d", newX, test[0]);

		if(newX != -1){
			stack[stack_index] = core::vector2di(cur.X, cur.Y);
			stack_index++;
			cur.X = newX; cur.Y = newY;
			visited++;
		} else{
			stack_index--;
			printf("Stack index is now : %d\n, visted = %d", stack_index, visited);
			cur = stack[stack_index];
		}

	}


	cells[0][rand()%MAX_Y][WALL][WEST] = false;
	cells[MAX_X-1][rand()%MAX_Y][WALL][EAST] = false;


/*
	//randomly take out some walls
	for(i=0; i<MAX_X; i++){
		for(j=0; j<MAX_Y; j++){
			if(i!=0&& j!=0&& i<MAX_X-1 &&j<MAX_Y-1){
				for(k=0;k<4;k++){
					if(rand() % 10 > 7)	cells[i][j][WALL][k] = false;
				}
			}
		}
	}
*/
	scene::ISceneNode* wall;
	for(i=0; i<MAX_X; i++){
		for(j=0; j<MAX_Y; j++){
		
			if(cells[i][j][WALL][WEST]){
				wall = sm->addTestSceneNode (10.0f, 0, -1, core::vector3df(i * xScale - xScale/2, 0, j * zScale - zScale/2) + init, core::vector3df(0, 90, 0), core::vector3df(xScale / 9, yScale/ 10, zScale / 100));
				makeWall(wall);
			}
			if(cells[i][j][WALL][NORTH]){
				wall = sm->addTestSceneNode (10.0f, 0, -1, core::vector3df(i * xScale, 0, j * zScale) + init, core::vector3df(0, 0, 0), core::vector3df(xScale / 10, yScale / 10, zScale / 100));
				makeWall(wall);
			}
			if(cells[i][j][WALL][SOUTH]){
				wall = sm->addTestSceneNode (10.0f, 0, -1, core::vector3df(i * xScale, 0, j * zScale - zScale) + init, core::vector3df(0, 0, 0), core::vector3df(xScale / 10, yScale / 10, zScale / 100));
				makeWall(wall);
			}
			if(cells[i][j][WALL][EAST]){
				wall = sm->addTestSceneNode (10.0f, 0, -1, core::vector3df(i * xScale + xScale/2, 0, j * zScale - zScale/2) + init, core::vector3df(0, 90, 0), core::vector3df(xScale / 9, yScale / 10, zScale / 100));
				makeWall(wall);
			}
		}
	}

	
	//CElevator* elevator = new CElevator(this, operative->getModel(), core::aabbox3df(999,999,999,999,999,999));
	//elevator->addElevator(core::vector3df(80, 10, 80), 5.0f, 100);

	//build elevator

}

bool CMazeScenario::isClosed(bool *cellWalls){
	return (cellWalls[WEST] && cellWalls[SOUTH] && cellWalls[EAST] && cellWalls[NORTH]);
}

void  CMazeScenario::makeWall(scene::ISceneNode *wall){
	wall->setMaterialTexture(0, device->getVideoDriver()->getTexture("media\\maze_wall.jpg"));
	
	ITriangleSelector* selector = device->getSceneManager()->createTriangleSelectorFromBoundingBox(wall);
	stationaryTangibleRegistry.add(selector, 0);
	
}



