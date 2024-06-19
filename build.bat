@echo off

mkdir build
cd build
cmake ..
if errorlevel 1 goto :error

make -j4
if errorlevel 1 goto :error

cd ..
GlCraft.exe
if errorlevel 1 goto :error

call clean.bat
if errorlevel 1 goto :error

goto :eof

:error
echo An error occurred.
pause
