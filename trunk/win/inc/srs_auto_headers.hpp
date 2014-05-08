#ifndef SRS_AUTO_HEADER_HPP
#define SRS_AUTO_HEADER_HPP

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include "inttypes.h"
#include "utils.h"

#endif

#define SRS_AUTO_BUILD_DATE "2014-05-04 20:23:56"
#define SRS_AUTO_USER_CONFIGURE
#define SRS_AUTO_CONFIGURE
#define SRS_AUTO_UNAME "modified by hetao.su"
//#define SRS_AUTO_HTTP_SERVER
//#define SRS_AUTO_HTTP_API
//#define SRS_AUTO_HTTP_PARSER
//#define SRS_AUTO_HTTP_CALLBACK

#endif