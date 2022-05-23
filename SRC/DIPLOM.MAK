#
#Borland C++ IDE generated makefile
#
.AUTODEPEND


#
#Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
#IDE macros
#


#
#Options
#
IDE_LFLAGS =  -LD:\BC4\LIB
IDE_RFLAGS =  -ID:\BC4\INCLUDE
LW16Optimiert_lGeschwdr = 
RW16Optimiert_lGeschwdr = 
BW16Optimiert_lGeschwdr = 
LNATW16_diplomdexe = $(LW16Optimiert_lGeschwdr)
RNATW16_diplomdexe = $(RW16Optimiert_lGeschwdr)
BNATW16_diplomdexe = $(BW16Optimiert_lGeschwdr)
LLATW16_diplomdexe =  -Twe -c -C -v -n -d
RLATW16_diplomdexe =  -31
BLATW16_diplomdexe = 
LEAT_diplomdexe = $(LNATW16_diplomdexe) $(LLATW16_diplomdexe)
REAT_diplomdexe = $(RNATW16_diplomdexe) $(RLATW16_diplomdexe)
BEAT_diplomdexe = $(BNATW16_diplomdexe) $(BLATW16_diplomdexe)

#
#Dependency List
#
Dep_diplom = \
   diplom.exe

diplom : BccW16.cfg $(Dep_diplom)
  echo IDE_Editor diplom

Dep_diplomdexe = \
   tabu.obj\
   dialog.obj\
   util.obj\
   graphio.obj\
   init.obj\
   fileio.obj\
   compute.obj\
   debug.obj\
   memory.obj\
   werror.obj\
   diplom.obj\
   diplom.res

diplom.exe : $(Dep_diplomdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_diplomdexe) +
D:\BC4\LIB\c0wl.obj+
tabu.obj+
dialog.obj+
util.obj+
graphio.obj+
init.obj+
fileio.obj+
compute.obj+
debug.obj+
memory.obj+
werror.obj+
diplom.obj
$<,$*
D:\BC4\LIB\bwcc.lib+
D:\BC4\LIB\import.lib+
D:\BC4\LIB\crtldll.lib
diplom.def
|
   $(BRC) diplom.res $<

tabu.obj :  tabu.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ tabu.c

dialog.obj :  dialog.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ dialog.c

util.obj :  util.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ util.c

graphio.obj :  graphio.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ graphio.c

init.obj :  init.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ init.c

fileio.obj :  fileio.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ fileio.c

compute.obj :  compute.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ compute.c

debug.obj :  debug.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ debug.c

memory.obj :  memory.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ memory.c

werror.obj :  werror.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ werror.c

diplom.obj :  diplom.c
  $(BCC)   -P- -c $(CEAT_diplomdexe) -o$@ diplom.c

diplom.res :  diplom.rc
  $(BRC) $(IDE_RFLAGS) $(REAT_diplomdexe) -R -FO$@ diplom.rc

#Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-X-
-H
-ID:\BC4\INCLUDE
-H=diplom.csm
-O-d
-O
-Ob
-Oe
-Og
-Oi
-Ol
-Om
-Ot
-Op
-Ov
-k-
-Z
-ml
-WE
-D_RTLDLL;
-Ot
-Ol
-v-
-R-
-y
-Og
-O
-Ob
-wpch
-wuse
-wnod
-N
-3
-Ff
-ff
-a
-r-
-Od
| $@


