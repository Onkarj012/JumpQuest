#ifndef SPRITES_H
#define SPRITES_H

#include "raylib.h"
#include <vector>
#include <string>

// SpriteSheet struct stores texture and frame data
struct SpriteSheet
{
    Texture2D Texture;
    std::vector<Rectangle> Frames;
};

// SpriteAnimation struct for animation control
struct SpriteAnimation
{
    std::string Name;
    int FrameStart = 0;
    int FrameEnd = 0;
    int CurrentFrame = 0;
    float FPS = 10.0f;
    float Elapsed = 0.0f;
    bool Loop = true;
};

// SpriteInstance struct represents an individual sprite in-game
struct SpriteInstance
{
    Vector2 Position;
    Vector2 Offset;
    SpriteSheet* SpriteSheet;
    SpriteAnimation Animation;
    bool AnimationDone = false;
};

// Function declarations
SpriteSheet LoadSpriteSheet(const char* file, int cols, int rows);
int AddFippedFrames(SpriteSheet& sheet, int start, int end, bool flipX, int flipY);
void DrawSprite(SpriteInstance& sprite);
void UpdateSpriteAnimation(SpriteInstance& sprite);
void SetSpriteAnimation(SpriteInstance& sprite, const SpriteAnimation& animation);

#endif
