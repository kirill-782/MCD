#ifndef INCLUDES_H
#define INCLUDES_H

#ifdef WIN32

#ifdef GHOST_EXPORTS
#define GAPI __declspec(dllexport) 
#else
#define GAPI __declspec(dllimport) 
#endif

#else
#define GAPI 
#endif


#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>


using namespace std;

typedef vector<unsigned char> BYTEARRAY;

void CONSOLE_Print( string s );
GAPI vector<string> *CONSOLE_Get( );

#endif
