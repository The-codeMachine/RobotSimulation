# Robot Simulation

I will fill this readme.md later. 

### Compilation & Running tests

To compile this please enter the following commands from this projects root directory:

```powershell

mkdir build

cmake -B build
cmake --build build

ctest --test-dir build -C Debug --output-on-failure

```