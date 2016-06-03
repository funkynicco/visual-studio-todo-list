#include "StdAfx.h"
#include "VCProject.h"

#include "..\lib\rapidxml\rapidxml.hpp"
using namespace rapidxml;

inline void ScanNodes( vector<string>& files, xml_node<>* node, const char* sibling_name, const char* attribute_name )
{
    xml_node<>* child = node->first_node( sibling_name );
    while( child )
    {
        xml_attribute<>* attribute = child->first_attribute( attribute_name );
        if( attribute )
            files.push_back( attribute->value() );

        child = child->next_sibling( sibling_name );
    }
}

CVCProject::CVCProject( string name ) :
m_name( name )
{
}

CVCProject::~CVCProject()
{
}

BOOL CVCProject::Open( const char* filename )
{
    m_filename = filename;

    char root_dir[ MAX_PATH ];
    char temp[ MAX_PATH ];
    strcpy( temp, filename );
    PathRemoveFileSpec( temp );
    if( PathIsRelative( temp ) )
    {
        char current_dir[ MAX_PATH ];

        GetCurrentDirectory( MAX_PATH, current_dir );
        PathCombine( root_dir, current_dir, temp );
    }
    else
        strcpy( root_dir, temp );

    m_rootDirectory = root_dir;

    m_files.clear();

    FILE* fp = fopen( filename, "rb" );
    if( !fp )
        return FALSE;

    fseek( fp, 0, SEEK_END );
    long size = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    char* mem = (char*)malloc( size + 1 );

    long pos = 0;
    while( pos < size )
    {
        size_t n = fread( mem + pos, 1, min( 16384, size - pos ), fp );
        if( n == 0 )
        {
            free( mem );
            fclose( fp );
            return FALSE;
        }

        pos += n;
    }

    fclose( fp );

    mem[ size ] = 0;

    xml_document<> doc;
    doc.parse<0>( mem );

    // load...
    xml_node<>* project_node = doc.first_node( "Project" );
    if( project_node )
    {
        xml_node<>* itemgroup_node = project_node->first_node( "ItemGroup" );
        while( itemgroup_node )
        {
            // C/C++
            ScanNodes( m_files, itemgroup_node, "ClCompile", "Include" );
            ScanNodes( m_files, itemgroup_node, "ClInclude", "Include" );

            // C#
            ScanNodes( m_files, itemgroup_node, "Compile", "Include" );

            itemgroup_node = itemgroup_node->next_sibling( "ItemGroup" );
        }
    }

    free( mem );

    return TRUE;
}