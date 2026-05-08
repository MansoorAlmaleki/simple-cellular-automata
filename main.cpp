#include <iostream>
#include <vector>
#include <raylib.h>

using namespace std;

enum class ParticleType{
    AIR,
    SAND,
    STONE
};

struct PixelColor{
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 255;

    PixelColor(int _r, int _g, int _b, int _a){
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
};

struct Particle{
    PixelColor color{0, 0, 0, 0};
    ParticleType PixelType;

    Particle(ParticleType _PixelType){
        PixelType = _PixelType;

        if(PixelType == ParticleType::AIR){color.r = 0, color.g = 0, color.b = 0, color.a = 0;}
        if(PixelType == ParticleType::SAND){color.r = 255, color.g = 170, color.b = 0, color.a = 255;}
        if(PixelType == ParticleType::STONE){color.r = 100, color.g = 100, color.b = 100, color.a = 255;}

    }

    void CheckVectorRangeAndApplyToBuffer(vector<Particle>& ParticleGrid, vector<Particle>& ParticleGridBuffer, int x, int y, int width, int height) {
      ParticleType currentType = ParticleGrid[y*width+x].PixelType;

       if(currentType == ParticleType::AIR) return;

      if(currentType == ParticleType::SAND) {
        int belowIndex = (y+1)*width+x;
        if(y < height - 1) {
            if(ParticleGrid[belowIndex].PixelType == ParticleType::AIR) {
                ParticleGridBuffer[belowIndex] = Particle{ParticleType::SAND};
                return;
            }
        }
        if(x > 0 || x < width-1){
            if((ParticleGrid[belowIndex].PixelType == ParticleType::STONE || ParticleGrid[belowIndex].PixelType == ParticleType::SAND) && (ParticleGrid[y*width+(x-1)].PixelType == ParticleType::AIR) && (ParticleGrid[(y+1)*width+(x-1)].PixelType == ParticleType::AIR)) {

                ParticleGridBuffer[y*width+(x-1)] = Particle{ParticleType::SAND};
                return;
            }
            if((ParticleGrid[belowIndex].PixelType == ParticleType::STONE || ParticleGrid[belowIndex].PixelType == ParticleType::SAND) && (ParticleGrid[y*width+(x+1)].PixelType == ParticleType::AIR) && (ParticleGrid[(y+1)*width+(x+1)].PixelType == ParticleType::AIR)) {

                ParticleGridBuffer[y*width+(x+1)] = Particle{ParticleType::SAND};
                return;
            }
        }
    }
    ParticleGridBuffer[y*width+x] = ParticleGrid[y*width+x];
    }

    void UpdateGrid(vector<Particle>& ParticleGrid, vector<Particle>& ParticleGridBuffer, int x, int y, int width, int height){
                ParticleGrid[y*width+x] = ParticleGridBuffer[y*width+x];
                ParticleGridBuffer[y*width+x] = Particle{ParticleType::AIR};
    }
};

int main() {
    const int ScreenWidth = 640;
    const int ScreenHeight = 640;
    const int TileSize = 16;

    double Dt;
    double Time = 0;
    double PreviousTime = 0;

    int MousePosX;
    int MousePosY;

    ParticleType Material = ParticleType::AIR;

    InitWindow(ScreenWidth, ScreenHeight, "sand cellular automata");
    SetTargetFPS(144);

    vector<Particle> PixelGrid;
    vector<Particle> PixelGridBuffer;
    PixelGrid.resize((ScreenWidth/TileSize)*(ScreenHeight+TileSize/TileSize), Particle{ParticleType::AIR});
    PixelGridBuffer.resize((ScreenWidth/TileSize)*(ScreenHeight+TileSize/TileSize), Particle{ParticleType::AIR});
    while (!WindowShouldClose()){

        Dt = GetFrameTime();
        Time += Dt;

        MousePosX = GetMousePosition().x;
        MousePosY = GetMousePosition().y;
        int GhostPixelPositionX = MousePosX >> 4;
        int GhostPixelPositionY = MousePosY >> 4;
        DrawRectangle(GhostPixelPositionX*TileSize, GhostPixelPositionY*TileSize, TileSize, TileSize,Color{255,255,255,100});
        if(IsKeyPressed(KEY_ONE)){Material = ParticleType::AIR;}
        if(IsKeyPressed(KEY_TWO)){Material = ParticleType::SAND;}
        if(IsKeyPressed(KEY_THREE)){Material = ParticleType::STONE;}
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){PixelGrid[GhostPixelPositionY*(ScreenWidth/TileSize)+GhostPixelPositionX] = Particle{Material};}

        if(IsKeyPressed(KEY_Q)){fill(PixelGrid.begin(), PixelGrid.end(), Particle{ParticleType::AIR});}

        if(Time-PreviousTime >= 1.0f/30.0f){
            PreviousTime = Time;
            for(int y = 0; y < ScreenHeight/TileSize; y++){
                for(int x = 0; x < ScreenWidth/TileSize; x++){
                    PixelGrid[y*(ScreenWidth/TileSize)+x].CheckVectorRangeAndApplyToBuffer(PixelGrid, PixelGridBuffer, x, y, ScreenWidth/TileSize, ScreenHeight/TileSize);
                    PixelGrid[y*(ScreenWidth/TileSize)+x].UpdateGrid(PixelGrid, PixelGridBuffer, x, y, ScreenWidth/TileSize, ScreenHeight/TileSize);
                }
            }
        }

        BeginDrawing();
        ClearBackground(Color{15,10,15,255});

        for(int y = 0; y < ScreenHeight/TileSize; y++){
            for(int x = 0; x < ScreenWidth/TileSize; x++){
                Color pixelCol = {PixelGrid[y*(ScreenWidth/TileSize)+x].color.r, PixelGrid[y*(ScreenWidth/TileSize)+x].color.g, PixelGrid[y*(ScreenWidth/TileSize)+x].color.b, PixelGrid[y*(ScreenWidth/TileSize)+x].color.a};
                DrawRectangle(x*TileSize, y*TileSize, TileSize, TileSize,pixelCol);
            }
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
