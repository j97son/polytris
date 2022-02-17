/* polytris: a simple tetris-like game. inspriation drawn from:
 * https://github.com/brenns10/tetris
 * https://9p.io/sources/plan9/sys/src/games/ (4s/5s) */
#include <curses.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include "polytris.h"

#define dt 34

int ncolors;

static void draw_piece(WINDOW *w, Point pt, Piece *p,
	chtype ch1, chtype ch2);
static void draw_board(WINDOW *w, Point pt, short board[HGHT][WDTH],
	chtype ch1, chtype ch2);
static void draw_stats(WINDOW *w, Point pt, PolytrisGame *pg);
static void draw_game(PolytrisGame *pg);
static void init_polytris_colors(void);
static short get_pair(Piece *p);

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

static void
draw_board(WINDOW *w, Point pt, short board[HGHT][WDTH],
	chtype ch1, chtype ch2)
{
	int i, j;
	if(has_colors()){
		for(i=0; i<HGHT; i++)
			for(j=0; j<WDTH; j++)
				if(board[i][j]){
					mvwaddch(w, i+pt.y, j*2+pt.x,
						ch1|COLOR_PAIR(board[i][j] % ncolors+1));
					mvwaddch(w, i+pt.y, j*2+1+pt.x,
						ch2|COLOR_PAIR(board[i][j] % ncolors+1));
				}else{
					mvwprintw(w, i+pt.y, j*2+pt.x, "  ");
				}
	}else{
		for(i=0; i<HGHT; i++)
			for(j=0; j<WDTH; j++)
				if(board[i][j]){
					mvwaddch(w, i+pt.y, j*2+pt.x, ch1 | A_REVERSE);
					mvwaddch(w, i+pt.y, j*2+1+pt.x, ch2 | A_REVERSE);
				}else{
					mvwprintw(w, i+pt.y, j*2+pt.x, "  ");
				}
	}
	wnoutrefresh(w);
}

static void
draw_stats(WINDOW *w, Point pt, PolytrisGame *pg)
{
	mvwprintw(w, pt.y++, pt.x, "score: %02ld", pg->score);
	mvwprintw(w, pt.y++, pt.x, "level: %02d", pg->level);
	mvwprintw(w, pt.y++, pt.x, "lines: %02d", pg->lines);
	mvwprintw(w, pt.y, pt.x, "goal: %02d", pg->goal);
	wnoutrefresh(w);
}

static short
get_pair(Piece *p)
{
	return p->color % ncolors+1;
}

static void
init_polytris_colors(void)
{
	int i;
	start_color();
	use_default_colors();
	if(COLORS < 16){
		ncolors = 6;
		for(i=1; i<=ncolors; i++)
			init_pair(i, 7, i);
		init_pair(7, -1, -1);
	}else{
		ncolors = 12;
		if(can_change_color()){
			short r, g, b;
			color_content(11, &r, &g, &b);
			init_color(11, r-200, g-200, b);
		}
		for(i=1; i<=ncolors/2; i++)
			init_pair(i, 15, i);
		for(i=ncolors/2+1; i<=ncolors; i++)
			init_pair(i, 15, i+2);
		init_pair(13, 8, -1);
	}
}

WINDOW *win_left, *win_middle, *win_right;

static void
draw_game(PolytrisGame *pg)
{
	if(has_colors()){
		draw_board(win_middle, mkpt(0, 0), pg->board, ' ', '`');
		draw_piece(win_middle, ghost_pos(pg), pg->piece,
			'[' | COLOR_PAIR(ncolors+1),
			']' | COLOR_PAIR(ncolors+1));
		draw_piece(win_middle, pg->pos, pg->piece,
			' ' | COLOR_PAIR(get_pair(pg->piece)),
			'`' | COLOR_PAIR(get_pair(pg->piece)));
		draw_piece(win_right, mkpt(2, 12), pg->piece, ' ', ' ');
		draw_piece(win_right, mkpt(2, 12), pg->piece_next,
			' ' | COLOR_PAIR(get_pair(pg->piece_next)),
			'`' | COLOR_PAIR(get_pair(pg->piece_next)));
		draw_stats(win_left, mkpt(2, 1), pg);
	}else{
		draw_board(win_middle, mkpt(0, 0), pg->board,
			' ' | A_REVERSE,
			'`' | A_REVERSE);
		draw_piece(win_middle, ghost_pos(pg), pg->piece, '[', ']');
		draw_piece(win_middle, pg->pos, pg->piece,
			' ' | A_REVERSE,
			'`' | A_REVERSE);
		draw_piece(win_right, mkpt(2, 12), pg->piece, ' ', ' ');
		draw_piece(win_right, mkpt(2, 12), pg->piece_next,
			' ' | A_REVERSE,
			'`' | A_REVERSE);
		draw_stats(win_left, mkpt(2, 1), pg);
	}
}

int
main(int argc, char *argv[])
{
	long time_start, time_end;
	PolytrisGame *pg;

	time_start = time(NULL);
	pg = polytris_create(time_start);
	if(argc == 2){
		pg->level = atoi(argv[1]) % MAXLVL;
		pg->goal += pg->level * LVLGOAL;
	}

	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	if(has_colors())
		init_polytris_colors();

	win_left = newwin(30, 20, 0, 0);
	win_middle = newwin(HGHT+1, WDTH*2, 0, 20);
	win_right = newwin(30, 20, 0, 20+WDTH*2);

	mvwvline(win_left, 0, 19, '|', HGHT+1);
	mvwhline(win_middle, HGHT, 0, '=', WDTH*2);
	mvwvline(win_right, 0, 0, '|', HGHT+1);
	mvwprintw(win_right, 10, 4, "next:");
	mvwprintw(win_left, 10, 4, "held:");
	refresh();

	while(polytris_tick(pg, dt)){
		draw_game(pg);
		doupdate();
		napms(dt);
		switch(getch()){
		case KEY_RESIZE:
		case KEY_F(1):
		case 'p':
			werase(win_middle);
			mvwhline(win_middle, HGHT, 0, '=', WDTH*2);
			wattron(win_middle, A_REVERSE | A_BOLD);
			mvwhline(win_middle, HGHT/2, 0, '-', WDTH*2);
			mvwprintw(win_middle, HGHT/2, WDTH-3, "paused");
			wattroff(win_middle, A_REVERSE | A_BOLD);
			wrefresh(win_middle);
			nodelay(stdscr, FALSE);
			while(getch() == KEY_RESIZE);
			nodelay(stdscr, TRUE);
			mvwvline(win_left, 0, 19, '|', HGHT+1);
			mvwhline(win_middle, HGHT, 0, '=', WDTH*2);
			mvwvline(win_right, 0, 0, '|', HGHT+1);
			mvwprintw(win_right, 10, 4, "next:");
			mvwprintw(win_left, 10, 4, "held:");
			refresh();
			break;
		case 'q':
			goto end;
			break;
		case 'c':
		case 's':
		case 'g':
			if(pg->holds < 3){
				if(pg->piece_held != NULL)
					draw_piece(win_left, mkpt(2, 12), pg->piece_held,
						' ', ' ');
				hold(pg);
				if(has_colors()){
					draw_piece(win_left, mkpt(2, 12), pg->piece_held,
						' ' | COLOR_PAIR(get_pair(pg->piece_held)),
						'`' | COLOR_PAIR(get_pair(pg->piece_held)));
				}else{
					draw_piece(win_left, mkpt(2, 12), pg->piece_held,
						' ' | A_REVERSE,
						'`' | A_REVERSE);
				}
			}
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
			if(drop(pg))
				goto end;
			break;
		}
	}
end:
	draw_game(pg);
	wattron(win_middle, A_REVERSE | A_BOLD);
	mvwhline(win_middle, HGHT/2, 0, '-', WDTH*2);
	mvwprintw(win_middle, HGHT/2, WDTH-4, "gameover");
	wattroff(win_middle, A_REVERSE | A_BOLD);
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

