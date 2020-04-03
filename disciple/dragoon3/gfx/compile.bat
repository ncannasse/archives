rc d3gfx.rc
cvtres /NOLOGO /machine:ix86 d3gfx.res
@del d3gfx.res
link /dll /out:d3gfx.dll /NOENTRY d3gfx.obj
@pause