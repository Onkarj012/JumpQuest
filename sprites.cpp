#include "sprites.h"

// Load sprite sheet from file
SpriteSheet LoadSpriteSheet(const char* file, int cols, int rows)
{
    Texture2D tex = LoadTexture(file);
    SpriteSheet sheet{ tex };

    // Divide texture into frames
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            Rectangle frame{ (float)(tex.width / cols * c), (float)(tex.height / rows * r), (float)(tex.width / cols), (float)(tex.height / rows) };
            sheet.Frames.push_back(frame);
        }
    }

    return sheet;
}

// Add flipped frames to the sprite sheet
int AddFlippedFrames(SpriteSheet& sheet, int start, int end, bool flipX, int flipY)
{
    int startIndex = (int)sheet.Frames.size();
    for (int i = start; i <= end; i++) {
        Rectangle frame = sheet.Frames[i];
        if (flipX)
            frame.width *= -1;
        if (flipY)
            frame.height *= -1;
        sheet.Frames.push_back(frame);
    }
    return startIndex;
}

// Draw a sprite on the screen
void DrawSprite(SpriteInstance& sprite)
{
    Rectangle source = sprite.SpriteSheet->Frames[sprite.Animation.CurrentFrame];
    Rectangle dest = Rectangle{ sprite.Position.x, sprite.Position.y, source.width, source.height };

    // Handle flipped frames
    if (source.width < 0)
        dest.x -= dest.width;
    if (source.height < 0)
        dest.y -= dest.height;

    // Draw texture
    DrawTexturePro(sprite.SpriteSheet->Texture, source, dest, sprite.Offset, 0, WHITE);
}

// Update sprite animation based on frame rate
void UpdateSpriteAnimation(SpriteInstance& sprite)
{
    SpriteAnimation& anim = sprite.Animation;
    anim.Elapsed += GetFrameTime();
    if (anim.Elapsed >= 1.0f / anim.FPS) {
        anim.Elapsed = 0;
        anim.CurrentFrame++;

        if (anim.CurrentFrame > anim.FrameEnd) {
            if (anim.Loop)
                anim.CurrentFrame = anim.FrameStart;
            else
                anim.CurrentFrame = anim.FrameEnd;

            sprite.AnimationDone = !anim.Loop;
        }
    }
}

// Set a specific animation to a sprite
void SetSpriteAnimation(SpriteInstance& sprite, const SpriteAnimation& animation)
{
    sprite.Animation = animation;
    sprite.Animation.CurrentFrame = animation.FrameStart;
    sprite.AnimationDone = false;
}
