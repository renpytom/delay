#ifdef HAVE_LIBCURSES
#ifdef HAVE_CURSES_H

#include <curses.h>

#define FWIDTH 6
#define FHEIGHT 7

char *digits[10][7] = {
	{
		" **** ",
		"**  **",
		"**  **",
		"**  **",
		"**  **",
		"**  **",
		" **** ",
	 }, {
		"   ** ",
		"  *** ",
		"   ** ",
		"   ** ",
		"   ** ",
		"   ** ",
		"   ** ",
	 }, {
		" **** ",
		"**  **",
		"    **",
		"   ** ",
		"  **  ",
		" **   ",
		"******",
	 }, {
		" **** ",
		"**  **",
		"    **",
		" **** ",
		"    **",
		"**  **",
		" **** ",
	 }, {
		"   ** ",
		"  *** ",
		" * ** ",
		"*  ** ",
		"******",
		"   ** ",
		"   ** ",
	 }, {
		"******",
		"**    ",
		"**    ",
		"***** ",
		"    **",
		"    **",
		"***** ",
	 }, {
		" **** ",
		"**  **",
		"**    ",
		"***** ",
		"**  **",
		"**  **",
		" **** ",
	 }, {
		"******",
		"    **",
		"   ** ",
		"  **  ",
		"  **  ",
		"  **  ",
		"  **  ",
	 }, {
		" **** ",
		"**  **",
		"**  **",
		" **** ",
		"**  **",
		"**  **",
		" **** ",
	 }, {
		" **** ",
		"**  **",
		"**  **",
		" *****",
		"    **",
		"**  **",
		" **** ",
	 }
};

void printrow(char *s) {
	while (*s) {
		switch(*s) {
		case '*':
			addch(' ' | A_REVERSE);
			break;
		default:
			addch(*s);
			break;
		}
		s++;
	}
}

void printdigit(int r, int c, int d) {
	int i;
	for(i=0; i < 7; i++) {
		move(r+i, c);
		printrow(digits[d][i]);
	}
}

void printcolon(int r, int c) {
	move(r+2, c);
	addch(' ' | A_REVERSE);
	move(r+5, c);
	addch(' ' | A_REVERSE);
}		

void printstring(char *s) {
	int col = 2;
	int row = 2;

	while (*s) {
		switch (*s) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			printdigit(row, col, *s - '0');
			col += FWIDTH + 1;
			break;
		case ' ':
			col += FWIDTH + 1;
			break;
		case ':':
			printcolon(row, col);
			col += 2;
		}
		s++;
	}
}

void curshowcount(int n) {
	char buf[25];

	sprintf(buf, "%d:%02d:%02d", n / 3600,
		(n/60) % 60, n % 60);

	erase();
	printstring(buf);
	move(LINES-1, COLS-1);
	refresh();
}
	

#endif
#endif
