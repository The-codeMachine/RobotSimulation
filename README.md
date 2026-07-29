# Robot Simulation

This is a C++17 library built with CMake. It provides you with a system able to simulate robots.
Currently, this project is pretty new. This means that it only supports 2D enviroments with a limited
list of devices. The only objects that can populate a world consist of:

 - An empty space
 - A wall
 - A robot

### Design & Documentation

Design notes and documentation are found in the docs directory. Code comments can be found in the header. Design notes are described in documents, while implementations are described in code files. 

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