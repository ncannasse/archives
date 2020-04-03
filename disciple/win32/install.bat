@nmake all
@cp dllwin32.dll c:\ocaml\lib\stublibs
@cp win32.cma win32.cmi win32_core.cmi win32.cmxa win32.lib libwin32.lib c:\ocaml\lib\
@nmake clean
@pause
