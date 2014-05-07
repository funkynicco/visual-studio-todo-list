#pragma once

#include <iostream>
#include <Windows.h>
#include <Shlwapi.h>

#pragma comment( lib, "shlwapi.lib" )

#include <string>
#include <functional>
#include <list>
using namespace std;

BOOL StringEndsWith( const char* str, const char* find, BOOL bCaseSensitive = TRUE );
BOOL GrabFirstLine( const char* str, char* output, size_t sizeOfOutput );
int StringTrim( const char* str, char* output, size_t sizeOfOutput );
BOOL GetFirstWord( const char* str, char* output, size_t sizeOfOutput );

#include "FileCommentScanner.h"
#include "ResultList.h"