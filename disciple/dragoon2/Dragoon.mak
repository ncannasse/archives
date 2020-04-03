# ---------------------------------------------------------------------------
VERSION = BCB.01
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = Dragoon.exe
OBJFILES = Dragoon.obj Main_Defs.obj Console.obj Load_Form.obj PAK_Defs.obj \
   PRJ_Defs.obj Input_Form.obj Types_Form.obj Types_Defs.obj Folders_Defs.obj \
   PAK_Form.obj Balises.obj PPrefs_Form.obj Plugins.obj PlugInfo_Form.obj \
   Plug_Form.obj PAK_Browser.obj Main_Form.obj PlugActions.obj
RESFILES = Dragoon.res
RESDEPEN = $(RESFILES) Console.dfm Load_Form.dfm Input_Form.dfm Types_Form.dfm \
   PAK_Form.dfm PPrefs_Form.dfm PlugInfo_Form.dfm Plug_Form.dfm PAK_Browser.dfm \
   Main_Form.dfm
LIBFILES = 
DEFFILE = 
# ---------------------------------------------------------------------------
CFLAG1 = -Od -Hc -w -k -r- -y -v -vi- -c -a4 -b- -w-par -w-inl -Vx -Ve -x
CFLAG2 = -DDRAGOON -I$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl \
   -H=$(BCB)\lib\vcld.csm 
PFLAGS = -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE \
   -DDRAGOON -U$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib \
   -I$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl -v -$Y -$W -$O- -JPHNV -M  
RFLAGS = -DDRAGOON -i$(BCB)\bin;$(BCB)\include;$(BCB)\include\vcl 
LFLAGS = -L$(BCB)\bin;$(BCB)\lib\obj;$(BCB)\lib -aa -Tpe -x -v -V4.0
IFLAGS = 
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) vcl.lib import32.lib cp32mt.lib 
# ---------------------------------------------------------------------------
.autodepend

$(PROJECT): $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES) 
!

.pas.hpp:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.pas.obj:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.cpp.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $* 

.c.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $**

.rc.res:
    $(BCB)\BIN\brcc32 $(RFLAGS) $<
#-----------------------------------------------------------------------------
