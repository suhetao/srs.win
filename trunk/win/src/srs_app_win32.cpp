#include <srs_app_win32.hpp>

void srs_close_stfd(SOCKET& fd)
{
    if (fd != INVALID_SOCKET) {
		closesocket(fd);
		fd = INVALID_SOCKET;
	}
}
