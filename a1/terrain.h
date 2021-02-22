typedef struct {
    float x, y;
} vector2;

float perlin(float, float, float);
float dotGridGradient(int ix, int iy, float x, float y);
float interpolate(float, float, float);
vector2 randomGradient(int, int);