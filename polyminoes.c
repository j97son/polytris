#include "polytris.h"

Piece pieces[] = {
	/* trominoes */
	/* I */
	{0,1,{{0,1},{1,0},{1,0}}},
	{1,1,{{1,0},{0,1},{0,1}}},
	{2,1,{{0,1},{1,0},{1,0}}},
	{3,1,{{1,0},{0,1},{0,1}}},
	/* V */
	{0,2,{{1,0},{0,1},{1,0}}},
	{1,2,{{2,0},{-1,0},{0,1}}},
	{2,2,{{1,0},{1,0},{0,1}}},
	{3,2,{{2,0},{0,1},{-1,0}}},
	/* tetrominoes */
	/* O */
	{0,1,{{1,0},{1,0},{0,1},{-1,0}}},
	{1,1,{{1,0},{1,0},{0,1},{-1,0}}},
	{2,1,{{1,0},{1,0},{0,1},{-1,0}}},
	{3,1,{{1,0},{1,0},{0,1},{-1,0}}},
	/* I */
	{0,2,{{0,1},{1,0},{1,0},{1,0}}},
	{1,2,{{2,0},{0,1},{0,1},{0,1}}},
	{2,2,{{0,2},{1,0},{1,0},{1,0}}},
	{3,2,{{1,0},{0,1},{0,1},{0,1}}},
	/* T */
	{0,3,{{0,1},{1,0},{1,0},{-1,-1}}},
	{1,3,{{1,0},{0,1},{0,1},{1,-1}}},
	{2,3,{{0,1},{1,0},{1,0},{-1,1}}},
	{3,3,{{1,0},{0,1},{0,1},{-1,-1}}},
	/* L */
	{0,4,{{0,1},{1,0},{1,0},{0,-1}}},
	{1,4,{{1,0},{0,1},{0,1},{1,0}}},
	{2,4,{{0,2},{0,-1},{1,0},{1,0}}},
	{3,4,{{0,0},{1,0},{0,1},{0,1}}},
	/* J */
	{0,5,{{0,0},{0,1},{1,0},{1,0}}},
	{1,5,{{2,0},{-1,0},{0,1},{0,1}}},
	{2,5,{{0,1},{1,0},{1,0},{0,1}}},
	{3,5,{{1,0},{0,1},{0,1},{-1,0}}},
	/* S */
	{0,6,{{0,1},{1,0},{0,-1},{1,0}}},
	{1,6,{{1,0},{0,1},{1,0},{0,1}}},
	{2,6,{{0,2},{1,0},{0,-1},{1,0}}},
	{3,6,{{0,0},{0,1},{1,0},{0,1}}},
	/* Z */
	{0,7,{{0,0},{1,0},{0,1},{1,0}}},
	{1,7,{{2,0},{0,1},{-1,0},{0,1}}},
	{2,7,{{0,1},{1,0},{0,1},{1,0}}},
	{3,7,{{1,0},{0,1},{-1,0},{0,1}}},
	/* pentominoes */
	/* I */
	{0,1,{{-1,1},{1,0},{1,0},{1,0},{1,0}}},
	{1,1,{{1,-1},{0,1},{0,1},{0,1},{0,1}}},
	{2,1,{{-1,1},{1,0},{1,0},{1,0},{1,0}}},
	{3,1,{{1,-1},{0,1},{0,1},{0,1},{0,1}}},
	/* F */
	{0,2,{{0,0},{0,1},{1,0},{1,0},{-1,1}}},
	{1,2,{{1,0},{1,0},{-1,1},{-1,0},{1,1}}},
	{2,2,{{1,0},{-1,1},{1,0},{1,0},{0,1}}},
	{3,2,{{1,0},{1,1},{-1,0},{0,1},{-1,0}}},
	/* F' */
	{0,3,{{2,0},{0,1},{-1,0},{-1,0},{1,1}}},
	{1,3,{{1,0},{-1,1},{1,0},{0,1},{1,0}}},
	{2,3,{{0,2},{0,-1},{1,0},{1,0},{-1,-1}}},
	{3,3,{{0,0},{1,0},{0,1},{0,1},{1,-1}}},
	/* L */
	{0,4,{{0,1},{1,0},{1,0},{1,0},{0,-1}}},
	{1,4,{{1,-1},{0,1},{0,1},{0,1},{1,0}}},
	{2,4,{{0,1},{0,-1},{1,0},{1,0},{1,0}}},
	{3,4,{{1,-1},{1,0},{0,1},{0,1},{0,1}}},
	/* L' */
	{0,5,{{0,0},{0,1},{1,0},{1,0},{1,0}}},
	{1,5,{{2,-1},{-1,0},{0,1},{0,1},{0,1}}},
	{2,5,{{0,0},{1,0},{1,0},{1,0},{0,1}}},
	{3,5,{{2,-1},{0,1},{0,1},{0,1},{-1,0}}},
	/* N */
	{0,6,{{0,0},{1,0},{0,1},{1,0},{1,0}}},
	{1,6,{{2,-1},{0,1},{-1,0},{0,1},{0,1}}},
	{2,6,{{0,0},{1,0},{1,0},{0,1},{1,0}}},
	{3,6,{{2,-1},{0,1},{0,1},{-1,0},{0,1}}},
	/* N' */
	{0,7,{{0,1},{1,0},{1,0},{0,-1},{1,0}}},
	{1,7,{{1,-1},{0,1},{0,1},{1,0},{0,1}}},
	{2,7,{{0,1},{1,0},{0,-1},{1,0},{1,0}}},
	{3,7,{{1,-1},{0,1},{1,0},{0,1},{0,1}}},
	/* P */
	{0,1,{{0,0},{1,0},{-1,1},{1,0},{1,0}}},
	{1,1,{{0,0},{1,0},{0,1},{-1,0},{0,1}}},
	{2,1,{{-1,0},{1,0},{1,0},{-1,1},{1,0}}},
	{3,1,{{1,-1},{0,1},{0,1},{-1,0},{0,-1}}},
	/* P' */
	{0,2,{{1,0},{1,0},{0,1},{-1,0},{-1,0}}},
	{1,2,{{1,-1},{0,1},{0,1},{1,-1},{0,1}}},
	{2,2,{{1,0},{0,1},{1,0},{0,-1},{1,0}}},
	{3,2,{{1,0},{0,1},{1,-1},{0,1},{0,1}}},
	/* T */
	{0,3,{{1,0},{0,1},{-1,1},{1,0},{1,0}}},
	{1,3,{{0,0},{0,1},{0,1},{1,-1},{1,0}}},
	{2,3,{{0,0},{1,0},{1,0},{-1,1},{0,1}}},
	{3,3,{{0,1},{1,0},{1,-1},{0,1},{0,1}}},
	/* U */
	{0,4,{{0,0},{0,1},{1,0},{1,0},{0,-1}}},
	{1,4,{{1,-1},{-1,0},{0,1},{0,1},{1,0}}},
	{2,4,{{0,0},{0,-1},{1,0},{1,0},{0,1}}},
	{3,4,{{1,-1},{1,0},{0,1},{0,1},{-1,0}}},
	/* V */
	{0,5,{{0,0},{0,1},{0,1},{1,0},{1,0}}},
	{1,5,{{0,2},{0,-1},{0,-1},{1,0},{1,0}}},
	{2,5,{{0,0},{1,0},{1,0},{0,1},{0,1}}},
	{3,5,{{2,0},{0,1},{0,1},{-1,0},{-1,0}}},
	/* W */
	{0,6,{{0,0},{0,1},{1,0},{0,1},{1,0}}},
	{1,6,{{0,2},{0,-1},{1,0},{0,-1},{1,0}}},
	{2,6,{{0,0},{1,0},{0,1},{1,0},{0,1}}},
	{3,6,{{2,0},{0,1},{-1,0},{0,1},{-1,0}}},
	/* X */
	{0,7,{{0,1},{1,-1},{0,1},{0,1},{1,-1}}},
	{1,7,{{0,1},{1,-1},{0,1},{0,1},{1,-1}}},
	{2,7,{{0,1},{1,-1},{0,1},{0,1},{1,-1}}},
	{3,7,{{0,1},{1,-1},{0,1},{0,1},{1,-1}}},
	/* Y */
	{0,1,{{0,1},{1,0},{1,0},{1,0},{-1,-1}}},
	{1,1,{{1,-1},{0,1},{0,1},{0,1},{1,-1}}},
	{2,1,{{0,0},{1,1},{0,-1},{1,0},{1,0}}},
	{3,1,{{2,-1},{-1,1},{1,0},{0,1},{0,1}}},
	/* Y' */
	{0,2,{{0,1},{1,-1},{0,1},{1,0},{1,0}}},
	{1,2,{{1,-1},{1,1},{-1,0},{0,1},{0,1}}},
	{2,2,{{0,0},{1,0},{1,0},{1,0},{-1,1}}},
	{3,2,{{2,-1},{0,1},{0,1},{0,1},{-1,-1}}},
	/* Z */
	{0,3,{{2,0},{0,1},{-1,0},{-1,0},{0,1}}},
	{1,3,{{0,0},{1,0},{0,1},{0,1},{1,0}}},
	{2,3,{{2,0},{0,1},{-1,0},{-1,0},{0,1}}},
	{3,3,{{0,0},{1,0},{0,1},{0,1},{1,0}}},
	/* Z' */
	{0,4,{{0,0},{0,1},{1,0},{1,0},{0,1}}},
	{1,4,{{1,0},{-1,0},{0,1},{0,1},{-1,0}}},
	{2,4,{{0,0},{0,1},{1,0},{1,0},{0,1}}},
	{3,4,{{1,0},{-1,0},{0,1},{0,1},{-1,0}}}
};

