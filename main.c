/* polytris: a simple tetris-like game. inspriation drawn from https://github.com/brenns10/tetris and https://9p.io/sources/plan9/sys/src/games/ (4s/5s) */
#include <curses.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#include "polytris.h"

#define NCOLORS 6
#define GET_COLOR(n) ((n) % NCOLORS + 1)

static void draw_piece(WINDOW *w, Point pt, Piece *p, chtype ch1, chtype ch2);
static void draw_board(WINDOW *w, Point pt, short board[HGHT][WDTH], chtype ch1, chtype ch2);
static void draw_stats(WINDOW *w, Point pt, PolytrisGame *pg);

/* draw_piece: draw a piece at pt in the window given */
static void
draw_piece(WINDOW *w, Point pt, Piece *p, chtype ch1, chtype ch2)
{
	int i;
	for(i=0; i<MAXPTS; i++){
		pt.x += p->pts[i].x;
		pt.y += p->pts[i].y;
		mvwaddch(w, pt.y, pt.x*2, ch1);
		mvwaddch(w, pt.y, pt.x*2+1, ch2);
	}
	wnoutrefresh(w);
}

/* draw_board: draw board[][] at pt in the window given */
static void
draw_board(WINDOW *w, Point pt, short board[HGHT][WDTH], chtype ch1, chtype ch2)
{
	int i, j;
	if(has_colors()){
	for(i=0; i<HGHT; i++)
		for(j=0; j<WDTH; j++)
			if(board[i][j]){
				mvwaddch(w, i+pt.y, j*2+pt.x,
					ch1|COLOR_PAIR(GET_COLOR(board[i][j])));
				mvwaddch(w, i+pt.y, j*2+1+pt.x,
					ch2|COLOR_PAIR(GET_COLOR(board[i][j])));
			}else
				mvwprintw(w, i+pt.y, j*2+pt.x, "  ");
	}else{
	for(i=0; i<HGHT; i++)
		for(j=0; j<WDTH; j++)
			if(board[i][j]){
				mvwaddch(w, i+pt.y, j*2+pt.x, ch1);
				mvwaddch(w, i+pt.y, j*2+1+pt.x, ch2);
			}else
				mvwprintw(w, i+pt.y, j*2+pt.x, "  ");
	}
	wnoutrefresh(w);
}

static void
draw_stats(WINDOW *w, Point pt, PolytrisGame *pg)
{
	mvwprintw(w, pt.y++, pt.x, "score: %05ld", pg->score);
	mvwprintw(w, pt.y++, pt.x, "level: %02d", pg->level);
	mvwprintw(w, pt.y++, pt.x, "lines: %02d", pg->lines);
	mvwprintw(w, pt.y, pt.x, "goal: %02d", pg->goal);
	wnoutrefresh(w);
}

/* main: run the game loop & handle io */
#define dt 34
int
main(int argc, char *argv[])
{
	long time_start, time_end;
	WINDOW *win_left, *win_middle, *win_right;
	PolytrisGame *pg;

	time_start = time(NULL);
	pg = polytris_create(time_start);
	//handle args; even if we get garbage passed in if atoi cannot convert it properly it returns zero
	if(argc == 2){
		pg->level = atoi(argv[1]) % MAXLVL;
		pg->goal += pg->level * LVLGOAL;
	}

	//curses setup
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	if(has_colors()){
		int i;
		start_color();
		use_default_colors();
		for(i=1; i<=NCOLORS; i++)
			init_pair(i, i, 0);
	}

	win_left = newwin(21, 20, 0, 0);
	win_middle = newwin(21, 20, 0, 20);
	win_right = newwin(21, 20, 0, 40);
	wborder(win_left, ' ', '|', ' ', ' ', ' ', '|', ' ', '=');
	wborder(win_middle, ' ', ' ', ' ', '=', ' ', ' ', '=', '=');
	wborder(win_right, '|', ' ', ' ', ' ', '|', ' ', '=', ' ');
	mvwprintw(win_right, 10, 4, "next:");
	mvwprintw(win_left, 10, 4, "held:");
	refresh();

	while(polytris_tick(pg, dt)){
		//slightly awkward func calls but being able to pass in exactly what chtypes we want is just too useful
		draw_board(win_middle, mkpt(0, 0), pg->board,
			' '|A_REVERSE, ' '|A_REVERSE);
		draw_piece(win_middle, ghost_pos(pg), pg->piece,
			'['|COLOR_PAIR(GET_COLOR(pg->piece->color)),
			']'|COLOR_PAIR(GET_COLOR(pg->piece->color)));
		draw_piece(win_middle, pg->pos, pg->piece,
			' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece->color)),
			' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece->color)));

		draw_piece(win_right, mkpt(2, 12), pg->piece, ' ', ' ');
		draw_piece(win_right, mkpt(2, 12), pg->piece_next,
			' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece_next->color)),
			' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece_next->color)));

		draw_stats(win_left, mkpt(2, 1), pg);

		doupdate();
		napms(dt);
		switch(getch()){
		case KEY_RESIZE:
		case KEY_F(1):
		case 'p':
			//pause
			draw_board(win_middle, mkpt(0, 0), pg->board, ' ', ' ');
			wattron(win_middle, A_REVERSE);
			mvwhline(win_middle, 10, 0, '-', 20);
			mvwprintw(win_middle, 10, WDTH-3, "paused");
			wattroff(win_middle, A_REVERSE);
			wrefresh(win_middle);
			nodelay(stdscr, FALSE);
			while(getch() == KEY_RESIZE);	//stay paused while resizing
			nodelay(stdscr, TRUE);
			//got to do a lot of redrawing just in case the term is resized
			wborder(win_left, ' ', '|', ' ', ' ', ' ', '|', ' ', '=');
			wborder(win_middle, ' ', ' ', ' ', '=', ' ', ' ', '=', '=');
			wborder(win_right, '|', ' ', ' ', ' ', '|', ' ', '=', ' ');
			mvwprintw(win_right, 10, 4, "next:");
			mvwprintw(win_left, 10, 4, "held:");
			refresh();
			break;
		case 'q':
			//quit
			goto end;	//evil goto
			break;
		case 'c':
		case 's':
		case 'g':
			//erase the last held piece if there is one
			if(pg->piece_held != NULL)
				draw_piece(win_left, mkpt(2, 12), pg->piece_held, ' ', ' ');
			hold(pg);
			draw_piece(win_left, mkpt(2, 12), pg->piece_held,
			' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece_held->color)),
			' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece_held->color)));
			break;
		case 'z':
		case 'd':
			rleft(pg);
			break;
		case KEY_UP:
		case 'x':
		case 'f':
		case 'k':
			rright(pg);
			break;
		case KEY_LEFT:
		case 'h':
			mleft(pg);
			break;
		case KEY_RIGHT:
		case 'l':
			mright(pg);
			break;
		case KEY_DOWN:
		case 'j':
			mdown(pg);
			break;
		case '\n':
		case ' ':
			drop(pg);
			if(collide(pg->pos, pg->piece, pg->board))
				goto end;	//evil goto
			break;
		}
	}
end:
	//draw everything one last time for the end screen
	draw_board(win_middle, mkpt(0, 0), pg->board,
		' '|A_REVERSE, ' '|A_REVERSE);
	draw_piece(win_middle, ghost_pos(pg), pg->piece,
		'['|COLOR_PAIR(GET_COLOR(pg->piece->color)),
		']'|COLOR_PAIR(GET_COLOR(pg->piece->color)));
	draw_piece(win_middle, pg->pos, pg->piece,
		' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece->color)),
		' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece->color)));
	draw_piece(win_right, mkpt(2, 12), pg->piece, ' ', ' ');
	draw_piece(win_right, mkpt(2, 12), pg->piece_next,
		' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece_next->color)),
		' '|A_REVERSE|COLOR_PAIR(GET_COLOR(pg->piece_next->color)));
	draw_stats(win_left, mkpt(2, 1), pg);
	wattron(win_middle, A_REVERSE);
	mvwhline(win_middle, 10, 0, '-', 20);
	mvwprintw(win_middle, 10, WDTH-4, "gameover");
	wattroff(win_middle, A_REVERSE);
	wnoutrefresh(win_middle);
	doupdate();
	nodelay(stdscr, FALSE);
	getch();
	endwin();
	time_end = time(NULL);
	printf("player: %s\nscore: %ld\ntime: %ld\nlevel: %d\nlines: %d\n",
		getenv("USER"), pg->score, time_end-time_start, pg->level, pg->lines);
	polytris_destroy(pg);
	exit(EXIT_SUCCESS);
}

