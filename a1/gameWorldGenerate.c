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

   // building the celling
   for (int i = startX; i < endX; i++) {
      for (int j = startZ; j <= endZ; j++) {
         world[i][28][j] = 19;
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
         
         //printf("here\n");
         //cubeLocX = randomNumber(startX + 5, endX / 2);
         //cubeLocZ = randomNumber(startZ + 5, endZ / 2);
         // a randomCube in side the room
         world[startX + 5][25][startZ + 5] = 5;
         
         //printf("here1\n");
         //cubeLocX = randomNumber(startX + 15, endX);
         //cubeLocZ = randomNumber(startZ + 15, endZ);
         world[endX - 5][25][endZ - 5] = 9;

         //printf("here2\n");
         
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
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j<= 1; j++) {
         buildHorizontalCorridorWall(horizontalCorridor[i][0 + j * 4], horizontalCorridor[i][1 + j * 4], 
               horizontalCorridor[i][2 + j * 4], horizontalCorridor[i][3 + j * 4]);

         buildVerticalCorridorWall(verticalCorridor[i][0 + j * 4], verticalCorridor[i][1 + j * 4], 
               verticalCorridor[i][2 + j * 4], verticalCorridor[i][3 + j * 4]);
      }
   }
}

void buildHorizontalCorridorWall(int leftX, int leftY, int rightX, int rightY) {
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
      
      world[leftX + i][28][leftY] = 18;
      world[rightX - i][28][rightY] = 18;

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

         world[leftX + i][28][leftY] = 18;
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

         world[currentRight - 1][28][rightY] = 18;
         world[currentRight - 2][28][rightY] = 18;
         world[currentLeft + 1][28][leftY] = 18;
         world[currentLeft + 2][28][leftY] = 18;


         for (int j = 1; j <= verticalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[currentRight - 2][24 + a][rightY + 1 - j] = 18;
               world[currentRight][24 + a][rightY - 1 - j] = 18;
            }
            world[currentRight - 1][28][rightY - j] = 18;
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
         
         world[currentRight - 1][28][rightY] = 18;
         world[currentRight - 2][28][rightY] = 18;
         world[currentLeft + 1][28][leftY] = 18;
         world[currentLeft + 2][28][leftY] = 18;

         for (int j = 1; j <= verticalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[currentLeft + 2][24 + a][leftY + 1 - j] = 18;
               world[currentLeft][24 + a][leftY - 1 - j] = 18;
            }
            world[currentLeft + 1][28][leftY - j] = 18;
         }
      }
   } else {
      for (int a = 1; a <= 4; a++) {
         world[currentLeft + 1][24 + a][leftY + 1] = 18;
         world[currentLeft + 1][24 + a][leftY - 1] = 18;
         world[currentRight - 1][24 + a][rightY + 1] = 18;
         world[currentRight - 1][24 + a][rightY - 1] = 18;
      }

      world[currentLeft + 1][28][leftY] = 18;
      world[currentLeft + 1][28][leftY] = 18;
      world[currentRight - 1][28][rightY] = 18;
      world[currentRight - 1][28][rightY] = 18;
   }

}

void buildVerticalCorridorWall(int topX, int topY, int buttomX, int buttomY) {
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

      world[topX][28][topY + i] = 18;
      world[buttomX ][28][buttomY - i] = 18;

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
         world[topX][28][topY + i] = 18;
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

         world[topX][28][currentTop + 1] = 18;
         world[topX][28][currentTop + 2] = 18;
         world[buttomX][28][currentButtom - 1] = 18;
         world[buttomX][28][currentButtom - 2] = 18;

         for (int j = 1; j <= horizaontalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[topX - 1 + j][24 + a][currentTop + 2] = 18;
               world[topX + 1 + j][24 + a][currentTop] = 18;
            }
            world[topX + j][28][currentTop + 1] = 18;
         }
      } else {
         for (int a = 1; a <= 4; a++) {
            world[topX + 1][24 + a][currentTop + 1] = 18;
            world[topX + 1][24 + a][currentTop + 2] = 18;
            world[buttomX - 1][24 + a][currentButtom - 1] = 18;
            world[buttomX - 1][24 + a][currentButtom - 2] = 18;
         }

         world[topX][28][currentTop + 1] = 18;
         world[topX][28][currentTop + 2] = 18;
         world[buttomX][28][currentButtom - 1] = 18;
         world[buttomX][28][currentButtom - 2] = 18;

         for (int j = 1; j <= horizaontalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[buttomX - 1 + j][24 + a][currentButtom - 2] = 18;
               world[buttomX + 1 + j][24 + a][currentButtom] = 18;
            }
            world[buttomX + j][28][currentButtom - 1] = 18;
         }
      }
   } else {
      for (int a = 0; a <= 4; a++) {
         world[topX + 1][24 + a][currentTop + 1] = 18;
         world[topX - 1][24 + a][currentTop + 1] = 18;
         world[buttomX + 1][24 + a][currentButtom - 1] = 18;
         world[buttomX - 1][24 + a][currentButtom - 1] = 18;
      }

      world[topX][28][currentTop + 1] = 18;
      world[buttomX][28][currentButtom - 1] = 18;
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
   
   srand(time(NULL));
   /* a platform*/
   for (i = 0; i < WORLDX; i++) {
      for (j = 0; j < WORLDZ; j++) {
         world[i][24][j] = randomNumber(10,12);
      }
   }

   generateRoom(0,33,0,33, 1); // index {0, 0}
   generateRoom(34,67,0,33, 2); // index {0, 1}
   generateRoom(68,99,0,33, 3); // index {0, 2}

   generateRoom(0,33,34,67, 4); // index {0, 1}
   generateRoom(34,67,34,67, 5); // index {1, 1}
   generateRoom(68,99,34,67, 6); // index {1, 2}

   generateRoom(0,33,68,99, 7); // index {0, 2}
   generateRoom(34,67,68,99, 8); // index {1, 2}
   generateRoom(69,99,69,99, 9); // index {2, 2}

   int starting = 1 + (currentLevel - 2) * 9;
   for (int i = 0; i < 9; i++) {
      int x = roomLocation[0][i + 1] + 8;
      int z = roomLocation[2][i + 1] + 9;
      int item = randomNumber(0,3);
      
      currentObj[i].id = starting + i;
      currentObj[i].item = item;
      currentObj[i].x = (float)x;
      currentObj[i].y = 25.3;
      currentObj[i].z = (float)z;
      setMeshID(currentObj[i].id, currentObj[i].item, currentObj[i].x, currentObj[i].y, currentObj[i].z);
      setScaleMesh(currentObj[i].id, 0.4);
   }
   
   buildCorridor();

   spawnLocation[1] = 25;
   setViewPosition((spawnLocation[0] * -1),-25- 0.2,(spawnLocation[2] * -1));
}
