OUTDIR=.\x64
LINK=link.exe

ALL : $(OUTDIR)\callin_multithreads-cache.exe $(OUTDIR)\callout-cache.dll

CLEAN :
        -@erase /Q $(OUTDIR)\*.exe,$(OUTDIR)\*.obj.,$(OUTDIR)\*.pdb,,$(OUTDIR)\*.ilk,$(OUTDIR)\*.dll,$(OUTDIR)\*.exp,$(OUTDIR)\*.lib

CPPFLAGS=\
        /source-charset:utf-8\
        /nologo\
        /GS /W3 /Zc:wchar_t\
        /Fo"$(OUTDIR)\\"\
        /Fd"$(OUTDIR)\\"\
        /c\
        /Zi /O2\
        /MD\
        /DNDEBUG /D_CONSOLE\
        /DUNICODE\
        /D_UNICODE\
        /I"..\\" /I"C:\InterSystems\Cache\dev\callin" 

LINK_FLAGS=\
        /nologo\
        /subsystem:console\
        /DEBUG:FULL /MACHINE:X64\
        /DYNAMICBASE:NO "cachet.lib" \
        /LIBPATH:"C:\InterSystems\Cache\bin"

LINK_DLL_FLAGS=\
        /nologo\
        /subsystem:console\
        /DEBUG:FULL /MACHINE:X64\
        /DLL

MT_OBJS=\
        $(OUTDIR)\callin_multithreads-cache.obj $(OUTDIR)\shdir-cache.obj

DLL_OBJS=\
        $(OUTDIR)\callout-cache.obj

$(OUTDIR)\callin_multithreads-cache.exe : $(MT_OBJS)
        $(LINK) $(LINK_FLAGS) /pdb:$(OUTDIR)\callin_multithreads-cache.pdb /out:$(OUTDIR)\callin_multithreads-cache.exe $(MT_OBJS)

$(OUTDIR)\callout-cache.dll : $(DLL_OBJS)
        $(LINK) $(LINK_DLL_FLAGS) /pdb:$(OUTDIR)\callout-cache.pdb /out:$(OUTDIR)\callout-cache.dll $(DLL_OBJS)

.c{$(OUTDIR)}.obj:
        $(CPP) $(CPPFLAGS) $<
