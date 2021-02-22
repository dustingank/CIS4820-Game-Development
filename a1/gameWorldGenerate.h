int horizontalCorridor[3][8];
int verticalCorridor[3][8];
int spawnLocation[3];

void buildCorridor();
void buildHorizontalCorridorWall(int, int, int, int);
void buildVerticalCorridorWall(int, int, int, int);
void buildWorld(); // create the world and spawn the player in one of the room
void generateRoom(int, int, int, int, int);