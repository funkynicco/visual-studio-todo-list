#pragma once

class CFileCommentScanner
{
public:
    CFileCommentScanner();
    virtual ~CFileCommentScanner();
    BOOL LoadFile( const char* filename );
    int ScanComments( function<void( int line, const char* comment )> method );

private:
    BOOL CheckBufferSize( size_t nSize );
    BYTE m_buffer[ 65536 ];
    LPBYTE m_lpBuf; // start
    LPBYTE m_lpEnd; // end
//    LPBYTE m_lpCur; // current
};