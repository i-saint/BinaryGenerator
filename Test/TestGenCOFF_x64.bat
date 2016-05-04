exec "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
cd %0\..
cl SayHelloMain.cpp SayHello_x64.obj /link /out:SayHelloMain_x64.exe
