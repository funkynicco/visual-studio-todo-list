#pragma once

class CVCProject
{
public:
    CVCProject( string name = "" );
    virtual ~CVCProject();

    BOOL Open( const char* filename );

    inline const string& GetName() { return m_name; }
    inline const string& GetFilename() { return m_filename; }
    inline const string& GetRootDirectory() { return m_rootDirectory; }
    inline vector<string>& GetFiles() { return m_files; }

private:
    string m_name;
    string m_filename;
    string m_rootDirectory;
    vector<string> m_files;
};