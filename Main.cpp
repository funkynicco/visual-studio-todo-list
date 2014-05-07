#include "StdAfx.h"

static CFileCommentScanner scanner;
static CResultList results;

typedef void( *PF_RecursiveResult )( const char* filename );

void RecursiveFileCheck( string directory, function<void( const char* )> pfnRecursiveResult )
{
    char path[ MAX_PATH ];
    PathCombine( path, directory.c_str(), "*.*" );

    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile( path, &wfd );

    if( hFind == INVALID_HANDLE_VALUE )
        return;

    do
    {
        if( *wfd.cFileName != '.' )
        {
            char fullpath[ MAX_PATH ];
            PathCombine( fullpath, directory.c_str(), wfd.cFileName );

            if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                RecursiveFileCheck( fullpath, pfnRecursiveResult );
            }
            else
            {
                // a file
                pfnRecursiveResult( fullpath );
            }
        }
    }
    while( FindNextFile( hFind, &wfd ) );
}

void CheckFileType( const char* name )
{
    while( *name && *name == '*' )
        ++name;

    if( *name == 0 )
        return;

    RecursiveFileCheck( "", [name]( const char* filename )
    {
        if( StringEndsWith( filename, name, FALSE ) )
        {
            if( scanner.LoadFile( filename ) )
            {
                scanner.ScanComments( [filename]( int line, const char* comment )
                {
                    char trimmed[ 1024 ] = { 0 };
                    if( StringTrim( comment, trimmed, sizeof( trimmed ) ) > 0 )
                    {
                        char word[ 1024 ] = { 0 };
                        if( GetFirstWord( trimmed, word, sizeof( word ) ) )
                        {
                            if( _strcmpi( word, "todo" ) == 0 ||
                                _strcmpi( word, "task" ) == 0 ||
                                _strcmpi( word, "hack" ) == 0 )
                            {
                                if( !results.Add( filename, line, trimmed ) )
                                    printf( "Failed to add comment at %s:%s ('%s') to result list\n", filename, line, trimmed );
                            }
                        }
                    }
                } );
            }
            else
                printf( "\tFailed to load '%s'!\n", filename );
        }
    } );
}

void DoProcess()
{
    CheckFileType( "*.cpp" );
    CheckFileType( "*.h" );
    CheckFileType( "*.hpp" );
    CheckFileType( "*.cs" );
    CheckFileType( "*.js" );

    // TODO: Add more types here


    // Print out the result

    int num = 0;

    int maxLen = 0;
    FileCommentResult* ptr = results.GetHead();
    while( ptr )
    {
        // calculate the maximum length for padding out the filename including the line number
        // we estimate the maximum lines of code there is to be maximum of 99999 per file
        // myfile.cpp(99999):
        // in the above, "(99999):" represents 8 characters long, thus we're adding 8 below to FileLength (length of "myfile.cpp")
        maxLen = max( maxLen, ptr->nFileLength + 8 );
        ptr = ptr->next;
        ++num;
    }

    printf( "%-*s Comment\n", maxLen, "Filename" );
    for( int i = 0; i < maxLen + 60; ++i )
        printf( "-" );
    printf( "\n" );

    char temp[ 268 ];

    ptr = results.GetHead(); // reset and lets print out the files
    while( ptr )
    {
        sprintf( temp, "%s(%d):", ptr->szFile, ptr->nLine );
        printf( "%-*s %s\n", maxLen, temp, ptr->szComment );
        ptr = ptr->next;
    }

    for( int i = 0; i < maxLen + 60; ++i )
        printf( "-" );
    printf( "\n" );
    printf( "%d task%s in list\n", num, num == 1 ? "" : "s" );
}

int main( int argc, char* argv[] )
{
    setbuf( stdout, NULL ); // Disable buffering

    if( argc < 2 )
    {
        printf( "Must provide a directory to scan.\n" );
        return 1;
    }

    char oldDirectory[ MAX_PATH ];
    if( !GetCurrentDirectory( sizeof( oldDirectory ), oldDirectory ) )
    {
        printf( "Could not receive current directory (code: %u).\n", GetLastError() );
        return 1;
    }

    char newPath[ MAX_PATH ] = { 0 };

    if( PathIsRelative( argv[ 1 ] ) )
        PathCombine( newPath, oldDirectory, argv[ 1 ] );
    else
        strcpy( newPath, argv[ 1 ] );

    if( !SetCurrentDirectory( newPath ) )
    {
        printf( "Failed to set working directory to '%s'.\n", newPath );
        return 1;
    }

    DoProcess();

    SetCurrentDirectory( oldDirectory ); // reset command line

    return 0;
}