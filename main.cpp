#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>
#include <SDL_ttf.h>
#include <sstream>
#include <fstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;
class LTexture {
    public:
        LTexture();
        ~LTexture();
        bool loadFromFile(std::string path);
        void free();
        void render(int x, int y);
        int getWidth();
        int getHeight();
        #if defined(SDL_TTF_MAJOR_VERSION)
            bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
        #endif
    private:
        SDL_Texture* texture;
        int width;
        int height;
};

bool init();
bool loadMedia();
void close();
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
LTexture Text0, Text2, Text4, Text8, Text16, Text32, Text64,
Text128, Text256, Text512, Text1024, Text2048;
LTexture gBackgroundText;
LTexture Button, ButtonDown;
LTexture ScoreText, BestText;
LTexture Score, Best;
LTexture victory, defeat;
TTF_Font* gFont = NULL;
Mix_Music* gMusic = NULL;
Mix_Chunk* gwin = NULL;
Mix_Chunk* glose = NULL;
SDL_Color textColor = { 0, 0, 0, 255 };
int score = 0, spm = 0, best = 0;
int** board;
bool win = false, lose = false;

LTexture::LTexture(){
	texture = NULL;
	width = 0;
	height = 0;
}

LTexture::~LTexture(){ free(); }

bool LTexture::loadFromFile(std::string path){
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL){
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL){
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		} else {
			width = loadedSurface->w;
			height = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface);
	}
	texture = newTexture;
	return texture != NULL;
}

void LTexture::free(){
	if (texture != NULL){
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
}

void LTexture::render(int x, int y){
	SDL_Rect renderQuad = { x, y, width, height };
	SDL_RenderCopy(gRenderer, texture, NULL, &renderQuad);
}

int LTexture::getWidth(){ return width; }
int LTexture::getHeight(){ return height; }

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor){
	free();
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL){
		//Create texture from surface pixels
		texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (texture == NULL){
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		} else {
			//Get image dimensions
			width = textSurface->w;
			height = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	} else {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	return texture != NULL;
}
#endif

bool init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 < 0){
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
			printf("Warning: Linear texture filtering not enabled!");
		}
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL){
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL){
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)){
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
                }

				if (TTF_Init() == -1){
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia(){
	bool success = true;
	Text0.loadFromFile("image/0.png");
	Text2.loadFromFile("image/2.png");
	Text4.loadFromFile("image/4.png");
	Text8.loadFromFile("image/8.png");
	Text16.loadFromFile("image/16.png");
	Text32.loadFromFile("image/32.png");
	Text64.loadFromFile("image/64.png");
	Text128.loadFromFile("image/128.png");
	Text256.loadFromFile("image/256.png");
	Text512.loadFromFile("image/512.png");
	Text1024.loadFromFile("image/1024.png");
	Text2048.loadFromFile("image/2048.png");
	victory.loadFromFile("image/victory.png");
	defeat.loadFromFile("image/defeat.png");
	gBackgroundText.loadFromFile("image/background.png");
	ButtonDown.loadFromFile("image/buttondown.png");
	Score.loadFromFile("image/scorebutton.png");
	gFont = TTF_OpenFont("image/arial.ttf", 20);
	gMusic = Mix_LoadMUS("image/nhac.mp3");
	gwin = Mix_LoadWAV("image/Victorysound.mp3");
	glose = Mix_LoadWAV("image/Defeatsound.mp3");
	return success;
}

void close(){
	Text0.free();
	Text2.free();
	Text4.free();
	Text8.free();
	Text16.free();
	Text32.free();
	Text64.free();
	Text128.free();
	Text256.free();
	Text512.free();
	Text1024.free();
	Text2048.free();
	gBackgroundText.free();
	Button.free();
	ButtonDown.free();
	Score.free();
	Best.free();
	victory.free();
	defeat.free();

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	Mix_FreeMusic(gMusic);
	Mix_FreeChunk(gwin);
	Mix_FreeChunk(glose);
	TTF_CloseFont(gFont);

	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

void randomtile(int** board, int check){
	int count = 0;
	srand(time(NULL));
	int value;
	if (check == 0){
		value = (rand() % 4 + 1);
		if (value == 4) value = 4;
		else value = 2;
	} else value = 2;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (board[i][j] == 0) count++;

	int res = rand() % count;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (board[i][j] == 0) {
				if (res == 0) { board[i][j] = value; res = -1; }
				else res--;
			}
		}
	}
}

void renderTile(int value, int i, int j){
	switch (value){
        case 0: Text0.render(131 + 97 * i, 198 + 97 * j); break;
        case 2: Text2.render(131 + 97 * i, 198 + 97 * j); break;
        case 4: Text4.render(131 + 97 * i, 198 + 97 * j); break;
        case 8: Text8.render(131 + 97 * i, 198 + 97 * j); break;
        case 16: Text16.render(131 + 97 * i, 198 + 97 * j); break;
        case 32: Text32.render(131 + 97 * i, 198 + 97 * j); break;
        case 64: Text64.render(131 + 97 * i, 198 + 97 * j); break;
        case 128: Text128.render(131 + 97 * i, 198 + 97 * j); break;
        case 256: Text256.render(131 + 97 * i, 198 + 97 * j); break;
        case 512: Text512.render(131 + 97 * i, 198 + 97 * j); break;
        case 1024: Text1024.render(131 + 97 * i, 198 + 97 * j); break;
        case 2048: Text2048.render(131 + 97 * i, 198 + 97 * j); break;
	}
}

void push(std::vector <int>& umain,int &spm){
	for (int i = 0; i < 4; i++){
		for (int j = i + 1; j < 4; j++){
			if ((umain[i] == umain[j]) && (umain[i] != 0)){
				umain[i] = umain[i] * 2;
				spm += umain[i];
				umain[j] = 0;
				break;
			} else if (umain[j] != 0) break;
		}
	}
	for (int i = 0; i < 4; i++){
		if (umain[i] == 0){
			umain.erase(umain.begin() + i);
			int temp = 0;
			umain.push_back(temp);
		}
	}
	for (int i = 0; i < 4; i++){
		if (umain[i] == 0){
			umain.erase(umain.begin() + i);
			int temp = 0;
			umain.push_back(temp);
		}
	}
}

void moveright(int** board,int &spm){
	for (int i = 0; i < 4; i++){
		std::vector<int> umain;
		for (int j = 3; j >= 0; j--){
			int temp = board[i][j];
			umain.push_back(temp);
		}
		push(umain,spm);
		for (int j = 3; j >= 0; j--){
			board[i][j] = umain[0];
			umain.erase(umain.begin());
		}
	}
}

void moveup(int** board,int &spm){
	for (int j = 0; j < 4; j++){
		std::vector<int> umain;
		for (int i = 0; i < 4; i++){
			int temp = board[i][j];
			umain.push_back(temp);
		}
		push(umain,spm);
		for (int i = 0; i < 4; i++){
			board[i][j] = umain[0];
			umain.erase(umain.begin());
		}
	}
}

void moveleft(int** board,int &spm){
	for (int i = 0; i < 4; i++){
		std::vector<int> umain;
		for (int j = 0; j < 4; j++){
			int temp = board[i][j];
			umain.push_back(temp);
		}
		push(umain,spm);
		for (int j = 0; j < 4; j++){
			board[i][j] = umain[0];
			umain.erase(umain.begin());
		}
	}
}

void movedown(int** board,int &spm){
	for (int j = 0; j < 4; j++){
		std::vector<int> umain;
		for (int i = 3; i >= 0; i--){
			int temp = board[i][j];
			umain.push_back(temp);
		}
		push(umain,spm);
		for (int i = 3; i >= 0; i--){
            board[i][j] = umain[0];
			umain.erase(umain.begin());
		}
	}
}

bool checktheresemblance(int** board, int** thepreviousboard){
	bool check = true;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (board[i][j] != thepreviousboard[i][j]) check = false;
		}
	}
	return check;
}

void updateboard(int** board, int** thepreviousboard){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			thepreviousboard[i][j] = board[i][j];
		}
	}
}

void handleEvent(SDL_Event* e){
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN
     || e->type == SDL_MOUSEBUTTONUP){
		int x, y;
		SDL_GetMouseState(&x, &y);
		bool inside = true;
		//Mouse is left of the button
		if (x < 415){ inside = false; }
		//Mouse is right of the button
		else if (x > 415 + 106) { inside = false; }
		//Mouse above the button
		else if (y < 119) { inside = false; }
		//Mouse below the button
		else if (y > 119 + 34) { inside = false; }

		if (!inside){
			Button.render(415, 119);
		} else {
			switch (e->type){
                case SDL_MOUSEMOTION:
                    ButtonDown.render(415, 119);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    ButtonDown.render(415, 119);
                    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) board[i][j] = 0;
                    score = 0;
                    randomtile(board, 1);
                    randomtile(board, 1);
                    win = false;
                    lose = false;
                    Mix_ResumeMusic();
                    break;
                case SDL_MOUSEBUTTONUP:
                    Button.render(415, 119);
                    break;
			}
		}
	}
}

bool fulltile(int** a){
	bool check = true;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++)
			if (a[i][j] == 0)
				check = false;
	}
	return check;
}

void checkwin(bool &win, int** a){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (a[i][j] == 2048){
				win = true;
				break;
			}
		}
	}
}

void checklose(bool& lose, int** a){
	bool check = false;
	for (int i = 0; i < 4; i++){
		for (int j = 3; j > 0; j--){
			if (a[i][j] == a[i][j-1]){
				check = true;
				break;
			}
		}
	}

	for (int j = 0; j < 4; j++){
		for (int i = 0; i < 3; i++){
			if (a[i][j] == a[i+1][j]){
				check = true;
				break;
			}
		}
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 3; j++){
			if (a[i][j] == a[i][j + 1]){
				check = true;
				break;
			}
		}
	}
	for (int j = 0; j < 4; j++){
		for (int i = 3; i > 0; i--){
			if (a[i][j] == a[i-1][j]){
				check = true;
				break;
			}
		}
	}
	if (check == false){
		if (fulltile(board) == true)
			lose = true;
	}
}

int main(int argc, char* args[]){
    try {
        if (!init()){
            printf("Failed to initialize!\n");
        } else {
            if (!loadMedia()){
                printf("Failed to load media!\n");
            } else {
                bool quit = false;
                SDL_Event e;
                board = new int* [4];
                for (int i = 0; i < 4; i++) board[i] = new int[4];
                for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) board[i][j] = 0;
                int** thepreviousboard = new int* [4];
                for (int i = 0; i < 4; i++) thepreviousboard[i] = new int[4];
                for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) thepreviousboard[i][j] = 0;
                randomtile(board, 1);
                randomtile(board, 1);
                std::stringstream scoretext, bestscoretext;
                Mix_PlayMusic(gMusic, -1);
                while (!quit) {
                    while (SDL_PollEvent(&e) != 0){
                        score += spm;
                        spm = 0;
                        if (e.type == SDL_QUIT){
                            quit = true;
                        } else if (e.type == SDL_KEYDOWN && win == false && lose == false) {
                            switch (e.key.keysym.sym) {
                            case SDLK_UP:
                                moveleft(board, spm);
                                if (checktheresemblance(board, thepreviousboard) == false) randomtile(board, 0);
                                break;

                            case SDLK_DOWN:
                                moveright(board, spm);
                                if (checktheresemblance(board, thepreviousboard) == false) randomtile(board, 0);
                                break;

                            case SDLK_LEFT:
                                moveup(board, spm);
                                if (checktheresemblance(board, thepreviousboard) == false) randomtile(board, 0);
                                break;

                            case SDLK_RIGHT:
                                movedown(board, spm);
                                if (checktheresemblance(board, thepreviousboard) == false) randomtile(board, 0);
                                break;

                            case SDLK_m:
                                if (Mix_PlayingMusic() == 0) {
                                    Mix_PlayMusic(gMusic, -1);
                                }
                                else {
                                    if (Mix_PausedMusic() == 1) Mix_ResumeMusic();
                                    else Mix_PauseMusic();
                                }
                            }
                            updateboard(board, thepreviousboard);
                        }
                    }
                    scoretext.str("");
                    scoretext << score;

                    //clrscr
                    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderClear(gRenderer);
                    ScoreText.loadFromRenderedText(scoretext.str().c_str(), textColor);
                    BestText.loadFromRenderedText(bestscoretext.str().c_str(), textColor);
                    gBackgroundText.render(0, 0);
                    Button.render(415, 119);
                    handleEvent(&e);
                    Best.render(418, 33);
                    Score.render(313, 33);
                    if (score > best) best = score;
                    if (score < 10) ScoreText.render(358, 55);
                    if (score < 100 && score > 10) ScoreText.render(353, 55);
                    if (score < 1000 && score > 100) ScoreText.render(346, 55);
                    if (score < 10000 && score > 1000) ScoreText.render(340, 55);
                    if (score > 10000) ScoreText.render(335, 55);
                    if (best < 10) BestText.render(464, 55);
                    if (best < 100 && best > 10) BestText.render(459, 55);
                    if (best < 1000 && best > 100) BestText.render(452, 55);
                    if (best < 10000 && best > 1000) BestText.render(448, 55);
                    if (best > 10000) ScoreText.render(437, 55);
                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++){
                            int value = board[i][j];
                            renderTile(value, i, j);
                        }
                    }
                    checklose(lose, board);
                    checkwin(win, board);
                    if (lose == true){
                        defeat.render(120, 120);
                        Mix_PauseMusic();
                        Mix_PlayChannel(-1, glose, 0);
                    }

                    if (win == true){
                        victory.render(120, 120);
                        Mix_PauseMusic();
                        Mix_PlayChannel(-1, gwin, 0);
                    }
                    SDL_RenderPresent(gRenderer);
                }
            }
        }
	} catch (std::exception e) {
        printf ("Error: %s\n", SDL_GetError());
    }

	close();
	return 0;
}
/*
 I also thank
 +, Teacher Le Duc Trong, Tran Truong Thuy for teaching me
 +, Mentor Nguyen Tan Nguyen for supported
 +, My bro Nguyen Duc Thien, Do Van Nam Khanh for supported
 +, My friends Pham Tuan Viet, Le Kim Thanh and Luu Trung Truc for remind and help me
 completed this project
 +, Youtube, google, github, lazyfoo ... help me to develop this project
 +, And also thank anyone to help me and using this game.

 Captain Coder 4/2024
*/
