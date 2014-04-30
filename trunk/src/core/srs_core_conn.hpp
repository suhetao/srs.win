/*
The MIT License (MIT)

Copyright (c) 2013 winlin

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SRS_CORE_CONN_HPP
#define SRS_CORE_CONN_HPP

/*
#include <srs_core_conn.hpp>
*/

#include <srs_core.hpp>
#ifndef WIN32
#include <st.h>
#else
#include "util.h"
#endif

class SrsServer;
class SrsConnection
{
protected:
	SrsServer* server;
#ifndef WIN32
	st_netfd_t stfd;
#else
	DWORD dwThreadId;
	HANDLE hThread;
	SOCKET sock;
#endif

public:
#ifndef WIN32
	SrsConnection(SrsServer* srs_server, st_netfd_t client_stfd);
#else

	SrsConnection(SrsServer* srs_server, SOCKET client);
#endif
	virtual ~SrsConnection();
public:
	virtual int start();
protected:
	virtual int do_cycle() = 0;
private:
	virtual void cycle();
#ifndef WIN32
	static void* cycle_thread(void* arg);
#else
	static DWORD WINAPI cycle_thread(LPVOID arg);
#endif

};

#endif