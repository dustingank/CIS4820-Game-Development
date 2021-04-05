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
		if (currentMonsterObj[i].item == 3 || currentMonsterObj[i].item == 2 || currentMonsterObj[i].item == 1) {
			cactusMove(i);
		}
	}
}

void cactusMove(int itemId) {
	int upperXrange = currentMonsterObj[itemId].x + 1;
	int lowerXrange = currentMonsterObj[itemId].x - 1;
	int upperZrange = currentMonsterObj[itemId].z + 1;
	int lowerZrange = currentMonsterObj[itemId].z - 1;
	float x, y, z;
	int x_int, y_int, z_int;
	getOldViewPosition(&x, &y, &z);
	x_int = (int)x * -1;
	y_int = (int)y * -1;
	z_int = (int)z * -1;

	if ((lowerXrange <= x_int && x_int <= upperXrange) && (lowerZrange <= z_int && z_int <= upperZrange)) {

		//printf("%d, %d, %d, %d, %d. %d", upperXrange, lowerXrange,upperZrange, lowerZrange, x_int, z_int);
		srand(time(NULL));
		int result = rand() % 2;
		if (currentMonsterObj[itemId].isAlive == 1) {
			if (result == 0) {
				if (currentMonsterObj[itemId].item == 1) {
					printf("fish(room %d) attack   Status: Missed\n", itemId);
				} else if (currentMonsterObj[itemId].item == 2) {
					printf("bat(room %d) attack   Status: Missed\n", itemId);
				} else if (currentMonsterObj[itemId].item == 3) {
					printf("cactus(room %d) attack   Status: Missed\n", itemId);
				}
			} else if (result == 1) {
				if (currentMonsterObj[itemId].item == 1) {
					printf("fish(room %d) attack   Status: hit\n", itemId);
				} else if (currentMonsterObj[itemId].item == 2) {
					printf("bat(room %d) attack   Status: hit\n", itemId);
				} else if (currentMonsterObj[itemId].item == 3) {
					printf("cactus(room %d) attack   Status: hit\n", itemId);
				}
			}
		}
	}

}