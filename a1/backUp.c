
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "graphics.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];
int horizontalCorridor[3][8];
int verticalCorridor[3][8];
int spawnLocation[9][2];
int spawnXInedx = 0;
int spawnZIndex = 0;

	/* mouse function called by GLUT when a button is pressed or released */
void mouse(int, int, int, int);

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void setOldViewPosition(float, float, float);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern void addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* tube controls */
extern void createTube(int, float, float, float, float, float, float, int);
extern void hideTube(int);
extern void showTube(int);

	/* 2D drawing functions */
extern void  draw2Dline(int, int, int, int, int);
extern void  draw2Dbox(int, int, int, int);
extern void  draw2Dtriangle(int, int, int, int, int, int);
extern void  set2Dcolour(float []);


	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate the space bar has been pressed */
extern int space;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer; 
	/* size of the window in pixels */
extern int screenWidth, screenHeight;
	/* flag indicates if map is to be printed */
extern int displayMap;
	/* flag indicates use of a fixed viewpoint */
extern int fixedVP;

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);

	/* allows users to define colours */
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);
void unsetUserColour(int);
extern void getUserColour(int, GLfloat *, GLfloat *, GLfloat *, GLfloat *,
    GLfloat *, GLfloat *, GLfloat *, GLfloat *); 

/********* end of extern variable declarations **************/

/********* My functions *********/
int randomNumber(int, int);
void generateRoom(int, int, int, int, int);
int collionDetaction(float, float, float);
int climbAble(float, float, float);
void gravity();
void buildCorridor();
void buildHorizontalCorridorWall(int, int, int, int);
void buildVerticalCorridorWall(int, int, int, int);
void buildWorld();
/********************************/


	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {
   /* your code for collisions goes here */
   float x, y, z;
   float oldX,oldY,oldZ;
   int currentX, currentY, currentZ;
  
   getViewPosition(&x, &y, &z);
   getOldViewPosition(&oldX, &oldY, &oldZ);
   

   //checking the x heading and setting the buffer
   if((x-oldX) > 0){ 
      x = x + 0.12;
   } else if((x-oldX) < 0){
      x = x - 0.12;
   }

   //checking the y heading and setting the buffer
   if((z-oldZ) > 0){ 
      z = z + 0.12; 
   } else if((z-oldZ) < 0){
      z = z - 0.12;
   }   

   currentX = x * -1;
   currentY = y * -1;
   currentZ = z * -1;
   
   
      
   if(world[currentX][currentY][currentZ] != 0){
      if(world[currentX][currentY+1][currentZ] == 0){
         setViewPosition(x,y-1,z);
      } else{
         setViewPosition(oldX, oldY, oldZ);
      }
   }
   
}


	/******* draw2D() *******/
	/* draws 2D shapes on screen */
	/* use the following functions: 			*/
	/*	draw2Dline(int, int, int, int, int);		*/
	/*	draw2Dbox(int, int, int, int);			*/
	/*	draw2Dtriangle(int, int, int, int, int, int);	*/
	/*	set2Dcolour(float []); 				*/
	/* colour must be set before other functions are called	*/
void draw2D() {

   if (testWorld) {
		/* draw some sample 2d shapes */
      if (displayMap == 1) {
         GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
         set2Dcolour(green);
         draw2Dline(0, 0, 500, 500, 15);
         draw2Dtriangle(0, 0, 200, 200, 0, 200);

         GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
         set2Dcolour(black);
         draw2Dbox(500, 380, 524, 388);
      }
   } else {

	/* your code goes here */

   }

}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
int i, j, k;
float *la;
float x, y, z;

	/* sample animation for the testworld, don't remove this code */
	/* demo of animating mobs */
   if (testWorld) {

	/* update old position so it contains the correct value */
	/* -otherwise view position is only correct after a key is */
	/*  pressed and keyboard() executes. */
#if 0
// Fire a ray in the direction of forward motion
float xx, yy, zz;
getViewPosition(&x, &y, &z);
getOldViewPosition(&xx, &yy, &zz);
printf("%f %f %f %f %f %f\n", xx, yy, zz, x, y, z);
printf("%f %f %f\n",  -xx+((x-xx)*25.0), -yy+((y-yy)*25.0), -zz+((z-zz)*25.0));
createTube(2, -xx, -yy, -zz, -xx-((x-xx)*25.0), -yy-((y-yy)*25.0), -zz-((z-zz)*25.0), 5);
#endif

      getViewPosition(&x, &y, &z);
      setOldViewPosition(x,y,z);

	/* sample of rotation and positioning of mob */
	/* coordinates for mob 0 */
      static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
      static float mob0ry = 0.0;
      static int increasingmob0 = 1;
	/* coordinates for mob 1 */
      static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
      static float mob1ry = 0.0;
      static int increasingmob1 = 1;
	/* counter for user defined colour changes */
      static int colourCount = 0;
      static GLfloat offset = 0.0;

	/* move mob 0 and rotate */
	/* set mob 0 position */
      setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

	/* move mob 0 in the x axis */
      if (increasingmob0 == 1)
         mob0x += 0.2;
      else 
         mob0x -= 0.2;
      if (mob0x > 50) increasingmob0 = 0;
      if (mob0x < 30) increasingmob0 = 1;

	/* rotate mob 0 around the y axis */
      mob0ry += 1.0;
      if (mob0ry > 360.0) mob0ry -= 360.0;

	/* move mob 1 and rotate */
      setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

	/* move mob 1 in the z axis */
	/* when mob is moving away it is visible, when moving back it */
	/* is hidden */
      if (increasingmob1 == 1) {
         mob1z += 0.2;
         showMob(1);
      } else {
         mob1z -= 0.2;
         hideMob(1);
      }
      if (mob1z > 72) increasingmob1 = 0;
      if (mob1z < 52) increasingmob1 = 1;

	/* rotate mob 1 around the y axis */
      mob1ry += 1.0;
      if (mob1ry > 360.0) mob1ry -= 360.0;

	/* change user defined colour over time */
      if (colourCount == 1) offset += 0.05;
      else offset -= 0.01;
      if (offset >= 0.5) colourCount = 0;
      if (offset <= 0.0) colourCount = 1;
      setUserColour(9, 0.7, 0.3 + offset, 0.7, 1.0, 0.3, 0.15 + offset, 0.3, 1.0);

	/* sample tube creation  */
	/* draws a purple tube above the other sample objects */
       createTube(1, 45.0, 30.0, 45.0, 50.0, 30.0, 50.0, 6);

    /* end testworld animation */


   } else {
      /* your code goes here */

      getViewPosition(&x, &y, &z);
      setOldViewPosition(x,y,z);
      gravity();
   }
}


	/* called by GLUT when a mouse button is pressed or released */
	/* -button indicates which button was pressed or released */
	/* -state indicates a button down or button up event */
	/* -x,y are the screen coordinates when the mouse is pressed or */
	/*  released */ 
void mouse(int button, int state, int x, int y) {

   if (button == GLUT_LEFT_BUTTON)
      printf("left button - ");
   else if (button == GLUT_MIDDLE_BUTTON)
      printf("middle button - ");
   else
      printf("right button - ");

   if (state == GLUT_UP)
      printf("up - ");
   else
      printf("down - ");

   printf("%d %d\n", x, y);
}



int main(int argc, char** argv) {
   int i, j, k;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);


	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;

	/* some sample objects */
	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;

	/* create user defined colour and draw cube */
      setUserColour(9, 0.7, 0.3, 0.7, 1.0, 0.3, 0.15, 0.3, 1.0);
      world[54][25][50] = 9;


	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);
   } else {          
      /* my world starts here */
      /* make some cousmise colors */
      float rValue = 0.0;
      float gValue = 0.0;
      float bValue = 0.0;

      rValue = (float)128 / 255;
      gValue = (float) 128/ 255;
      bValue = (float) 128 / 255;
      printf("%f\n", rValue);
      setUserColour(9, 0.5, 0.5, 0.5, 1.0, 0.2, 0.2, 0.2, 1.0); // grey cube
      buildWorld();
   }

   int randomNum = randomNumber(0, 8);
   spawnXInedx = spawnLocation[randomNum][0] * -1;
   spawnZIndex = spawnLocation[randomNum][1] * -1;

	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   setViewPosition(spawnXInedx,-25,spawnZIndex);
   
   glutMainLoop();
   return 0; 
}

/***********Healper functions***********/

   /* return a certain random number within the range*/
int randomNumber(int minNumber, int maxNumber) {
   return rand() % (maxNumber + 1 - minNumber) + minNumber;
}

   /* generate a random room with random size and shape*/
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
         world[i][24 + a][startZ] = 6;
         world[i][24 + a][endZ] = 6;
      }
   }

   for (int j = startZ; j <= endZ; j++) {
      for (int a = 1; a <= 4; a++) {
         world[startX][24 + a][j] = 6;
         world[endX][24 + a][j] = 6;
      }
   }

   // building the celling
   for (int i = startX; i < endX; i++) {
      for (int j = startZ; j <= endZ; j++) {
         world[i][28][j] = 6;
      }
   }



   switch (location) {
      case 1:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[0][0] = spawnX;
         spawnLocation[0][1] = spawnZ;

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
         break;

      case 2:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[1][0] = spawnX;
         spawnLocation[1][1] = spawnZ;
         
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

         break;
      case 3:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;
         
         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[2][0] = spawnX;
         spawnLocation[2][1] = spawnZ;
         
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

         break;
      case 4:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[3][0] = spawnX;
         spawnLocation[3][1] = spawnZ;

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
         break;
      case 5:
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);
         
         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[4][0] = spawnX;
         spawnLocation[4][1] = spawnZ;

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
         break;
      case 6:
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);
         centerDoorBottom = randomNumber(startX + 3, endX - 3);
      
         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[5][0] = spawnX;
         spawnLocation[5][1] = spawnZ;

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
         break;
      case 7:
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[6][0] = spawnX;
         spawnLocation[6][1] = spawnZ;

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
         break;
      case 8:
         centerDoorRight = randomNumber(startZ + 3, endZ - 3);
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[7][0] = spawnX;
         spawnLocation[7][1] = spawnZ;


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
         break;
      case 9:
         centerDoorTop = randomNumber(startX + 3, endX - 3);
         centerDoorLeft = randomNumber(startZ + 3, endZ - 3);

         cubeLocX = randomNumber((endX - startX) / 2 - 5, (endX - startX) / 2 + 5);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 5, (endZ - startZ) / 2 + 5);
         // a randomCube in side the room
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 9;

         spawnX = randomNumber(startX + 3, endX - 3);
         spawnZ = randomNumber(startZ + 3, endZ - 3);
         spawnLocation[8][0] = spawnX;
         spawnLocation[8][1] = spawnZ;
         
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
         break;
      default:
         printf("Error at room generate function: unknown room location %d\n", location);
         exit(1);
   }

}

void gravity() {
   float currentX, currentY, currentZ;
   int x, y, z;
   flycontrol = 0;

   getOldViewPosition(&currentX, &currentY, &currentZ);
   x = -currentX; 
   y = -currentY; 
   z = -currentZ; 

   while (world[(int)(currentX * -1)][(int)(currentY * -1 - 0.5)][(int)(currentZ * -1)] == 0) {
      currentY = currentY + 0.0001;
      setViewPosition(currentX, currentY, currentZ);
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
         world[leftX + i][24 + a][leftY + 1] = 6;
         world[leftX + i][24 + a][leftY - 1] = 6;
         world[rightX - i][24 + a][rightY + 1] = 6;
         world[rightX - i][24 + a][rightY - 1] = 6;
      }
      
      world[leftX + i][28][leftY] = 6;
      world[rightX - i][28][rightY] = 6;

      currentLeft = leftX + i;
      currentRight = rightX - i;
   }
   remainder = horizontalDistance % 2;

   // if the haorizontal distance if not events
   if (remainder != 0) {
      leftX = currentLeft;

      for (int i = 1; i < remainder; i++) {
         for (int a = 1; a <= 4; a++) {
            world[leftX + i][24 + a][leftY + 1] = 6;
            world[leftX + i][24 + a][leftY - 1] = 6;
         }

         world[leftX + i][28][leftY] = 7;
         currentLeft = leftX + i;
      }
   }

   // building the wall at vertical vertax
   if (verticalDistance != 0) {
      if (leftY < rightY) {
         // extend 2 unit for the most upper wall
         for (int a = 1; a <= 4; a++) {
            world[currentRight - 1][24 + a][rightY + 1] = 6;
            world[currentRight - 2][24 + a][rightY + 1] = 6;
            world[currentLeft + 1][24 + a][leftY - 1] = 6;
            world[currentLeft + 2][24 + a][leftY - 1] = 6;
         }

         world[currentRight - 1][28][rightY] = 6;
         world[currentRight - 2][28][rightY] = 6;
         world[currentLeft + 1][28][leftY] = 6;
         world[currentLeft + 2][28][leftY] = 6;


         for (int j = 1; j <= verticalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[currentRight - 2][24 + a][rightY + 1 - j] = 6;
               world[currentRight][24 + a][rightY - 1 - j] = 6;
            }
            world[currentRight - 1][28][rightY - j] = 6;
         }
      } else {
         for (int a = 1; a <= 4; a++) {
            // extend 2 unit for the most bottom wall
            world[currentRight - 1][24 + a][rightY - 1] = 6;
            world[currentRight - 2][24 + a][rightY - 1] = 6;

            // extend 2 unit for the most bottom wall
            world[currentLeft + 1][24 + a][leftY + 1] = 6;
            world[currentLeft + 2][24 + a][leftY + 1] = 6;
         }
         
         world[currentRight - 1][28][rightY] = 6;
         world[currentRight - 2][28][rightY] = 6;
         world[currentLeft + 1][28][leftY] = 6;
         world[currentLeft + 2][28][leftY] = 6;

         for (int j = 1; j <= verticalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[currentLeft + 2][24 + a][leftY + 1 - j] = 6;
               world[currentLeft][24 + a][leftY - 1 - j] = 6;
            }
            world[currentLeft + 1][28][leftY - j] = 6;
         }
      }
   } else {
      for (int a = 1; a <= 4; a++) {
         world[currentLeft + 1][24 + a][leftY + 1] = 6;
         world[currentLeft + 1][24 + a][leftY - 1] = 6;
         world[currentRight - 1][24 + a][rightY + 1] = 6;
         world[currentRight - 1][24 + a][rightY - 1] = 6;
      }

      world[currentLeft + 1][28][leftY] = 6;
      world[currentLeft + 1][28][leftY] = 6;
      world[currentRight - 1][28][rightY] = 6;
      world[currentRight - 1][28][rightY] = 6;
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
         world[topX + 1][24 + a][topY + i] = 6;
         world[topX - 1][24 + a][topY + i] = 6;
         world[buttomX + 1][24 + a][buttomY - i] = 6;
         world[buttomX - 1][24 + a][buttomY - i] = 6;
      }

      world[topX][28][topY + i] = 6;
      world[buttomX ][28][buttomY - i] = 6;

      currentTop = topY + i;
      currentButtom = buttomY - i;
   }
   remainder = verticalDistance % 2;

   if (remainder != 0) {
      topY = currentTop;

      for (int i = 1; i < remainder; i++) {
         for (int a = 1; a <= 4; a++) {
            world[topX + 1][24 + a][topY + i] = 6;
            world[topX - 1][24 + a][topY + i] = 6;
         } 
         currentTop = topY + i;
         world[topX][28][topY + i] = 6;
      }
   }

   if (horizaontalDistance != 0) {
      if (topX < buttomX) {
         for (int a = 1; a <= 4; a++) {
            world[topX - 1][24 + a][currentTop + 1] = 6;
            world[topX - 1][24 + a][currentTop + 2] = 6;
            world[buttomX + 1][24 + a][currentButtom - 1] = 6;
            world[buttomX + 1][24 + a][currentButtom - 2] = 6;
         }

         world[topX][28][currentTop + 1] = 6;
         world[topX][28][currentTop + 2] = 6;
         world[buttomX][28][currentButtom - 1] = 6;
         world[buttomX][28][currentButtom - 2] = 6;

         for (int j = 1; j <= horizaontalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[topX - 1 + j][24 + a][currentTop + 2] = 6;
               world[topX + 1 + j][24 + a][currentTop] = 6;
            }
            world[topX + j][28][currentTop + 1] = 6;
         }
      } else {
         for (int a = 1; a <= 4; a++) {
            world[topX + 1][24 + a][currentTop + 1] = 6;
            world[topX + 1][24 + a][currentTop + 2] = 6;
            world[buttomX - 1][24 + a][currentButtom - 1] = 6;
            world[buttomX - 1][24 + a][currentButtom - 2] = 6;
         }

         world[topX][28][currentTop + 1] = 6;
         world[topX][28][currentTop + 2] = 6;
         world[buttomX][28][currentButtom - 1] = 6;
         world[buttomX][28][currentButtom - 2] = 6;

         for (int j = 1; j <= horizaontalDistance; j++) {
            for (int a = 1; a <= 4; a++) {
               world[buttomX - 1 + j][24 + a][currentButtom - 2] = 6;
               world[buttomX + 1 + j][24 + a][currentButtom] = 6;
            }
            world[buttomX + j][28][currentButtom - 1] = 6;
         }
      }
   } else {
      for (int a = 0; a <= 4; a++) {
         world[topX + 1][24 + a][currentTop + 1] = 6;
         world[topX - 1][24 + a][currentTop + 1] = 6;
         world[buttomX + 1][24 + a][currentButtom - 1] = 6;
         world[buttomX - 1][24 + a][currentButtom - 1] = 6;
      }

      world[topX][28][currentTop + 1] = 6;
      world[buttomX][28][currentButtom - 1] = 6;
   }

}

void buildWorld() {
   int i, j, k;

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
         if (i % 2 == 0 && j % 2 == 0) {
            world[i][24][j] = 4;
         } else {
            world[i][24][j] = 5;
         }
         
      }
   }
   srand(time(NULL));

   generateRoom(0,33,0,33, 1); // index {0, 0}
   generateRoom(34,67,0,33, 2); // index {0, 1}
   generateRoom(68,99,0,33, 3); // index {0, 2}

   generateRoom(0,33,34,67, 4); // index {0, 1}
   generateRoom(34,67,34,67, 5); // index {1, 1}
   generateRoom(68,99,34,67, 6); // index {1, 2}

   generateRoom(0,33,68,99, 7); // index {0, 2}
   generateRoom(34,67,68,99, 8); // index {1, 2}
   generateRoom(68,99,69,99, 9); // index {2, 2}
   
   buildCorridor();
}