#pragma warning(disable : 4996)

bool start = 0;
int text_aux = 0;

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<iostream>
#include<cstdlib>
#include<ctime>

const int WIN_HEIGHT = 400;
const int WIN_WIDTH = 450;
const int SOSEA_HEIGHT = WIN_HEIGHT;
const int SOSEA_WIDTH = 300;
int PLR_HEIGHT = 78;
int PLR_WIDTH = 52;
const int CAR_HEIGHT = PLR_HEIGHT;
const int CAR_WIDTH = PLR_WIDTH;

int x = 120;
int y = 250;
int zoom = 0;

int score = 0;

short int aux[5], demutat[5];

struct car {
	int x, y;
	int speed;
	SDL_Texture* car;
	SDL_Rect carRect;
}cars[6];

bool sosea = 0;
int lane = 2;

bool restart = 0, salt=0;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Surface* surface = nullptr;
SDL_Texture* fundal = nullptr;
SDL_Texture* sosea1 = nullptr;
SDL_Texture* sosea2 = nullptr;
SDL_Texture* player = nullptr;
SDL_Texture* player_fly = nullptr;
SDL_Texture* text = nullptr;
SDL_Texture* textscore = nullptr;
SDL_Texture* deadtext = nullptr;
SDL_Texture* welcome = nullptr;
SDL_Texture* indications = nullptr;
SDL_Texture* explosion = nullptr;
SDL_Rect playerRect;
SDL_Rect soseaRect = { 0, 0, SOSEA_WIDTH, SOSEA_HEIGHT };
TTF_Font* font;
Mix_Chunk* dead;
Mix_Chunk* jump;
Mix_Chunk* music;
Mix_Music* back;

char scoretext[100];
int nrcifre() {
	int aux = score, c = (aux<0);
	do {
		c++;
		aux /= 10;
	} while (aux);
	return c;
}
void move_car(int i, int lane) {
	if (cars[i].x < 20 + 100 * lane) cars[i].x += 20;
	else if (cars[i].x > 20 + 100 * lane) cars[i].x -= 20;
	else demutat[i] = 0;
}

void init();
void play();
void key(bool& quit);
void move();
void render();
void quit();

int main() {
	init();
	play();
	quit();
	return 0;
}

void init() {
	srand(time(NULL));

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	dead = Mix_LoadWAV("dead.mp3");
	music = Mix_LoadWAV("engine.mp3");
	back = Mix_LoadMUS("background.mp3");
	jump = Mix_LoadWAV("jump.mp3");
	Mix_PlayMusic(back, -1);
	Mix_VolumeMusic(30);


	window = SDL_CreateWindow("DRACING", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	surface = IMG_Load("sosea1.png");
	sosea1=SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);


	surface = IMG_Load("sosea2.png");
	sosea2 = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("car0.png");
	player = SDL_CreateTextureFromSurface(renderer, surface);
	if (player == nullptr)
		std::cout << "Cannot load player texture!" << std::endl;
	SDL_FreeSurface(surface);

	surface = IMG_Load("car0_fly.png");
	player_fly = SDL_CreateTextureFromSurface(renderer, surface);
	if (player == nullptr)
		std::cout << "Cannot load player texture!" << std::endl;
	SDL_FreeSurface(surface);

	for (int i = 1; i < 5; i++) {
		cars[i].x = 20 + rand() % 3 * 100;
		cars[i].y = -100 - i * CAR_HEIGHT - i * 20;
		if (i == 0) {
			surface = IMG_Load("car0.png");
		}
		else if (i == 1) {
			surface = IMG_Load("car1.png");
		}
		else if (i == 2) {
			surface = IMG_Load("car2.png");
		}
		else if (i == 3) {
			surface = IMG_Load("car3.png");
		}
		else if (i == 4) {
			surface = IMG_Load("car4.png");
		}
		cars[i].car = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		cars[i].carRect = { cars[i].x, cars[i].y, CAR_WIDTH, CAR_HEIGHT };
	}

	sprintf(scoretext, "%d", score);
	font = TTF_OpenFont("pixel_fundal.ttf", 20);
	surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
	if (surface == nullptr) {
		std::cout << "Unable to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
	text = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	surface = TTF_RenderText_Solid(font, "SCORE", { 255, 255, 255, 255 });
	textscore = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	surface = TTF_RenderText_Solid(font, "Press 'r' to restart or 'q' to quit.", { 255, 255, 255, 255 });
	if (surface == nullptr) {
		std::cout << "Unable to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
	deadtext = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("fundal.png");
	if (surface == nullptr) {
		std::cout << "Unable to load Start-image. Error: " << IMG_GetError() << std::endl;
	}
	welcome = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	
	surface = TTF_RenderText_Solid(font, "Press 'e' to start or 'q' to quit", { 255, 255, 255, 255 });
	if (surface == nullptr) {
		std::cout << "Unable to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
	indications = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("explosion.png");
	explosion = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
}

void play() {
	bool quit = 0;
	while (!quit) {
		key(quit);
		if (!restart && start) {
			move();
			Mix_PlayChannel(0, music, 0);
			Mix_VolumeMusic(30);
		}
		render();
	}
}

void key(bool &quit) {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT)
			quit = 1;
		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_q:
				quit = 1;
				break;
			case SDLK_UP:
				if(!restart && start)
					if (y > 10)
						y -= 10;
				break;
			case SDLK_DOWN:
				if (!restart && start)
				if (y + PLR_HEIGHT < WIN_HEIGHT)
					y += 10;
				break;
			case SDLK_RIGHT:
				if (!restart && start)
				if (lane < 3)
					lane++;
				break;
			case SDLK_LEFT:
				if (!restart && start)
				if (lane > 1)
					lane--;
				break;
			case SDLK_SPACE:
				if (!restart && start)
					if (salt == 0) {
						Mix_VolumeMusic(0);
						Mix_PlayChannel(-1, jump, 0);
						Mix_VolumeMusic(128);
						score -= 20;
						salt = 1;
						zoom = 20;
					}
				break;
			case SDLK_r:
				if (restart) {
					for (int i = 1; i < 5; i++) {
						cars[i].x = 20 + rand() % 3 * 100;
						if (i == 1) cars[i].y = -100;
						else cars[i].y = -100 - rand() % CAR_HEIGHT - 2 * CAR_HEIGHT - 40;
						cars[i].carRect = { cars[i].x, cars[i].y, CAR_WIDTH, CAR_HEIGHT };
					}
					int PLR_HEIGHT = 80;
					int PLR_WIDTH = 60;
					int x = 20;
					int y = 250;
					int zoom = 0;

					score = 0;
					sprintf_s(scoretext, "%d", score);
					SDL_DestroyTexture(text);
					surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
					text = SDL_CreateTextureFromSurface(renderer, surface);
					SDL_FreeSurface(surface);

					restart = 0;

					SDL_Delay(500);
				}
				break;
			case SDLK_e:
				if (!start) {
					start = 1;
					for (int i = 1; i < 5; i++) {
						cars[i].x = rand() % 3 * 100 + 20;
						cars[i].y = -100 - i * CAR_HEIGHT - i * 20;
					}
					SDL_DestroyTexture(welcome);
					SDL_DestroyTexture(indications);
					SDL_Delay(500);
				}
			}
		}
	}
}

void move() {
	if (lane == 1 && x > 20)
		x -= 20;
	else if (lane == 2 && x > 120)
		x -= 20;
	else if (lane == 2 && x < 120)
		x += 20;
	else if (lane == 3 && x<220)
		x += 20;

	for (int i = 1; i < 4; i++) {
		for (int j = i + 1; j < 5; j++) {
			if (SDL_HasIntersection(&cars[i].carRect, &cars[j].carRect))
				cars[i].y += 20, demutat[i] = (cars[i].x-20)/100;
		}
	}

	for (int i = 1; i < 5; i++)
		if (demutat[i])
			move_car(i, demutat[i]);

	if (zoom) PLR_HEIGHT = 78 + zoom, PLR_WIDTH = 52 + zoom, zoom--;
	if (zoom == 0) PLR_HEIGHT = 78, PLR_WIDTH = 52, salt=0;

	for (int i = 1; i < 5; i++) {
		if (cars[i].y >= y) { 
			if(aux[i]==0)
				score += 1; 
			aux[i] = 1;
			sprintf_s(scoretext, "%d", score);
			SDL_DestroyTexture(text);
			surface = TTF_RenderText_Solid(font, scoretext, { 255, 255, 255, 255 });
			text = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
		}
		if (SDL_HasIntersection(&playerRect, &cars[i].carRect) && salt == 0) 
		{ 
			Mix_PlayChannel(-1, dead, 0);
			restart = 1;
		}
		cars[i].y += 10+i+1;
		if (cars[i].y >= WIN_HEIGHT) {
			cars[i].x = rand() % 3 * 100 + 20;
			cars[i].y = -100 - i * CAR_HEIGHT - i * 20;
			aux[i] = 0;
		}
	}
	

}

void render() {
    SDL_RenderClear(renderer);

	if (start) {

		if (!sosea || restart) {
			SDL_RenderCopy(renderer, sosea1, NULL, &soseaRect);
			sosea = 1;
		}
		else {
			SDL_RenderCopy(renderer, sosea2, NULL, &soseaRect);
			sosea = 0;
		}

		for (int i = 1; i < 5; i++) {
			cars[i].carRect = { cars[i].x + 4, cars[i].y, CAR_WIDTH, CAR_HEIGHT };
			SDL_RenderCopy(renderer, cars[i].car, NULL, &cars[i].carRect);
		}

		playerRect = { x + 4, y, PLR_WIDTH, PLR_HEIGHT };
		if (!salt)
			SDL_RenderCopy(renderer, player, NULL, &playerRect);
		else
			SDL_RenderCopy(renderer, player_fly, NULL, &playerRect);

		SDL_Rect texttext = { 320, 40, 20 * nrcifre(), 20 };
		SDL_RenderCopy(renderer, text, NULL, &texttext);

		texttext = { 320, 10, 100, 20 };
		SDL_RenderCopy(renderer, textscore, NULL, &texttext);


		if (restart) {
			SDL_Rect textdead = { 10, 190, 420, 20 };
			SDL_RenderCopy(renderer, deadtext, NULL, &textdead);
			SDL_Rect exp = { x, y, 70, 70 };
			SDL_RenderCopy(renderer, explosion, NULL, &exp);
		}
	}
	else {
		text_aux++;
		SDL_Rect welcomeRect = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
		SDL_RenderCopy(renderer, welcome, NULL, &welcomeRect);

		if (text_aux <= 10) {
			SDL_Rect startRect = { 15, 340, 420, 20 };
			SDL_RenderCopy(renderer, indications, NULL, &startRect);
		}
		if (text_aux >= 20) text_aux = 0;
	}
	
	SDL_RenderPresent(renderer);


    SDL_Delay(60);
}

void quit() {
	SDL_DestroyTexture(text);
	SDL_DestroyTexture(textscore);
	SDL_DestroyTexture(deadtext);
	SDL_DestroyTexture(welcome);
	SDL_DestroyTexture(indications);
	SDL_DestroyTexture(player);
	for (int i = 1; i < 5; i++)
		SDL_DestroyTexture(cars[i].car);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}