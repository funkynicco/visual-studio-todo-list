#include "StdAfx.h"
#include "VCSolution.h"

#include <regex>

CVCSolution::CVCSolution()
{
}

CVCSolution::~CVCSolution()
{
	for (vector<CVCProject*>::iterator it = m_projects.begin(); it != m_projects.end(); ++it)
		delete *it;
	m_projects.clear();
}

BOOL CVCSolution::Open(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
		return FALSE;

	char old_working_directory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, old_working_directory);
	SetCurrentDirectory(GetDirectoryName(filename).c_str());

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* mem = (char*)malloc(size + 1);

	long pos = 0;
	while (pos < size)
	{
		size_t n = fread(mem + pos, 1, min(16384, size - pos), fp);
		if (n == 0)
		{
			free(mem);
			fclose(fp);
			SetCurrentDirectory(old_working_directory);
			return FALSE;
		}

		pos += n;
	}

	fclose(fp);

	mem[size] = 0;

	// load

	string s(mem);
	smatch m;
	regex e("Project\\(\"\\{[a-zA-Z0-9-]+\\}\"\\)\\s*=\\s*\"([^\"]+)\",\\s+\"([^\"]+)\",\\s*\"\\{[a-zA-Z0-9-]+\\}\"");

	int i = 1;

	while (regex_search(s, m, e))
	{
		auto n = m.size();
		if (n == 3)
		{
			auto name = m[1];
			auto filename = m[2];

			if (_strcmpi(filename.str().c_str(), "Unit Tests") != 0) // Don't attempt to load Unit Tests projects (unsupported)
			{
				auto project = new CVCProject(name.str());
				if (!project->Open(filename.str().c_str()))
				{
					printf(__FUNCTION__ " - Error: Couldn't load project '%s'\n", filename.str().c_str());
					delete project;
				}
				else
					m_projects.push_back(project);
			}
		}

		s = m.suffix().str();
	}

	free(mem);

	SetCurrentDirectory(old_working_directory);

	return TRUE;
}