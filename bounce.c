#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define WIDTH 1280
#define HEIGHT 960

#define BALL_RADIUS 40
#define BALL_SPEED 500 // pixels per sec
#define FPS 144
#define MILISECS (int)floor((1 / (float)FPS) * 1000)
#define NUM_BALLS 8
#define NUM_COLORS 5 // dont forget to update once adding colors
#define WALLS_FACTOR 1.0f

const int COLORS[NUM_COLORS][3] = {
    {255, 0, 0},   // red
    {255, 128, 0}, // orange
    {255, 255, 0}, // yellow
    {0, 200, 0},   // green
    {0, 100, 255}    // blue
};

// link to SDL2 documentation: https://wiki.libsdl.org/SDL2/FrontPage

typedef struct
{
  int r, g, b;
} RGB;

typedef struct
{
  float x;
  float y;
  float velX;
  float velY;
  int radius; // in pixels
  RGB c;
} Ball;

int getRandomSpeed()
{
  int rval = rand() % BALL_SPEED + (BALL_SPEED / 2);
  if (rval % 2 == 0)
  {
    rval = -rval;
  }
  return rval;
}

RGB getRandomCol()
{
  int idx = rand() % NUM_COLORS;
  RGB col = {COLORS[idx][0], COLORS[idx][1], COLORS[idx][2]};
  return col;
}

void initBalls(Ball *balls)
{
  float angle = 0;
  float radius = 0;
  for (size_t i = 0; i < NUM_BALLS; i++)
  {
    // spiral spawn starting from the center, cos=horz, sin=vert
    int mult = i == 0 ? 0 : 1;
    balls[i].x = WIDTH / 2 + radius * cos(angle) + 20 * mult;
    balls[i].y = (HEIGHT / 2 - HEIGHT / 4) + radius * sin(angle) + 20 * mult;

    balls[i].velX = getRandomSpeed();
    balls[i].velY = getRandomSpeed();
    balls[i].c = getRandomCol();
    balls[i].radius = BALL_RADIUS;

    angle += 0.3; // half a radian per iteration
    radius += 2 * BALL_RADIUS;
  }
}

// O(n^2) collision checks for now, will do quadtree later TODO!
void updateBalls(Ball *balls, Uint32 *last_tick)
{
  Uint32 curr_tick = SDL_GetTicks();
  float dt = (curr_tick - *last_tick) / 1000.0f;
  *last_tick = curr_tick;

  for (int i = 0; i < NUM_BALLS; i++)
  {
    balls[i].x += balls[i].velX * dt;
    balls[i].y += balls[i].velY * dt;

    int hit = 0;
    // Wall check and seperation from wall (window border)
    if (balls[i].x - balls[i].radius < 0)
    {
      balls[i].velX = -balls[i].velX;
      balls[i].x = balls[i].radius; // ball center exactly one radius to the right of the window border
      hit = 1;
    }
    else if (balls[i].x + balls[i].radius > WIDTH)
    {
      balls[i].velX = -balls[i].velX;
      balls[i].x = WIDTH - balls[i].radius;
      hit = 1;
    }
    if (balls[i].y - balls[i].radius < 0)
    {
      balls[i].velY = -balls[i].velY;
      balls[i].y = balls[i].radius;
      hit = 1;
    }
    else if (balls[i].y + balls[i].radius > HEIGHT)
    {
      balls[i].velY = -balls[i].velY;
      balls[i].y = HEIGHT - balls[i].radius;
      hit = 1;
    }
    if (hit)
    {
      balls[i].velX *= WALLS_FACTOR;
      balls[i].velY *= WALLS_FACTOR;
    }
  }

  // O(n^2) ball collision check
  for (int i = 0; i < NUM_BALLS; i++)
  {
    for (int j = i + 1; j < NUM_BALLS; j++)
    {
      // helper vars, shorter
      Ball *a = &balls[i];
      Ball *b = &balls[j];

      float dx = b->x - a->x;
      float dy = b->y - a->y;
      float distance = sqrt(dx * dx + dy * dy);
      float min_distance = a->radius + b->radius;

      if (distance < min_distance)
      {
        // seperate the current overlap

        // normal calculation
        float nx = dx / distance;
        float ny = dy / distance;
        float overlap = min_distance - distance;

        a->x -= overlap * nx * 0.5f;
        a->y -= overlap * ny * 0.5f;
        b->x += overlap * nx * 0.5f;
        b->y += overlap * ny * 0.5f;

        // velocity swap with a bit of dropoff - maybe add real surface physics later? TODO:
        float tempVelX = a->velX;
        float tempVelY = a->velY;
        a->velX = b->velX * 0.98f;
        a->velY = b->velY * 0.98f;
        b->velX = tempVelX * 0.98f;
        b->velY = tempVelY * 0.98f;
      }
    }
  }
}

void renderBalls(SDL_Renderer *renderer, Ball *balls)
{
  for (size_t i = 0; i < NUM_BALLS; i++)
  {
    SDL_SetRenderDrawColor(renderer, balls[i].c.r, balls[i].c.g, balls[i].c.b, 255);
    for (int w = -BALL_RADIUS; w <= BALL_RADIUS; w++)
    {
      for (int h = -BALL_RADIUS; h <= BALL_RADIUS; h++)
      {
        // we can fill the circle using basic geometry:
        // the outline of a circle can be parametrized using
        // x^2 + y^2 = r^2, we check if the pixels are inside:
        if (w * w + h * h <= BALL_RADIUS * BALL_RADIUS)
        {
          // add pixel to backbuffer
          SDL_RenderDrawPoint(renderer, (int)balls[i].x + w, (int)balls[i].y + h);
        }
      }
    }
  }
}

int main()
{
  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *win =
      SDL_CreateWindow("Bouncing Animation", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  Ball *balls = (Ball *)malloc(NUM_BALLS * sizeof(Ball));
  initBalls(balls);

  int running = 1;
  SDL_Event event;
  Uint32 last_tick = SDL_GetTicks();
  while (running)
  {
    while (SDL_PollEvent(&event))
    {
      // graceful exit
      if (event.type == SDL_QUIT)
        running = 0;
    }
    updateBalls(balls, &last_tick);

    // clear canvas
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    // Draw the ball
    renderBalls(renderer, balls);
    SDL_RenderPresent(renderer);

    SDL_Delay(MILISECS);
  }

  // cleanup
  free(balls);
  printf("Mem freed!\n");
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
