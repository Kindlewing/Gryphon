# Gryphon

<p align="center">
    A handmade graphics engine written in C, with minimal external dependencies.
</p>

## Getting Started

### Prerequisites

Linux:

- Cmake
- Make
- X11
- OpenGL

Windows:

- Visual Studio
- Cmake
- OpenGL

### Building

1. Clone the repository:

```bash
    git clone https://github.com/Kindlewing/gryphon
    cd gryphon
```

2. Generate build files.

```bash
    mkdir build && cd build
    cmake ..
```

> [!NOTE]
> To build the test binary I use for development:

```bash
    cmake -DBUILD_TEST_APP=on ..
```
