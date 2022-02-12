#define WDTH 10
#define HGHT 20
#define MAXPTS 5
#define NPIECES 27
#define MAXLVL 15
#define LVLGOAL 5

typedef struct Point Point;
struct Point{
	int x, y;
};

typedef struct Piece Piece;
struct Piece{
	short rot;
	short color;
	Point pts[MAXPTS];
};

typedef struct PolytrisGame PolytrisGame;
struct PolytrisGame{
	long score;
	int lines, level, goal;
	int time_until_fall, time_until_lock;
	short board[HGHT][WDTH];
	Point pos;
	Piece *piece, *piece_held, *piece_next;
};

extern Piece pieces[];

void mleft(PolytrisGame *pg);
void mright(PolytrisGame *pg);
void mdown(PolytrisGame *pg);
void rleft(PolytrisGame *pg);
void rright(PolytrisGame *pg);
void hold(PolytrisGame *pg);
void drop(PolytrisGame *pg);
void polytris_destroy(PolytrisGame *pg);
int polytris_tick(PolytrisGame *pg, int dt);
Point mkpt(int x, int y);
Point ghost_pos(PolytrisGame *pg);
Piece *new_piece(void);
PolytrisGame *polytris_create(long seed);
int collide(Point pt, Piece *p, short board[HGHT][WDTH]);

