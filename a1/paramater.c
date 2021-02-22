#include <math.h>
#include <stdlib.h>
#include "graphics.h"
#include "paramater.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);

void setCoustmizeColor() {
    setUserColour(30, colorCodeConvert(244), colorCodeConvert(244), colorCodeConvert(244), 1.00, 
                        colorCodeConvert(244 / 2), colorCodeConvert(244 / 2), colorCodeConvert(244 / 2), 0.50); // another white

    setUserColour(9, colorCodeConvert(128), colorCodeConvert(128), colorCodeConvert(128), 1.0, 0.2, 0.2, 0.2, 1.0); // grey cube

    setUserColour(10, colorCodeConvert(77), colorCodeConvert(38), colorCodeConvert(15),1.0, 
                        colorCodeConvert(77 / 2), colorCodeConvert(38 / 2), colorCodeConvert(15 / 2), 1.0); // brown 1
    setUserColour(11, colorCodeConvert(61), colorCodeConvert(18), colorCodeConvert(0), 1.0 ,
                        colorCodeConvert(61 / 2), colorCodeConvert(18 / 2), colorCodeConvert(0 / 2), 1.0); // brown 2
    setUserColour(12, colorCodeConvert(66), colorCodeConvert(30), colorCodeConvert(0), 1.0,
                        colorCodeConvert(66 / 2), colorCodeConvert(30 / 2), colorCodeConvert(0 / 2), 1.0); // brown 3

    setUserColour(14, colorCodeConvert(0), colorCodeConvert(97), colorCodeConvert(0), 1.00, 
                        colorCodeConvert(0), colorCodeConvert(97 / 2), colorCodeConvert(0), 1.00); // dark green

    setUserColour(15, colorCodeConvert(0), colorCodeConvert(102), colorCodeConvert(0), 1.00, 
                        colorCodeConvert(0), colorCodeConvert(102 / 2), colorCodeConvert(0), 1.00); // dark green

    setUserColour(16, colorCodeConvert(0), colorCodeConvert(107), colorCodeConvert(0), 1.00, 
                        colorCodeConvert(0), colorCodeConvert(107 / 2), colorCodeConvert(0), 1.00); // dark green

    setUserColour(17, colorCodeConvert(0), colorCodeConvert(112), colorCodeConvert(0), 1.00, 
                        colorCodeConvert(0), colorCodeConvert(112 / 2), colorCodeConvert(0), 1.00); // dark green
}

int randomNumber(int minNumber, int maxNumber) {
   return rand() % (maxNumber + 1 - minNumber) + minNumber;
}

int worldRange(int x, int z) {
   if (x >= WORLDX || x < 0 || z >= WORLDZ || z < 0) {
      return 0;
   }
   return 1;
}

int checkAround(int x, int y, int z) {
   if (world[x][y + 1][z] == 0) {
      return 1;
   }
   return 0;
}

float colorCodeConvert(int value) {
   return (float)value / 255;
}