#include "StdAfx.h"
#include "FileCommentScanner.h"

#define MAX_FILESIZE 52428800 // 50 MB

CFileCommentScanner::CFileCommentScanner()
{
	m_lpBuf = m_buffer;
	m_lpEnd = m_buffer + sizeof(m_buffer);
}

CFileCommentScanner::~CFileCommentScanner()
{
	if (m_lpBuf != m_buffer)
		free(m_lpBuf);
}

BOOL CFileCommentScanner::LoadFile(const char* filename)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER li;
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!GetFileSizeEx(hFile, &li) ||
		li.QuadPart > MAX_FILESIZE ||
		!CheckBufferSize((size_t)li.QuadPart + 1))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	DWORD dwSize = (DWORD)li.QuadPart;
	DWORD dwPos = 0;
	DWORD dwRead;

	while (dwPos < dwSize)
	{
		if (!ReadFile(hFile, m_lpBuf + dwPos, min(16384, dwSize - dwPos), &dwRead, NULL))
		{
			CloseHandle(hFile);
			return FALSE;
		}

		dwPos += dwRead;
	}

	*(m_lpBuf + dwPos) = 0;

	CloseHandle(hFile);
	return TRUE;
}

BOOL CFileCommentScanner::CheckBufferSize(size_t nSize)
{
	if (size_t(m_lpEnd - m_lpBuf) < nSize)
	{
		void* block = 0;
		if (m_lpBuf != m_buffer)
			block = realloc(m_lpBuf, nSize);
		else
			block = malloc(nSize);

		if (!block)
		{
			free(m_lpBuf);
			return FALSE;
		}

		m_lpBuf = (LPBYTE)block;
		m_lpEnd = m_lpBuf + nSize;
	}

	return TRUE;
}

int CFileCommentScanner::ScanComments(function<void(int line, const char* comment)> method)
{
	int num = 0;
	int line = 1;

	LPCBYTE ptr = m_lpBuf;
	string comment;
	LPCBYTE end = ptr + strlen((const char*)ptr);

	while (ptr < end)
	{
		if (*ptr == '/' &&
			ptr + 1 < end)
		{
			++ptr;
			if (*ptr == '/')
			{
				++ptr;

				comment.clear();

				while (ptr < end &&
					*ptr != '\n')
				{
					if (*ptr != '\r')
						comment += *ptr;

					++ptr;
				}

				if (comment.length() > 0)
				{
					method(line, comment.c_str());
					++num;
				}

				++line;
			}
			else if (*ptr == '*')
			{
				++ptr;

				int commentLines = 0;

				comment.clear();

				while (ptr < end)
				{
					if (*ptr == '*' &&
						ptr + 1 < end &&
						*(ptr + 1) == '/')
					{
						break;
					}
					else
					{
						if (*ptr == '\n')
							++commentLines;
						if (*ptr != '\r')
							comment += *ptr;
					}

					++ptr;
				}

				if (comment.length() > 0)
				{
					method(line + (commentLines - 1), comment.c_str());
					++num;
				}

				line += commentLines;
			}
		}
		else if (*ptr == '"')
		{
			++ptr;
			LPCBYTE begin = ptr;
			while (ptr < end)
			{
				if (*ptr == '"')
				{
					if (ptr > begin &&
						*(ptr - 1) == '\\')
					{
						// the " was commented out, as in like: "this is a \"quote\""
					}
					else
						break; // stop reading the string, we reached the end of it
				}
				ptr++;
			}
		}
		else if (*ptr == '\n')
			++line;

		++ptr;
	}

	return num;
}