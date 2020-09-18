OUTDIR=.\x64
LINK=link.exe

ALL : $(OUTDIR)\callin_multithreads.exe $(OUTDIR)\callin_misc.exe $(OUTDIR)\callin_long_value.exe $(OUTDIR)\callout.dll

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
        /I"..\\" /I"C:\InterSystems\IRIS\dev\iris-callin\include" 

LINK_FLAGS=\
        /nologo\
        /subsystem:console\
        /DEBUG:FULL /MACHINE:X64\
        /DYNAMICBASE:NO "irisdbt.lib" \
        /LIBPATH:"C:\InterSystems\IRIS\dev\iris-callin\lib"

LINK_DLL_FLAGS=\
        /nologo\
        /subsystem:console\
        /DEBUG:FULL /MACHINE:X64\
        /DLL

MT_OBJS=\
        $(OUTDIR)\callin_multithreads.obj $(OUTDIR)\shdir.obj

MISC_OBJS=\
        $(OUTDIR)\callin_execute.obj $(OUTDIR)\callin_misc.obj $(OUTDIR)\callin_misc_calls.obj $(OUTDIR)\callin_misc_value.obj $(OUTDIR)\shdir.obj

LONG_VAL_OBJS=\
        $(OUTDIR)\callin_long_value.obj $(OUTDIR)\shdir.obj

DLL_OBJS=\
        $(OUTDIR)\callout.obj

$(OUTDIR)\callin_multithreads.exe : $(MT_OBJS)
        $(LINK) $(LINK_FLAGS) /pdb:$(OUTDIR)\callin_multithreads.pdb /out:$(OUTDIR)\callin_multithreads.exe $(MT_OBJS)

$(OUTDIR)\callin_misc.exe : $(MISC_OBJS)
        $(LINK) $(LINK_FLAGS) /pdb:$(OUTDIR)\callin_misc.pdb /out:$(OUTDIR)\callin_misc.exe $(MISC_OBJS)

$(OUTDIR)\callin_long_value.exe : $(LONG_VAL_OBJS)
        $(LINK) $(LINK_FLAGS) /pdb:$(OUTDIR)\callin_long_value.pdb /out:$(OUTDIR)\callin_long_value.exe $(LONG_VAL_OBJS)

$(OUTDIR)\callout.dll : $(DLL_OBJS)
        $(LINK) $(LINK_DLL_FLAGS) /pdb:$(OUTDIR)\callout.pdb /out:$(OUTDIR)\callout.dll $(DLL_OBJS)

.c{$(OUTDIR)}.obj:
        $(CPP) $(CPPFLAGS) $<
