/*
 * SDL Version
 * http://lostsidedead.com
 * (c) 2013 Jared Bruni
 */
/* This source code is Licensed under GPL. ( http://www.gnu.org/licenses/gpl.html ) */
#include"SDL.h"
#include<iostream>
#include<string>
#include"quadtris.h"
#include<cstdlib>
#include<ctime>
#include"cmx.h"
#include"cmx_event.h"
#include<cassert>
#include<sstream>
#include<unistd.h>
#include"font.mxf.h"
#include<ctime>
#include<cstdlib>
#include<ctype.h>
#include"afhd.h"
#include"msocket.h"
#include"scores.h"

// app variables

bool ac_mode = false;
enum SCREEN { SCR_GAME, SCR_GAMEOVER, SCR_START, SCR_SUBMIT, SCR_INTRO };
SCREEN game_screen = SCR_START;
quad::Game game;
int screen_width = 1280, screen_height = 720;
unsigned int timer1_ID = 0xFF, timer2_ID = 0xFF;
cmx::video::Surface background_surface, start_surface, blocks[8], logos[4], border, pointer, jaredb_logo, lost_logo;
cmx::video::Surface *front;//front surface
cmx::font::Font the_font;
static std::string input_text = "";
cmx::Rect grid0(screen_width/4, 0, screen_width/2, screen_height/2);
cmx::Rect grid1(screen_width/4, screen_height/2, screen_width/2, screen_height/2);
cmx::Rect grid2(0, screen_height/4, screen_width/4, screen_height/2);
cmx::Rect grid3(screen_width-(screen_width/4), screen_height/4, screen_width/4, screen_height/2);
cmx::Rect background_bg(15, 35, 325, 45);
cmx::Rect screen_rect(0, 0, screen_width, screen_height);
unsigned int image_shift = 0;
bool connected = true;
bool image_fade=false;
int current_index = 0;
static std::string iptext = "69.36.175.161";
unsigned int port_num = 80;

// functions

void video_Fade();
void drawGame();
void drawStart();
void drawGameOver();
inline void drawRect(cmx::video::Surface *, cmx::Rect &rect, unsigned int color);
void drawGrid(quad::GameGrid &grid, cmx::video::Surface *surface, cmx::Rect &source_rect, int direction);
void sendScore(std::string userName, unsigned int score, unsigned int num_clr);
void FadeFrom(cmx::video::Surface *src1, cmx::video::Surface *src2, float transparent);
void drawIntro();

unsigned int timerUpdateCallback(unsigned int old_value, void *param) {
	game.timer_Update();
	return quad::Game::score.game_speed;
}

unsigned int blockProc(unsigned int old_value, void *param) {
	game.update();
	return old_value;
}

void switchPiece(unsigned int &i) {
	switch(i) {
	case quad::MOVE_UP:
		i = quad::MOVE_RIGHT;
		break;
	case quad::MOVE_DOWN:
		i = quad::MOVE_LEFT;
		break;
	case quad::MOVE_LEFT:
		i = quad::MOVE_UP;
		break;
	case quad::MOVE_RIGHT:
		i = quad::MOVE_DOWN;
		break;
	}
}

void setActiveGrid(unsigned int i) {
	switchPiece(i);
	if(game[0].isGameOver() == true && game[1].isGameOver() == true && game[2].isGameOver() == true && game[3].isGameOver() == true) {
		game_screen = SCR_GAMEOVER;
		cmx::event::stopTimer(timer1_ID);
		cmx::event::stopTimer(timer2_ID);
		return;
	}
	if(game[i].isGameOver()==true) {
		switchPiece(i);
	}
	game.setActiveGrid(i);
}

void changePiece() {
	unsigned int i = game.getActiveGridIndex();
	setActiveGrid(i);
}


void loadGraphic(cmx::video::Surface *surf, std::string text) {
	if(surf->loadImage(text) != 1) {
		std::cerr << "Error could not load image: " << text << "\n";
		SDL_Quit();
		exit(0);
	} else {
		std::cout << "Image: " << text << " loaded.\n";
	}
}

void initGraphics() {
	loadGraphic(&background_surface, "img/background.jpg");
	loadGraphic(&start_surface, "img/start.jpg");
	for (unsigned int i = 1; i <= 8; ++i) {
		std::ostringstream stream;
		stream << "img/" << "block" << i << ".jpg";
		if (blocks[i - 1].loadImage(stream.str()) == 1) {
			std::cout << stream.str() << " successfully loaded.\n";
		} else {
			std::cout << "failed to load block graphics.\n";
			exit(0);
		}
	}
	loadGraphic(&logos[0], "img/newgame.jpg");
	loadGraphic(&logos[1], "img/aboutgame.jpg");
	loadGraphic(&logos[2], "img/exit.jpg");
	loadGraphic(&border, "img/boxborder.jpg");
	loadGraphic(&pointer, "img/cd.jpg");
	loadGraphic(&jaredb_logo, "img/jaredblogo.jpg");
	loadGraphic(&lost_logo, "img/lostlogo.jpg");
	SDL_Surface *bmp = SDL_LoadBMP("img/block4.bmp");
	if(bmp == 0) {
		std::cout << "Could not load icon..";
		return;
	}
	SDL_WM_SetIcon(bmp, 0);
	SDL_WM_SetCaption("Quadtris - LostSideDead.com", 0);
}

void startGame() {
	image_fade = true;
}

void keyDown(cmx::event::Event &e) {
	quad::GameGrid &grid = game.getActiveGrid();
	switch (e.skey) {
	case cmx::event::K_DOWN:
		switch (grid.getDirection()) {
		case quad::MOVE_UP:
			grid.movePos(quad::MOVE_UP);
			break;
		case quad::MOVE_DOWN:
			grid.movePos(quad::MOVE_DOWN);
			break;
		case quad::MOVE_LEFT:
			grid.movePos(quad::MOVE_RIGHT);
			break;
		case quad::MOVE_RIGHT:
			grid.movePos(quad::MOVE_RIGHT);
			break;
		default:
			break;
		}

		break;
	case cmx::event::K_UP:
		switch (grid.getDirection()) {
		case quad::MOVE_UP:
			grid.movePos(quad::MOVE_DOWN);
			break;
		case quad::MOVE_DOWN:
			grid.movePos(quad::MOVE_UP);
			break;
		case quad::MOVE_LEFT:
			grid.movePos(quad::MOVE_LEFT);
			break;
		case quad::MOVE_RIGHT:
			grid.movePos(quad::MOVE_LEFT);
			break;
		default:
			break;
		}
		break;
	case cmx::event::K_LEFT:
		switch (grid.getDirection()) {
		case quad::MOVE_UP:
			grid.movePos(quad::MOVE_LEFT);
			break;
		case quad::MOVE_DOWN:
			grid.movePos(quad::MOVE_LEFT);
			break;
		case quad::MOVE_LEFT:
			grid.movePos(quad::MOVE_DOWN);
			break;
		case quad::MOVE_RIGHT:
			grid.movePos(quad::MOVE_UP);
			break;
		default:
			break;
		}
		break;
	case cmx::event::K_RIGHT:
		switch (grid.getDirection()) {
		case quad::MOVE_UP:
			grid.movePos(quad::MOVE_RIGHT);
			break;
		case quad::MOVE_DOWN:
			grid.movePos(quad::MOVE_RIGHT);
			break;
		case quad::MOVE_LEFT:
			grid.movePos(quad::MOVE_UP);
			break;
		case quad::MOVE_RIGHT:
			grid.movePos(quad::MOVE_DOWN);
			break;
		default:
			break;
		}
		break;
	case cmx::event::K_RETURN: {
		grid.movePos(quad::MOVE_BLOCKSWITCH);
	}
		break;
	}
}

void changeTempPath(int argc, char **argv) {
	std::string path = argv[0];
	std::string temp_path = path.substr(0, path.rfind("/"));
	temp_path += "/../Resources";
	std::cout << temp_path << "\n";
	chdir(temp_path.c_str());
}

int main(int argc, char **argv) {
	std::cout << "Quadtris - SDL Version\n";
	changeTempPath(argc, argv);
	srand((unsigned int) time(0));
#ifdef WIN
	WSAData dat;
	if(WSAStartup(MAKEWORD(2,2), &dat) != 0) {
		std::cerr << "Error could not initalize sockets..\n";
		connected = false;
	}
#endif
	assert(cmx::system::init_system() == 1);
	int opt;
	bool full = false;
	while ((opt = getopt(argc, argv, "f")) != -1) {
		switch(opt) {
		case 'f':
			full = true;
			break;
		}
	}
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	front = cmx::system::init_surface(screen_width, screen_height);
	if(the_font.loadFont((const void*)font_mxf, sizeof(font_mxf)) == false) {
		std::cerr << "Could not load font\n";
		SDL_Quit();
		exit(0);
	}
	if(full == true) {
		cmx::system::toggle_fullscreen();
	}
	initGraphics();
	bool active = true;
	game_screen = SCR_INTRO;
	while(active) {
		static cmx::event::Event e;
		while(e.procEvent()) {
			switch(e.type) {
			case cmx::event::EVENT_QUIT:
				active = false;
				return cmx::system::stop_system();
				break;
			case cmx::event::EVENT_KEYUP:
			{
				if(e.key == 'l') ac_mode = !ac_mode;
			}
			break;
			case cmx::event::EVENT_KEYDOWN: {
				//_keys[e.key] = 1;
				if(game_screen != SCR_GAME && e.skey == cmx::event::K_ESCAPE) {
					active = false;
					return cmx::system::stop_system();
					break;
				} else if(game_screen == SCR_GAME && e.skey == cmx::event::K_ESCAPE) {
					game_screen = SCR_START;
					cmx::event::stopTimer(timer1_ID);
					cmx::event::stopTimer(timer2_ID);
					continue;
				}
				if(game_screen == SCR_START) {
					if(e.skey == cmx::event::K_RETURN) {
						switch(current_index) {
						case 0:
							startGame();
							break;
						case 1:
							game_screen = SCR_INTRO;
							break;
						case 2:
							std::cout << "Quit!\n";
							active = false;
							return cmx::system::stop_system();
							break;
						}
					}
					else if(e.skey == cmx::event::K_UP) {
						if(current_index > 0) --current_index;
					}
					else if(e.skey == cmx::event::K_DOWN) {
						if(current_index < 2) ++current_index;
					}
				}
				else if(game_screen == SCR_SUBMIT) {
					if(e.skey == cmx::event::K_RETURN) {
						sendScore(input_text, game.score.score, game.score.num_clr);
						input_text = "";
						game_screen = SCR_START;
					} else if(e.skey == 8) {
						if(input_text.length()>0) {
							input_text = input_text.substr(0, input_text.length()-1);
						}
					} else {
						if(isalpha(e.key) && input_text.length() < 20) input_text += e.key;
					}
				}
				else if(game_screen == SCR_GAMEOVER) {
					if(e.skey == cmx::event::K_RETURN) { game_screen = SCR_START; }
					if(e.skey == cmx::event::K_SPACE && connected == true) { if(game.score.score > 5) game_screen = SCR_SUBMIT; }
				}
				else if(game_screen == SCR_GAME) {
					keyDown(e);
				}
			}
			break;
			default:
				break;
			}
		}
		if(cmx::input::poll_joystick(0, cmx::input::BUTTON_CROSS)) {
			active = false;
		}
		switch(game_screen) {
		case SCR_SUBMIT: {
			static unsigned int sw_off = 0;
			static cmx::Rect screen_size(0, 0, screen_width, screen_height);
			drawRect(front,screen_size, 0x0);
			std::ostringstream stream;
			stream << "Enter your Name on the keyboard now and press Enter\n";
			the_font.printString((unsigned int *)front->buffer, front->w, 20, 45, 14, 10, _RGB(255,255,255), stream.str().c_str());
			stream.str("");
			stream << input_text << ((sw_off == 1) ? "_" : " ");
			the_font.printString((unsigned int *)front->buffer, front->w, 50, 70, 14, 10, _RGB(255, 0, 0), stream.str().c_str());
			sw_off = !sw_off;
		}
			break;
		case SCR_GAME: {
			std::ostringstream score;
			drawRect(front,background_bg, 0x0);
			score << "Level: " << quad::Game::score.level << " Score: " << quad::Game::score.score << " Cleared: " << quad::Game::score.num_clr;
			the_font.printString((unsigned int *)front->buffer, front->w, 20, 45, 14, 10, _RGB(255,255,255), score.str().c_str());
			static unsigned int backcolor = 0x0;
			drawRect(front, grid0, backcolor);
			drawRect(front, grid1, backcolor);
			drawRect(front, grid2, backcolor);
			drawRect(front, grid3, backcolor);
			drawGrid(game[0],front,grid0, 0);// 0 - Up

			drawGrid(game[1],front,grid1, 1);// 1 - down

			drawGrid(game[2],front,grid2, 2);// 2 - left

			drawGrid(game[3],front,grid3, 3);// 3 - right

		}
		break;
		case SCR_START:
			drawStart();
			break;
		case SCR_GAMEOVER:
			drawRect(front, screen_rect, _RGB(0, 0, 0));
			drawGameOver();
			break;
		case SCR_INTRO:
			drawIntro();
			break;
		}
		cmx::system::copy_surface(*front);
	}

	cmx::event::stopTimer(timer1_ID);
	cmx::event::stopTimer(timer2_ID);
#ifdef WIN
	WSACleanup();
#endif
	return cmx::system::stop_system();
}

inline void drawRect(cmx::video::Surface *surface,cmx::Rect &rect, unsigned int color) {
	unsigned int *buffer = (unsigned int *)surface->buffer;
	for(int i = rect.y; i < rect.y+rect.h; ++i) {
		for(int j = rect.x; j < rect.x+rect.w; ++j) {
			buffer[j+i*surface->w] = color;
		}
	}
}

void drawStart() {
	cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, start_surface, cmx::Rect(0, 0, start_surface.w, start_surface.h), cmx::Rect(0, 0, start_surface.w, start_surface.h));
	static cmx::Rect box(250, 190, 450, 220);
	static unsigned int whiteColor = _RGB(255,255,255);
	static unsigned int redColor = _RGB(255, 0, 0);
	static unsigned int blueColor = _RGB(0, 0, 255);
	if(image_fade == true) {
		static float fade_f = 0.0f;
		unsigned int *front_buffer = (unsigned int *)front->buffer;
		unsigned int *buffer = (unsigned int *)start_surface.buffer;
		unsigned int *bg_buffer = (unsigned int *)background_surface.buffer;
		for(int z = 0; z < start_surface.h; ++z) {
			for(int i = 0; i < start_surface.w-1; ++i) {
				cmx::Color color1, color2, color3;
				color1.color.color_value = buffer[i+z*start_surface.w];
				color2.color.color_value = bg_buffer[i+z*background_surface.w];
				color3.color.color_value = buffer[(i+1)+z*start_surface.w];
				for(int q = 0; q < 3; ++q) {
					color1.color.color[q] += color2.color.color[q] * fade_f;
					color1.color.color[q] += (color3.color.color[q]/2) * fade_f;
				}
				front_buffer[i+z*front->w] = color1.color.color_value;
			}
		}
		fade_f += 0.01f;
		if(fade_f >= 2.0) {
			fade_f = 0.0f;
			image_fade = false;
			game_screen = SCR_GAME;
			game.newGame((screen_width/2)/16, (screen_height/2)/16, (grid3.w/16), (grid3.h/16));
			game.setCallback(changePiece);
			timer1_ID = cmx::event::createTimer(quad::Game::score.game_speed, timerUpdateCallback);
			timer2_ID = cmx::event::createTimer(25, blockProc);
			game.setActiveGrid(1);
		}
	}
	else {
		video_Fade();
		cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, border, cmx::Rect(0, 0, border.w, border.h), cmx::Rect(box.x, box.y, border.w, border.h));
		the_font.printString((unsigned int *)front->buffer, front->w, box.x+45, box.y+25, 25, 25, 0x0, "LostSideDead Production");
		the_font.printString((unsigned int *)front->buffer, front->w, box.x+45, box.y+65, 25, 25, whiteColor, "Start New Game");
		the_font.printString((unsigned int *)front->buffer, front->w, box.x+45, box.y+105, 25, 25, redColor, "About");
		the_font.printString((unsigned int *)front->buffer, front->w, box.x+45, box.y+145, 25, 25, blueColor, "Exit");
		the_font.printString((unsigned int *)front->buffer, front->w, box.x+45, box.y+185, 25, 25, _RGB(rand()%255, rand()%255, rand()%255),"www.LostSideDead.com");
		unsigned int c_i = (box.y+62)+(current_index*40);
		cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, pointer, cmx::Rect(0, 0, pointer.w, pointer.h), cmx::Rect(box.x+10, c_i, pointer.w, pointer.h), true);
		cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, logos[current_index], cmx::Rect(0, 0,logos[current_index].w, logos[current_index].h), cmx::Rect(box.x+box.w+25, 140, logos[current_index].w, logos[current_index].h));
	}
}

void drawGame() {


}

void FadeFrom(cmx::video::Surface *src1, cmx::video::Surface *src2, float transparent) {
	unsigned int *src_p1 = (unsigned int *)src1->buffer;
	unsigned int *src_p2 = (unsigned int *)src2->buffer;
	unsigned int *video = (unsigned int *)front->buffer;
	for(int ch = 0; ch < front->h; ++ch) {
		for(int cw = 0; cw < front->w; ++cw) {
			cmx::Color col[3];
			col[0].color.color_value = src_p1[cw+ch*src1->w];
			col[1].color.color_value = src_p2[cw+ch*src2->w];
			for(int q = 0; q < 4; ++q)
			col[2].color.color[q] = col[0].color.color[q]+(transparent*col[1].color.color[q]);
			video[cw+ch*front->w] = col[2].color.color_value;
		}
	}
}

void drawIntro() {
	static unsigned int times = 0;
	cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, lost_logo, cmx::Rect(0, 0, lost_logo.w, lost_logo.h), cmx::Rect(0, 0, lost_logo.w, lost_logo.h));
	static int frame_index = 0;
	switch(frame_index) {
	case 0:
		cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, lost_logo, cmx::Rect(0, 0, lost_logo.w, lost_logo.h), cmx::Rect(0, 0, lost_logo.w, lost_logo.h));
	break;
	case 1:
		cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, jaredb_logo, cmx::Rect(0, 0, jaredb_logo.w, jaredb_logo.h), cmx::Rect(0, 0, jaredb_logo.w, jaredb_logo.h));
	break;
	case 2:
		game_screen = SCR_START;
		frame_index = 0;
		times = 0;
		cmx::Rect r(0, 0, front->w, front->h);
		drawRect(front,r, 0x0);
		break;
	}
	if(times == 0) times = SDL_GetTicks();
	unsigned int tick = SDL_GetTicks();
	if(tick-times > 3000) {
		times = tick;
		++frame_index;
	}
}

void video_Fade() {
	unsigned int *video_buffer = (unsigned int *)front->buffer;
	static float alpha = 1.4f;
	for(int cy = 0; cy < front->h; ++cy) {
		for(int cx = 0; cx < front->w; ++cx) {
			cmx::Color col1 = video_buffer[cx+cy*front->w];
			for(unsigned int ci = 0; ci < 3; ++ci) {
				col1[ci] *= alpha;
			}
			video_buffer[cx+cy*front->w] = col1.color.color_value;
		}
	}
	static bool direction = true;
	if(direction == true) {
		alpha += 0.1f;
		if(alpha > 5.0f) direction = false;
	} else {
		alpha -= 0.1f;
		if(alpha < 1.4f) direction = true;
	}
	alpha += 0.01f;
	if(alpha > 10.0) alpha = 1.4f;
}

void drawGameOver() {
	static cmx::Rect box(100, 100, 650, 170);
	static unsigned int whiteColor = _RGB(255,255,255);
	cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, start_surface, cmx::Rect(0, 0, start_surface.w, start_surface.h), cmx::Rect(0, 0, start_surface.w, start_surface.h));
	video_Fade();
	drawRect(front, box, 0x0);
	std::ostringstream str_stream;
	str_stream << "Your score: " << game.score.score << " Lines cleared: " << game.score.num_clr;
	if(game.score.num_clr < 6) {
		str_stream << " \nTry again, youll figure it out";
	}
	else if(game.score.num_clr > 6) {
		str_stream << " \nGood job";
	} else if(game.score.num_clr > 6 && game.score.num_clr < 9) {
		str_stream << "\n Excellent ";
	} else if(game.score.num_clr > 9 && game.score.num_clr < 20) {
		str_stream << "\n Cool you did good.";
	} else if(game.score.num_clr > 20) {
		str_stream << "\n You got this figured out.";
	}
	str_stream << "\nPress enter to return";
	if(game.score.score > 5 && connected == true)
		str_stream << "\nPress space to submit your score\n(" << game.score.score << ") to LostSideDead.com ";

	the_font.printString((unsigned int *)front->buffer, front->w, 115, 115, 35, 35, whiteColor, str_stream.str().c_str());
}

void drawBlock(quad::GamePiece &p,cmx::Rect source_rect,cmx::video::Surface *surface, unsigned int x, unsigned int y, int direction) {
	switch(direction) {
	case 0: {
		for(unsigned int i = 0; i < 4; ++i) {
			unsigned int block_x = p.x+p.blocks[i].x;
			unsigned int block_y = p.y+p.blocks[i].y;
			cmx::Rect rct(x+(block_x*16), source_rect.h-(y+(block_y*16)), 16, 16);
			if(p.blocks[i].color>0)
				cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[p.blocks[i].color-1], cmx::Rect(0, 0, 16, 16), rct);
		}
	}
	break;
	case 1: {
		for(unsigned int i = 0; i < 4; ++i) {
			unsigned int block_x = p.x+p.blocks[i].x;
			unsigned int block_y = p.y+p.blocks[i].y;
			cmx::Rect rct(x+(block_x*16), (y+(block_y*16)), 16, 16);
			if(p.blocks[i].color>0)
				cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[p.blocks[i].color-1], cmx::Rect(0, 0, 16, 16), rct);
		}
	}
	break;
	case 2: {
		for(unsigned int i = 0; i < 4; ++i) {
			unsigned int pos_x = source_rect.x+source_rect.w+16;
			unsigned int block_x = p.x+p.blocks[i].x, block_y = p.y+p.blocks[i].y;
			cmx::Rect rct(pos_x-(block_y*16), source_rect.y+(block_x*16), 16, 16);
			if(p.blocks[i].color>0)
				cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[p.blocks[i].color-1], cmx::Rect(0, 0, 16, 16), rct);
		}
	}
	break;
	case 3: {
		for(unsigned int i = 0; i < 4; ++i) {
			unsigned int pos_x = source_rect.x-32;
			unsigned int block_x = p.x+p.blocks[i].x, block_y = p.y+p.blocks[i].y;
			cmx::Rect rct(pos_x+(block_y*16), source_rect.y+(block_x*16), 16, 16);
			if(p.blocks[i].color>0)
				cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[p.blocks[i].color-1], cmx::Rect(0, 0, 16, 16), rct);
		}

	}
	break;
	}
}

void drawGrid(quad::GameGrid &grid, cmx::video::Surface *surface, cmx::Rect &source_rect, int direction) {
	unsigned int x = 0, y = 0;
	unsigned int source_x = 0, source_y = 0;
	int grid_index = game.getActiveGridIndex();
	switch(direction) {
	case 0: // UP

	{
		int pos_x = source_rect.x;
		int pos_y = source_rect.y+source_rect.h;
		for(x = 0; x < grid.grid_width(); ++x) {
			for(y = 0; y < grid.grid_height(); ++y) {
				cmx::Rect pos_rect(pos_x, pos_y, 16, 16);
				unsigned int color = grid.game_grid[x][y].getColor();
				if(color != 0 && color != 0xFE && grid.isGameOver() == false)
					cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[color-1], cmx::Rect(0, 0, 16, 16), pos_rect);
				else if(color == 0xFE || (color != 0 && grid.isGameOver() == true)) drawRect(surface, pos_rect, _RGB(rand()%255, rand()%255, rand()%255));
				pos_y -= 16;
			}
			pos_y = source_rect.y+source_rect.h;
			pos_x += 16;
		}
	}
	break;
	case 1: // Down

	{
		unsigned int pos_x = source_rect.x;
		unsigned int pos_y = source_rect.y;
		for(x = 0; x < grid.grid_width(); ++x) {
			for(y = 0; y < grid.grid_height(); ++y) {
				// draw
				cmx::Rect pos_rect(pos_x, pos_y, 16, 16);
				unsigned int color = grid.game_grid[x][y].getColor();
				if(color != 0 && color != 0xFE && grid.isGameOver() == false)
					cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[color-1], cmx::Rect(0, 0, 16, 16), pos_rect);

				else if(color == 0xFE || (color != 0 && grid.isGameOver() == true)) drawRect(surface, pos_rect, _RGB(rand()%255, rand()%255, rand()%255));
				pos_y += 16;
			}
			pos_y = source_rect.y;
			pos_x += 16;
		}
	}
	break;
	case 2: // Left

	{
		unsigned int pos_x = source_rect.x+source_rect.w+16;
		for(x = 0; x < grid.grid_width(); ++x) {
			for(y = 0; y < grid.grid_height(); ++y) {
				cmx::Rect posRect(pos_x-(y*16), source_rect.y+(x*16), 16, 16);
				unsigned int color = grid.game_grid[x][y].getColor();
				if(color != 0 && color != 0xFE && grid.isGameOver() == false)
					cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[color-1], cmx::Rect(0, 0, 16, 16), posRect);

				else if(color == 0xFE || (color != 0 && grid.isGameOver() == true)) drawRect(surface, posRect, _RGB(rand()%255, rand()%255, rand()%255));
			}
		}
	}
	break;
	case 3: // Right

	{
		unsigned int pos_x = source_rect.x-32;
		for(x = 0; x < grid.grid_width(); ++x) {
			for(y = 0; y < grid.grid_height(); ++y) {
				cmx::Rect posRect(pos_x+(y*16), source_rect.y+(x*16), 16, 16);
				unsigned int color = grid.game_grid[x][y].getColor();
				if(color != 0 && color != 0xFE && grid.isGameOver() == false)
					cmx::video::copySurfaceToBuffer((unsigned int *)front->buffer, front->w, front->h, blocks[color-1], cmx::Rect(0, 0, 16, 16), posRect);
				else if(color == 0xFE || (color != 0 && grid.isGameOver() == true)) drawRect(surface, posRect, _RGB(rand()%255, rand()%255, rand()%255));
			}
		}

	}
	break;
	}
	if(direction == grid_index) {
		source_x += source_rect.x;
		source_y += source_rect.y;
		drawBlock(grid.piece,source_rect,surface,source_x, source_y,direction);
	}
}

void transmitData(const Score &s) {
	TempSocketObject tsocket;
	if(tsocket.initConnection(iptext, port_num) == false) {
		std::cerr << "Error connection could not be made\n";
		return;
	}
	std::ostringstream stream, buffer_str;
	unsigned int content_length = 0;
	buffer_str << "user_name=" << URL_encode(s.username) << "&score=" << s.score;
	content_length = buffer_str.str().size();
	stream
			<< "POST /high_score.php HTTP/1.0\nFrom: user@lostsidedead.com\nUser-Agent: MUTSCORE/1.0\nContent-Type: application/x-www-form-urlencoded\nContent-Length: "
			<< content_length << "\n\n" << buffer_str.str();
	std::cout << "Sending ... " << stream.str() << std::endl;
	tsocket.SendString(stream.str());
	std::string total;
	size_t offset = 0;
	while (1) {
		static char buf[1024];
		offset = tsocket.Read(buf, 1023);
		if (offset == 0)
			break;
		buf[offset] = 0;
		total += buf;
	}
	std::cout << "Response: ...: " << total << "\n";
	tsocket.Close();
}


void sendScore(std::string userName, unsigned int score, unsigned int num_clr) {
	if(connected == false) return;
	std::cout << "Score for " << userName << " sending..." << "\n";
	Score s(userName.c_str(), score);
	transmitData(s);
	//SDL_CreateThread(backgroundFunc, 0x0);
}


