# Microsoft Developer Studio Project File - Name="Construct" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Construct - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Construct.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Construct.mak" CFG="Construct - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Construct - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Construct - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Construct - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Construct - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir "..\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\irrlicht\include" /I "..\audiere\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /out:"..\construct.exe" /pdbtype:sept /libpath:"..\Debug" /libpath:"..\audiere\lib"

!ENDIF 

# Begin Target

# Name "Construct - Win32 Release"
# Name "Construct - Win32 Debug"
# Begin Group "scenario"

# PROP Default_Filter ""
# Begin Group "city"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCityEscapeScenario.h
# End Source File
# Begin Source File

SOURCE=.\CCityInsertionScenario.h
# End Source File
# Begin Source File

SOURCE=.\ICityScenario.cpp
# End Source File
# Begin Source File

SOURCE=.\ICityScenario.h
# End Source File
# End Group
# Begin Group "castle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCastleScenario.cpp
# End Source File
# Begin Source File

SOURCE=.\CCastleScenario.h
# End Source File
# End Group
# Begin Group "maze"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CElevator.h
# End Source File
# Begin Source File

SOURCE=.\CMazeScenario.cpp
# End Source File
# Begin Source File

SOURCE=.\CMazeScenario.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\IScenario.cpp
# End Source File
# Begin Source File

SOURCE=.\IScenario.h
# End Source File
# End Group
# Begin Group "menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CMainMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CMainMenu.h
# End Source File
# Begin Source File

SOURCE=.\CStartupMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CStartupMenu.h
# End Source File
# End Group
# Begin Group "person"

# PROP Default_Filter ""
# Begin Group "operative"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\COperativePerson.cpp
# End Source File
# Begin Source File

SOURCE=.\COperativePerson.h
# End Source File
# End Group
# Begin Group "drone"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDronePerson.cpp
# End Source File
# Begin Source File

SOURCE=.\CDronePerson.h
# End Source File
# End Group
# Begin Group "ai"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CFollowerAI.cpp
# End Source File
# Begin Source File

SOURCE=.\CFollowerAI.h
# End Source File
# End Group
# Begin Group "civilians"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCivilianPerson.cpp
# End Source File
# Begin Source File

SOURCE=.\CCivilianPerson.h
# End Source File
# End Group
# Begin Group "ally"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CAllyPerson.cpp
# End Source File
# Begin Source File

SOURCE=.\CAllyPerson.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\IPerson.cpp
# End Source File
# Begin Source File

SOURCE=.\IPerson.h
# End Source File
# End Group
# Begin Group "music"

# PROP Default_Filter ""
# End Group
# Begin Group "registry"

# PROP Default_Filter ""
# Begin Group "animatable"

# PROP Default_Filter ""
# Begin Group "triggerlocation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CVictoryTriggerLocation.h
# End Source File
# Begin Source File

SOURCE=.\ITriggerLocation.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CAnimatableRegistry.h
# End Source File
# Begin Source File

SOURCE=.\IAnimatable.h
# End Source File
# End Group
# Begin Group "tangible"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CTangibleRegistry.h
# End Source File
# End Group
# Begin Group "damagable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDamagableRegistry.h
# End Source File
# Begin Source File

SOURCE=.\IDamagable.h
# End Source File
# End Group
# Begin Group "fightable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CFightable.h
# End Source File
# Begin Source File

SOURCE=.\IFightable.h
# End Source File
# End Group
# End Group
# Begin Group "projectile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CBulletProjectile.h
# End Source File
# Begin Source File

SOURCE=.\CFireballProjectile.h
# End Source File
# Begin Source File

SOURCE=.\CSlowMotionBulletProjectile.h
# End Source File
# Begin Source File

SOURCE=.\IProjectile.cpp
# End Source File
# Begin Source File

SOURCE=.\IProjectile.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Target
# End Project
