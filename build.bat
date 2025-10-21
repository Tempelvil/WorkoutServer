mkdir _intermidiate_64
cd _intermidiate_64
cmake .. -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
cd ..