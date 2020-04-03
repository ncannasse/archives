##
 # OCaml-Win32
 # Makefile
 # By Harry Chomsky and Nicolas Cannasse
 # Copyright (c) 2002 by Harry Chomsky
 #
 # This library is free software; you can redistribute it and/or
 # modify it under the terms of the GNU Library General Public
 # License as published by the Free Software Foundation; either
 # version 2 of the License, or (at your option) any later version.
 #
 # This library is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # Library General Public License for more details.
 #
 # You should have received a copy of the GNU Library General Public
 # License along with this library; if not, write to the Free
 # Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ##

######################################################################
# DIRECTIVES :

.SUFFIXES : .ml .mli .cmo .cmx

#####
# EXT

EXTLIBS= comctl32.lib comdlg32.lib
CCEXTLIBS = -cclib comctl32.lib -cclib comdlg32.lib
EXTFLAGS= -I ext
EXTMODULES= \
    ext\win32_pack.+ ext\win32_comctrl.+ ext\win32_comdlg.+ ext\win32_dynlink.+ \
    ext\win32_richedit.+ ext\win32_shell.+ ext\win32_system.+

######################################################################
# MACROS :



CFLAGS=-ccopt /W3 -ccopt /YX -ccopt /Fpwin32.pch
WINLIBS=user32.lib gdi32.lib shell32.lib advapi32.lib $(EXTLIBS)
CCWINLIBS=-cclib user32.lib -cclib gdi32.lib -cclib shell32.lib -cclib advapi32.lib $(CCEXTLIBS)
MODULES= \
    win32_bitmap.+ win32_brush.+ win32_clipping.+ win32_color.+ win32_coord.+ win32_dc.+ win32_filledshape.+ win32_text.+ win32_line.+ win32_paint.+ win32_path.+ win32_pen.+ win32_rect.+ win32_region.+ \
    win32_control.+ win32_button.+ win32_combobox.+ win32_editcontrol.+ win32_listbox.+ win32_scrollbar.+ win32_staticcontrol.+ \
    win32_caret.+ win32_cursor.+ win32_icon.+ win32_menu.+ win32_resource.+ \
    win32_mouse.+ win32_keyboard.+ win32_accelerator.+ \
    win32_dialog.+ win32_message.+ win32_windowclass.+ win32_windowproc.+ win32_windowprop.+ win32_window.+ \
    win32_error.+ \
    win32_process.+ \
    win32_console.+ win32_file.+ \
    win32_handle.+ win32_string.+ win32_time.+ \
    win32_ipc.+ win32_atom.+ win32_pipe.+ win32_synch.+ $(EXTMODULES)

######################################################################
# TARGETS :

all : static dynamic opt mkwinapp

static : libwin32.lib win32.cma lkwinapp.obj test_static.exe

dynamic : dllwin32.dll win32.cma test_dynamic.exe

opt : libwin32.lib win32.cmxa lkwinapp.obj test_opt.exe

mkwinapp : mkwinapp.exe

#---------------------------------------------------------------------

clean :
    -@del win32.pch 2>NUL
    -@del win32_core.sobj 2>NUL
    -@del win32_core.dobj 2>NUL
    -@del win32_core.cmo 2>NUL
    -@del win32_core.cmi 2>NUL
    -@del win32_core.cmx 2>NUL
    -@del win32_core.obj 2>NUL
    -@del $(MODULES:+=sobj) 2>NUL
    -@del $(MODULES:+=dobj) 2>NUL
    -@del $(MODULES:+=cmo) 2>NUL
    -@del $(MODULES:+=cmi) 2>NUL
    -@del $(MODULES:+=cmx) 2>NUL
    -@del $(MODULES:+=obj) 2>NUL
    -@del win32.cmi 2>NUL
    -@del win32.cmo 2>NUL
    -@del win32.cmx 2>NUL
    -@del win32.obj 2>NUL
    -@del win32.cma 2>NUL
    -@del win32.cmxa 2>NUL
    -@del win32.lib 2>NUL
    -@del libwin32.lib 2>NUL
    -@del dllwin32.lib 2>NUL
    -@del dllwin32.exp 2>NUL
    -@del dllwin32.dll 2>NUL
    -@del mkwinapp.cmi 2>NUL
    -@del mkwinapp.cmo 2>NUL
    -@del mkwinapp.exe 2>NUL
    -@del lkwinapp.obj 2>NUL
    -@del test_res.obj 2>NUL
    -@del test.cmi 2>NUL
    -@del test.cmo 2>NUL
    -@del test.cmx 2>NUL
    -@del test.obj 2>NUL
    -@del testnores.cmi 2>NUL
    -@del testnores.cmo 2>NUL
    -@del test_static.exe 2>NUL
    -@del test_dynamic.exe 2>NUL
    -@del test_opt.exe 2>NUL

######################################################################
# BUILDS :

libwin32.lib : win32_core.sobj $(MODULES:+=sobj)
    -@del $@ 2>NUL
    lib /out:$@ $**

dllwin32.dll : win32_core.dobj $(MODULES:+=dobj)
    -@del $@ 2>NUL
    link $(WINLIBS) ocamlrun.lib /dll /out:$@ $**

win32.cma : win32_core.cmo $(MODULES:+=cmo) win32.cmo
    ocamlc -a -o $@ -cclib -lwin32 $(CCWINLIBS) -dllib -lwin32 $**

win32.cmxa : win32_core.cmx $(MODULES:+=cmx) win32.cmx
    ocamlopt -a -o $@ -cclib -lwin32 $(CCWINLIBS) $**

win32_core.cmi : win32_core.mli
    ocamlc -c $**

win32_core.cmo : win32_core.cmi win32_core.ml

win32_core.cmx : win32_core.cmi win32_core.ml

win32.cmo : win32_core.cmo $(MODULES:+=cmo) win32.ml

win32.cmx : win32_core.cmx $(MODULES:+=cmx) win32.ml

mkwinapp.exe : mkwinapp.ml
    ocamlc -o $@ unix.cma $**

test_static.exe : win32.cma test.ml test_res.obj libwin32.lib lkwinapp.obj
    ocamlc -o $@ -custom win32.cma test.ml test_res.obj lkwinapp.obj

test_dynamic.exe : win32.cma testnores.ml
    ocamlc -o $@ win32.cma testnores.ml

test_opt.exe : win32.cmxa test.ml test_res.obj libwin32.lib lkwinapp.obj
    ocamlopt -o $@ win32.cmxa test.ml test_res.obj lkwinapp.obj

######################################################################
# SUFFIXES :

.c.obj :
    ocamlc $(CFLAGS) $*.c

.c.sobj :
    ocamlc $(CFLAGS) -ccopt /Fo$@ $*.c

.c.dobj :
    ocamlc $(CFLAGS) -ccopt /Fo$@ -ccopt /DCAML_DLL  $*.c

.rc.obj :
    rc $*.rc
    cvtres /machine:ix86 $*.res
    @del $*.res

.ml.cmo :
    ocamlc $(EXTFLAGS) -c $*.ml

.ml.cmx :
    ocamlopt $(EXTFLAGS) -c $*.ml

######################################################################
# END
