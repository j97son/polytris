/* polytris: a simple tetris-like game. inspriation drawn from:
 * https://github.com/brenns10/tetris
 * https://9p.io/sources/plan9/sys/src/games/ (4s/5s) */
#include <curses.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include "polytris.h"

int ncolors;

static void draw_piece(WINDOW *w, Point pt, Piece *p, chtype ch1, chtype ch2);
static void draw_board_mono(WINDOW *w, Point pt, short board[HGHT][WDTH],
	chtype ch1, chtype ch2);
static void draw_board_color(WINDOW *w, Point pt, short board[HGHT][WDTH],
	chtype ch1, chtype ch2);
static void draw_stats(WINDOW *w, Point pt, PolytrisGame *pg);
static short get_pair(Piece *p);

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
/* monochrome version */
static void
draw_board_mono(WINDOW *w, Point pt, short board[HGHT][WDTH],
	chtype ch1, chtype ch2)
{
	int i, j;
	for(i=0; i<HGHT; i++)
		for(j=0; j<WDTH; j++)
			if(board[i][j]){
				mvwaddch(w, i+pt.y, j*2+pt.x, ch1);
				mvwaddch(w, i+pt.y, j*2+1+pt.x, ch2);
			}else
				mvwprintw(w, i+pt.y, j*2+pt.x, "  ");
	wnoutrefresh(w);
}

/* color version */
static void
draw_board_color(WINDOW *w, Point pt, short board[HGHT][WDTH],
	chtype ch1, chtype ch2)
{
	int i, j;
	for(i=0; i<HGHT; i++)
		for(j=0; j<WDTH; j++)
			if(board[i][j]){
				mvwaddch(w, i+pt.y, j*2+pt.x,
					ch1|COLOR_PAIR(board[i][j] % ncolors+1));
				mvwaddch(w, i+pt.y, j*2+1+pt.x,
					ch2|COLOR_PAIR(board[i][j] % ncolors+1));
			}else
				mvwprintw(w, i+pt.y, j*2+pt.x, "  ");
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

/* get_pair: return the color pair number for the piece given */
static short
get_pair(Piece *p)
{
	return p->color % ncolors + 1;
}

/* init_polytris_colors: set up colors based on how many the term supports */
void
init_polytris_colors(void)
{
	start_color();
	use_default_colors();
	if(COLORS < 16){
		init_pair(1, 7, 1);
		init_pair(2, 7, 2);
		init_pair(3, 7, 3);
		init_pair(4, 7, 4);
		init_pair(5, 7, 5);
		init_pair(6, 7, 6);
		init_pair(7, -1, -1);
		ncolors = 6;
	}else{
		//bright yellow is just a bit too bright
		if(can_change_color()){
			short r, g, b;
			color_content(11, &r, &g, &b);
			init_color(11, r-200, g-200, b);
		}
		init_pair(1, 15, 1);
		init_pair(2, 15, 2);
		init_pair(3, 15, 3);
		init_pair(4, 15, 4);
		init_pair(5, 15, 5);
		init_pair(6, 15, 6);
		init_pair(7, 15, 9);
		init_pair(8, 15, 10);
		init_pair(9, 15, 11);
		init_pair(10, 15, 12);
		init_pair(11, 15, 13);
		init_pair(12, 15, 14);
		init_pair(13, 8, -1);
		ncolors = 12;
	}
}

#define dt 34
/* main: run the game loop & handle io */
int
main(int argc, char *argv[])
{
	void (*draw_board)(WINDOW *, Point, short [HGHT][WDTH], chtype, chtype);
	long time_start, time_end;
	WINDOW *win_left, *win_middle, *win_right;
	PolytrisGame *pg;

	time_start = time(NULL);
	pg = polytris_create(time_start);
	draw_board = draw_board_mono;

	/* handle args; if argv isn't convertible atoi will return zero */
	if(argc == 2){
		pg->level = atoi(argv[1]) % MAXLVL;
		pg->goal += pg->level * LVLGOAL;
	}

	setlocale(LC_ALL, "");
	/* curses setup */
	initscr();
	if(has_colors()){
		init_polytris_colors();
		draw_board = draw_board_color;
	}
	cbreak();
	noecho();
	curs_set(0);
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

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
		/* slightly lengthy func calls */
		draw_board(win_middle, mkpt(0, 0), pg->board,
			' ', '`');
		draw_piece(win_middle, ghost_pos(pg), pg->piece,
			'['|COLOR_PAIR(ncolors+1),
			']'|COLOR_PAIR(ncolors+1));
		draw_piece(win_middle, pg->pos, pg->piece,
			' '|COLOR_PAIR(get_pair(pg->piece)),
			'`'|COLOR_PAIR(get_pair(pg->piece)));

		draw_piece(win_right, mkpt(2, 12), pg->piece, ' ', ' ');
		draw_piece(win_right, mkpt(2, 12), pg->piece_next,
			' '|COLOR_PAIR(get_pair(pg->piece_next)),
			'`'|COLOR_PAIR(get_pair(pg->piece_next)));

		draw_stats(win_left, mkpt(2, 1), pg);

		doupdate();
		napms(dt);
		switch(getch()){
		case KEY_RESIZE:
		case KEY_F(1):
		case 'p':
			/* pause */
			//pause();
			draw_board_mono(win_middle, mkpt(0, 0), pg->board, ' ', ' ');
			wattron(win_middle, A_REVERSE);
			mvwhline(win_middle, HGHT/2, 0, '-', WDTH*2);
			mvwprintw(win_middle, HGHT/2, WDTH-3, "paused");
			wattroff(win_middle, A_REVERSE);
			wrefresh(win_middle);
			nodelay(stdscr, FALSE);
			while(getch() == KEY_RESIZE);	//stay paused while resizing
			nodelay(stdscr, TRUE);
			/* got to do a lot of redrawing just in case the term is resized */
			mvwvline(win_left, 0, 19, '|', HGHT+1);
			mvwhline(win_middle, HGHT, 0, '=', WDTH*2);
			mvwvline(win_right, 0, 0, '|', HGHT+1);
			mvwprintw(win_right, 10, 4, "next:");
			mvwprintw(win_left, 10, 4, "held:");
			refresh();
			break;
		case 'q':
			/* quit */
			goto end;
			break;
		case 'c':
		case 's':
		case 'g':
			if(pg->holds < 3){
				/* erase the last held piece if there is one */
				if(pg->piece_held != NULL)
					draw_piece(win_left, mkpt(2, 12), pg->piece_held, ' ', ' ');
				hold(pg);
				draw_piece(win_left, mkpt(2, 12), pg->piece_held,
				' '|COLOR_PAIR(get_pair(pg->piece_held)),
				'`'|COLOR_PAIR(get_pair(pg->piece_held)));
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
	/* draw everything one last time for the end screen */
	draw_board(win_middle, mkpt(0, 0), pg->board,
		' ', '`');
	draw_piece(win_middle, ghost_pos(pg), pg->piece,
		'['|COLOR_PAIR(ncolors+1),
		']'|COLOR_PAIR(ncolors+1));
	draw_piece(win_middle, pg->pos, pg->piece,
		' '|COLOR_PAIR(get_pair(pg->piece)),
		'`'|COLOR_PAIR(get_pair(pg->piece)));

	draw_piece(win_right, mkpt(2, 12), pg->piece, ' ', ' ');
	draw_piece(win_right, mkpt(2, 12), pg->piece_next,
		' '|COLOR_PAIR(get_pair(pg->piece_next)),
		'`'|COLOR_PAIR(get_pair(pg->piece_next)));

	draw_stats(win_left, mkpt(2, 1), pg);
	wattron(win_middle, A_REVERSE);
	mvwhline(win_middle, HGHT/2, 0, '-', WDTH*2);
	mvwprintw(win_middle, HGHT/2, WDTH-4, "gameover");
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

