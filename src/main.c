#include "raylib.h"
#include <math.h>
#include <stdbool.h>

#define WINDOW_WIDTH  2000
#define WINDOW_HEIGHT 1000
#define THRUST_FORCE  300
#define MAX_SPEED     400
#define MAX_BULLETS   5

#define global_variable static

#define MATH_PI            3.14159265
#define MIN(a, b)          (a) < (b) ? (a) : (b)
#define MAX(a, b)          (a) > (b) ? (a) : (b)
#define CLAMP(v, min, max) (MAX(min, (MIN(max, v))))
#define ARR_LENGTH(arr)    sizeof(arr) / sizeof(arr[0])

global_variable bool debug = false;

// ---------------------------------
// 		Simple Mathematics
// ---------------------------------
typedef struct Vec2
{
    float x;
    float y;
} Vec2;

float radians(float degrees);
float degrees(float radians);
Vec2  vector_rotate(Vec2 v, float angle);
Vec2  get_forward_vector(float direction);
float length_squared(Vec2 v);
float length(Vec2 v);

// ---------------------------------
// 				Game
// ---------------------------------
// NOTE: Should we only have a single Entity structure with different types
// or just different structures
typedef struct Bullet
{
    Vec2  position;
    Vec2  velocity;
    float direction;
    bool  is_active;
} Bullet;

typedef struct Player
{
    Bullet bullets[MAX_BULLETS];
    Vec2   vertices[3]; // Used to draw the player
    Vec2   position;
    Vec2   velocity;
    Vec2   accelaration;
    Vec2   forward;          // forward vector derived from direction
    float  angluar_velocity; // rotation speed
    float  direction;        // player direction in degrees
    int    lives;
} Player;

typedef struct Asteroid
{
    Vec2  position;
    Vec2  velocity;
    Vec2  forward;
    float direction;
    int   lives;
} Asteroid;

void process_input(Player *player, float dt);
void update_player(Player *player, float dt);
void update_asteroids(Player *player, float dt);
void draw_player(Player *player);
void draw_bullets(Bullet bullets[], int count);
void wrap_to_screen(Vec2 *position);

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Asteroids");

    // Initialize game state
    Player player;
    player.position         = (Vec2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
    player.velocity         = (Vec2){0.0f, 0.0f};
    player.accelaration     = (Vec2){0.0f, 0.0f};
    player.direction        = -90.0f;
    player.angluar_velocity = 200.0f;
    player.lives            = 4;
    player.forward          = get_forward_vector(radians(player.direction));

    // Player vertices
    player.vertices[0] = (Vec2){-25.0f, -25.0f};
    player.vertices[1] = (Vec2){-25.0f, 25.0f};
    player.vertices[2] = (Vec2){50.0f, 0.0f};

    // Initialize Bullets
    // for (int i = 0; i < ARR_LENGTH(player.bullets); ++i)
    // {
    // }

    Asteroid asteroid;
    asteroid.position   = (Vec2){200.0f, 200.0f};
    asteroid.velocity.x = 200.0f;
    asteroid.velocity.y = 0.0f;
    asteroid.direction  = 0.0f;
    asteroid.lives      = 3;
    asteroid.forward    = get_forward_vector(radians(asteroid.direction));

    // Game Loop
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        process_input(&player, dt);

        update_player(&player, dt);

        // Update Asteroids
        // NOTE: The forward vector is calculated only once during spawn
        asteroid.position.x += asteroid.velocity.x * asteroid.forward.x * dt;
        asteroid.position.y += asteroid.velocity.y * asteroid.forward.y * dt;

        wrap_to_screen(&asteroid.position);

        BeginDrawing();
        ClearBackground((Color){20, 20, 20, 255});

        // Drawing the player
        draw_player(&player);
		draw_bullets(player.bullets, ARR_LENGTH(player.bullets));

        // NOTE: Rectangle for now later drawing
        // Draw Asteroid
        DrawRectangleLines(asteroid.position.x, asteroid.position.y, 100, 100, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

float radians(float degrees) { return degrees * (MATH_PI / 180); }

float degrees(float radians) { return radians * (180 / MATH_PI); }

Vec2 vector_rotate(Vec2 v, float angle)
{
    Vec2 result = {0};

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.x = v.x * cosres - v.y * sinres;
    result.y = v.x * sinres + v.y * cosres;

    return result;
}

Vec2 get_forward_vector(float direction)
{
    Vec2 forward;
    forward.x = cos(direction);
    forward.y = sin(direction);
    return forward;
}

float length_squared(Vec2 v) { return v.x * v.x + v.y * v.y; }

float length(Vec2 v) { return sqrtf(length_squared(v)); }

// NOTE: wrap_to_screen: make sure objects are on the screen
// First implementation that needs later improvement
void wrap_to_screen(Vec2 *position)
{
    if (position->x < 0)
    {
        position->x = WINDOW_WIDTH;
    }
    else if (position->x > WINDOW_WIDTH)
    {
        position->x = 0;
    }
    else if (position->y < 0)
    {
        position->y = WINDOW_HEIGHT;
    }
    else if (position->y > WINDOW_HEIGHT)
    {
        position->y = 0;
    }
}

void process_input(Player *player, float dt)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        WindowShouldClose();
    }
    else if (IsKeyPressed(KEY_T))
    {
        debug = !debug;
    }

    if (IsKeyDown(KEY_W))
    {
        player->forward        = get_forward_vector(radians(player->direction));
        player->accelaration.x = player->forward.x * THRUST_FORCE;
        player->accelaration.y = player->forward.y * THRUST_FORCE;
        player->velocity.x += player->accelaration.x * dt;
        player->velocity.y += player->accelaration.y * dt;
    }

    if (IsKeyDown(KEY_A))
    {
        player->direction -= player->angluar_velocity * dt;
    }

    if (IsKeyDown(KEY_D))
    {
        player->direction += player->angluar_velocity * dt;
    }

    if (IsKeyDown(KEY_SPACE))
    {
        for (int i = 0; i < ARR_LENGTH(player->bullets); ++i)
        {
            player->bullets[i].is_active = true;
            player->bullets[i].position = (Vec2){player->position.x, player->position.y};
        }
    }
}

void update_player(Player *player, float dt)
{
    player->accelaration.x = 0;
    player->accelaration.y = 0;

    // NOTE: Clamp speed
    float current_speed = length(player->velocity);
    if (current_speed > MAX_SPEED)
    {
        player->velocity.x = (player->velocity.x / current_speed) * MAX_SPEED;
        player->velocity.y = (player->velocity.y / current_speed) * MAX_SPEED;
    }

    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;

    wrap_to_screen(&player->position);
}

void draw_player(Player *player)
{
    // 1. Local offset
    // Defined only once in the beggining player vertices
    Vec2 v1, v2, v3;
    v1 = player->vertices[0];
    v2 = player->vertices[1];
    v3 = player->vertices[2];

    // 2. Rotate vertices
    float player_direction = radians(player->direction);
    Vec2  rot_1, rot_2, rot_3;
    rot_1 = vector_rotate(v1, player_direction);
    rot_2 = vector_rotate(v2, player_direction);
    rot_3 = vector_rotate(v3, player_direction);

    // 3. Add to final position
    Vector2 fin_1, fin_2, fin_3;
    fin_1 = (Vector2){player->position.x + rot_1.x, player->position.y + rot_1.y};
    fin_2 = (Vector2){player->position.x + rot_2.x, player->position.y + rot_2.y};
    fin_3 = (Vector2){player->position.x + rot_3.x, player->position.y + rot_3.y};

    // Draw player
    DrawTriangleLines(fin_1, fin_2, fin_3, WHITE);

    if (debug)
    {
        // Player center position
        DrawCircle(player->position.x, player->position.y, 5, PURPLE);

        // Player vertices
        DrawRectangle(fin_1.x, fin_1.y, 10, 10, RED);
        DrawRectangle(fin_2.x, fin_2.y, 10, 10, GREEN);
        DrawRectangle(fin_3.x, fin_3.y, 10, 10, BLUE);

        DrawText(TextFormat("Acceleration %0.f %0.f", player->accelaration.x, player->accelaration.y), 50, 50, 20.0f,
                 GREEN);
        DrawText(TextFormat("Velocity %0.f %0.f", player->velocity.x, player->velocity.y), 50, 100, 20.0f, GREEN);
        // DrawText(TextFormat("Direction degrees %f", player.direction), 50, 100, 20.0f, GREEN);
        // DrawText(TextFormat("Direction radians %f", radians(player.direction)), 50, 200, 20.0f, GREEN);
    }
}

void draw_bullets(Bullet bullets[], int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (bullets[i].is_active)
        {
            DrawRectangle(bullets[i].position.x, bullets[i].position.x, 5, 5, PINK);
        }
    }
}
