#include "StdAfx.h"
#include "ResultList.h"

CResultList::CResultList() :
m_pHead( NULL ),
m_pTail( NULL ),
m_pFreeList( NULL )
{

}

CResultList::~CResultList()
{
    for( list<void*>::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it )
        free( *it );
}

BOOL CResultList::Add( const char* file, int line, const char* comment )
{
    if( strlen( file ) >= MAX_PATH ||
        strlen( comment ) >= MAX_COMMENT_SIZE )
        return FALSE;

    FileCommentResult* node = m_pFreeList;
    if( m_pFreeList == NULL )
    {
        // allocate a few nodes
        size_t count = 64;
        void* mem = malloc( sizeof(FileCommentResult)* count );
        if( mem == NULL )
            throw __FUNCTION__ " - FATAL: malloc returned NULL";

        m_blocks.push_back( mem );

        node = (FileCommentResult*)mem; // first node in the data block assigned

        for( size_t i = 1; i < count; ++i )
        {
            FileCommentResult* this_node = (FileCommentResult*)mem + i;

            this_node->next = m_pFreeList;
            m_pFreeList = this_node; // add to free list
        }
    }
    else
        m_pFreeList = m_pFreeList->next;

    strcpy( node->szFile, file );
    node->nFileLength = (int)strlen( file );
    node->nLine = line;
    strcpy( node->szComment, comment );
    node->nCommentLength = (int)strlen( comment );

    // append to list
    if( m_pTail )
        m_pTail->next = node;
    node->prev = m_pTail;
    node->next = NULL;
    m_pTail = node;
    if( m_pHead == NULL )
        m_pHead = node;

    return TRUE;
}