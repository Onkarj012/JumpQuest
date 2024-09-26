
#pragma once

#include "raylib.h"
#include <vector>
#include <string>

struct SpriteSheet
{
    Texture2D SheetTexture = { 0 };
    std::vector<Rectangle> Frames;
};

struct SpriteAnimation
{
    std::string Name;
    int StartFrame = -1;
    int EndFrame = -1;
    float FPS = 15;
    bool Loops = true;
};

struct SpriteInstance
{
    Vector2 Position = { 0 };

    Vector2 Offset = { 0,0 };
    const SpriteSheet* Sheet = nullptr;
    const SpriteAnimation* Animation = nullptr;
    bool AnimationDone = false;
    int CurrentFrame = -1;
    float FrameLifetime = 0;
};

SpriteSheet LoadSpriteSheet(const char* file, int cols, int rows);
int AddFippedFrames(SpriteSheet& sheet, int start, int end, bool flipX, int flipY);

void DrawSprite(SpriteInstance& sprite);
void UpdateSpriteAnimation(SpriteInstance& sprite);
void SetSpriteAnimation(SpriteInstance& sprite, const SpriteAnimation& animation);
