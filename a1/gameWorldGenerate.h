int horizontalCorridor[3][8];
int verticalCorridor[3][8];
int spawnLocation[3];
struct mesh {
   int id;
   int item;
   int visability;
   float x;
   float y;
   float z;
   int isAlive; // for monster is alive or die; for item is still on ground or not
};

struct mesh currentMonsterObj[9];
struct mesh currentItemObj[5];

// roomLocation[0][0]: startX; roomLocation[1][0]: endX; 
// roomLocation[2][0]: startZ; roomLocation[3][0]: endZ
int roomLocation[5][9];

void buildCorridor();
void buildHorizontalCorridorWall(int, int, int, int, int);
void buildVerticalCorridorWall(int, int, int, int, int);
void buildWorld(int); // create the world and spawn the player in one of the room
void generateRoom(int, int, int, int, int);
void buildOutDoorWorld(int);
void buildCaveLevel();