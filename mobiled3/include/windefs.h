//
//
//

#ifndef _WINDEFS_H
#define _WINDEFS_H_

#ifdef _WIN32
	#ifdef _DLL
		#define WINFLAGS __declspec(dllexport)
	#else
		#define WINFLAGS __declspec(dllimport)
	#endif
#else
	#define WINFLAGS
#endif

#endif
