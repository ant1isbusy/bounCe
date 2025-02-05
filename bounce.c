#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 1280
#define HEIGHT 960

#define BALL_RADIUS 40
#define BALL_SPEED 400 // pixels per sec
#define FPS 75
#define MILISECS (int)floor((1 / (float)FPS) * 1000)

// link to SDL2 documentation: https://wiki.libsdl.org/SDL2/FrontPage

typedef struct {
  float x;
  float y;
  float velX;
  float velY;
  int radius; // in pixels
};

int getRandomSpeed()
{
  int rval = rand() % BALL_SPEED + (BALL_SPEED / 2);
  if (rval % 2 == 0)
  {
    rval = -rval;
  }
  return rval;
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
  // initial x, y in the center of the screen
  float ballX = WIDTH / 2.f;
  float ballY = HEIGHT / 2.f;

  // TODO: add physics
  float ballVelX = getRandomSpeed();
  printf("X: %.2f, Y: ", ballVelX);
  float ballVelY = getRandomSpeed();
  printf("%.2f\n", ballVelY);

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
    Uint32 curr_tick = SDL_GetTicks();
    float dt = (curr_tick - last_tick) / 1000.0f;
    last_tick = curr_tick;

    // linear motion for now, TODO: add gravity and velocity
    ballX += ballVelX * dt;
    ballY += ballVelY * dt;

    // change direction if we touch one of the "walls"
    if (ballX - BALL_RADIUS < 0 || ballX + BALL_RADIUS > WIDTH)
      ballVelX = -ballVelX;
    if (ballY - BALL_RADIUS < 0 || ballY + BALL_RADIUS > HEIGHT)
      ballVelY = -ballVelY;

    // clear canvas
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the ball
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White ball
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
          SDL_RenderDrawPoint(renderer, (int)ballX + w, (int)ballY + h);
        }
      }
    }

    SDL_RenderPresent(renderer);

    SDL_Delay(MILISECS);
  }

  // cleanup
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
