#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define BASE_COLOR 255
#define POINT_THICK 5

#define TARGET_FPS 60
#define FRAME_DELAY_MS (1000 / TARGET_FPS)
#define MARGEN 30

//TODO
//soporte para configurar los colores
//automatizar el margen


struct Pix
{
	Uint8 blue;
	Uint8 green;
	Uint8 red;
	Uint8 alpha;
} typedef Pix;

struct Point
{
	int x;
	int y;
} typedef Point;

int anillo_cual(int x1, int x2, int y1, int y2)
{
	double distancia = hypot(x2 - x1, y2 - y1);

	if (distancia < POINT_THICK/2) return 0;

	//double distancia = 225.34;
	//el primer radio es Medio point thick
	//calculo en mitades
	//sumo 1 (una) y divido por dos
	//
	//mitad => anillo
	//0 => 0
	//1 => 1
	//2 => 1
	//3 => 2
	//4 => 2
	//
	return (int) ( floor( (distancia / (POINT_THICK / 2)) + 1) / 2);
}

int is_in_range0(Point *p, int x, int y)
{
	return abs(x - p->x) <= POINT_THICK/2 && abs(y - p->y) <= POINT_THICK/2;	
}

int is_in_range(Point *p, int x, int y, int iteracion)
{
	int difclic = POINT_THICK/2 + POINT_THICK * iteracion;
	int difclic_anterior = POINT_THICK/2 + POINT_THICK *  (iteracion-1);

	return (abs(x - p->x) <= difclic 
			&&  abs(x - p->x) >= difclic_anterior
			&& y < p->y + difclic
			&& y > p->y - difclic) 
		|| (abs(y - p->y) <= difclic			 
			&&  abs(y - p->y) >= difclic_anterior
			&& x < p->x + difclic
			&& x > p->x - difclic);  
}

int mod512(int val)
{
	if (val < 0 || val > 512) return 0;
	if (val > 255) return 511 - val;
	return val;	
}

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);
      	SDL_Window* window =   SDL_CreateWindow("Gotita",SDL_WINDOWPOS_UNDEFINED, 
		      SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,  SDL_WINDOW_SHOWN);

	SDL_Surface * sfc = SDL_GetWindowSurface(window);
      
	Point hit_point = {0};
	int mode_QUIT = 0;
	int mode_CURRENT = 0;
	int mode_STILL = 1;
	
        SDL_Event e = {0};
      	
	while(!mode_QUIT)
	{
		int total_pixels = sfc->w * sfc->h;
		Pix *pixels = (Pix*) sfc->pixels;	
      	
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
				{
					mode_QUIT = 1;
					break;
				}
				case SDL_TEXTINPUT:
				{
					printf("Any other key but escape: %s\n", e.text.text);
					break;
				}
				case SDL_KEYDOWN:
				{
					if (e.key.keysym.sym == SDLK_ESCAPE){
						mode_QUIT = 1;
						break;
					}
				}
				case SDL_MOUSEBUTTONDOWN:
				{
					printf("Mauss %i %i\n",e.button.x,e.button.y);
					hit_point.x = e.button.x; 
					hit_point.y = e.button.y;
					mode_CURRENT = 1;
					mode_STILL = 0;
				}
				default: //printf("hel %i\n",e.type);
			}
		}
	
      	
		if (mode_QUIT)
		{	
			SDL_DestroyWindow(window);
      			SDL_Quit();
			return 0;
		}

		if (mode_CURRENT)
		{
			int range_delay = 3; // cantidad de contador k hasta empezar nuevo anillo
			
			//calculamos la máxima distancia que es la diagonal al punto
			//calculo y mayor y x mayor y hago la distancia con 0,0
			int y_mayor = WINDOW_HEIGHT - hit_point.y < WINDOW_HEIGHT / 2 ? hit_point.y : WINDOW_HEIGHT - hit_point.y;
			int x_mayor = WINDOW_WIDTH - hit_point.x < WINDOW_WIDTH / 2 ? hit_point.x : WINDOW_WIDTH - hit_point.x;
			
			double diagonal_mayor_al_punto = sqrt(x_mayor*x_mayor + y_mayor*y_mayor);
			int cantidad_en_thick = (int) (diagonal_mayor_al_punto / POINT_THICK) + MARGEN;
			printf("diagonal %f >\n", diagonal_mayor_al_punto);	
			printf("cantidad en thicks %i\n",cantidad_en_thick);	
			
			for (int k = 0; k < 512 + (range_delay * cantidad_en_thick); k=k+6)
			{
				Uint32 frame_start = SDL_GetTicks();
				SDL_Event ev;
				while (SDL_PollEvent(&ev))
				{
				    if (ev.type == SDL_QUIT) { mode_QUIT = 1; }
				}
				if (mode_QUIT) break; 
				int pixindex = 0;
				for (int iy = 0; iy < sfc->h; iy++)
				{
					for (int ix = 0; ix < sfc->w; ix++)
					{
						//TODO:
						//averiguar en qué anillo está el pixel => devuelve anillo
						int anillo = anillo_cual(ix,hit_point.x,iy,hit_point.y);
						//averiguar en qué momento (momento_color) se encuentra
						int momento_color = k - range_delay * anillo;

						if ( momento_color < 0 || momento_color > 511)
						{					
							pixels[pixindex].blue = BASE_COLOR;
							pixels[pixindex].green = BASE_COLOR;
							pixels[pixindex].red = BASE_COLOR;

						} else
						{										
							
							//printf("momento_color %i\n", momento_color);
							pixels[pixindex].blue = 255 - mod512(momento_color);
							pixels[pixindex].green = 255 - mod512(momento_color);
							pixels[pixindex].red = 255 - mod512(momento_color);
						}
						pixindex++;
					}
					//pixindex++;	
				}	

				SDL_UpdateWindowSurface(window);
				//SDL_Delay(2);

				Uint32 elapsed = SDL_GetTicks() - frame_start;
				if (elapsed < FRAME_DELAY_MS)
    					SDL_Delay(FRAME_DELAY_MS - elapsed);
			}
			mode_CURRENT = 0;
			mode_STILL = 1;

		}

		Uint32 frame_start = SDL_GetTicks();
		if (mode_STILL)
		{		
			for (int i = 0; i < total_pixels ; i++)
			{
				pixels[i].blue = BASE_COLOR;
				pixels[i].green = BASE_COLOR;
				pixels[i].red = BASE_COLOR;	
			}	
		}
	
		SDL_UpdateWindowSurface(window);

		Uint32 elapsed = SDL_GetTicks() - frame_start;
		if (elapsed < FRAME_DELAY_MS)
    			SDL_Delay(FRAME_DELAY_MS - elapsed);
	}

      	return 0;
}
