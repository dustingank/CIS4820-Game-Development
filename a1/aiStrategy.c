#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "graphics.h"
#include "paramater.h"
#include "gameWorldGenerate.h"
#include "aiStrategy.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];
	/* viewpoint control */
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

   /* mesh creation, translatio, rotation functions */
extern void unsetMeshID(int);
extern void setTranslateMesh(int, float, float, float);
extern void setRotateMesh(int, float, float, float);
extern void setScaleMesh(int, float);
extern void drawMesh(int);
extern void hideMesh(int); 

void aiStatus() {
	for (int i = 0; i < 9; i++) {
		if (currentObj[i].item == 3) {
			cactusMove(currentObj[i].id);
		}
	}
}

void cactusMove(int itemId) {
	int upperXrange = 
}