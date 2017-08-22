@echo off
CD /D "%~dp0"
del pHook.exe
cls
call "%VS140COMNTOOLS%\vsvars32.bat"
"cl.exe" /EHa /O2 /MD *.cpp kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /link /FORCE:MULTIPLE /OUT:pHook.exe
del *.obj 
pause