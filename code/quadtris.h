/*
 *
 *
 * http://lostsidedead.com
 * (c) 2012 Jared Bruni GPL
 */
/* This source code is Licensed under GPL. ( http://www.gnu.org/licenses/gpl.html ) */

#ifndef QUADTRIS_H_
#define QUADTRIS_H_
#include<iostream>
#include<iomanip>
#include<string>
#include<cstdlib>

	namespace quad {

		class GameGrid;
		class Game;
		class GamePiece;

		class Block {
		public:
			unsigned int x,y,color,timeout;

			Block();
			Block(const Block &b);
			void operator=(const Block &b);
			bool operator==(const Block &b);
			friend std::ostream &operator<<(std::ostream &out, const Block &b);
			void clear();
			inline void setColor(const unsigned int color) { this->color = color; }
			unsigned int getColor() { return color; }
			inline void setPosition( const unsigned int x, const unsigned int y) { this->x = x; this->y = y; }
			inline void getPosition(unsigned int &x, unsigned int &y) { x = this->x; y = this->y; }
			void Swap(Block &b);
		};

		enum block_t { BLOCK_NULL=0, BLOCK_VERT, BLOCK_HORIZ, BLOCK_SQUARE };

		class GamePiece {
		public:
			Block blocks[4];
			unsigned int x,y;
			enum BlockIndex { BLOCK_0=0, BLOCK_1, BLOCK_2, BLOCK_3 };
			GamePiece();
			GamePiece(const GamePiece &p);
			void randBlock();
			void copyBlock(const GamePiece *p);
			void operator=(const GamePiece &p);
			friend std::ostream &operator<<(std::ostream &out, GamePiece &p);
			void moveLeft();
			void moveRight();
			void moveDown();
			void showPiece(bool t);
			inline bool isPieceHidden() const { return visible; }
			// assumes color index is correct

			inline Block &colorAt(const BlockIndex i) { return blocks[static_cast<unsigned int>(i)]; }
			void swapColorsUp();
			void swapColorsDown();
			void setBlockType(block_t type);

			bool block_lock;
		protected:
			friend class GameGrid;
			bool visible;
			block_t type;
		};

		enum Direction { MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_BLOCKSWITCH, MOVE_NULL };

		class GameGrid {
			void (*callback_x)();
		public:
			GamePiece piece;
			GameGrid();
			GameGrid(const unsigned int width, const unsigned int height);
			~GameGrid();
			void resetGrid(const unsigned int width, const unsigned int height);
			void eraseGrid();
			void fillGrid(const unsigned int color);
			void setDirection(const enum Direction d);
			inline Direction getDirection() { return direction; }
			void mergeBlocks();
			void mergeDown();
			void procBlocks();
			void update();
			void update_moveDown();
			void erase();
			void releaseBlock();
			bool testBlockPos(unsigned int x, unsigned int y);
			void setCallback(void (*callback)()) { callback_x = callback; }
			void setBlock(Block &b, unsigned int x, unsigned int y);
			void movePos(Direction dir);
			unsigned int grid_width() const { return width; }
			unsigned int grid_height() const { return height; }
			friend std::ostream &operator<<(std::ostream &out, const GameGrid &g);
			friend class Game;
			bool isGameOver() const { return gameOver; }

			Block **game_grid;
		protected:
			unsigned int width, height, position;
			enum Direction direction;
			bool gameOver;
			bool switch_block;
		};

		enum { DEFAULT_WIDTH=25, DEFAULT_HEIGHT=25 };

		struct Score {
			unsigned int score;
			unsigned int num_clr;
			unsigned int total_moves;
			unsigned int game_speed;
			unsigned int level;
			unsigned int BLOCK_COUNT;
			Score() { BLOCK_COUNT=5; score = num_clr = total_moves = level = game_speed = 0; }
			inline void clear() { score = num_clr = total_moves = 0; level = 1; setTimerInterval(1000); }
			void addClear(int total) { score += total; num_clr += 1; ++total_moves; if((num_clr%6)==0) increaseSpeed();  }
			int getTimerInterval() const { return game_speed; }
			void setTimerInterval(const int t) {
				game_speed = t;
				// set the timer
			}
			void increaseSpeed() {
				++level;
				if(game_speed > 100) game_speed -= 100;
				setTimerInterval(game_speed);
				if((level%2==0) && BLOCK_COUNT<8) ++BLOCK_COUNT;
				std::cout << "Speed increase.\n";
			}
		};

		std::ostream &operator<<(std::ostream &out, const Score &s);

		class Game {
		public:
			enum { GRID_0=0, GRID_1, GRID_2, GRID_3 };
			Game();
			void newGame(unsigned int width_, unsigned int height_, unsigned int sw, unsigned int sh);
			inline GameGrid &operator[](unsigned int grid) { return grids[grid]; }
			inline GameGrid &getActiveGrid() { return grids[current]; }
			inline GamePiece &getActiveBlock() { return grids[current].piece; }
			void update();
			void setActiveGrid(const unsigned int cur);
			unsigned int getActiveGridIndex() const;
			bool changeActiveGrid() const;
			void procBlocks();
			friend std::ostream &operator<<(std::ostream &out, const Game &g);
			void timer_Update();
			void setCallback( void (*callback)()) {
				for(unsigned int i = 0; i < 4; ++i) {
					grids[i].setCallback(callback);
				}
			}
		protected:
			GameGrid grids[4];
			unsigned int current;
		public:
			static Score score;
		};
	}

#endif


