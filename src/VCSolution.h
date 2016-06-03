#pragma once

#include "VCProject.h"

class CVCSolution
{
public:
    CVCSolution();
    virtual ~CVCSolution();

    BOOL Open( const char* filename );
    inline vector<CVCProject*>& GetProjects() { return m_projects; }

private:
    vector<CVCProject*> m_projects;
};