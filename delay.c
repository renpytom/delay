/* delay - counts down a specified number of seconds.
   Copyright (C) 1998-2014 Tom Rothamel.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "parsetime.h"

#ifdef HAVE_CURSES_H
#include <curses.h>
#endif

/* The following code implements the actual delaying. If precise is true, it
   attempts to compensate for the time spend in calculating, printing, etc.
*/

struct timeval tv;

void initdelay(int precise) {
	if (!precise) return;
	gettimeofday(&tv, NULL);
}

void delay(int secs, int precise) {
	struct timeval ctv;
	struct timeval dtv;

	if (!precise) {
		sleep(secs);
		return;
	}

	tv.tv_sec += secs;

	gettimeofday(&ctv, NULL);
	dtv.tv_sec  = tv.tv_sec - ctv.tv_sec;
	dtv.tv_usec  = tv.tv_usec - ctv.tv_usec;

	if (dtv.tv_usec < 0) {
		dtv.tv_usec += 1000000;
		dtv.tv_sec -= 1;
	}

	select(0, NULL, NULL, NULL, &dtv);
}

/* These two functions  handle the parsing of standard-format arguments.
   This format allows for HH:MM:SS, DDdHHhMMmSSs, or SSSSs style inputs.
   (Multiple arguments are added together.
*/

int parseonestd(char *entry) {
	int commit = 0;
        int t = 0;
	int usfact = 1000000;

	while (1) {
		if (!*entry) return commit + t;


		if (('0' <= *entry) && (*entry <= '9')) {
			t *= 10;
			t += *entry - '0';
		} else if (*entry == '.') {
			break;
		} else switch(*entry) {
		case ':':
			commit +=t;
			commit *= 60;
			t = 0;
			break;
		case 's':
			commit += t;
			t = 0;
			break;
		case 'm':
			commit +=  t * 60;
			t = 0;
			break;
		case 'h':
			commit += t * 3600;
			t = 0;
			break;
		case 'd':
			commit += t * 86400;
			break;
		}

		entry++;
	}

	while (*++entry) {
		usfact /= 10;
		if (('0' <= *entry) && (*entry <= '9')) {
			usleep((*entry - '0') * usfact);
		}
	}

	return commit + t;
}


int parsestandard(int argc, char **argv) {
	int i;
	int dt = 0;
	int tdt;

	for (i = 0; i < argc; i++) {
		tdt = parseonestd(argv[i]);
		if (tdt == -1) return -1;
		dt += tdt;
	}

	return dt;
}



/* Prints out the time, using the format code. The format is somewhat like
   the integer codes used in printf. You have all the flags, and then
   d for days, h for hours, m for minutes, s for seconds, and n for the
   total number of seconds remaining.
*/

void printtime(char *fmt, int time) {
	char buf[16];
	char *c;
	int i;
	char b;

	c = fmt;


	while (*c) {
		switch (*c) {
		case '\\':
			c++;

			switch (*c) {
			case 0:
				return;
			case '\\':
				printf("\\");
				break;
			case 'r':
				printf("\r");
				break;
			case 'n':
				printf("\n");
				break;
			case 't':
				printf("\t");
				break;
			case '%':
				printf("%%");
				break;
			}

			break;

		case '%':
			strncpy(buf, c, 15);
			for (i = 1; i < 100; i++) {
				 if (buf[i] == 0) return;
				 if (buf[i] == 'd') break;
				 if (buf[i] == 'h') break;
				 if (buf[i] == 'm') break;
				 if (buf[i] == 's') break;
				 if (buf[i] == 'n') break;
			}

			b = buf[i];
			buf[i] = 'd';
			buf[i+1] = 0;

			switch (b) {
			case 'd':
				printf(buf, time/86400);
				break;
			case 'h':
				printf(buf, (time/3600) % 24);
				break;
			case 'm':
				printf(buf, (time/60) % 60);
				break;
			case 's':
				printf(buf, time % 60);
				break;
			case 'n':
				printf(buf, time);
				break;
			case 0:
				return;
			}

			c += i;
			break;
		default:
			printf("%c", *c);
			break;
		}

		c++;
	}

}


/* The rest of the code is responsible for displaying the countdown,
   parsing the arguments, and otherwise making this thing work.
*/

char *custmessage;

void usage(char *name) {
	fprintf(stdout,
		"Usage: %s [options] <seconds to delay> [-- command]\n"
		"   or  %s [options] until <timespec> [-- command]\n", name, name);

	exit(1);
}

void curshowcount(int);

void showcount(int dtime, int ctype) {
	switch(ctype) {
	case 0:
		printtime("\r% 3d %02h:%02m:%02s", dtime);
		break;
	case 1:
		printtime("\rTime Remaining: %d days, %02h:%02m:%02s.", dtime);
		break;
	case 2:
		printtime(custmessage, dtime);
		break;
	case 3:
		printf("\r% 8d", dtime);
		break;
	case 4:
		curshowcount(dtime);
		break;

	}

	fflush(stdout);
}

int main(int argc, char **argv) {
	int opt;
	int dtime;
	int ctype = 0;
	int bell = 0;
	int update = 1;
	int i;
	char **cmd = NULL;


	if (strstr(argv[0], "sleep")) ctype = -1;

	for (i = 0; i < argc; i++) {
		if(!strcmp(argv[i], "--")) {
			if (i == argc - 1) {
				fprintf(stderr, "%s: with '--' you must specify a command to run.\n", argv[0]);
				usage(argv[0]);
			}

			cmd = &argv[i+1];
			argv[i] = NULL;
			argc = i;
			break;
		}
	}

	while (1) {
		opt = getopt(argc, argv, "qu:bhdvmc:CV");
		if (opt == EOF) break;

		switch (opt) {
		case 'q':
			ctype = -1;
			break;
		case 'u':
			update = atoi(optarg);
			break;
		case 'h':
			usage(argv[0]);
			break;
		case 'd':
			ctype = 0;
			break;
		case 'v':
		        ctype = 1;
			break;
		case 'm':
			ctype = 3;
			break;
		case 'c':
			custmessage = optarg;
			ctype=2;
			break;
		case 'C':
			ctype = 4;
			break;
		case 'V':
			printf("delay - Version " VERSION "\n"
			       "  Copyright (c) 1998-2002 Tom Rothamel\n"
			       "  This program has ABSOLUTELY NO WARRANTY. It can be distributed under\n"
			       "  the terms of the GNU General Public License.\n");
			exit(0);
		case 'b':
			bell = 1;
			break;
		}

	}

	if (optind >= argc) {
		fprintf(stderr, "%s: You must supply a time to delay.\n", argv[0]);
		usage(argv[0]);
	}

	if (update < 1) {
		fprintf(stderr, "%s: The update rate must be a positive integer.\n", argv[0]);
		exit(1);
	}

	if (!strcmp(argv[optind], "until")) {
		dtime = parsetime(argc-optind, &argv[optind+1]);
		if (!dtime) exit(1);
		dtime -= time(NULL);
	} else {
		dtime = parsestandard(argc-optind, &argv[optind]);
	}

	if (dtime < 0) {
		fprintf(stderr, "%s: You've specified an invalid delay time.\n", argv[0]);
		usage(argv[0]);
	}

	initdelay(1);

	if (ctype == 4) initscr();

	while (dtime > 0) {
		if (ctype >= 0) showcount(dtime, ctype);

		delay((dtime < update) ? dtime : update, 1); /* always precise. */
		dtime -= (dtime < update) ? dtime : update;
	}

	if (ctype >= 0) {
		showcount(dtime, ctype);
		if (ctype != 4) printf("\n");
	}

	if (ctype == 4) endwin();

	if (bell) printf("\a");

	if (cmd) {
		execvp(cmd[0], cmd);
		perror(argv[0]); /* If it worked, we won't get here. */
		exit(1);
	}

	exit(0);
}
