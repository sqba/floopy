// OutputCache.cpp: implementation of the COutputCache class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "OutputCache.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutputCache::COutputCache()
{
	m_pHead = NULL;
}

COutputCache::~COutputCache()
{

}

void COutputCache::Add(char *signature, int position, BYTE *buff, int size)
{
	if(m_pHead == NULL)
	{
		m_pTail = m_pHead = new CCachedBuffer(signature, position, buff, size);
	}
	else
	{
		m_pTail->Add(signature, position, buff, size);
	}
}

int COutputCache::Get(char *signature, int position, BYTE *buff, int size)
{
	if(m_pHead == NULL)
	{
		return m_pHead->Get(signature, position, buff, size);
	}
	return 0;
}





// CCachedBuffer
COutputCache::CCachedBuffer::CCachedBuffer(char *signature, int position, BYTE *buff, int size)
{
	m_buff = new BYTE(size);
	memcpy(m_buff, buff, size);
}

COutputCache::CCachedBuffer::~CCachedBuffer()
{
	delete m_buff;
}

void COutputCache::CCachedBuffer::GetBuffer(BYTE *buff, int size)
{
	memcpy(buff, m_buff, size);
}

void COutputCache::CCachedBuffer::Add(char *signature, int position, BYTE *buff, int size)
{
	int firstNum = atoi( (const char*)signature[0] );
	m_Next[firstNum] = new CCachedBuffer(signature, position, buff, size);
}

int COutputCache::CCachedBuffer::Get(char *signature, int position, BYTE *buff, int size)
{
	if(signature == NULL)
		return 0;
	int firstNum = atoi( (const char*)signature[0] );
	//m_Next[firstNum]->Get();
	return 0;
}
