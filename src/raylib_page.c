/*******************************************************************************************
*
*   raylib [core] example - Basic window (adapted for HTML5 platform)
*
*   This example is prepared to compile for PLATFORM_WEB, PLATFORM_DESKTOP and PLATFORM_RPI
*   As you will notice, code structure is slightly diferent to the other examples...
*   To compile it for PLATFORM_WEB just uncomment #define PLATFORM_WEB at beginning
*
*   This example has been created using raylib 1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <stdlib.h>

#include "../inc/jet.h"
#include "../inc/raylib.h"

#include "../inc/temperature_matrix.h"

#define PIXEL 4
#define WINDOW_WIDTH ((PIXEL * NX) + 3 * NX)
#define WINDOW_HEIGHT ((PIXEL * NY) + 3 * NY)
#define FPS 10

typedef enum
{
  HELP,
  STOP,
  PLAY,
  PAUSE
} player_status_t;

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//------------------------------------------------------------------------------
// Global Variables Definition
//------------------------------------------------------------------------------
//const int screenWidth = 800;
//const int screenHeight = 450;
//------------------------------------------------------------------------------
// Module Functions Declaration
//------------------------------------------------------------------------------

Color TtoC (double, double, double);

void UpdateDrawFrame (void); // Update and Draw one frame

//------------------------------------------------------------------------------
// Main Enry Point
//------------------------------------------------------------------------------
int
main ()
{
  // Initialization
  //--------------------------------------------------------------------------
  InitWindow (WINDOW_WIDTH, WINDOW_HEIGHT, "Rayleigh-Benard convection");

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop (UpdateDrawFrame, 5, 1);
#else
  SetTargetFPS (FPS);
  //--------------------------------------------------------------------------

  // Main loop
  while (!WindowShouldClose ()) // Detect window close button or ESC key
    {
      UpdateDrawFrame ();
    }
#endif

  // De-Initialization
  //--------------------------------------------------------------------------
  CloseWindow (); // Close window and OpenGL context
  //--------------------------------------------------------------------------

  return 0;
}

//------------------------------------------------------------------------------
// Module Functions Definition
//------------------------------------------------------------------------------

Color
TtoC (double cold, double hot, double t)
{
  /*
  (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
  */
  if ((t <= hot) && (t >= cold))
    {
      int index = (t - cold) * (JETSIZE - 1) / (hot - cold);
      return jet[index];
    }
  else
    {
      return (Color){ 0, 0, 0, 255 };
    }
}

void
UpdateDrawFrame (void)
{
  // Update
  //----------------------------------------------------------------------------
  // TODO: Update your variables here
  static int n_step = 0;
  static player_status_t player_status = STOP;
  static player_status_t old_player_status = STOP;
  int key_pressed = GetKeyPressed ();
  switch (key_pressed)
    {
    case 74: //j
      n_step += 5;
      if (n_step >= N_STEPS)
        n_step = N_STEPS - 1;
      break;

    case 75: //k
      n_step -= 5;
      if (n_step < 0)
        n_step = 0;
      break;

    case 83: //s
      player_status = STOP;
      n_step = 0;
      break;

    case 32: //space
      if (player_status == PAUSE || player_status == STOP)
        player_status = PLAY;
      else if (player_status == PLAY)
        player_status = PAUSE;
      break;

    case 72: //h
      if (player_status == HELP)
        {
          player_status = old_player_status;
        }
      else
        {
          old_player_status = player_status;
          player_status = HELP;
        }
      break;

    default:
      break;
    }
  if (n_step >= N_STEPS)
    exit (0);
  //----------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------
  BeginDrawing ();

  ClearBackground (RAYWHITE);
  DrawText ("Rayleigh Benard Convection Simulator", 10, 5, 30, BLACK);
  DrawText ("press 'h' for help", 10, 40, 20, BLACK);
  DrawText (TextFormat ("%d/%d", n_step + 1, N_STEPS), 150, 80, 20, BLACK);
  int posY = WINDOW_HEIGHT / 2 - JETSIZE / 2 + 4;
  DrawText (TextFormat ("%.1lfC", cold_temp),
            WINDOW_WIDTH / 2 + NX * PIXEL / 2 + 40, posY - 9, 20, BLACK);
  for (unsigned int i = 0; i < JETSIZE; ++i)
    {
      DrawRectangle (WINDOW_WIDTH / 2 + NX * PIXEL / 2 + 30, posY, 5, 1,
                     jet[i]);
      posY += 1;
    }
  DrawText (TextFormat ("%.1lfC", hot_temp),
            WINDOW_WIDTH / 2 + NX * PIXEL / 2 + 40, posY - 10, 20, BLACK);

  switch (player_status)
    {
    case PAUSE:
      DrawText ("PAUSE", 10, WINDOW_HEIGHT - 40, 30, BLACK);
      for (unsigned int i = 0; i < NX; ++i)
        {
          for (unsigned int k = 0; k < NY; ++k)
            {
              DrawRectangle ((WINDOW_WIDTH / 2 - NX * PIXEL / 2) + i * PIXEL,
                             (WINDOW_HEIGHT / 2 + NY * PIXEL / 2) - k * PIXEL,
                             PIXEL, PIXEL,
                             TtoC (cold_temp, hot_temp, T[n_step][k][i]));
            }
        }
      break;
    case PLAY:
      for (unsigned int i = 0; i < NX; ++i)
        {
          for (unsigned int k = 0; k < NY; ++k)
            {
              DrawRectangle ((WINDOW_WIDTH / 2 - NX * PIXEL / 2) + i * PIXEL,
                             (WINDOW_HEIGHT / 2 + NY * PIXEL / 2) - k * PIXEL,
                             PIXEL, PIXEL,
                             TtoC (cold_temp, hot_temp, T[n_step][k][i]));
            }
        }
      n_step += 1;
      break;

    case STOP:
      DrawText ("PRESS SPACE TO PLAY", 10, WINDOW_HEIGHT - 40, 30, BLACK);
      for (unsigned int i = 0; i < NX; ++i)
        {
          for (unsigned int k = 0; k < NY; ++k)
            {
              DrawRectangle ((WINDOW_WIDTH / 2 - NX * PIXEL / 2) + i * PIXEL,
                             (WINDOW_HEIGHT / 2 + NY * PIXEL / 2) - k * PIXEL,
                             PIXEL, PIXEL, BLACK);
            }
        }
      break;

    case HELP:
      for (unsigned int i = 0; i < NX; ++i)
        {
          for (unsigned int k = 0; k < NY; ++k)
            {
              DrawRectangle ((WINDOW_WIDTH / 2 - NX * PIXEL / 2) + i * PIXEL,
                             (WINDOW_HEIGHT / 2 + NY * PIXEL / 2) - k * PIXEL,
                             PIXEL, PIXEL, BLACK);
            }
        }
      DrawText ("h          show help", WINDOW_WIDTH / 2 - NX * PIXEL / 2 + 50,
                WINDOW_HEIGHT / 2 - 110, 30, RAYWHITE);
      DrawText ("j           -5 frame", WINDOW_WIDTH / 2 - NX * PIXEL / 2 + 50,
                WINDOW_HEIGHT / 2 - 60, 30, RAYWHITE);
      DrawText ("k          +5 frame", WINDOW_WIDTH / 2 - NX * PIXEL / 2 + 50,
                WINDOW_HEIGHT / 2 - 10, 30, RAYWHITE);
      DrawText ("s          stop", WINDOW_WIDTH / 2 - NX * PIXEL / 2 + 50,
                WINDOW_HEIGHT / 2 + 40, 30, RAYWHITE);
      DrawText ("space    play/pause", WINDOW_WIDTH / 2 - NX * PIXEL / 2 + 50,
                WINDOW_HEIGHT / 2 + 90, 30, RAYWHITE);
      break;
    }
  EndDrawing ();

  //----------------------------------------------------------------------------
}
