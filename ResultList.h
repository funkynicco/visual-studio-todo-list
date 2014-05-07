#pragma once

#define MAX_COMMENT_SIZE 256

struct FileCommentResult
{
    char szFile[ MAX_PATH ];
    int nFileLength;
    int nLine;
    char szComment[ MAX_COMMENT_SIZE ];
    int nCommentLength;

    FileCommentResult* prev;
    FileCommentResult* next;
};

class CResultList
{
public:
    CResultList();
    virtual ~CResultList();
    BOOL Add( const char* file, int line, const char* comment );

    inline FileCommentResult* GetHead() { return m_pHead; }

private:
    FileCommentResult* m_pHead;
    FileCommentResult* m_pTail;
    FileCommentResult* m_pFreeList;
    list<void*> m_blocks;
};