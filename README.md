# Robot Simulation

Robot Simulation is a C++20 library for simulating robots in virtual environments.

The project is currently in early development and focuses on providing a modular foundation for robot simulation. At present, it supports two-dimensional environments and a growing collection of objects and devices.

Currently, the objects that can populate a world include:

* Empty space
* Walls
* Robots

The library is designed to be extended as additional robots, devices, sensors, environments, and simulation features are developed.

## Design & Documentation

Design notes and documentation can be found in the [`docs/`](docs/) directory.

The project follows a distinction between design documentation and implementation:

* **Design notes** describe architecture, intended behavior, and design decisions.
* **Header files** contain documentation for the public API.
* **Source files** contain the implementation.

See the [`docs/`](docs/) directory for the current design documentation.

## Building

Robot Simulation uses **CMake** and requires a compiler with C++20 support.

MSVC 2026 is recommended for development, but the project is intended to be portable across C++20-compatible compilers.

MSVC 2022 is tested. MSVC 2026 is recommended for development.

### Configure the project

Create a build directory:

```powershell
mkdir build
```

Configure the project with CMake:

```powershell
cmake -B build
```

### Build

Build the project with:

```powershell
cmake --build build
```

### Run tests

Run the test suite with:

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

The test suite is an important part of the project. Changes should not be considered complete until the relevant tests pass.

## Project Status

Robot Simulation is actively being developed.

The current focus is on establishing a reliable simulation core, expanding the supported devices and sensors, improving testing, and developing examples that demonstrate how the library can be used.

The API should therefore be considered subject to change until the project reaches a stable release.

## Open Source & Licensing

Robot Simulation is developed as an open-source project.

**Open-source releases are licensed under the MIT License.** See [`LICENSE.md`](LICENSE.md) for the complete license.

Each release is licensed according to the license included or identified for that release. Open-source releases will remain available under the license under which they were released.

The project may change its licensing model for **future releases**. In particular, a future version may be released under a proprietary or otherwise different license.

Changing the license of a future release does **not** change the license of previous releases. For example, if version 1.x is released under the MIT License and version 2.0 is released under a proprietary license, users who received version 1.x under the MIT License retain the rights granted by that license.

### Contributions

Contributions from the community are welcome.

Because contributions may be incorporated into future releases under different licensing models, contributors are required to agree to the project's Contributor License Agreement (CLA) before their contributions can be accepted.

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for:

* Development guidelines
* The standard contribution workflow
* Testing requirements
* The project's contribution and relicensing policy

## Dependencies

`CMakeLists.txt` will attempt to obtain the project's required dependencies during configuration.

The project currently depends on:

* [`nlohmann::json`](https://github.com/nlohmann/json)

Additional dependencies may be introduced as the project develops. Third-party dependencies remain subject to their respective licenses.

## Tests

The project includes automated tests using CTest.

All tests are expected to pass before a release is made.

Testing is still being expanded as new functionality is added.

## Roadmap

Some current development goals include:

* Expand the supported simulation objects.
* Expand the sensor and device systems.
* Improve collision detection and physics behavior.
* Improve test coverage.
* Add integration tests demonstrating use of the library from an external project.
* Create complete example programs.
* Create a standalone demonstration of the library.
* Improve build and distribution support.
* Establish a stable public API.
* Publish documented releases.

## Distribution

Release and distribution infrastructure is currently under development.

Once a stable distribution system is available, releases will provide the appropriate build artifacts and instructions for integrating Robot Simulation into other C++ projects.

Until then, building the library from source is the recommended approach.

## Security Reporting

Security reporting can be made to the maintainer's email address: [Rey Slimmon](mailto:coolestbro5113@gmail.com)