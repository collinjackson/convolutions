#include "CFollowerAI.h"
#include "IScenario.h"
#include "assert.h"
#include "CMainMenu.h"

void CFollowerAI::animate(u32 now){
	int i;


	f64 distance = ((source->getPosition()).getDistanceFrom(target->getPosition()));

	if(distance < threshold){  // if the target has moved within our radius

		core::vector3df Position = source->getPosition();
		
		if(numSteps >= MAXPATH){  //if at maxpath, compact the path
			int currentIndex;
			for(i=0; i<MAXPATH; i++){
				if(now - path[i].scent < pathDecayThreshold){
					currentIndex = i;
					break;
				}
			}
			for(i=0; i+currentIndex<MAXPATH; i++){
				path[i] = path[currentIndex+i];
			}
			numSteps = MAXPATH - currentIndex;
			currentStep = 0;
		}
		
		path[numSteps].position = target->getPosition();
		path[numSteps].scent = now;
		int oldNumSteps = numSteps;
		
		
		int lastMerge = numSteps;
		for(i=currentStep; i<oldNumSteps; i++){  // merge paths as needed
			if(path[oldNumSteps].position.getDistanceFrom(path[i].position) < mergePathThreshold){		
				//printf("Merging!\n");
				lastMerge = i;
				path[i].position = path[oldNumSteps].position;
				path[i].scent = path[oldNumSteps].scent;
				numSteps = i;
			}
		}
	
		

		numSteps++;
		
		
		//printf("2numSteps = %d, currentStep = %d\n", numSteps, currentStep);
		//}

		
		
		if(hostility){
			if(source->find(target->getPosition())){
				//printf("facing!\n");
				facingIndex = facingIndex > 10 ? facingIndex : facingIndex+1 ;
				currentStep = numSteps - 1;
				source->face(target->getPosition());
				source->sprint(1);
				source->shoot();
			} else if(currentStep < numSteps && now - path[currentStep].scent < pathDecayThreshold)	{ 
				//printf("following: step %d of %d!\n", currentStep, numSteps);
				source->face(path[currentStep].position);  //if we have a viable path, advance upon it
				source->sprint(1);
				if(path[currentStep].position.getDistanceFrom(source->getPosition()) < epsilon)	currentStep++;
			} 
		}
		else {
			bool tookAction = false;
			for(i=0; i<numEnemies; i++){
				//printf("consideringenemy: %f!\n", enemies[i]->getHealth());
				if(enemies[i] && (enemies[i]->getHealth() > 0 && enemies[i]->getHealth() <= 1) && source->find(enemies[i]->getPosition())){
					//printf("Shooting enemy!\n");
					tookAction = true;
					source->face(enemies[i]->getPosition());
					source->shoot();
				}	
			} 
			if(tookAction == false)	{ 
				if(source->find(target->getPosition())){
					
					currentStep = numSteps - 1;
					source->face(target->getPosition());
					source->sprint(1);
				} else {
					source->face(path[currentStep].position);  //if we have a viable path, advance upon it
					source->sprint(1);
					if(path[currentStep].position.getDistanceFrom(source->getPosition()) < epsilon)	currentStep++;
				}
			} 

		}

	}
	else{
		numSteps = 0;
		currentStep = 0;
		//printf("You lurk beyond the threshold!\n");
	}
}


void CFollowerAI::addEnemy(IPerson *enemy){
	if(numEnemies < MAXENEMIES){
		enemies[numEnemies] = enemy;
		numEnemies++;
	}
}