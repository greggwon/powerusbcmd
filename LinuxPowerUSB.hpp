#ifndef __LINUX_POWER_USB_H__
#define __LINUX_POWER_USB_H__

#include"powerusb.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <PwrUSBImp.h>

class LinuxPowerUSB
{
private:
	static bool debugging;

	template <typename T, typename... Args>
	void report( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		printf( "%s", buf );
	}

	template <typename T, typename... Args>
	void error( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		fprintf( stderr, "%s", buf );
	}

	template <typename T, typename... Args>
	void debug( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		if( debugging ) printf( "%s", buf );
	}

public :
	PowerUSB pwrusb;

	LinuxPowerUSB() :pwrusb()
	{
	}
	virtual ~LinuxPowerUSB() {}

	void setDebug(bool how) { debugging = how; PowerUSB::debugging = how; }

	void Setup() {
		init();
		checkStatus();
	}

	void  init()
	{
		int i = -1;
		int v = pwrusb.init( &i );
		debug("init(&i) = %d max units, model=%d\n", v, i );
	}

	bool  checkStatus()
	{
		int status = pwrusb.checkStatus();
		debug("checkStatus = %d\n", status);
		if(!status)
		{
			error("LinuxPowerUSB Not Connected\n");
		}
		return status;
	}

	void  reportStatus(int status, int port)
	{
		if(status == 0) report("Port %d = Off\n", port);
		if(status == 1) report("Port %d = On\n", port);
	}

	void  reportDefault(int status, int port)
	{
		if(status == 0) report("Default Port %d State = Off\n", port);
		else if(status == 1) report("Default Port %d State = On\n", port);
		else error("unknown default status: %d for port %d\n", status, port );
	}
};

bool LinuxPowerUSB::debugging;
#endif
