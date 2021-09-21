#include"powerusb.h"
#include <cstddef>
#include <string>

int port11 = 1, port12 = 0, port13 = 0;
int port21 = 0, port22 = 1, port23 = 0;
int port31 = 0, port32 = 0, port33 = 1;

static int debugging = 1;

template <typename T, typename... Args>
static void debug( T fmt, Args... args ) {
        int len = std::snprintf( nullptr, 0, fmt, args... );
        char buf[len+100];
        std::snprintf( buf, sizeof(buf), fmt, args... );
        if( debugging ) printf( "%s", buf );
}

LinuxPowerUSB :: LinuxPowerUSB() :pwrusb()
{
}

void LinuxPowerUSB::Setup() {
	//loadSharedLibrary();
	initPowerUSB();
	checkStatusPowerUSB();
}

void LinuxPowerUSB :: loadSharedLibrary()
{
	sharedLibraryHandle = dlopen ("/usr/local/lib/libpowerusb.so", RTLD_LAZY);
	if (!sharedLibraryHandle) 
	{
            fputs (dlerror(), stderr);
	    sharedLibraryHandle = NULL;
            exit(1);
        }
}

void LinuxPowerUSB :: initPowerUSB()
{
	int i = -1;
	int v = pwrusb.init( &i );
	debug("init(&i) = %d max units, model=%d\n", v, i );
}

void LinuxPowerUSB :: checkStatusPowerUSB()
{
	int status = pwrusb.checkStatus();
	debug("checkStatus = %d\n", status);
	if(!status)
	{
		fprintf(stderr, "LinuxPowerUSB Not Connected\n");
		exit(1);
	}
}

void usage(char **argv) {
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  1) %s portnumber state       ---Sets the state of the respective port---\n", argv[0]);
	fprintf(stderr, "     portnumber = 1-3, state = on/off\n");			
	fprintf(stderr, "     Ex: powerusb 2 on               ---Switches on Port 2---\n");
	fprintf(stderr, "  2) %s -d portnumber state    ---Sets the default state of the respective port---\n", argv[0]);
	fprintf(stderr, "     portnumber = 1-3, state = on/off\n");
	fprintf(stderr, "     Ex: powerusb -d 2 on            ---Sets the default state of Port 2 to on---\n");			
	fprintf(stderr, "  3) %s portnumber             ---Reads the state of the respective port---\n", argv[0]);
	fprintf(stderr, "     portnumber = 1-3\n");
	fprintf(stderr, "     Ex: powerusb 2                  ---Returns the state of Port 2---\n");
	fprintf(stderr, "  4) %s -d portnumber          ---Reads the default state of the respective port---\n", argv[0]);
	fprintf(stderr, "     portnumber = 1-3\n");
	fprintf(stderr, "     Ex: powerusb -d 2               ---Returns the default state of Port 2---\n");
	exit( 2);
}

int LinuxPowerUSB :: handleRequest(int argc, char* argv[])
{
	debug( "handleRequest( %d, ... )\n", argc );
	if(argc == 1)
	{
		fprintf(stderr, "%s: missing file operand\n", argv[0]);
		fprintf(stderr, "Try `%s --help` for more information.\n", argv[0]);
	}
	else if(argc == 2)
	{
		if(strcmp(argv[1], "--help") == 0)
		{
			usage(argv);
		}
		else
		{
			int p1, p2, p3;
			int p = atoi( argv[1] );
			if( p > 0 && p < 4 ) {
				readPortStatus(pwrusb.readPortState(&p1, &p2, &p3), p);
			}
			else
			{
				fprintf(stderr,"Invalid Argument: Try `%s --help` for more information.\n", argv[0]);
			}
		}
	}
	else if(argc == 3)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			int p1, p2, p3;
			int p = atoi( argv[1] );
			if( p > 0 && p < 4 ) {
				readDefaultPortStatus(pwrusb.readDefaultPortState(&p1, &p2, &p3), p);
			}
			else
			{
				fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);
			}
		}
		else
		{
			int p = atoi(argv[1]);
			bool how = strcmp(argv[2],"off");  // match "off" == 0
			
			if( p > 0 && p < 4 ) {
				pwrusb.setPort(
					p == 1 ? how : -1,
					p == 2 ? how : -1,
					p == 3 ? how : -1 );
			}
			else
			{
				fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
			}
		}
	}
	else if(argc == 4)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			int p = atoi( argv[2] );
			int how = strcmp( argv[3], "off" );
			
			if( p > 0 && p < 4 ) {
				pwrusb.setDefaultState(
					p == 1 ? how : -1,
					p == 2 ? how : -1,
					p == 3 ? how : -1 );
			}
			else
			{
				fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
			}
		}
		else
		{
			fprintf(stderr,"Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
		}
	}
	else
	{
		fprintf(stderr,"Incorrect Usage: Try `%s --help` for more information.\n", argv[0]);
	}
	return 1;
}

void LinuxPowerUSB :: readPortStatus(int status, int port)
{
	if(status == 0)printf("Port %d = Off\n", port);
	if(status == 1)printf("Port %d = On\n", port);
}

void LinuxPowerUSB :: readDefaultPortStatus(int status, int port)
{
	if(status == 0)printf("Default Port %d State = Off\n", port);
	if(status == 1)printf("Default Port %d State = On\n", port);
}

int main(int argc, char* argv[])
{
	LinuxPowerUSB p;
	p.Setup();
	p.handleRequest(argc, argv);
	return 1;
}
