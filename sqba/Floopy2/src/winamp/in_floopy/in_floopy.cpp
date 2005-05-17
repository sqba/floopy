/*
** Example Winamp input plug-in
** Copyright (c) 1998, Justin Frankel/Nullsoft Inc.
*/

#include <windows.h>
#include <mmreg.h>
#include <msacm.h>
#include <math.h>

#include "in2.h"

#include "engine.h"

#define WM_WA_MPEG_EOF WM_USER+2

// raw configuration
#define NCH 2
#define SAMPLERATE 44100
#define BPS 16

//#define BUFF_SIZE	2048
extern In_Module mod;
CEngine engine;
char sample_buffer[576*NCH*(BPS/8)*2]; // sample buffer
//char lastfn[MAX_PATH]; // currently playing file (used for getting info on the current file)
int decode_pos_ms; // current decoding position, in milliseconds
int paused; // are we paused?
int seek_needed; // if != -1, it is the point that the decode thread should seek to, in ms.
int killDecodeThread=0;
HANDLE thread_handle=INVALID_HANDLE_VALUE;
int file_length=0;
BOOL file_open=FALSE;

//int file_length; // file length, in ms
//return (file_length*10)/(SAMPLERATE/100*NCH*(BPS/8)); 


#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) In_Module * winampGetInModule2()
{
	engine.init();
	return &mod;
}
#ifdef __cplusplus
}
#endif


// avoid CRT. Evil. Big. Bloated.
BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}


DWORD WINAPI __stdcall PlayThread(void *b);


void config(HWND hwndParent)
{
	MessageBox(hwndParent,"No configuration.","Configuration",MB_OK);
}
void about(HWND hwndParent)
{
	MessageBox(hwndParent,"Floopy Player\n  Esqban S. Simon","About Floopy Player",MB_OK);
}

void init() { }

void quit() { }

int isourfile(char *fn) 
{ 
	char *ext = strrchr(fn, '.');
	if(ext)
		return(strcmpi(ext, ".xml")?0:1);
	return 0;
}

int play(char *fn)
{ 
	int maxlatency;
	unsigned long thread_id;
	
	if(!engine.Create("engine"))
	{
		char *msg = engine.GetLastErrorDesc();
		if(msg && *msg)
			MessageBox(NULL, msg, "in_floopy", MB_OK);
		else
			MessageBox(NULL, "Failed to create engine.", "in_floopy", MB_OK);
		return 0;
	}

	if(!engine.Open(fn))
	{
		char *msg = engine.GetLastErrorDesc();
		if(msg && *msg)
			MessageBox(NULL, msg, "in_floopy", MB_OK);
		else
			MessageBox(NULL, "Failed to open file.", "in_floopy", MB_OK);
		return 0;
	}

	SOUNDFORMAT *fmt = engine.GetFormat();
	int samplesize = fmt->bitsPerSample / 8;

	maxlatency = mod.outMod->Open(SAMPLERATE,NCH,BPS, -1,-1);
	if (maxlatency < 0) // error opening device
	{
		engine.Close();
		return 1;
	}
	seek_needed=-1;
	file_open = TRUE;

	// Mora ovako zbog toga sto se GetSize izracunava u odnosu
	// na trenutnu poziciju!
	file_length = (engine.GetSize() / fmt->frequency) * 1000;

	// dividing by 1000 for the first parameter of setinfo makes it
	// display 'H'... for hundred.. i.e. 14H Kbps.
	mod.SetInfo((SAMPLERATE*BPS*NCH)/1000,SAMPLERATE/1000,NCH,1);

	// initialize vis stuff
	mod.SAVSAInit(maxlatency,SAMPLERATE);
	mod.VSASetInfo(SAMPLERATE,NCH);

	mod.outMod->SetVolume(-666); // set the output plug-ins default volume

	killDecodeThread=0;
	thread_handle = (HANDLE) CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) PlayThread,(void *) &killDecodeThread,0,&thread_id);
	
	return 0; 
}

void pause() { paused=1; mod.outMod->Pause(1); }
void unpause() { paused=0; mod.outMod->Pause(0); }
int ispaused() { return paused; }

void stop() { 
	if (thread_handle != INVALID_HANDLE_VALUE)
	{
		killDecodeThread=1;
		if (WaitForSingleObject(thread_handle,INFINITE) == WAIT_TIMEOUT)
		{
			MessageBox(mod.hMainWindow,"error asking thread to die!\n","error killing decode thread",0);
			TerminateThread(thread_handle,0);
		}
		CloseHandle(thread_handle);
		thread_handle = INVALID_HANDLE_VALUE;
	}
	if (file_open)
	{
		engine.Close();
		file_open = FALSE;
	}

	mod.outMod->Close();
	mod.SAVSADeInit();
}

int getlength()
{
	//int samplesize = fmt->bitsPerSample / 8;
	//return file_length;
	return file_length;
}
int getoutputtime() { return mod.outMod->GetOutputTime(); }
void setoutputtime(int time_in_ms)
{
	seek_needed=time_in_ms; 
	/*SOUNDFORMAT *fmt = engine.GetFormat();
	int samples = (time_in_ms / 1000) * fmt->frequency;
	engine.MoveTo( samples );*/
}

void setvolume(int volume) { mod.outMod->SetVolume(volume); }
void setpan(int pan) { mod.outMod->SetPan(pan); }

int infoDlg(char *fn, HWND hwnd)
{
	return 0;
}

void getfileinfo(char *filename, char *title, int *length_in_ms)
{
	if (!filename || !*filename)  // currently playing file
	{
		SOUNDFORMAT *fmt = engine.GetFormat();
		if(fmt->frequency > 0)
			*length_in_ms = engine.GetSize() / fmt->frequency * 1000;
		char *name = engine.GetDisplayName();
		if(name)
			wsprintf(title, name);
	}
	else
		wsprintf(title,"%s", strrchr(filename, '\\')+1);
}

void eq_set(int on, char data[10], int preamp) 
{ 
}


In_Module mod = 
{
	IN_VER,
	"Floopy Player "
#ifdef __alpha
	"(AXP)"
#else
	"(x86)"
#endif
	,
	0,	// hMainWindow
	0,  // hDllInstance
	"\0"
	,
	1,	// is_seekable
	1, // uses output
	config,
	about,
	init,
	quit,
	getfileinfo,
	infoDlg,
	isourfile,
	play,
	pause,
	unpause,
	ispaused,
	stop,
	
	getlength,
	getoutputtime,
	setoutputtime,

	setvolume,
	setpan,

	0,0,0,0,0,0,0,0,0, // vis stuff


	0,0, // dsp

	eq_set,

	NULL,		// setinfo

	0 // out_mod

};


/*
int get_576_samples(char *buf)
{
	//memset(buf, 0, sizeof(buf));
	return engine.Read((BYTE*)buf, 576*NCH*(BPS/8));
}
*/

DWORD WINAPI __stdcall PlayThread(void *b)
{
	int done=0;
	while (! *((int *)b) ) 
	{
		if (seek_needed != -1)
		{
			int offs;
			decode_pos_ms = seek_needed-(seek_needed%1000);
			seek_needed=-1;
			done=0;
			mod.outMod->Flush(decode_pos_ms);

			SOUNDFORMAT *fmt = engine.GetFormat();
			offs = (decode_pos_ms / 1000) * fmt->frequency;
			engine.MoveTo( offs );
		}
		if (done)
		{
			mod.outMod->CanWrite();
			if (!mod.outMod->IsPlaying())
			{
				PostMessage(mod.hMainWindow,WM_WA_MPEG_EOF,0,0);
				return 0;
			}
			Sleep(10);
		}
		else if (mod.outMod->CanWrite() >= ((576*NCH*(BPS/8))<<(mod.dsp_isactive()?1:0)))
		{	
			int l=576*NCH*(BPS/8);
			//l=get_576_samples(sample_buffer);
			memset(sample_buffer, 0, sizeof(sample_buffer));
			l = engine.Read((BYTE*)sample_buffer, 576*NCH*(BPS/8));
			if (!l) 
			{
				done=1;
			}
			else
			{
				mod.SAAddPCMData((char *)sample_buffer,NCH,BPS,decode_pos_ms);
				mod.VSAAddPCMData((char *)sample_buffer,NCH,BPS,decode_pos_ms);
				decode_pos_ms+=(576*1000)/SAMPLERATE;
				if (mod.dsp_isactive())
					l=mod.dsp_dosamples((short *)sample_buffer,l/NCH/(BPS/8),BPS,NCH,SAMPLERATE)*(NCH*(BPS/8));
				mod.outMod->Write(sample_buffer,l);
			}
		}
		else Sleep(20);
	}
	return 0;
}