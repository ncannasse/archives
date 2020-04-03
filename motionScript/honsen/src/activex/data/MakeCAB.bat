@CABARC.EXE -s 6144 N HonsenPlugin.cab HonsenPlugin.inf ..\..\..\bin\honsen_activex.ocx ..\..\..\bin\honsen.dll ..\..\..\bin\gc.dll ..\..\..\bin\mtsvm.dll
@signcode.exe -spc cert.spc -v mykey HonsenPlugin.cab
@pscp -scp HonsenPlugin.cab *.html *.hs ncannasse@www.motion-twin.com:~/www
@pause
