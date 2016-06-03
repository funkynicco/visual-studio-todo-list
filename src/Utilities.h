#pragma once

BOOL StringBeginsWith(const char* str, const char* find, BOOL bCaseSensitive = TRUE);
BOOL StringEndsWith(const char* str, const char* find, BOOL bCaseSensitive = TRUE);
BOOL GrabFirstLine(const char* str, char* output, size_t sizeOfOutput);
int StringTrim(const char* str, char* output, size_t sizeOfOutput);
BOOL GetFirstWord(const char* str, char* output, size_t sizeOfOutput);
BOOL GetFirstWord2(const char* str, char* output, size_t sizeOfOutput);

string GetFullPath(const string& filename);
string GetFullPath(const string& filename, const string& rootDirectory);
string GetDirectoryName(const string& filename);

/************************************************************************/
/* CWorkingDirectory                                                    */
/************************************************************************/

class CWorkingDirectory
{
public:
	CWorkingDirectory(const char* directory = NULL);
	virtual ~CWorkingDirectory();
	BOOL SetDirectory(const char* directory);
	void Reset();

	inline const char* GetDirectory() { return m_szCurrentDirectory; }
	inline const char* GetOldDirectory() { return m_szOldDirectory; }

private:
	char m_szOldDirectory[MAX_PATH];
	char m_szCurrentDirectory[MAX_PATH];
};