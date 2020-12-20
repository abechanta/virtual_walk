#include <windows.h>
#include <stdio.h>
#include "VirtualWalk.h"

int main(int argc, char** argv)
{
	DWORD procID = 0;
	if (argc > 1) {
		sscanf(*++argv, "%li", &procID);
	}
	//printf( "ローカル変数のアドレス: 0x%08lx\n", &procID);
	return VirtualWalk(procID);
}
