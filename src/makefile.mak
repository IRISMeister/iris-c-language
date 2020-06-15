OUTDIR=.\x64
LINK32=link.exe

ALL : $(OUTDIR)\callin_multithreads.exe $(OUTDIR)\callin_misc.exe

CLEAN :
        -@erase /Q $(OUTDIR)\*

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
        /I"..\\" /I"C:\InterSystems\IRIS\dev\iris-callin\include" 

LINK32_FLAGS=\
        /nologo\
        /subsystem:console\
        /DEBUG:FULL /MACHINE:X64\
        /DYNAMICBASE:NO "irisdbt.lib" \
        /LIBPATH:"C:\InterSystems\IRIS\dev\iris-callin\lib"

MT_OBJS=\
        $(OUTDIR)\callin_multithreads.obj

MISC_OBJS=\
        $(OUTDIR)\callin_execute.obj $(OUTDIR)\callin_misc.obj $(OUTDIR)\callin_misc_calls.obj $(OUTDIR)\callin_misc_value.obj

$(OUTDIR)\callin_multithreads.exe : $(MT_OBJS)
        $(LINK32) $(LINK32_FLAGS) /pdb:$(OUTDIR)\callin_multithreads.pdb /out:$(OUTDIR)\callin_multithreads.exe $(MT_OBJS)

$(OUTDIR)\callin_misc.exe : $(MISC_OBJS)
        $(LINK32) $(LINK32_FLAGS) /pdb:$(OUTDIR)\callin_misc.pdb /out:$(OUTDIR)\callin_misc.exe $(MISC_OBJS)

.c{$(OUTDIR)}.obj:
        $(CPP) $(CPPFLAGS) $<
