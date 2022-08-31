@echo off
set hasit=no
for /F "tokens=1*" %%a in ('tasklist /nh /fi "imagename eq putty.exe"') do if %%a == putty.exe set hasit=yes
if %hasit%==yes (
	taskkill /f /im putty.exe 2>nul
)
start /b %~dp0/putty.exe -serial %1 -sercfg %2,8,n,1,N