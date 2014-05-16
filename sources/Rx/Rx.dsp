# Microsoft Developer Studio Project File - Name="Rx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Rx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Rx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Rx.mak" CFG="Rx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Rx - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Rx - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Rx - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy help\Rx.hlp Release\Rx.hlp	copy help\Rx.cnt Release\Rx.cnt
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Rx - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy help\Rx.hlp Debug\Rx.hlp	copy help\Rx.cnt Debug\Rx.cnt
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Rx - Win32 Release"
# Name "Rx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Dib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDib.cpp
# End Source File
# Begin Source File

SOURCE=.\DibHistogram.cpp
# End Source File
# Begin Source File

SOURCE=.\DibInformation.cpp
# End Source File
# Begin Source File

SOURCE=.\DibTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\DibTrackerManager.cpp
# End Source File
# End Group
# Begin Group "Vector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AVector.cpp
# End Source File
# Begin Source File

SOURCE=.\AVectorManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MetricDialog.cpp
# End Source File
# End Group
# Begin Group "Annotations"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Annotation.cpp
# End Source File
# Begin Source File

SOURCE=.\AnnotationManager.cpp
# End Source File
# End Group
# Begin Group "Base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ObjectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBase.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorObject.cpp
# End Source File
# End Group
# Begin Group "Document/Vue"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BufferView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Rx.cpp
# End Source File
# Begin Source File

SOURCE=.\Rx.rc
# End Source File
# Begin Source File

SOURCE=.\RxDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\RxView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Automation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ToolCervicales.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ATraceReport.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\res\Application 2000.ico"
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Rx.ico
# End Source File
# Begin Source File

SOURCE=.\res\Rx.rc2
# End Source File
# Begin Source File

SOURCE=.\res\RxDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Dib Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDib.h
# End Source File
# Begin Source File

SOURCE=.\DibHistogram.h
# End Source File
# Begin Source File

SOURCE=.\DibInformation.h
# End Source File
# Begin Source File

SOURCE=.\DibTracker.h
# End Source File
# Begin Source File

SOURCE=.\DibTrackerManager.h
# End Source File
# End Group
# Begin Group "Vector Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AVector.h
# End Source File
# Begin Source File

SOURCE=.\AVectorManager.h
# End Source File
# Begin Source File

SOURCE=.\MetricDialog.h
# End Source File
# End Group
# Begin Group "Annotations Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Annotation.h
# End Source File
# Begin Source File

SOURCE=.\AnnotationManager.h
# End Source File
# End Group
# Begin Group "Base Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ObjectManager.h
# End Source File
# Begin Source File

SOURCE=.\ToolBase.h
# End Source File
# Begin Source File

SOURCE=.\VectorObject.h
# End Source File
# End Group
# Begin Group "Document/Vue Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BufferView.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "Rx - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=Rx
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Rx - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=Rx
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Rx.h
# End Source File
# Begin Source File

SOURCE=.\RxDoc.h
# End Source File
# Begin Source File

SOURCE=.\RxView.h
# End Source File
# End Group
# Begin Group "Automation Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ToolCervicales.h
# End Source File
# End Group
# Begin Group "Other Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ATraceReport.h
# End Source File
# Begin Source File

SOURCE=.\defuns.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Rx.reg
# End Source File
# End Target
# End Project
