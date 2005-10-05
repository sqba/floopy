# Microsoft Developer Studio Project File - Name="stdlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=stdlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stdlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stdlib.mak" CFG="stdlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stdlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stdlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/sqba/Floopy2/src/stdlib", EODAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stdlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDLIB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDLIB_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 winmm.lib /nologo /dll /machine:I386 /out:"../../Release/stdlib.dll"

!ELSEIF  "$(CFG)" == "stdlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDLIB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDLIB_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /dll /debug /machine:I386 /out:"../../Debug/stdlib.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "stdlib - Win32 Release"
# Name "stdlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cache\Cache.cpp
# End Source File
# Begin Source File

SOURCE=.\8to16bit\Convert8to16bit.cpp
# End Source File
# Begin Source File

SOURCE=.\echo\Echo.cpp
# End Source File
# Begin Source File

SOURCE=.\invert\Invert.cpp
# End Source File
# Begin Source File

SOURCE=.\loop\Loop.cpp
# End Source File
# Begin Source File

SOURCE=.\mixer\Mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\mono2stereo\Mono2Stereo.cpp
# End Source File
# Begin Source File

SOURCE=.\Performance\Performance.cpp
# End Source File
# Begin Source File

SOURCE=.\playrgn\PlayRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\reverse\Reverse.cpp
# End Source File
# Begin Source File

SOURCE=.\stdlib.cpp
# End Source File
# Begin Source File

SOURCE=.\tonegen\ToneGen.cpp
# End Source File
# Begin Source File

SOURCE=.\track\Track.cpp
# End Source File
# Begin Source File

SOURCE=.\volume\Volume.cpp
# End Source File
# Begin Source File

SOURCE=.\waveout\WaveOut.cpp
# End Source File
# Begin Source File

SOURCE=.\wavfile\WavFileIn.cpp
# End Source File
# Begin Source File

SOURCE=.\wavfile\WavFileOut.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cache\Cache.h
# End Source File
# Begin Source File

SOURCE=.\8to16bit\Convert8to16bit.h
# End Source File
# Begin Source File

SOURCE=.\echo\Echo.h
# End Source File
# Begin Source File

SOURCE=.\wavfile\headers.h
# End Source File
# Begin Source File

SOURCE=.\invert\Invert.h
# End Source File
# Begin Source File

SOURCE=.\loop\Loop.h
# End Source File
# Begin Source File

SOURCE=.\mixer\Mixer.h
# End Source File
# Begin Source File

SOURCE=.\mono2stereo\Mono2Stereo.h
# End Source File
# Begin Source File

SOURCE=.\Performance\Performance.h
# End Source File
# Begin Source File

SOURCE=.\playrgn\PlayRegion.h
# End Source File
# Begin Source File

SOURCE=.\reverse\Reverse.h
# End Source File
# Begin Source File

SOURCE=.\tonegen\ToneGen.h
# End Source File
# Begin Source File

SOURCE=.\track\Track.h
# End Source File
# Begin Source File

SOURCE=.\volume\Volume.h
# End Source File
# Begin Source File

SOURCE=.\waveout\WaveOut.h
# End Source File
# Begin Source File

SOURCE=.\wavfile\WavFileIn.h
# End Source File
# Begin Source File

SOURCE=.\wavfile\WavFileOut.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
