//  $Id$
//
//  SuperTux -  A Jump'n Run
//  Copyright (C) 2000 Bill Kendrick <bill@newbreedsoftware.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef WIN32
#include <sys/types.h>
#include <ctype.h>
#endif

#include "defines.h"
#include "globals.h"
#include "screen.h"
#include "setup.h"
#include "type.h"

/* Needed for line calculations */
#define SGN(x) ((x)>0 ? 1 : ((x)==0 ? 0:(-1)))
#define ABS(x) ((x)>0 ? (x) : (-x))

/* --- CLEAR SCREEN --- */

void clearscreen(int r, int g, int b)
{
#ifndef NOOPENGL
  if(use_gl)
    {
      glClearColor(r/256.0f, g/256.0f, b/256.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  else
  {
#endif
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
#ifndef NOOPENGL
    }
#endif
}

/* --- DRAWS A VERTICAL GRADIENT --- */

void drawgradient(Color top_clr, Color bot_clr)
{
#ifndef NOOPENGL
  if(use_gl)
    {
      glBegin(GL_QUADS);
      glColor3ub(top_clr.red, top_clr.green, top_clr.blue);
      glVertex2f(0, 0);
      glVertex2f(640, 0);
      glColor3ub(bot_clr.red, bot_clr.green, bot_clr.blue);
      glVertex2f(640, 480);
      glVertex2f(0, 480);
      glEnd();
    }
  else
  {
#endif
    for(float y = 0; y < 480; y += 2)
      fillrect(0, (int)y, 640, 2,
                     (int)(((float)(top_clr.red-bot_clr.red)/(0-480)) * y + top_clr.red),
                     (int)(((float)(top_clr.green-bot_clr.green)/(0-480)) * y + top_clr.green),
                     (int)(((float)(top_clr.blue-bot_clr.blue)/(0-480)) * y + top_clr.blue), 255);
/* calculates the color for each line, based in the generic equation for functions: y = mx + b */

#ifndef NOOPENGL
    }
#endif
}

/* --- FADE IN --- */

/** Fades the given surface into a black one. If fade_out is true, it will fade out, else
it will fade in */

void fade(Surface *surface, int seconds, bool fade_out);

void fade(const std::string& surface, int seconds, bool fade_out)
{
  Surface* sur = new Surface(datadir + surface, IGNORE_ALPHA);
  fade(sur, seconds, fade_out);
  delete sur;
}

void fade(Surface *surface, int seconds, bool fade_out)
{
  float alpha;
  if (fade_out)
    alpha = 0;
  else
    alpha = 255;

  int cur_time, old_time;
  cur_time = SDL_GetTicks();

  while(alpha >= 0 && alpha < 256)
    {
      surface->draw(0, 0, (int)alpha);
      flipscreen();

      old_time = cur_time;
      cur_time = SDL_GetTicks();

      /* Calculate the next alpha value */
      float calc = (float) ((cur_time - old_time) / seconds);
      if(fade_out)
        alpha += 255 * calc;
      else
        alpha -= 255 * calc;
    }
}

/* 'Stolen' from the SDL documentation.
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp)
    {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16 *)p = pixel;
      break;

    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
          p[0] = (pixel >> 16) & 0xff;
          p[1] = (pixel >> 8) & 0xff;
          p[2] = pixel & 0xff;
        }
      else
        {
          p[0] = pixel & 0xff;
          p[1] = (pixel >> 8) & 0xff;
          p[2] = (pixel >> 16) & 0xff;
        }
      break;

    case 4:
      *(Uint32 *)p = pixel;
      break;
    }
}

/* Draw a single pixel on the screen. */
void drawpixel(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawPoint(renderer, x, y);
}


void drawline(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{


#ifndef NOOPENGL
  if(use_gl)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4ub(r, g, b, a);

      glBegin(GL_LINES);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
      glEnd();

      glDisable(GL_BLEND);
    }
  else
    {
#endif
      SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(renderer, r, g, b, a);
      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
#ifndef NOOPENGL
    }
#endif
}

/* --- FILL A RECT --- */

void fillrect(float x, float y, float w, float h, int r, int g, int b, int a)
{
  if(w < 0)
    {
      x += w;
      w = -w;
    }
  if(h < 0)
    {
      y += h;
      h = -h;
    }


#ifndef NOOPENGL
  if(use_gl)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4ub(r, g, b, a);

      glBegin(GL_POLYGON);
      glVertex2f(x, y);
      glVertex2f(x+w, y);
      glVertex2f(x+w, y+h);
      glVertex2f(x, y+h);
      glEnd();

      glDisable(GL_BLEND);
    }
  else
    {
#endif
      SDL_Rect rect;
      rect.x = (int)x;
      rect.y = (int)y;
      rect.w = (int)w;
      rect.h = (int)h;

      SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(renderer, r, g, b, a);
      SDL_RenderFillRect(renderer, &rect);
#ifndef NOOPENGL
    }
#endif
}

/* --- UPDATE SCREEN --- */

void updatescreen(void)
{
  if(use_gl)
    SDL_GL_SwapWindow(window);
  else
    SDL_RenderPresent(renderer);
}

void flipscreen(void)
{
  if(use_gl)
    SDL_GL_SwapWindow(window);
  else
    SDL_RenderPresent(renderer);
}

void fadeout()
{
  clearscreen(0, 0, 0);
  
  int win_w, win_h;
  SDL_GetWindowSize(window, &win_w, &win_h);
  
  white_text->draw_align("Loading...", win_w/2, win_h/2, A_HMIDDLE, A_TOP);
  flipscreen();
}

void update_rect(SDL_Surface *scr, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
  // In SDL2, partial screen updates are not needed.
  // The renderer handles this automatically with SDL_RenderPresent().
  // This function is kept for compatibility but does nothing.
  (void)scr;
  (void)x;
  (void)y;
  (void)w;
  (void)h;
}