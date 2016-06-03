#include "StdAfx.h"

BOOL StringBeginsWith(const char* str, const char* find, BOOL bCaseSensitive)
{
	size_t str_len = strlen(str);
	size_t find_len = strlen(find);

	if (find_len > str_len)
		return FALSE;

	if (!bCaseSensitive)
		return memcmp(str, find, find_len) == 0;

	return _memicmp(str, find, find_len) == 0;
}

BOOL StringEndsWith(const char* str, const char* find, BOOL bCaseSensitive)
{
	size_t str_len = strlen(str);
	size_t find_len = strlen(find);

	if (find_len > str_len)
		return FALSE;

	if (bCaseSensitive)
		return _strcmpi(str + str_len - find_len, find) == 0;

	return memcmp(str + str_len - find_len, find, find_len) == 0;
}

BOOL GrabFirstLine(const char* str, char* output, size_t sizeOfOutput)
{
	const char* ptr = str;
	while (*ptr &&
		*ptr != '\n')
		++ptr;

	if (size_t(ptr - str) + 1 > sizeOfOutput)
		return FALSE;

	memcpy(output, str, size_t(ptr - str));
	output[size_t(ptr - str)] = 0;
	return TRUE;
}

int StringTrim(const char* str, char* output, size_t sizeOfOutput)
{
	if (sizeOfOutput == 0)
		return -1;

	if (*str == 0)
	{
		*output = 0;
		return 0;
	}

	const char* ptr = str;
	while (*ptr && isspace(*ptr))
		++ptr;

	const char* ptr_end = ptr;
	while (*ptr_end) // go to end
		++ptr_end;

	// roll back until we either reach the start or a character that is not a whitespace
	while (--ptr_end > ptr && isspace(*ptr_end));

	++ptr_end; // forward 1 character otherwise we miss the last one

	size_t finalSize = size_t(ptr_end - ptr);
	if (finalSize + 1 > sizeOfOutput)
		return -1;

	memcpy(output, ptr, finalSize);
	output[finalSize] = 0;
	return (int)finalSize;
}

// Gets the first identified word from a string
// "- TODO: hi" will extract "TODO"
BOOL GetFirstWord(const char* str, char* output, size_t sizeOfOutput)
{
	char* out = output;

	while (*str)
	{
		if (out + 1 > output + sizeOfOutput)
			return FALSE;

		if (isalpha(*str))
		{
			*out++ = *str;
		}
		else if (out > output)
		{
			*out++ = 0;
			return TRUE;
		}

		++str;
	}

	return FALSE;
}

BOOL GetFirstWord2(const char* str, char* output, size_t sizeOfOutput)
{
	char* out = output;

	while (*str)
	{
		if (out + 1 > output + sizeOfOutput)
			return FALSE;

		if (isalpha(*str))
		{
			*out++ = *str;
		}
		else
		{
			*out = 0;
			return
				out > output &&
				*str != '_' &&
				*str != '-';
		}

		++str;
	}

	return FALSE;
}

string GetFullPath(const string& filename)
{
	char root[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, root);
	return GetFullPath(filename, root);
}

string GetFullPath(const string& filename, const string& rootDirectory)
{
	if (!PathIsRelative(filename.c_str()))
		return filename;

	//char root[ MAX_PATH ];
	char dest[MAX_PATH];
	//GetCurrentDirectory( MAX_PATH, root );
	PathCombine(dest, rootDirectory.c_str(), filename.c_str());

	return dest;
}

string GetDirectoryName(const string& filename)
{
	char temp[MAX_PATH];
	strcpy(temp, filename.c_str());
	PathRemoveFileSpec(temp);
	return temp;
}

/************************************************************************/
/* CWorkingDirectory                                                    */
/************************************************************************/

CWorkingDirectory::CWorkingDirectory(const char* directory)
{
	GetCurrentDirectory(MAX_PATH, m_szCurrentDirectory);
	strcpy(m_szOldDirectory, m_szCurrentDirectory);

	if (directory)
		SetDirectory(directory);
}

CWorkingDirectory::~CWorkingDirectory()
{
	SetCurrentDirectory(m_szOldDirectory);
}

BOOL CWorkingDirectory::SetDirectory(const char* directory)
{
	if (!SetCurrentDirectory(directory))
		return FALSE;

	strcpy(m_szCurrentDirectory, directory);
	return TRUE;
}

void CWorkingDirectory::Reset()
{
	SetDirectory(m_szOldDirectory);
}