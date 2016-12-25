@echo off
echo "Exporting DLLs"
copy bincpp\Release\Win32\jmtp.dll export\
copy bincpp\Release\x64\jmtp64.dll export\
copy bincpp\Release\Win32\jmtp.dll src\java-win32\
copy bincpp\Release\x64\jmtp64.dll src\java-win64\
pause
