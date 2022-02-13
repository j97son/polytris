#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "polytris.h"

#define BEGINX 3
#define BEGINY 0
#define DELAY 500
#define NELEM(a) (sizeof(a)/sizeof(a[0]))

const int levels[MAXLVL] = {
	1000,793,618,473,355,262,190,135,94,64,43,28,18,11,7
};

void mleft(PolytrisGame *pg);
void mright(PolytrisGame *pg);
void mdown(PolytrisGame *pg);
void rleft(PolytrisGame *pg);
void rright(PolytrisGame *pg);
void hold(PolytrisGame *pg);
int drop(PolytrisGame *pg);
void polytris_destroy(PolytrisGame *pg);
int polytris_tick(PolytrisGame *pg, int dt);
Point mkpt(int x, int y);
Point ghost_pos(PolytrisGame *pg);
Piece *new_piece(void);
PolytrisGame *polytris_create(long seed);
static int collide(Point pt, Piece *p, short board[HGHT][WDTH]);
static Piece *rotr(Piece *p);
static Piece *rotl(Piece *p);
static void polytris_init(PolytrisGame *pg);
static void lockdown(Point pt, Piece *p, short board[HGHT][WDTH]);
static void rotate(PolytrisGame *pg, int left);
static void remove_lines(PolytrisGame *pg);
static int full_line(int y, short board[HGHT][WDTH]);
static void shift_lines(int y, short board[HGHT][WDTH]);

/* full_line: returns true if y is a full line */
static int
full_line(int y, short board[HGHT][WDTH])
{
	int i;
	for(i=0; i<WDTH; i++)
		if(board[y][i] == 0)
			return 0;
	return 1;
}

/* shift_lines: modifies board; shifts all lines above y down by one */
static void
shift_lines(int y, short board[HGHT][WDTH])
{
	int i;
	for(y--; y>=0; y--)
		for(i=0; i<WDTH; i++)
			board[y+1][i] = board[y][i];
}

/* remove_lines: remove full lines from board */
static void
remove_lines(PolytrisGame *pg)
{
	int i, l;
	static int points[] = {0,50,100,300,1200,6000};
	l = 0;
	for(i=HGHT-1; i>=0; i--)
		if(full_line(i, pg->board)){
			shift_lines(i, pg->board);
			i++; l++;
		}
	pg->score += (pg->level+1)*points[l];
	pg->lines += l;	pg->goal -= l;
	if(pg->goal <= 0){
		pg->level++;
		pg->goal += LVLGOAL;
	}
}

/* ghost_pos: return the position of the ghost piece */
Point
ghost_pos(PolytrisGame *pg)
{
	Point pt;
	pt.x = pg->pos.x;
	pt.y = pg->pos.y;
	while(!collide(mkpt(pt.x, ++pt.y), pg->piece, pg->board));
	pt.y--;
	return pt;
}

/* new_piece: returns a pointer to a new piece; randomly at the moment */
Piece *
new_piece(void)
{
	return &pieces[rand() % NPIECES * 4];
}

/* mleft/mright/rleft/rright: simple functions that do what they say */
void
mleft(PolytrisGame *pg)
{
	if(!collide(mkpt(pg->pos.x-1, pg->pos.y), pg->piece, pg->board))
		pg->pos.x--;
}

void
mright(PolytrisGame *pg)
{
	if(!collide(mkpt(pg->pos.x+1, pg->pos.y), pg->piece, pg->board))
		pg->pos.x++;
}

void
rleft(PolytrisGame *pg)
{
	rotate(pg, 1);
}

void
rright(PolytrisGame *pg)
{
	rotate(pg, 0);
}

/* mdown: move the piece down one line reset timers */
void
mdown(PolytrisGame *pg)
{
	if(!collide(mkpt(pg->pos.x, pg->pos.y+1), pg->piece, pg->board)){
		pg->pos.y++; pg->score++;
	}
	pg->time_until_fall = levels[pg->level % MAXLVL];
}

/* hold: swaps piece and held_piece */
void
hold(PolytrisGame *pg)
{
	//if can hold
	if(pg->piece_held != NULL){
		Piece *tmp;
		tmp = pg->piece_held;
		pg->piece_held = pg->piece;
		pg->piece = tmp;
	}else{
		pg->piece_held = pg->piece;
		pg->piece = pg->piece_next;
		pg->piece_next = new_piece();
	}
	pg->piece_held -= pg->piece_held->rot;
	pg->pos.x = BEGINX; pg->pos.y = BEGINY;
	pg->time_until_fall = pg->time_until_lock = DELAY;
}

/* drop: drops the piece down, locks it into place, and checks for gameover */
int
drop(PolytrisGame *pg)
{
	//move down until we hit the floor or a block
	while(!collide(mkpt(pg->pos.x, ++pg->pos.y), pg->piece, pg->board))
		pg->score += 2;
	//unhit that
	pg->pos.y--;
	lockdown(pg->pos, pg->piece, pg->board);
	remove_lines(pg);
	pg->pos.x = BEGINX; pg->pos.y = BEGINY;
	pg->time_until_fall = pg->time_until_lock = DELAY;
	pg->piece = pg->piece_next;
	pg->piece_next = new_piece();
	if(collide(pg->pos, pg->piece, pg->board))
		return 1;
	return 0;
}

/* rotate: rotate the falling piece left or right */
static void
rotate(PolytrisGame *pg, int left)
{
	int i;
	Point pt;
	Piece *(*rfunc)(Piece *), *p;
	static Point kicks[] = {{0,0},{1,0},{-1,0},{0,1},{0,-1},{2,0},{-2,0},{0,2},{0,-2}};
	if(left)
		rfunc = rotl;
	else
		rfunc = rotr;
	for(i=0; i<(int) NELEM(kicks); i++){
		pt.x = pg->pos.x + kicks[i].x;
		pt.y = pg->pos.y + kicks[i].y;
		p = rfunc(pg->piece);
		if(!collide(pt, p, pg->board)){
			pg->pos.x = pt.x;
			pg->pos.y = pt.y;
			pg->piece = p;
			return;
		}
	}
}

/* rotr/rotl: returns the corresponding rotation of a piece */
static Piece *
rotr(Piece *p)
{
	if(p->rot == 3)
		return p-3;
	return p+1;
}

static Piece *
rotl(Piece *p)
{
	if(p->rot == 0)
		return p+3;
	return p-1;
}

/* collide: returns true if we have a collision */
static int
collide(Point pt, Piece *p, short board[HGHT][WDTH])
{
	int i;
	for(i=0; i<MAXPTS; i++){
		pt.x += p->pts[i].x;
		pt.y += p->pts[i].y;
		if(pt.x<0 || pt.x>=WDTH || pt.y<0 || pt.y>=HGHT)
			return 1;
		if(board[pt.y][pt.x])
			return 1;
	}
	return 0;
}

/* lockdown: modifies board; puts a piece into board */
static void
lockdown(Point pt, Piece *p, short board[HGHT][WDTH])
{
	int i;
	for(i=0; i<MAXPTS; i++){
		pt.x += p->pts[i].x;
		pt.y += p->pts[i].y;
		board[pt.y][pt.x] = p->color;
	}
}

/* polytris_tick: game logic */
int
polytris_tick(PolytrisGame *pg, int dt)
{
	/* we want to fall consistently */
	if(pg->time_until_fall >= 0){
		pg->time_until_fall -= dt;
	}else{
		if(!collide(mkpt(pg->pos.x, pg->pos.y+1), pg->piece, pg->board))
			pg->pos.y++;
		pg->time_until_fall = levels[pg->level % MAXLVL];
	}
	/* we only keep track of time_until_lock while the falling piece touches
	 * the floor or a block below */
	if(collide(mkpt(pg->pos.x, pg->pos.y+1), pg->piece, pg->board)){
		if(pg->time_until_lock >= 0){
			pg->time_until_lock -= dt;
		}else{
			lockdown(pg->pos, pg->piece, pg->board);
			remove_lines(pg);
			pg->pos.x = BEGINX; pg->pos.y = BEGINY;
			pg->time_until_fall = pg->time_until_lock = DELAY;
			pg->piece = pg->piece_next;
			pg->piece_next = new_piece();
			if(collide(pg->pos, pg->piece, pg->board))
				return 0;
		}
	}
	return 1;
}

/* polytris_init: set our data to their initial values */
static void
polytris_init(PolytrisGame *pg)
{
	pg->score = 0L;
	pg->lines = pg->level = 0;
	pg->goal = LVLGOAL;
	pg->time_until_fall = pg->time_until_lock = DELAY;
	memset(pg->board, 0, sizeof(pg->board));
	pg->pos.x = BEGINX; pg->pos.y = BEGINY;
	pg->piece_held = NULL;
	pg->piece_next = new_piece();
	pg->piece = new_piece();
}

/* polytris_create: return an allocated and initialized game structure */
PolytrisGame *
polytris_create(long seed)
{
	PolytrisGame *pg;
	pg = malloc(sizeof(PolytrisGame));
	if(pg == NULL){
		perror("polytris_create");
		exit(EXIT_FAILURE);
	}
	srand(seed);
	polytris_init(pg);
	return pg;
}

/* polytris_destroy: give back the memory from our game structure */
void
polytris_destroy(PolytrisGame *pg)
{
	free(pg);
}

/* mkpt: make a point from two ints */
Point
mkpt(int x, int y)
{
	Point pt;
	pt.x = x; pt.y = y;
	return pt;
}

