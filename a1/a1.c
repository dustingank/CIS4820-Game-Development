
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
#include <sys/time.h>

#include "graphics.h"
#include "paramater.h"
#include "terrain.h"
#include "gameWorldGenerate.h"
#include "clouds.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];
int currentLevel = 1; // to track which level player current is
GLint m_viewport[4];
int screenHeight;
int screenWidth;

struct Node {
   int level;
   int spawnXInedx;
   int spawnZIndex;
   int spawnYIndex;
   GLubyte  worldSaved[WORLDX][WORLDY][WORLDZ];
   struct mesh allMeshObjs[9];
   struct Node *next;
};
struct Node *head = NULL;

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

   /* texture functions */
extern int setAssignedTexture(int, int);
extern void unsetAssignedTexture(int);
extern int getAssignedTexture(int);
extern void setTextureOffset(int, float, float);


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

   /* mesh creation, translatio, rotation functions */
extern void setMeshID(int, int, float, float, float);
extern void unsetMeshID(int);
extern void setTranslateMesh(int, float, float, float);
extern void setRotateMesh(int, float, float, float);
extern void setScaleMesh(int, float);
extern void drawMesh(int);
extern void hideMesh(int); 

/********* end of extern variable declarations **************/

/********* My functions *********/
int collionDetaction(float, float, float);
void gravity();

int getScreenWeight();
int getScreenHeight();
// linkedList functions
void appendLinkedList(struct Node **, int, GLubyte[WORLDX][WORLDY][WORLDZ]);
struct Node* getNode(struct Node*, int);

// game saving / loding functions
void saveLevel(struct Node**, int);
void loadLevel(struct Node**, int);
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
      
      if (displayMap == 1) {
         screenHeight = getScreenHeight();
         screenWidth = getScreenWeight();

         GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
         GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

         int mapSize = ((screenWidth / 2) / WORLDX) * WORLDX;
         int blockSize = mapSize / WORLDX;
         //printf("mapSize: %d blockSize: %d height: %d width: %d\n", mapSize, blockSize, screenHeight, screenWidth);
         int offsetWidth = (screenWidth - mapSize) / 2;
         int offsetHeigh = (screenHeight - mapSize) / 2 + (mapSize - (2 * blockSize));

         float x = 0.0;
         float y = 0.0;
         float z = 0.0;
         getViewPosition(&x, &y, &z);
         x = x * -1;
         z = z * -1;

         set2Dcolour(black);
         draw2Dbox(offsetWidth + (blockSize * (int)x), offsetHeigh - (blockSize * (int)z),
                     offsetWidth + (blockSize * (int)x) + blockSize, offsetHeigh - (blockSize * (int)z) + blockSize);

         for (int i = 0; i < WORLDX; i++) {
            for (int j = 0; j < WORLDZ; j++) {
               if (world[i][25][j] != 0 && world[i][25][j] != 6) {
                  set2Dcolour(black);
                  draw2Dbox(offsetWidth + (blockSize * i), offsetHeigh - (blockSize * j),
                     offsetWidth + (blockSize * i) + blockSize, offsetHeigh - (blockSize * j) + blockSize);
               }
               if (world[i][28][j] != 0)  {
                  set2Dcolour(white);
                  draw2Dbox(offsetWidth + (blockSize * i), offsetHeigh - (blockSize * j),
                     offsetWidth + (blockSize * i) + blockSize, offsetHeigh - (blockSize * j) + blockSize);
               }
            }
         }
      }

      //if (displayMap == 2) {}
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

   /* offset counter for animated texture */
      static float textureOffset = 0.0;

	/* scaling values for fish mesh */
      static float fishScale = 1.0;
      static int scaleCount = 0;
      static GLfloat scaleOffset = 0.0;

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

    /* move texture for lava effect */
      textureOffset -= 0.01;
      setTextureOffset(18, 0.0, textureOffset);

	/* make fish grow and shrink (scaling) */
      if (scaleCount == 1) scaleOffset += 0.01;
      else scaleOffset -= 0.01;
      if (scaleOffset >= 0.5) scaleCount = 0;
      if (scaleOffset <= 0.0) scaleCount = 1;
      setScaleMesh(1, 0.5 + scaleOffset);

	/* make cow with id == 2 appear and disappear */
	/* use scaleCount as switch to flip draw/hide */
	/* rotate cow while it is visible */
      if (scaleCount == 0) {
         drawMesh(2);
         setRotateMesh(2, 0.0, 180.0 + scaleOffset * 100.0, 0.0);
      } else {
         hideMesh(2);
      }

    /* end testworld animation */

   } else {
      /* your code goes here */
      getViewPosition(&x, &y, &z);
      setOldViewPosition(x,y,z);
      gravity();

      struct timeval systemTime;
      static double cloudTime = 0.0;
      static double meshTime = 0.0;

      double currentTime;
      gettimeofday(&systemTime, NULL);
      currentTime = (systemTime.tv_sec) * 1000 + (systemTime.tv_usec) / 1000;

      if (currentLevel == 1) {
         if (currentTime - cloudTime >= 400.00) {
            cloudTime = currentTime;
            for (int i = 0; i < WORLDX; i++) {
               for (int k = 0; k < WORLDZ; k++) {
                  world[i][48][k] = 0; 
               }
            }
            moveCloud();
         }
      } else {
         if (currentTime - meshTime >= 400.00) {
            meshTime = currentTime;
            for (int i = 0; i < 9; i++) {
               currentObj[i].x += 0.8;
               if (currentObj[i].x >= roomLocation[1][i + 1] - 3) {
                  currentObj[i].x = roomLocation[0][i + 1] + 8;
               }
               setTranslateMesh(currentObj[i].id, currentObj[i].x, currentObj[i].y, currentObj[i].z);
            }
         }
      }


      getOldViewPosition(&x, &y, &z);
      if (world[(int)x * -1][((int)y * -1) - 1][(int)z * -1] == 5) { // stair to go up

         for (int i = 0; i < 9; i++) { // leveing current level / hide all the mesh obj
            printf("Hiding obj id: %d\n\n", currentObj[i].id);
            hideMesh(currentObj[i].id);
         }

         saveLevel(&head, currentLevel); // save the current level
         currentLevel -= 1;
         loadLevel(&head, currentLevel); // get the next level

         for (int i = 0; i < 9; i++) { // leveing current level / hide all the mesh obj
            printf("showing obj id: %d\n", currentObj[i].id);
            drawMesh(currentObj[i].id);
         }
      }

      if (world[(int)x * -1][((int)y * -1) - 1][(int)z * -1] == 9) { // stair to go down
         for (int i = 0; i < 9; i++) { // leveing current level / hide all the mesh obj
            printf("Hiding obj id: %d\n\n", currentObj[i].id);
            hideMesh(currentObj[i].id);
         }
         saveLevel(&head, currentLevel); // save the current level
         currentLevel += 1;
         loadLevel(&head, currentLevel); // get the next level
         for (int i = 0; i < 9; i++) { // leveing current level / hide all the mesh obj
            printf("showing obj id: %d\n\n", currentObj[i].id);
            drawMesh(currentObj[i].id);
         }
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

	/* texture examples */

	/* create textured cube */
	/* create user defined colour with an id number of 11 */
      setUserColour(11, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
	/* attach texture 22 to colour id 11 */
      setAssignedTexture(11, 22);
	/* place a cube in the world using colour id 11 which is texture 22 */
      world[59][25][50] = 11;

	/* create textured cube */
      setUserColour(12, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(12, 27);
      world[61][25][50] = 12;

	/* create textured cube */
      setUserColour(10, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(10, 26);
      world[63][25][50] = 10;

	/* create textured floor */
      setUserColour(13, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(13, 8);
      for (i=57; i<67; i++)
         for (j=45; j<55; j++)
            world[i][24][j] = 13;

	/* create textured wall */
      setUserColour(14, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(14, 18);
      for (i=57; i<67; i++)
         for (j=0; j<4; j++)
            world[i][24+j][45] = 14;

	/* create textured wall */
      setUserColour(15, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(15, 42);
      for (i=45; i<55; i++)
         for (j=0; j<4; j++)
            world[57][24+j][i] = 15;

		// two cubes using the same texture but one is offset
		// cube with offset texture 33
      setUserColour(16, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(16, 33);
      world[65][25][50] = 16;
      setTextureOffset(16, 0.5, 0.5);
		// cube with non-offset texture 33
      setUserColour(17, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(17, 33);
      world[66][25][50] = 17;

		// create some lava textures that will be animated
      setUserColour(18, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(18, 24);
      world[62][24][55] = 18;
      world[63][24][55] = 18;
      world[64][24][55] = 18;
      world[62][24][56] = 18;
      world[63][24][56] = 18;
      world[64][24][56] = 18;

		// draw cow mesh and rotate 45 degrees around the y axis
		// game id = 0, cow mesh id == 0
      setMeshID(0, 0, 48.0, 26.0, 50.0);
      setRotateMesh(0, 0.0, 45.0, 0.0);

		// draw fish mesh and scale to half size (0.5)
		// game id = 1, fish mesh id == 1
      setMeshID(1, 1, 51.0, 28.0, 50.0);
      setScaleMesh(1, 0.5);

		// draw cow mesh and rotate 45 degrees around the y axis
		// game id = 2, cow mesh id == 0
      setMeshID(2, 0, 59.0, 26.0, 47.0);

		// draw bat
		// game id = 3, bat mesh id == 2
      setMeshID(3, 2, 61.0, 26.0, 47.0);
      setScaleMesh(3, 0.5);
		// draw cactus
		// game id = 4, cactus mesh id == 3
      setMeshID(4, 3, 63.0, 26.0, 47.0);
      setScaleMesh(4, 0.5);
   } else {          
      /* my world starts here */
      /* make some cousmise colors */
      //printf("%f\n", rValue);
      displayMap = 0;
      setCoustmizeColor();
      setCustmizeTexture();
      buildOutDoorWorld(0);
      makeClouds();
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

   /* save the current world into the linkedList */
void saveLevel(struct Node** head, int level) {
   struct Node *targetNode = NULL;

   // check if current level already been saved
   targetNode = getNode(*head, level);

   if (targetNode == NULL) {
      printf("Saveing Level#%d into linkedList\n", level);
      appendLinkedList(head, level, world);
   }
   printf("\n");
}
   /* load the world(either from linkedlist or generate a new world)*/
void loadLevel(struct Node** head, int level) {
   struct Node *targetNode = NULL;
   int i, j, k;
   printf("now searching level %d....\n", level);
   // check if current level already been saved
   targetNode = getNode(*head, level);
   // if there is level / load the world
   if (targetNode != NULL) {
      printf("Loading world...\n\n");
      for (i = 0; i < WORLDX; i++) {
         for (j = 0; j < WORLDY; j++) {
            for (k = 0; k < WORLDZ; k++) {
               world[i][j][k] = targetNode->worldSaved[i][j][k];
            }
         }
      }

      for (int i = 0; i < 9; i++) {
         currentObj[i] = targetNode->allMeshObjs[i];
      }
      setViewPosition((int)targetNode->spawnXInedx * -1, (int)targetNode->spawnYIndex * -1-0.2, (int)targetNode->spawnZIndex * -1);
   } else {
      printf("Build world...\n\n");
      buildWorld(level);
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

int getScreenHeight() {
   glGetIntegerv(GL_VIEWPORT, m_viewport);
   return m_viewport[3];
}

int getScreenWeight() {
   glGetIntegerv(GL_VIEWPORT, m_viewport);
   return m_viewport[2];
}

/***********Healper functions(LinkedList)***********/
void appendLinkedList(struct Node **headReference, int level, GLubyte toBeSaved[WORLDX][WORLDY][WORLDZ]) {
   struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
   struct Node* temp = *headReference;
   newNode->level = level;
   newNode->spawnXInedx = spawnLocation[0];
   newNode->spawnYIndex = spawnLocation[1];
   newNode->spawnZIndex = spawnLocation[2];

   for (int i = 0; i < 9; i++) {
      newNode->allMeshObjs[i].id = currentObj[i].id; 
      newNode->allMeshObjs[i].item = currentObj[i].item;
      newNode->allMeshObjs[i].x = currentObj[i].x;
      newNode->allMeshObjs[i].y = currentObj[i].y;
      newNode->allMeshObjs[i].z = currentObj[i].z;
   }

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
