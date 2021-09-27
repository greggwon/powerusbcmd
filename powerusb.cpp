#include "powerusb.h"
#include "LinuxPowerUSB.hpp"

#include <curses.h>

const int numinput = 7;

void usage(char **argv) {
	fprintf( stderr, "Usage: %s [-devw] -p port [-s state]\n", argv[0]);
}

int handleRequest(LinuxPowerUSB &lp, int argc, char* argv[])
{
	bool verbose = false;
	bool defState = false;
	bool useexit = false;
	int port = 0;
	char *state = NULL;
	bool windowing = false;
	bool inputs = false;
	int c;
	while( (c = getopt(argc, argv, "s:p:devwi")) != -1 ) {
		switch( c ) {
			case 'i': inputs = true; break;
			case 'v': verbose = true; break;
			case 'w': windowing = true; break;
			case 'd': defState = true; break;
			case 'p': port = atoi(optarg); break;
			case 's': state = strdup(optarg); break;
			case 'e': useexit = true; break;
			case -1: break;
			default:
				throw LinuxPowerUSBError( "bad argument: '%d'", c);
		}
	}
	if( windowing ) return 5;
	if( port == 0 && !inputs ) {
		throw LinuxPowerUSBError( "missing port #: -p N required");
	}

	bool how = false;
	if( state != NULL ) {
		how = strcasecmp( state, "on") == 0;
	}

	if( port > 0 ) {
		if( defState ) {
			if( state == NULL )
				if( useexit ) return(lp.getPortDefaultState(port));
				else lp.reportDefault(lp.getPortDefaultState(port), port, verbose);
			else
				lp.setPortDefaultState( port, how );

		} else {
			if( state == NULL )
				if( useexit ) return(lp.getPortState(port));
				else lp.reportStatus(lp.getPortState(port), port, verbose);
			else
				lp.setPortState( port, how );
		}
	}
	if( inputs ) {
		int st[8];
		lp.getInputStates(st);
		printf("Inputs: ");
		for( int i = 0; i < numinput; ++i ) {
			printf("%d=%d", i+1, st[i] );
			if( i < 6 ) printf(", ");
		}
		printf("\n");
	}
	return 0;
}



int main(int argc, char* argv[])
{
	LinuxPowerUSB p;
	try {
		const int spc = 8;
		const int plugs = 3;
		const int cw =4;
		p.setDebug(false);
		p.Setup();
		int ret = handleRequest(p, argc, argv);
		if( ret == 5 ) {
			WINDOW *win = NULL;
			bool quit = false;
			initscr();
			cbreak();
			noecho();
			//nonl();
			//intrflush( stdscr, FALSE );
			keypad( stdscr, TRUE );
			nodelay( stdscr, TRUE );
			win = subwin( stdscr, 7, std::max(spc * plugs + 4, numinput * cw + 8), 0, 0 );
			int idx = 0;
			while(!quit) {
				move( 1, 1 );
				printw("plg:");
				for( int i = 0; i < plugs; ++i ) {
					bool st = p.getPortState( i+1 );
					move( 1, (i*spc)+7 );
					if( i == idx ) {
						standout();
					}
					printw("%d", i+1 );
					if( i == idx ) {
						standend();
					}
					addch('=');
					wattron( stdscr, st ? A_BOLD : A_DIM );
					printw("%s", st ? "on" : "off");
					wattroff( stdscr, st ? A_BOLD : A_DIM );
					clrtoeol();
				}
				box( win, '|', '-');
				int ch = getch();
				int sts[numinput];
				p.getInputStates( sts );
				move( 3, 1 );
				printw("inp:");
				for( int i = 0; i < numinput; ++i ) {
					move( 3, 5+(i*cw)+2 );
					printw ("%d",i+1) ;
					move( 4, 5+(i*cw)+1 );
					wattron( stdscr, sts[i] ? A_BOLD : A_DIM );
					printw("%3s", sts[i] ? "on" : "off" );
					wattroff( stdscr, sts[i] ? A_BOLD : A_DIM );
				}
				wrefresh(win);
				refresh();
				switch( ch ) {
					case 'q': quit = true; break;
					case ' ': {
						bool st = p.getPortState( idx+1 );
						p.setPortState( idx+1, !st );
						continue;
						}
					case KEY_BTAB:
						idx = ((idx - 1 ) + plugs) % plugs;
						break;
					case '\t':
						idx = (idx + 1 ) % plugs;
						break;
					case ERR:
						break;
					case '1':
					case '2':
					case '3':
					case '4':
					case '5': {
					    int nidx = ch - '1';
					    if( nidx < plugs ) idx = nidx;
					}
					default:
						move(8, 0);
						printw("key=%d    ", ch );
						break;
				}
				move( 1, (idx*spc)+7 );
				refresh();
				usleep( 50000 );
			}
			endwin();
		}
	} catch( LinuxPowerUSBError &ex ) {
		p.error( "%s: %s\n", argv[0], ex.what() );
		usage( argv );
	}
	//return 2;
}
