#include "raylib.h"
#include "raymath.h"
#include "sprites.h"

#include <unordered_map>
#include <vector>

// Global variables for obstacles and textures
std::vector<Rectangle> Obstacles;
NPatchInfo ObstacleNpatch = { 0 };
Texture2D ObstacleTexture = { 0 };

// Enum to manage different player states
enum class ActorStates
{
    Idle,
    Run,
    JumpStart,
    JumpUp,
    FallDown,
};

// Struct to hold player (Warrior) data
struct Actor
{
    ActorStates State = ActorStates::Idle; // Current state
    bool FacingRight = true;               // Direction the player faces
    SpriteInstance Sprite;                 // Player's sprite instance
    std::unordered_map<ActorStates, std::vector<SpriteAnimation>> AnimationStates; // Animation mapping
    float RunSpeed = 200;
    float jumpAcceleration = -350;
    float jumpVelocityDampen = 1.0f;
    Vector2 Velocity = { 0,0 };            // Movement speed
};
Actor Warrior;

// Load obstacles into the game world
void LoadObstacles()
{
    ObstacleTexture = LoadTexture("resources/panel_blue.png");
    ObstacleNpatch.source = Rectangle{ 0, 0, (float)ObstacleTexture.width, (float)ObstacleTexture.height };
    ObstacleNpatch.top = 20;
    ObstacleNpatch.bottom = 20;
    ObstacleNpatch.right = 20;
    ObstacleNpatch.left = 20;

    // Add multiple platform obstacles
    Obstacles.emplace_back(Rectangle{ 200, -100, 200, 50 });
    Obstacles.emplace_back(Rectangle{ 400, -200, 100, 25 });
    Obstacles.emplace_back(Rectangle{ 550, -300, 100, 50 });
    // Add more obstacles here...
}

// Load background elements (sky, mountains, clouds)
void LoadBackground() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), SKYBLUE); // Sky

    // Draw mountains
    DrawTriangle(Vector2{ 200, 450 }, Vector2{ 400, 150 }, Vector2{ 600, 450 }, DARKGREEN);
    DrawTriangle(Vector2{ 600, 450 }, Vector2{ 800, 200 }, Vector2{ 1000, 450 }, DARKGREEN);

    // Draw clouds
    DrawCircle(150, 100, 40, WHITE);
    DrawCircle(180, 120, 40, WHITE);
    DrawCircle(130, 120, 40, WHITE);

    // Draw sun
    DrawCircle(700, 60, 50, YELLOW);
}

int main(void)
{
    // Screen setup
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "sprite animation");

    // Camera setup
    Camera2D cam = { 0 };
    cam.zoom = 1;
    cam.offset.x = 20;
    cam.offset.y = GetScreenHeight() - 50;
    cam.target.x = 0;
    cam.target.y = 0;

    float gravity = 500.0f;

    // Load game assets
    LoadBackground();
    LoadObstacles();

    // Load warrior sprite sheet
    SpriteSheet warriorSheet = LoadSpriteSheet("resources/Warrior_Sheet-Effect.png", 6, 17);

    // Create left-facing frames
    int leftFrameStart = AddFlippedFrames(warriorSheet, 0, (int)warriorSheet.Frames.size() - 1, true, false);

    // Initialize warrior position and sprite
    Warrior.Sprite = SpriteInstance{ Vector2{0, 0}, Vector2{warriorSheet.Frames[0].width / 2,warriorSheet.Frames[0].height }, &warriorSheet };

    // Set warrior animations for each state
    Warrior.AnimationStates[ActorStates::Idle].emplace_back(SpriteAnimation{ "right_idle", 0, 5, 5 });
    Warrior.AnimationStates[ActorStates::Idle].emplace_back(SpriteAnimation{ "left_idle", leftFrameStart + 0, leftFrameStart + 5, 5 });
    Warrior.AnimationStates[ActorStates::Run].emplace_back(SpriteAnimation{ "right_run", 6, 13 });
    Warrior.AnimationStates[ActorStates::Run].emplace_back(SpriteAnimation{ "left_run", leftFrameStart + 6, leftFrameStart + 13 });
    Warrior.AnimationStates[ActorStates::JumpStart].emplace_back(SpriteAnimation{ "right_jump", 41, 43, 15, false });
    Warrior.AnimationStates[ActorStates::JumpStart].emplace_back(SpriteAnimation{ "left_jump", leftFrameStart + 41, leftFrameStart + 43, 15, false });
    Warrior.AnimationStates[ActorStates::JumpUp].emplace_back(SpriteAnimation{ "right_fly", 43, 43, 10 });
    Warrior.AnimationStates[ActorStates::JumpUp].emplace_back(SpriteAnimation{ "left_fly", leftFrameStart + 43, leftFrameStart + 43, 10 });
    Warrior.AnimationStates[ActorStates::FallDown].emplace_back(SpriteAnimation{ "right_fall", 46, 48, 10 });
    Warrior.AnimationStates[ActorStates::FallDown].emplace_back(SpriteAnimation{ "left_fall", leftFrameStart + 46, leftFrameStart + 48, 10 });

    // Set initial animation (Idle)
    SetSpriteAnimation(Warrior.Sprite, Warrior.AnimationStates[ActorStates::Idle][0]);

    SetTargetFPS(60); // 60 FPS

    // Main game loop
    while (!WindowShouldClose()) // Runs until the window is closed
    {
        // Get player input
        int runDir = 0;
        if (IsKeyDown(KEY_D))
            runDir += 1;
        if (IsKeyDown(KEY_A))
            runDir += -1;

        bool wantJump = IsKeyDown(KEY_SPACE);

        // Zoom in/out with Z key
        if (IsKeyPressed(KEY_Z))
            cam.zoom = (cam.zoom == 1) ? 1.5f : 1;

        // Update player state based on input
        switch (Warrior.State)
        {
        case ActorStates::Idle:
            if (wantJump) {
                Warrior.State = ActorStates::JumpStart;
                Warrior.Velocity.y = Warrior.jumpAcceleration;
            }
            else if (runDir != 0) {
                Warrior.State = ActorStates::Run;
                Warrior.FacingRight = runDir > 0;
            }
            break;

        case ActorStates::Run:
            if (wantJump) {
                Warrior.State = ActorStates::JumpStart;
                Warrior.Velocity.y = Warrior.jumpAcceleration;
            }
            else if (runDir == 0) {
                Warrior.State = ActorStates::Idle;
            }
            else {
                Warrior.FacingRight = runDir > 0;
                Warrior.Velocity.x = runDir * Warrior.RunSpeed;
            }
            break;

        case ActorStates::JumpStart:
            if (Warrior.Velocity.y >= 0)
                Warrior.State = ActorStates::FallDown;
            else if (Warrior.Sprite.AnimationDone)
                Warrior.State = ActorStates::JumpUp;
            break;

        case ActorStates::JumpUp:
            if (Warrior.Velocity.y >= 0)
                Warrior.State = ActorStates::FallDown;
            break;

        case ActorStates::FallDown:
            break;
        }

        // Apply gravity
        Warrior.Velocity.y += gravity * GetFrameTime();

        // Calculate new position
        Vector2 newPos = Vector2Add(Warrior.Sprite.Position, Vector2Scale(Warrior.Velocity, GetFrameTime()));

        // Check collision with ground
        if (newPos.y > 0) {
            newPos.y = 0;
            Warrior.Velocity.y = 0;
            Warrior.State = ActorStates::Idle;
        }

        // Check collisions with obstacles
        for (const Rectangle& obstacle : Obstacles) {
            if (CheckCollisionPointRec(newPos, obstacle) && Warrior.Sprite.Position.y <= obstacle.y) {
                if (Warrior.Sprite.Position.y < newPos.y) {
                    Warrior.Velocity.y = 0;
                    newPos.y = obstacle.y;
                    Warrior.State = ActorStates::Idle;
                }
            }
        }

        // Update animation based on state
        if (Warrior.Sprite.Animation != &(Warrior.AnimationStates[Warrior.State][Warrior.FacingRight ? 0 : 1])) {
            SetSpriteAnimation(Warrior.Sprite, Warrior.AnimationStates[Warrior.State][Warrior.FacingRight ? 0 : 1]);
        }

        // Update sprite animation
        UpdateSpriteAnimation(Warrior.Sprite);

        // Adjust camera position based on player movement
        Vector2 playerScreenPos = GetWorldToScreen2D(Warrior.Sprite.Position, cam);
        if (playerScreenPos.x < 10)
            cam.target.x -= (GetScreenWidth() - 50) / cam.zoom;
        if (playerScreenPos.x > GetScreenWidth() - 25)
            cam.target.x += (GetScreenWidth() - 50) / cam.zoom;
        if (playerScreenPos.y < 10)
            cam.target.y -= (GetScreenHeight() - 50) / cam.zoom;
        if (playerScreenPos.y > GetScreenHeight() - 25)
            cam.target.y += (GetScreenHeight() - 50) / cam.zoom;

        // Render game world
        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(cam);

        LoadBackground();

        // Draw obstacles
        for (Rectangle& r : Obstacles)
            DrawTextureNPatch(ObstacleTexture, ObstacleNpatch, r, Vector2{ 0, 0 }, 0, WHITE);

        // Draw player sprite
        DrawSprite(Warrior.Sprite);

        EndMode2D();

        // Display current player state as text
        DrawText(TextFormat("State: %s", Warrior.State == ActorStates::Idle ? "Idle" :
            Warrior.State == ActorStates::Run ? "Run" :
            Warrior.State == ActorStates::JumpStart ? "JumpStart" :
            Warrior.State == ActorStates::JumpUp ? "JumpUp" :
            Warrior.State == ActorStates::FallDown ? "FallDown" : "Unknown"), 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow(); // Close game window
    return 0;
}
