
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
int currentLevel = 1; // to track which level player current is

struct Node {
   int level;
   GLubyte  worldSaved[WORLDX][WORLDY][WORLDZ];
   struct Node *next;
};
struct Node *head = NULL;

typedef struct {
    float x, y;
} vector2;

// perlin noise permutation set
static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

// function declarations start here

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
void gravity();
void buildCorridor();
void buildHorizontalCorridorWall(int, int, int, int);
void buildVerticalCorridorWall(int, int, int, int);
void buildWorld(); // create the world and spawn the player in one of the room
float colorCodeConvert(int);
// linkedList functions
void appendLinkedList(struct Node **, int, GLubyte[WORLDX][WORLDY][WORLDZ]);
struct Node* getNode(struct Node*, int);
// game saving / loding functions
void saveLevel(struct Node**, int);
void loadLevel(struct Node**, int);
// perlin noise functoins 
//(Reference: https://en.wikipedia.org/wiki/Perlin_noise)
float perlin(float, float, float);
float dotGridGradient(int ix, int iy, float x, float y);
float interpolate(float, float, float);
vector2 randomGradient(int, int);
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
      //gravity();

      getOldViewPosition(&x, &y, &z);
      if (world[(int)x * -1][((int)y * -1) - 1][(int)z * -1] == 5) { // stair to go up
         saveLevel(&head, currentLevel); // save the current level
         currentLevel -= 1;
         loadLevel(&head, currentLevel); // get the next level
      }

      if (world[(int)x * -1][((int)y * -1) - 1][(int)z * -1] == 9) { // stair to go down
         saveLevel(&head, currentLevel); // save the current level
         currentLevel += 1;
         loadLevel(&head, currentLevel); // get the next level
      }
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
      //printf("%f\n", rValue);
      setUserColour(9, colorCodeConvert(128), colorCodeConvert(128), colorCodeConvert(128), 1.0, 0.2, 0.2, 0.2, 1.0); // grey cube

      setUserColour(10, colorCodeConvert(92), colorCodeConvert(34), colorCodeConvert(0), 0.36, 0.2, 0.2, 0.2, 1.0); // brown 1
      setUserColour(11, colorCodeConvert(66), colorCodeConvert(38), colorCodeConvert(21), 0.26,0.2, 0.2, 0.2, 1.0); // brown 2
      setUserColour(12, colorCodeConvert(122), colorCodeConvert(45), colorCodeConvert(0), 0.48,0.2, 0.2, 0.2, 1.0); // brown 3
      
      for (i = 0; i < WORLDX; i++) {
         for (j = 0; j < WORLDY; j++) {
            for (k = 0; k < WORLDZ; k++) {
               world[i][j][k] = 0;
            }
         }
      }

      for (i = 0; i < WORLDX; i++) {
         for (k = 0; k < WORLDZ; k++) {
            float result = (float)perlin(i, k, 0.09);
            result = result * 15;
            //printf("%f\n", result);
            if (result < 0.0) {
               if (result > -0.4) {
                  for (int a = 0; a < 12; a++) {
                     world[i][24 - a][k] = 11;
                  }
               } else if (result < -0.5 && result > -1.0) {
                  for (int a = 0; a < 12; a++) {
                     world[i][24 - 1 - a][k] = 11;
                  }
               } else {
                  for (int a = 0; a < 10; a++) {
                     world[i][24 + (int)result - a][k] = 11;
                  }
               }
            } else if (result > 0.0) {
               if (result < 0.4) {
                  for (int a = 0; a < 12; a++) {
                     world[i][24 - a][k] = 11;
                  }
               } else if (result > 0.5 && result < 1.0){
                  for (int a = 0; a < 12; a++) {
                     world[i][25 - a][k] = 11;
                  }
                  world[i][24 + 1][k] = 1;
               } else {
                  for (int a = 0; a < 18; a++) {
                     world[i][(24 + (int)result) - a][k] = 11;
                  }
               }
            } else {
               world[i][24][k] = 3;
            }
         }
      }

       for (i = 0; i < WORLDX; i++) {
         for (j = 0; j < WORLDY; j++) {
            for (k = 0; k < WORLDZ; k++) {
               if (world[i][j][k] != 0) {
                  if (j <= 18) {
                     world[i][j][k] = 10;
                  } else if (j >= 30) {
                     world[i][j][k] = 5;
                  } else {
                     world[i][j][k] = 1;
                  }
               }
            }
         }
      }


      //buildWorld();
   }

	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   struct Node *temp = NULL;

   while (head != NULL) {
      temp = head;
      head = head->next;
      free(temp);
   }
   printf("LinkedList Free\n");
   return 0; 
}



/***********Healper functions(General)***********/

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
         world[startX + cubeLocX][25][startZ + cubeLocZ] = 5;

         cubeLocX = randomNumber((endX - startX) / 2 - 7, (endX - startX) / 2 + 7);
         cubeLocZ = randomNumber((endZ - startZ) / 2 - 7, (endZ - startZ) / 2 + 7);
         world[startX + cubeLocX][24][startZ + cubeLocZ] = 9;

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

   while (world[(int)(currentX * -1)][(int)(currentY * -1 - 0.1)][(int)(currentZ * -1)] == 0) {
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
   generateRoom(68,99,69,99, 9); // index {2, 2}
   
   buildCorridor();

   
   spawnXInedx = spawnLocation[8][0] * -1;
   spawnZIndex = spawnLocation[8][1] * -1;
   setViewPosition(spawnXInedx,-25 - 0.2,spawnZIndex);
}

float colorCodeConvert(int value) {
   return (float)value / 255;
}

   /* save the current world into the linkedList */
void saveLevel(struct Node** head, int level) {
   struct Node *targetNode = NULL;

   // check if current level already been saved
   targetNode = getNode(*head, level);

   if (targetNode == NULL) {
      //printf("Saveing Level#%d into linkedList\n", level);
      appendLinkedList(head, level, world);
   }
   //printf("\n");
}

   /* load the world(either from linkedlist or generate a new world)*/
void loadLevel(struct Node** head, int level) {
   struct Node *targetNode = NULL;
   int i, j, k;
   //printf("now searching level %d....\n", level);
   // check if current level already been saved
   targetNode = getNode(*head, level);
   // if there is level / load the world
   if (targetNode != NULL) {
      //printf("Loading world...\n\n");
      for (i = 0; i < WORLDX; i++) {
         for (j = 0; j < WORLDY; j++) {
            for (k = 0; k < WORLDZ; k++) {
               world[i][j][k] = targetNode->worldSaved[i][j][k];
            }
         }
      }
   } else {
      //printf("Build world...\n\n");
      buildWorld();
   }
}

/***********Healper functions(LinkedList)***********/
void appendLinkedList(struct Node **headReference, int level, GLubyte toBeSaved[WORLDX][WORLDY][WORLDZ]) {
   struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
   struct Node* temp = *headReference;

   newNode->level = level;
   for (int i = 0; i < WORLDX; i++) {
      for (int j = 0; j < WORLDY; j++) {
         for (int k = 0; k < WORLDZ; k++) {
            newNode->worldSaved[i][j][k] = toBeSaved[i][j][k];
         }
      }
   }
   newNode->next = NULL;

   if (*headReference == NULL) {
      *headReference = newNode;
      return;
   }

   while(temp->next != NULL) {
      temp = temp->next;
   }

   temp->next = newNode;
   return;
}

struct Node* getNode(struct Node* head, int level) {
   struct Node* temp = head;
   while (temp != NULL) {
      //printf("getNode: %d\n", temp->level);
      if (temp->level == level) {
         return temp;
      }
      temp = temp->next;
   }
   return NULL;
}


/***********Healper functions(Perlin Noise)***********/

float perlin(float x, float y, float freq) {
   // determine grid cell coordinates
   x = x * freq;
   y = y * freq;

   int x0 = (int)x;
   int x1 = x0 + 1;
   int y0 = (int)y;
   int y1 = y0 + 1;

   // Determine interpolation weights
   // Could also use higher order polynomial/s-curve here
   float sx = x - (float)x0;
   float sy = y - (float)y0;

   // Interpolate between grid point gradients
   float n0, n1, ix0, ix1, value;

   n0 = dotGridGradient(x0, y0, x, y);
   n1 = dotGridGradient(x1, y0, x, y);
   ix0 = interpolate(n0, n1, sx);

   n0 = dotGridGradient(x0, y1, x, y);
   n1 = dotGridGradient(x1, y1, x, y);
   ix1 = interpolate(n0, n1, sx);

   value = interpolate(ix0, ix1, sy);
   return value;
}

float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    vector2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

vector2 randomGradient(int ix, int iy) {
    // Random float. No precomputed gradients mean this works for any number of grid coordinates
    float random = 2920.f * sin(ix * 21942.f + iy * 171324.f + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f);
    return (vector2) { .x = cos(random), .y = sin(random) };
}

float interpolate(float a0, float a1, float w) {
   return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}