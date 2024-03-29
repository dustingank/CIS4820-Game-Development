#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "graphics.h"
#include "gameWorldGenerate.h"
#include "paramater.h"
#include "terrain.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];

extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void setOldViewPosition(float, float, float);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);
extern int setAssignedTexture(int, int);
extern void unsetAssignedTexture(int);
extern int getAssignedTexture(int);
extern void setTextureOffset(int, float, float);
extern void setMeshID(int, int, float, float, float);
extern void unsetMeshID(int);
extern void setTranslateMesh(int, float, float, float);
extern void setRotateMesh(int, float, float, float);
extern void setScaleMesh(int, float);
extern void drawMesh(int);
extern void hideMesh(int); 

void buildOutDoorWorld(int offset){
   int i, j, k;

   for (i = 0; i < WORLDX; i++) {
         for (j = 0; j < WORLDY; j++) {
            for (k = 0; k < WORLDZ; k++) {
               world[i][j][k] = 0;
            }
         }
      }

   for (i = 0; i < WORLDX; i++) {
      for (k = 0; k < WORLDZ; k++) {
         float result = (float)perlin(i + offset, k, 0.085);
         result = result * 11.7;
         //printf("%f\n", result);
         if (result < 0.0) {
            if (result > -0.4) {
               for (int a = 0; a < 12; a++) {
                  world[i][24 - a][k] = 4;
               }
            } else if (result < -0.5 && result > -1.0) {
               for (int a = 0; a < 12; a++) {
                  world[i][24 - 1 - a][k] = 4;
               }
            } else {
               for (int a = 0; a < 10; a++) {
                  world[i][24 + (int)result - a][k] = 4;
               }
            }
         } else if (result > 0.0) {
            if (result < 0.4) {
               for (int a = 0; a < 12; a++) {
                  world[i][24 - a][k] = 4;
               }
            } else if (result > 0.5 && result < 1.0){
               for (int a = 0; a < 12; a++) {
                  world[i][25 - a][k] = 4;
               }
               world[i][24 + 1][k] = 1;
            } else {
               for (int a = 0; a < 18; a++) {
                  world[i][(24 + (int)result) - a][k] = 4;
               }
            }
         } else {
            world[i][24][k] = 3;
         }
      }
   }

   int entry = 0;
   for (i = 0; i < WORLDX; i++) {
      for (j = 0; j < WORLDY; j++) {
         for (k = 0; k < WORLDZ; k++) {
            if (world[i][j][k] != 0) {
               if (j <= 21 && isCubeVisible(i, j, k)) {
                  world[i][j][k] = randomNumber(10, 12);
               } else if (j >= 28 && isCubeVisible(i, j, k)) {
                  world[i][j][k] = 31;
               } else {
                  if (i > 45 && k > 45 && j > 25 && entry == 0) {
                     if (checkAround(i, j, k)) {
                        //printf("%d, %d, %d\n", i, j, k);
                        spawnLocation[0] = i + 1;
                        spawnLocation[1] = j;
                        spawnLocation[2] = k + 1;
                        world[i][j][k] = 9;
                        entry = 1;
                     }
                  } else if (isCubeVisible(i, j, k)){
                     world[i][j][k] = randomNumber(14, 17);
                  }
               }
            }
         }
      }
   }

   for (int i = 0; i < 9; i++) {
      
      currentMonsterObj[i].id = 0;
      currentMonsterObj[i].item = 0;
      currentMonsterObj[i].x = 0;
      currentMonsterObj[i].y = 0;
      currentMonsterObj[i].z = 0;
      currentMonsterObj[i].isAlive = 0;

      roomLocation[0][i] = 0;
      roomLocation[1][i] = 0;
      roomLocation[2][i] = 0;
      roomLocation[3][i] = 0;
      roomLocation[4][i] = 0; // make sure none of the room been visited
   }

   

   spawnLocation[1] = 29;
   setViewPosition((spawnLocation[0] * -1),-29-0.2,(spawnLocation[2] * -1));
}

void generateRoom(int minWidth, int maxWidth, int minLength, int maxLength, int location) {
   int centerDoorBottom = 0;
   int centerDoorRight = 0;
   int centerDoorTop = 0;
   int centerDoorLeft = 0;

   int cubeLocX = 0;
   int cubeLocZ = 0;
   int spawnX = 0;
   int spawnZ = 0;

   int centerX = randomNumber(minWidth + 10, maxWidth - 10);
   int centerZ = randomNumber(minLength + 10, maxLength - 10);

   int range1 = randomNumber(5, centerX - minWidth - 3); // lower bound for width
   int range2 = randomNumber(5, maxWidth - centerX); // higher bound for width
   int range3 = randomNumber(5, centerZ - minLength - 3); // lower bound for length
   int range4 = randomNumber(5, maxLength - centerZ); // higher bound for length

   int maxIndexWidth = range1 + range2; // total width
   int maxIndexLength = range3 + range4; // total length

   int startX = centerX - range1;
   int endX = centerX + range2;
   int startZ = centerZ - range3;
   int endZ = centerZ + range4;

   /* creat the wall */
   for (int i = startX; i <= endX; i++) { 
      for (int a = 1; a <= 4; a++) {
         world[i][24 + a][startZ] = 18;
         world[i][24 + a][endZ] = 18;
      }
   }

   for (int j = startZ; j <= endZ; j++) {
      for (int a = 1; a <= 4; a++) {
         world[startX][24 + a][j] = 18;
         world[endX][24 + a][j] = 18;
      }
   }

   // building the celling and floor
   for (int i = startX; i < endX; i++) {
      for (int j = startZ; j <= endZ; j++) {
         world[i][28][j] = 19;
         world[i][24][j] = 20;
      }
   }

   switch (location) {
      case 1:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);

         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the bottom
               world[centerDoorBottom + i][24 + j][endZ] = 0;

               // door at the right
               world[endX][24 + j][centerDoorRight + i] = 0;
            }
         }
         horizontalCorridor[0][0] = endX;
         horizontalCorridor[0][1] = centerDoorRight;

         verticalCorridor[0][0] = centerDoorBottom;
         verticalCorridor[0][1] = endZ;
         
         roomLocation[0][0] = startX;
         roomLocation[1][0] = endX;
         roomLocation[2][0] = startZ;
         roomLocation[3][0] = endZ;
         break;

      case 2:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);
         
         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the bottom
               world[centerDoorBottom + i][24 + j][endZ] = 0;

               // door at the right
               world[endX][24 + j][centerDoorRight + i] = 0;

               // door at the left
               world[startX][24 + j][centerDoorLeft + i] = 0;
            }
         }
         horizontalCorridor[0][2] = startX;
         horizontalCorridor[0][3] = centerDoorLeft;
         horizontalCorridor[0][4] = endX;
         horizontalCorridor[0][5] = centerDoorRight;

         verticalCorridor[1][0] = centerDoorBottom;
         verticalCorridor[1][1] = endZ;

         roomLocation[0][1] = startX;
         roomLocation[1][1] = endX;
         roomLocation[2][1] = startZ;
         roomLocation[3][1] = endZ;
         break;

      case 3:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);
         
         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the bottom
               world[centerDoorBottom + i][24 + j][endZ] = 0;

               // door at the left
               world[startX][24 + j][centerDoorLeft + i] = 0;
            }
         }
         horizontalCorridor[0][6] = startX;
         horizontalCorridor[0][7] = centerDoorLeft;

         verticalCorridor[2][0] = centerDoorBottom;
         verticalCorridor[2][1] = endZ;

         // save the room location
         roomLocation[0][2] = startX;
         roomLocation[1][2] = endX;
         roomLocation[2][2] = startZ;
         roomLocation[3][2] = endZ;
         break;

      case 4:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);

         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the bottom
               world[centerDoorBottom + i][24 + j][endZ] = 0;

               // door at the right
               world[endX][24 + j][centerDoorRight + i] = 0;

               // door at the Top
               world[centerDoorTop + i][24 + j][startZ] = 0;
            }
         }
         horizontalCorridor[1][0] = endX;
         horizontalCorridor[1][1] = centerDoorRight;

         verticalCorridor[0][2] = centerDoorTop;
         verticalCorridor[0][3] = startZ;
         verticalCorridor[0][4] = centerDoorBottom;
         verticalCorridor[0][5] = endZ;

         // save the room location 
         roomLocation[0][3] = startX;
         roomLocation[1][3] = endX;
         roomLocation[2][3] = startZ;
         roomLocation[3][3] = endZ;
         break;

      case 5:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 3;


         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the bottom
               world[centerDoorBottom + i][24 + j][endZ] = 0;

               // door at the right
               world[endX][24 + j][centerDoorRight + i] = 0;

                // door at the left
               world[startX][24 + j][centerDoorLeft + i] = 0;

               // door at the Top
               world[centerDoorTop + i][24 + j][startZ] = 0;
            }
         }

         horizontalCorridor[1][2] = startX;
         horizontalCorridor[1][3] = centerDoorLeft;
         horizontalCorridor[1][4] = endX;
         horizontalCorridor[1][5] = centerDoorRight;

         verticalCorridor[1][2] = centerDoorTop;
         verticalCorridor[1][3] = startZ;
         verticalCorridor[1][4] = centerDoorBottom;
         verticalCorridor[1][5] = endZ;

         // save the room location 
         roomLocation[0][4] = startX;
         roomLocation[1][4] = endX;
         roomLocation[2][4] = startZ;
         roomLocation[3][4] = endZ;
         break;

      case 6:
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);
         centerDoorBottom = randomNumber(startX + 3, endX - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 3;


         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the bottom
               world[centerDoorBottom + i][24 + j][endZ] = 0;

               // door at the left
               world[startX][24 + j][centerDoorLeft + i] = 0;

               // door at the Top
               world[centerDoorTop + i][24 + j][startZ] = 0;
            }

         }
         horizontalCorridor[1][6] = startX;
         horizontalCorridor[1][7] = centerDoorLeft;

         verticalCorridor[2][2] = centerDoorTop;
         verticalCorridor[2][3] = startZ;
         verticalCorridor[2][4] = centerDoorBottom;
         verticalCorridor[2][5] = endZ;

         // save the room location 
         roomLocation[0][5] = startX;
         roomLocation[1][5] = endX;
         roomLocation[2][5] = startZ;
         roomLocation[3][5] = endZ;
         break;

      case 7:
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 3;

         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the right
               world[endX][24 + j][centerDoorRight + i] = 0;

               // door at the Top
               world[centerDoorTop + i][24 + j][startZ] = 0;
            }
         }
         horizontalCorridor[2][0] = endX;
         horizontalCorridor[2][1] = centerDoorRight;

         verticalCorridor[0][6] = centerDoorTop;
         verticalCorridor[0][7] = startZ;

         // save the room location 
         roomLocation[0][6] = startX;
         roomLocation[1][6] = endX;
         roomLocation[2][6] = startZ;
         roomLocation[3][6] = endZ;
         break;

      case 8:
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 3;

         for (int i = -1; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the right
               world[endX][24 + j][centerDoorRight + i] = 0;

               // door at the left
               world[startX][24 + j][centerDoorLeft + i] = 0;

               // door at the Top
               world[centerDoorTop + i][24 + j][startZ] = 0;
            }
         }
         horizontalCorridor[2][2] = startX;
         horizontalCorridor[2][3] = centerDoorLeft;
         horizontalCorridor[2][4] = endX;
         horizontalCorridor[2][5] = centerDoorRight;

         verticalCorridor[1][6] = centerDoorTop;
         verticalCorridor[1][7] = startZ;

         // save the room location 
         roomLocation[0][7] = startX;
         roomLocation[1][7] = endX;
         roomLocation[2][7] = startZ;
         roomLocation[3][7] = endZ;
         break;

      case 9:
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);
         
         // a randomCube in side the room
         world[startX + 5][25][startZ + 5] = 5;
         
         world[endX - 5][25][endZ - 5] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[0] = endX - 2;
         spawnLocation[1] = 26;
         spawnLocation[2] = endZ - 3;
         
         for (int i = 0; i <= 1; i++) {
            for (int j = 1; j <= 3; j++) {
               // door at the left
               world[startX][24 + j][centerDoorLeft + i] = 0;

               // door at the Top
               world[centerDoorTop + i][24 + j][startZ] = 0;
            }
         }
         horizontalCorridor[2][6] = startX;
         horizontalCorridor[2][7] = centerDoorLeft;

         verticalCorridor[2][6] = centerDoorTop;
         verticalCorridor[2][7] = startZ;
         
         // save the room location 
         roomLocation[0][8] = startX;
         roomLocation[1][8] = endX;
         roomLocation[2][8] = startZ;
         roomLocation[3][8] = endZ;
         break;

      default:
         printf("Error at room generate function: unknown room location %d\n", location);
         exit(1);
   }

}

void buildCorridor() {
   //printf("should be: %d %d %d %d %d %d %d %d\n", test[0],test[1],test[2],test[3],test[4],test[5],test[6],test[7]);
   int count = 0;
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j<= 1; j++) {
         buildHorizontalCorridorWall(horizontalCorridor[i][0 + j * 4], horizontalCorridor[i][1 + j * 4], 
               horizontalCorridor[i][2 + j * 4], horizontalCorridor[i][3 + j * 4], count);

         buildVerticalCorridorWall(verticalCorridor[i][0 + j * 4], verticalCorridor[i][1 + j * 4], 
               verticalCorridor[i][2 + j * 4], verticalCorridor[i][3 + j * 4], count);
         
         count += 1;
      }
   }
}

void buildHorizontalCorridorWall(int leftX, int leftY, int rightX, int rightY, int count) {
   int remainder = 0;
   int currentLeft = 0;
   int currentRight = 0;
   //printf("%d %d %d %d\n", leftX, leftY, rightX, rightY);
   int horizontalDistance = abs(leftX - rightX); // horizontal distance between two wall
   int verticalDistance = abs(leftY - rightY); // vertical distance betweeen two wall

   // build the wall at horizontal vertex
   for (int i = 0; i < horizontalDistance / 2; i++) {
      for (int a = 1; a <= 4; a++) {
         world[leftX + i][24 + a][leftY + 1] = 18;
         world[leftX + i][24 + a][leftY - 1] = 18;
         world[rightX - i][24 + a][rightY + 1] = 18;
         world[rightX - i][24 + a][rightY - 1] = 18;
      }
      
      world[leftX + i][28][leftY] = 19;
      world[rightX - i][28][rightY] = 19;

      world[leftX + i][24][leftY] = 21;
      world[rightX - i][24][rightY] = 21;

      currentLeft = leftX + i;
      currentRight = rightX - i;
   }
   remainder = horizontalDistance % 2;

   // if the haorizontal distance if not events
   if (remainder != 0) {
      leftX = currentLeft;

      for (int i = 1; i < remainder; i++) {
         for (int a = 1; a <= 4; a++) {
            world[leftX + i][24 + a][leftY + 1] = 18;
            world[leftX + i][24 + a][leftY - 1] = 18;
         }

         world[leftX + i][28][leftY] = 19;
         world[leftX + i][24][leftY] = 21;
         currentLeft = leftX + i;
      }
   }

   // building the wall at vertical vertax
   if (verticalDistance != 0) {
      if (leftY < rightY) {
         // extend 2 unit for the most upper wall
         for (int a = 1; a <= 4; a++) {
            world[currentRight - 1][24 + a][rightY + 1] = 18;
            world[currentRight - 2][24 + a][rightY + 1] = 18;
            world[currentLeft + 1][24 + a][leftY - 1] = 18;
            world[currentLeft + 2][24 + a][leftY - 1] = 18;
         }

         world[currentRight - 1][28][rightY] = 19;
         world[currentRight - 2][28][rightY] = 19;
         world[currentLeft + 1][28][leftY] = 19;
         world[currentLeft + 2][28][leftY] = 19;

         world[currentRight - 1][24][rightY] = 21;
         world[currentRight - 2][24][rightY] = 21;
         world[currentLeft + 1][24][leftY] = 21;
         world[currentLeft + 2][24][leftY] = 21;


         for (int j = 1; j <= verticalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[currentRight - 2][24 + a][rightY + 1 - j] = 18;
               world[currentRight][24 + a][rightY - 1 - j] = 18;
            }
            world[currentRight - 1][28][rightY - j] = 19;
            world[currentRight - 1][24][rightY - j] = 21;
         }
      } else {
         for (int a = 1; a <= 4; a++) {
            // extend 2 unit for the most bottom wall
            world[currentRight - 1][24 + a][rightY - 1] = 18;
            world[currentRight - 2][24 + a][rightY - 1] = 18;

            // extend 2 unit for the most bottom wall
            world[currentLeft + 1][24 + a][leftY + 1] = 18;
            world[currentLeft + 2][24 + a][leftY + 1] = 18;
         }
         
         world[currentRight - 1][28][rightY] = 19;
         world[currentRight - 2][28][rightY] = 19;
         world[currentLeft + 1][28][leftY] = 19;
         world[currentLeft + 2][28][leftY] = 19;

         world[currentRight - 1][24][rightY] = 21;
         world[currentRight - 2][24][rightY] = 21;
         world[currentLeft + 1][24][leftY] = 21;
         world[currentLeft + 2][24][leftY] = 21;

         for (int j = 1; j <= verticalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[currentLeft + 2][24 + a][leftY + 1 - j] = 18;
               world[currentLeft][24 + a][leftY - 1 - j] = 18;
            }
            world[currentLeft + 1][28][leftY - j] = 19;

            world[currentLeft + 1][24][leftY - j] = 21;
         }
      }
   } else {
      for (int a = 1; a <= 4; a++) {
         world[currentLeft + 1][24 + a][leftY + 1] = 18;
         world[currentLeft + 1][24 + a][leftY - 1] = 18;
         world[currentRight - 1][24 + a][rightY + 1] = 18;
         world[currentRight - 1][24 + a][rightY - 1] = 18;
      }

      world[currentLeft + 1][28][leftY] = 19;
      world[currentLeft + 1][28][leftY] = 19;
      world[currentRight - 1][28][rightY] = 19;
      world[currentRight - 1][28][rightY] = 19;

      world[currentLeft + 1][24][leftY] = 21;
      world[currentLeft + 1][24][leftY] = 21;
      world[currentRight - 1][24][rightY] = 21;
      world[currentRight - 1][24][rightY] = 21;
   }
}

void buildVerticalCorridorWall(int topX, int topY, int buttomX, int buttomY, int count) {
   int remainder = 0;
   int currentTop = 0;
   int currentButtom = 0;

   int horizaontalDistance = abs(topX - buttomX);
   int verticalDistance = abs(topY - buttomY);

   for (int i = 0; i < verticalDistance / 2; i++) {
      for (int a = 1; a <= 4; a++) {
         world[topX + 1][24 + a][topY + i] = 18;
         world[topX - 1][24 + a][topY + i] = 18;
         world[buttomX + 1][24 + a][buttomY - i] = 18;
         world[buttomX - 1][24 + a][buttomY - i] = 18;
      }

      world[topX][28][topY + i] = 19;
      world[buttomX ][28][buttomY - i] = 19;

      world[topX][24][topY + i] = 21;
      world[buttomX ][24][buttomY - i] = 21;

      currentTop = topY + i;
      currentButtom = buttomY - i;
   }
   remainder = verticalDistance % 2;

   if (remainder != 0) {
      topY = currentTop;

      for (int i = 1; i < remainder; i++) {
         for (int a = 1; a <= 4; a++) {
            world[topX + 1][24 + a][topY + i] = 18;
            world[topX - 1][24 + a][topY + i] = 18;
         } 
         currentTop = topY + i;
         world[topX][28][topY + i] = 19;
         world[topX][24][topY + i] = 21;
      }
   }

   if (horizaontalDistance != 0) {
      if (topX < buttomX) {
         for (int a = 1; a <= 4; a++) {
            world[topX - 1][24 + a][currentTop + 1] = 18;
            world[topX - 1][24 + a][currentTop + 2] = 18;
            world[buttomX + 1][24 + a][currentButtom - 1] = 18;
            world[buttomX + 1][24 + a][currentButtom - 2] = 18;
         }

         world[topX][28][currentTop + 1] = 19;
         world[topX][28][currentTop + 2] = 19;
         world[buttomX][28][currentButtom - 1] = 19;
         world[buttomX][28][currentButtom - 2] = 19;

         world[topX][24][currentTop + 1] = 21;
         world[topX][24][currentTop + 2] = 21;
         world[buttomX][24][currentButtom - 1] = 21;
         world[buttomX][24][currentButtom - 2] = 21;

         for (int j = 1; j <= horizaontalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[topX - 1 + j][24 + a][currentTop + 2] = 18;
               world[topX + 1 + j][24 + a][currentTop] = 18;
            }
            world[topX + j][28][currentTop + 1] = 19;
            world[topX + j][24][currentTop + 1] = 21;
         }
      } else {
         for (int a = 1; a <= 4; a++) {
            world[topX + 1][24 + a][currentTop + 1] = 18;
            world[topX + 1][24 + a][currentTop + 2] = 18;
            world[buttomX - 1][24 + a][currentButtom - 1] = 18;
            world[buttomX - 1][24 + a][currentButtom - 2] = 18;
         }

         world[topX][28][currentTop + 1] = 19;
         world[topX][28][currentTop + 2] = 19;
         world[buttomX][28][currentButtom - 1] = 19;
         world[buttomX][28][currentButtom - 2] = 19;

         world[topX][24][currentTop + 1] = 21;
         world[topX][24][currentTop + 2] = 21;
         world[buttomX][24][currentButtom - 1] = 21;
         world[buttomX][24][currentButtom - 2] = 21;

         for (int j = 1; j <= horizaontalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[buttomX - 1 + j][24 + a][currentButtom - 2] = 18;
               world[buttomX + 1 + j][24 + a][currentButtom] = 18;
            }
            world[buttomX + j][28][currentButtom - 1] = 19;
            world[buttomX + j][24][currentButtom - 1] = 21;
         }
      }
   } else {
      for (int a = 0; a <= 4; a++) {
         world[topX + 1][24 + a][currentTop + 1] = 18;
         world[topX - 1][24 + a][currentTop + 1] = 18;
         world[buttomX + 1][24 + a][currentButtom - 1] = 18;
         world[buttomX - 1][24 + a][currentButtom - 1] = 18;
      }

      world[topX][28][currentTop + 1] = 19;
      world[buttomX][28][currentButtom - 1] = 19;

      world[topX][24][currentTop + 1] = 21;
      world[buttomX][24][currentButtom - 1] = 21;
   }
}

void buildWorld(int level) {
   int i, j, k;
   int currentLevel = level;

   /* initialize the world */ 
   for (i = 0; i < WORLDX; i++) {
      for (j = 0; j < WORLDY; j++) {
         for (k = 0; k < WORLDZ; k++) {
            world[i][j][k] = 0;
         }
      }
   }
   /* a platform*/
   for (i = 0; i < WORLDX; i++) {
      for (j = 0; j < WORLDZ; j++) {
         world[i][24][j] = randomNumber(10,12);
      }
   }
   int starting = 1 + (currentLevel - 2) * 10;

   if (level % 2 != 0) {
      buildCaveLevel();
      for (int i = 0; i < 9; i++) {
      
         currentMonsterObj[i].id = 0;
         currentMonsterObj[i].item = 0;
         currentMonsterObj[i].x = 0;
         currentMonsterObj[i].y = 0;
         currentMonsterObj[i].z = 0;
         currentMonsterObj[i].isAlive = 0;

         roomLocation[0][i] = 0;
         roomLocation[1][i] = 0;
         roomLocation[2][i] = 0;
         roomLocation[3][i] = 0;
         roomLocation[4][i] = 0;
      }

      currentMonsterObj[0].id = starting;
      currentMonsterObj[0].item = 3;
      currentMonsterObj[0].visability = 0;
      currentMonsterObj[0].x = 54;
      currentMonsterObj[0].y = 25.3;
      currentMonsterObj[0].z = 50;
      currentMonsterObj[0].isAlive = 1;
      setMeshID(currentMonsterObj[0].id, currentMonsterObj[0].item, currentMonsterObj[0].x, currentMonsterObj[0].y, currentMonsterObj[0].z);
      setScaleMesh(currentMonsterObj[0].id, 0.4);
      
      currentItemObj[0].id = starting + 1;
      currentItemObj[0].item = 8;
      currentItemObj[0].visability = 0;
      currentItemObj[0].x = 46;
      currentItemObj[0].y = 25;
      currentItemObj[0].z = 46;
      currentItemObj[0].isAlive = 1;
      //printf("2\n");
      setMeshID(currentItemObj[0].id, currentItemObj[0].item, currentItemObj[0].x, currentItemObj[0].y, currentItemObj[0].z);
      setScaleMesh(currentItemObj[0].id, 1);
      //printf("All mesh object done\n");


   } else {
      srand(time(NULL));
      generateRoom(0,33,0,33, 1); // index {0, 0}
      generateRoom(34,67,0,33, 2); // index {0, 1}
      generateRoom(68,99,0,33, 3); // index {0, 2}

      generateRoom(0,33,34,67, 4); // index {0, 1}
      generateRoom(34,67,34,67, 5); // index {1, 1}
      generateRoom(68,99,34,67, 6); // index {1, 2}

      generateRoom(0,33,68,99, 7); // index {0, 2}
      generateRoom(34,67,68,99, 8); // index {1, 2}
      generateRoom(69,99,69,99, 9); // index {2, 2}
      //printf("All room generate done\n");

      for (int i = 0; i < 9; i++) {
         int x = roomLocation[0][i] + 8;
         int z = roomLocation[2][i] + 9;
         int item = randomNumber(0,3);
         
         currentMonsterObj[i].id = starting + i;
         if (i == 8) {
            currentMonsterObj[i].item = 3;
         } else {
            currentMonsterObj[i].item = randomNumber(1, 3);
         }
         currentMonsterObj[i].visability = 0;
         currentMonsterObj[i].x = (float)x;
         currentMonsterObj[i].y = 25.3;
         currentMonsterObj[i].z = (float)z;
         currentMonsterObj[i].isAlive = 1;
         setMeshID(currentMonsterObj[i].id, currentMonsterObj[i].item, currentMonsterObj[i].x, currentMonsterObj[i].y, currentMonsterObj[i].z);
         setScaleMesh(currentMonsterObj[i].id, 0.4);
         roomLocation[4][i] = 0; // make sure none of the room been visited
      }

      //printf("All monster done\n");

      int roomID = randomNumber(0,5);
      int spawnXindex = roomLocation[0][roomID] + 3;
      int spawnZindex = roomLocation[2][roomID] + 3;
      //printf("1\n");
      currentItemObj[0].id = starting + 9;
      currentItemObj[0].item = 8;
      currentItemObj[0].visability = 0;
      currentItemObj[0].x = (float)spawnXindex;
      currentItemObj[0].y = 25;
      currentItemObj[0].z = (float)spawnZindex;
      currentItemObj[0].isAlive = 1;
      //printf("2\n");
      setMeshID(currentItemObj[0].id, currentItemObj[0].item, currentItemObj[0].x, currentItemObj[0].y, currentItemObj[0].z);
      setScaleMesh(currentItemObj[0].id, 1);
      //printf("All mesh object done\n");

      buildCorridor();
      spawnLocation[1] = 25;
      setViewPosition((spawnLocation[0] * -1),-25- 0.2,(spawnLocation[2] * -1));
   }
}

void buildCaveLevel(){
   for (int i = 0; i < WORLDX; i++) {
      for (int j = 0; j< WORLDZ; j++) {
         float x_float = ((float)i - 50) / 50;
         float z_float = ((float)j - 50) / 50; 
         float y_float = 1.0 - ((float)(x_float * x_float + z_float * z_float) / 2.0);
         int y = (y_float) * 20 + 25;
         world[i][y][j] = 19;

         if (i == 0 || j == 0 || i == 99 || j == 99) {
            for (int a = 25; a < 45; a++) {
               world[i][a][j] = 19;
            }
         }
      }
   }
   world[24][25][30] = 5;
   world[40][25][70] = 9;

   spawnLocation[1] = 25;
   spawnLocation[0] = 25;
   spawnLocation[2] = 35;
   setViewPosition((spawnLocation[0] * -1),-25-0.2,(spawnLocation[2] * -1));
}
