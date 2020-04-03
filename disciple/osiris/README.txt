Osiris - OCaml Visual Components
(C)Copyright 2002 Nicolas Cannasse and Motion-Twin
Last Version : http://tech.motion-twin.com

OSIRIS
------
Osiris is a set of very-easy-to-use Class Wrappers for the Win32 Controls API.
The main goal of Osiris is to enable RAD (Rapid Application
Developpement) of user interfaces.

REQUIREMENTS
------------
- OCaml 3.06 (binaries require this version exactly)
- Windows 95/98/NT/2000/Me/...

INSTALLATION
------------
The current distribution include an already-compiled version of Osiris
in the /bin directory. Simply copy all theses files in your ocaml/lib
directory. You can now start working with osiris.

COMPILATION
-----------
You perhaps want to compile it by yourself by doing :

ocamlc -dllib -lwin32 -I win32 -a win32/win32.cma osiris.mli osiris.ml -o osiris.cma

You only need dllwin32.dll, osiris.cma and osiris.cmi, other files can be
safely deleted. Note that this compilation needs the exact version of ocaml
matched in "requirements" because it uses a precompiled version of the OCaml
Win32 API. If you want to recompile the whole sources, get a recent source
distribution of it.

SAMPLES
-------
See Documentation

LICENCE
-------
GPL'ed 
Contact the author for more informations.

AUTHOR
------
Nicolas Cannasse
ncannasse@motion-twin.com
http://tech.motion-twin.com

Thanks to Harry Chomsky and his OCaml Win32 API.
