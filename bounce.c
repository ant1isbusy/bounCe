#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

#define WIDTH 1280
#define HEIGHT 960

#define BALL_RADIUS 40
#define BALL_SPEED 5

// link to SDL2 documentation: https://wiki.libsdl.org/SDL2/FrontPage

int main(int argc, char* argv[])
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* win =
    SDL_CreateWindow("Bouncing Animation", SDL_WINDOWPOS_CENTERED,
                     SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  SDL_Surface* surface = SDL_GetWindowSurface(win);

  SDL_Renderer* renderer =
    SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  // initial x, y in the center of the screen
  int ballX = WIDTH / 2;
  int ballY = HEIGHT / 2;

  // TODO: add physics
  int ballVelX = BALL_SPEED;
  int ballVelY = BALL_SPEED;

  int running = 1;
  SDL_Event event;
  while (running)
  {
    while (SDL_PollEvent(&event))
    {
      // graceful exit
      if (event.type == SDL_QUIT)
        running = 0;
    }

    // linear motion for now, TODO: add gravity and velocity
    ballX += ballVelX;
    ballY += ballVelY;

    // change direction if we touch one of the "walls"
    if (ballX - BALL_RADIUS < 0 || ballX + BALL_RADIUS > WIDTH)
      ballVelX = -ballVelX;
    if (ballY - BALL_RADIUS < 0 || ballY + BALL_RADIUS > HEIGHT)
      ballVelY = -ballVelY;

    // clear canvas
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the ball
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White ball
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
          SDL_RenderDrawPoint(renderer, ballX + w, ballY + h);
        }
      }
    }

    SDL_RenderPresent(renderer);

    SDL_Delay(8);  // 8ms is around 120fps, gives me a smooth animation
  }

  // cleanup
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
