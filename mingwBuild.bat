if not exist build mkdir build
cd build

cmake -S ../ -B . -G "MinGW Makefiles"
mingw32-make.exe

cd ..