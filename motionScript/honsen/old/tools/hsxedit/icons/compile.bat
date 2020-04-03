@echo off
rc hsxicons.rc
cvtres /NOLOGO /machine:ix86 hsxicons.res
del hsxicons.res
link /nologo /dll /out:..\..\..\bin\hsxicons.dll /NOENTRY hsxicons.obj
del hsxicons.obj
echo Done.
pause
