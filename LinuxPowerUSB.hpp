#ifndef __LINUX_POWER_USB_H__
#define __LINUX_POWER_USB_H__

#include"powerusb.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <cstdarg>
#include <PwrUSBImp.h>

class LinuxPowerUSBError : public virtual std::exception {
	std::string message;
public:
	template <typename T, typename... Args>
	LinuxPowerUSBError( T msg, Args... args ) : std::exception() {
		int len = std::snprintf( nullptr, 0, msg, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), msg, args... );
		message = std::string(buf);
	}
	virtual const char* what() const throw() {
		return message.c_str();
	}
};

class LinuxPowerUSBArgsError : public virtual LinuxPowerUSBError {
public:
	template <typename T, typename...Args>
	LinuxPowerUSBArgsError( T msg, Args...args ) : LinuxPowerUSBError(msg, args...) {}
};

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
	void debug( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		if( debugging ) printf( "%s", buf );
	}

public :
	PowerUSB pwrusb;

	template <typename T, typename... Args>
	void error( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		fprintf( stderr, "%s", buf );
	}
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
			throw LinuxPowerUSBError("LinuxPowerUSB Not Connected");
		}
		return status;
	}

	void setPortDefaultOn( int p ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setDefaultState( 
				p == 1 ? true : PWRUSB_NO_CHANGE,
				p == 2 ? true : PWRUSB_NO_CHANGE,
				p == 3 ? true : PWRUSB_NO_CHANGE );
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setPortDefaultOff( int p ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setDefaultState( 
				p == 1 ? false : PWRUSB_NO_CHANGE,
				p == 2 ? false : PWRUSB_NO_CHANGE,
				p == 3 ? false : PWRUSB_NO_CHANGE );
			return;
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setPortDefaultState( int p, bool how ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setDefaultState( 
				p == 1 ? how : PWRUSB_NO_CHANGE,
				p == 2 ? how : PWRUSB_NO_CHANGE,
				p == 3 ? how : PWRUSB_NO_CHANGE );
			return;
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setDefaults( int state1, int state2, int state3 ) {
		pwrusb.setDefaultState( state1, state2, state3 );
	}

	void setPortStates( int state1, int state2, int state3 ) {
		pwrusb.setPort( state1, state2, state3 );
	}
	void setPortState( int p, bool state ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setPort(
				p == 1 ? state : PWRUSB_NO_CHANGE,
				p == 2 ? state : PWRUSB_NO_CHANGE,
				p == 3 ? state : PWRUSB_NO_CHANGE );
			return;
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setPortOn( int port ) {
		setPortState( port, true );
	}
	void setPortOff( int port ) {
		setPortState( port, false );
	}
	bool getPortState( int p ) { 
		int ps[3] = { p ==1, p ==2, p ==3 };
		if( p > 0 && p < 4 ) {
			if(pwrusb.readPortState(&ps[0], &ps[1], &ps[2]) > 0 ) {
				return(ps[p-1] );
			}
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}

	bool getPortDefaultState( int p ) {
		int ps[3] = { p == 1, p == 2, p == 3 };
		if( p > 0 && p < 4 ) {
			if( pwrusb.readDefaultPortState(&ps[0], &ps[1], &ps[2]) > 0 ) {
				return ps[p-1];
			}
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}

	void  reportStatus(int status, int port, bool verbose = true)
	{
		if( verbose ) report("Port %d = %s\n", port, status ? "On" : "Off");
		else report("%s\n", status ? "On" : "Off");
	}

	void  reportDefault(int status, int port, bool verbose = true)
	{
		if( verbose ) report("Default Port %d State = %s\n", port, status ? "On" : "Off");
		else report("%s\n", status ? "On" : "Off");
	}
};

bool LinuxPowerUSB::debugging;
#endif
