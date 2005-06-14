// Mp3File.h: interface for the CMp3File class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP3FILE_H__A60A6B75_EA30_4E33_9961_BBF355ADA60B__INCLUDED_)
#define AFX_MP3FILE_H__A60A6B75_EA30_4E33_9961_BBF355ADA60B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>



#define BUFF_LEN	20480


typedef BOOL	(FAR PASCAL *FilterGetActualFormat)	(HANDLE hInput,DWORD dwOptions,long * plSamprate, WORD * pwChannels, WORD * pwBitsPerSample);
typedef BOOL	(FAR PASCAL *FilterUnderstandsFormat)(LPSTR filename);
typedef BOOL	(FAR PASCAL *DIALOGMsgProc)			(HWND hWndDlg, UINT Message, WPARAM wParam, LPARAM lParam);
typedef DWORD	(FAR PASCAL *FilterGetOptions)		(HWND hWnd, HINSTANCE hInst, long lSamprate, WORD wChannels, WORD wBitsPerSample, DWORD dwOptions); // return 0 if no options box
typedef DWORD	(FAR PASCAL *FilterSetOptions2)		(HANDLE hInput, DWORD dwOptions, long * plSamprate, WORD * pwChannels, WORD * pwBitsPerSample);
typedef DWORD	(FAR PASCAL *FilterOptions)			(HANDLE hInput);
typedef DWORD	(FAR PASCAL *FilterOptionsString)	(HANDLE hInput, LPSTR szString);
typedef HANDLE	(FAR PASCAL *OpenFilterOutput)		(LPSTR lpstrFilename,long lSamprate,WORD wBitsPerSample,WORD wChannels,long lSize,long far *lpChunkSize,DWORD dwOptions);
typedef void	(FAR PASCAL *CloseFilterOutput)		(HANDLE hOutput);
typedef DWORD	(FAR PASCAL *WriteFilterOutput)		(HANDLE hOutput, unsigned char far *buf, long lBytes);
typedef void	(FAR PASCAL *GetSuggestedSampleType)(long far *lSugSamprate,WORD far *wSugBitsPerSample, WORD far *wSugChannels);

typedef long	(FAR PASCAL *FilterGetFileSize)		(HANDLE hInput);
typedef HANDLE	(FAR PASCAL *OpenFilterInput)		(LPSTR lpstrFilename, long *lSamprate, WORD *wBitsPerSample, WORD *wChannels, HWND hWnd, long *lChunkSize);
typedef void	(FAR PASCAL *CloseFilterInput)		(HANDLE hInput);
typedef DWORD	(FAR PASCAL *ReadFilterInput)		(HANDLE hInput, unsigned char *bufout, long lBytes);



class CMp3File : public IFloopySoundInput
{
public:
	CMp3File();
	virtual ~CMp3File();

	char *GetName()			{ return (strlen(m_filename)>0 ? m_filename : "mp3file"); }
	char *GetDescription()	{ return "Mp3 File Reader"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int Read(BYTE *data, int size);
	void Reset();
	BOOL Open(char *filename);
	void Close();
	int GetSize();

private:
	BYTE *m_pChunk;
	long m_lChunkSize;
	long m_lMaxChunkSize;
	int m_nChunkPos;
	int samplesToBytes();
	char m_filename[MAX_PATH];

	HINSTANCE m_hDllInstance;
	HINSTANCE m_hinstLib;
	TCHAR m_szFileName[MAX_PATH];

	HANDLE m_hInput;

	FilterGetOptions	m_FilterGetOptions;
	FilterGetFileSize	m_FilterGetFileSize;
	OpenFilterInput		m_OpenFilterInput;
	CloseFilterInput	m_CloseFilterInput;
	ReadFilterInput		m_ReadFilterInput;
	FilterSetOptions2	m_FilterSetOptions2;
};

#endif // !defined(AFX_MP3FILE_H__A60A6B75_EA30_4E33_9961_BBF355ADA60B__INCLUDED_)