set "PATH=C:\Program Files\LLVM\bin;C:\Program Files (x86)\LLVM\bin;%PATH%"
::set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\vcpackages;%PATH%"
clang-format -i src/Thunks/*.hpp
clang-format -i src/*.h
clang-format -i src/*.hpp
clang-format -i src/*.cpp
pause