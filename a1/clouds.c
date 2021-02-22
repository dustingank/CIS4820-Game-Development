#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graphics.h"
#include "clouds.h"
#include "paramater.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];

void makeClouds() {
    clouds[0][0] = 7;
    clouds[0][1] = 45;
    clouds[0][2] = 43;

    clouds[1][0] = 5;
    clouds[1][1] = 21;
    clouds[1][2] = 20;

    clouds[2][0] = 8;
    clouds[2][1] = 87;
    clouds[2][2] = 18;

    clouds[3][0] = 8;
    clouds[3][1] = 67;
    clouds[3][2] = 88;

    for (int i = 0; i < 4; i++) {
        drawCloud(clouds[i][0],clouds[i][1],clouds[i][2]);
    }
}

void moveCloud() { 
   for (int i = 0; i < 4; i++) {
      clouds[i][1] += 1;
      if ((clouds[i][1]) > WORLDX - 1) {
         clouds[i][1] = 0;
      }
      drawCloud(clouds[i][0],clouds[i][1],clouds[i][2]);
   }
}

void drawCloud (int radius, int xCenter, int zCenter) {
   for (int i = -radius; i < radius; i++) {
      for (int j = -radius; j < radius; j++) {
         if (i * i + j * j < radius * radius) {
            if (worldRange(xCenter + j, zCenter + i)) {
               world[xCenter + j][48][zCenter + i] = 30;
            } else {
               int newX = xCenter + j;
               int newZ = zCenter + i;

               if ((xCenter + j) > WORLDX - 1) {
                  newX -= WORLDX;
               }

               if ((zCenter + i) > WORLDZ - 1) {
                  newZ -= WORLDZ;
               }

               world[newX][48][newZ] = 30;
            }
         }
      }
   }
}