# Robot Simulation

This is a C++20 library built with CMake. It provides you with a system able to simulate robots.
Currently, this project is pretty new. This means that it only supports 2D enviroments with a limited
list of devices. The only objects that can populate a world consist of:

 - An empty space
 - A wall
 - A robot

### Design & Documentation

Design notes and documentation are found in the docs directory. Code comments can be found in the header. Design notes are described in documents, while implementations are described in code files. The docs/ subdirectory is here: [docs/](docs/)

### Compilation & Running tests

We recommend you use MSVC 2022 to compile this program but that is not strictly necessary. This can be compiled using any C++ compiler. 

We recommend that you create a build directory before building anything. All the following commands assumed you followed this suggestion. 

```powershell
mkdir build
```

To compile the program's cache use the following command: 

```powershell
cmake -B build
```

To compile and test this program please run the commands below: 

```powershell
# builds the project
cmake --build build

# runs the tests
ctest --test-dir build -C Debug --output-on-failure
```

### TODO

 - We need to make a test linking this library to something completely else.
 - We need to make a demo, maybe make this open source and then maybe market it IDK how.

### Notes

 - Release and distribution code has not been released, created, or tested. Once distribution code has been released, you will be able to simply
  download the ```lib```/```a``` file. 
 - Ctests have been made and every release ensures they all past. But, GitHub Actions has not been setup yet. 
 - The CMakeLists.txt will attempt to fetch all of its dependencies. Currently, this is only ```nlohmann::json``` but may be more later. 