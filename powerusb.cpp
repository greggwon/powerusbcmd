#include "powerusb.h"
#include "LinuxPowerUSB.hpp"

void usage(char **argv) {
	fprintf( stderr, "Usage:\n");
	fprintf( stderr, "  1) %s portnumber state       ---Sets the state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3, state = on/off\n");			
	fprintf( stderr, "     Ex: powerusb 2 on               ---Switches on Port 2---\n");
	fprintf( stderr, "  2) %s -d portnumber state    ---Sets the default state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3, state = on/off\n");
	fprintf( stderr, "     Ex: powerusb -d 2 on            ---Sets the default state of Port 2 to on---\n");			
	fprintf( stderr, "  3) %s portnumber             ---Reads the state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3\n");
	fprintf( stderr, "     Ex: powerusb 2                  ---Returns the state of Port 2---\n");
	fprintf( stderr, "  4) %s -d portnumber          ---Reads the default state of the respective port---\n", argv[0]);
	fprintf( stderr, "     portnumber = 1-3\n");
	fprintf( stderr, "     Ex: powerusb -d 2               ---Returns the default state of Port 2---\n");
}

int handleRequest(LinuxPowerUSB &lp, int argc, char* argv[])
{
	if(argc == 1)
	{
		fprintf(stderr, "%s: missing file operand\n", argv[0]);
		fprintf(stderr, "Try `%s --help` for more information.\n", argv[0]);
		return 1;
	}
	else if(argc == 2)
	{
		if(strcmp(argv[1], "--help") == 0)
		{
			usage(argv);
			return 1;
		}
		else
		{
			int p = atoi( argv[1] );
			int ps[3] = { p ==1, p ==2, p ==3 };
			if( p > 0 && p < 4 ) {
				if(lp.pwrusb.readPortState(&ps[0], &ps[1], &ps[2]) > 0 ) {
					lp.reportStatus(ps[p-1], p);
				}
			}
			else
			{
				fprintf(stderr,"Invalid Argument: Try `%s --help` for more information.\n", argv[0]);
				return 1;
			}
		}
	}
	else if(argc == 3)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			int p = atoi( argv[2] );
			int ps[3] = { p == 1, p == 2, p == 3 };
			if( p > 0 && p < 4 ) {
				if( lp.pwrusb.readDefaultPortState(&ps[0], &ps[1], &ps[2]) > 0 ) {
					lp.reportDefault(ps[p-1], p);
				}
				else {
					fprintf( stderr, "Could not read defaults for %d\n", p);
					return 1;
				}
			}
			else
			{
				fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);
				return 1;
			}
		}
		else
		{
			int p = atoi(argv[1]);
			bool how = strcmp(argv[2],"on") == 0;  // match "off" == 0
			
			if( p > 0 && p < 4 ) {
				lp.pwrusb.setPort(
					p == 1 ? how : PWRUSB_NO_CHANGE,
					p == 2 ? how : PWRUSB_NO_CHANGE,
					p == 3 ? how : PWRUSB_NO_CHANGE );
			}
			else
			{
				fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
				return 1;
			}
		}
	}
	else if(argc == 4)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			int p = atoi( argv[2] );
			int how = strcmp( argv[3], "on" ) == 0;
			
			if( p > 0 && p < 4 ) {
				lp.pwrusb.setDefaultState(

					p == 1 ? how : PWRUSB_NO_CHANGE,
					p == 2 ? how : PWRUSB_NO_CHANGE,
					p == 3 ? how : PWRUSB_NO_CHANGE );
			}
			else
			{
				fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
				return 1;
			}
		}
		else
		{
			fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
			return 1;
		}
	}
	else
	{
		fprintf(stderr,"Incorrect Usage: Try `%s --help` for more information.\n", argv[0]);
		return 1;
	}
	return 0;
}



int main(int argc, char* argv[])
{
	LinuxPowerUSB p;
	p.setDebug(false);
	p.Setup();
	return handleRequest(p, argc, argv);
}
