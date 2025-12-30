# OpenCV Retro Filter

A small **C++ + OpenCV** project that converts an input image into a **retro (GBA‑inspired) look** using classic image-processing steps:
pixelation, palette reduction, and ordered dithering. A **Python implementation** of the same effect is also included for comparison.

> **Note on “OpenCV only”:** the **image processing** uses OpenCV, while the multithreaded version uses the **Windows thread API** (`CreateThread`) for parallelism.

> **Note on Python 3.14 and multithreading:** I have implemented multithreading in **C++** (Windows `CreateThread`) instead of Python because CPython has historically been limited by the **Global Interpreter Lock (GIL)**, which prevents true parallel execution of Python bytecode in most standard builds. While **Python 3.13+ introduces experimental “free-threading” builds (PEP 703) that can run without the GIL**, this mode is not yet the default, remains experimental, and ecosystem/library support (including C-extension thread-safety considerations) is still maturing. For a reliable, deterministic, and performant parallel implementation today—especially for per-pixel image work—C++ multithreading is the most practical choice.


## What’s in this repo

This repository contains **two C++ implementations**:

- **`python/`**  
  Python implementation of the same GBA-style retro filter using **OpenCV (cv2)**.  Provides a simpler, single-threaded reference version of the pipeline (pixelation, ordered dithering, and palette reduction), intended for **rapid prototyping, visualization, and algorithm validation** alongside the C++ implementations.


- **`c++/version_1 - No_threading/`**  
  Single-threaded implementation.

- **`c++/version 2 - Windows_Multi_threading_enable/`**  
  Same filter pipeline, but the **ordered dithering stage is split into 4 quadrants** and processed with **Windows threads**.

**`c++/version_3 - Video_Multi_Threadings/`**
Extends the retro filter to animated input by processing a GIF frame-by-frame using OpenCV’s **VideoCapture**.


You’ll also see:

- `CMakeLists.txt` and `Compile_and_Build.bat` in the version folder (Windows build helpers)
- Example animated input `test.gif`
- Example output `gba_output.mp4` (processed frame-by-frame using OpenCV)

This version uses OpenCV’s `VideoCapture` to decode the GIF into individual frames and applies the same GBA-style filter pipeline to each frame before writing the result to an MP4 file via `VideoWriter`.

> Note: OpenCV does not reliably support writing GIF files, so MP4 is used as the output format.

The repository may include build artifacts (`build/` folders). If you want a cleaner repository, add them to `.gitignore`.





## Features (implemented)

- ✅ GBA‑inspired retro effect for **still images**
- ✅ Pixelation via downscaling + nearest-neighbor upscale
- ✅ Palette reduction (K‑means)
- ✅ Ordered Bayer dithering (8×8 matrix)
- ✅ Optional edge hinting (Canny)
- ✅ **4‑way Windows threading** (version 2) for the dithering stage 



## What this repo does **not** currently include

- ❌ **GIF input / video output** in the C++ folders as committed right now  
  (The checked-in examples are `test.jpg` → `gba_output.png`.) 

If you want GIF support later, you’d typically use `cv::VideoCapture` to read frames and `cv::VideoWriter` to write a video (e.g., MP4). OpenCV does not reliably write GIFs.



## Build (Windows)

### Requirements
- OpenCV installed (headers + libs)
- CMake + a compiler (MinGW or MSVC)

### Option A: Use the included batch file
Each C++ version folder includes `Compile_and_Build.bat`. Run it from that folder.

Example:
```bat
cd "c++\version 2 - Windows_Multi_threading_enable"
Compile_and_Build.bat
```

### Option B: CMake manually
```bat
cd "c++\version 2 - Windows_Multi_threading_enable"
mkdir build
cd build
cmake ..
cmake --build . --config Release
```



## Run

Put your input image in the **same folder as the executable** (or update the hardcoded filename/path in `main.cpp`), then run the program.

Expected output (by default):
- `gba_output.png` written to the **current directory**
- A preview window may appear (depends on the code path you’re using)



## Pipeline (high level)

1. Contrast enhancement (YCrCb)
2. Downscale to low internal resolution (pixelation)
3. Optional edge hinting (Canny)
4. Ordered dithering (optionally threaded into 4 regions)
5. K‑means palette reduction
6. Nearest‑neighbor upscale
7. Light sharpening




## 📁 Project Structure

```
OpenCV-Retro-Filter/
|   README.md
|   treelist.txt
|   
+---c++
|   |   test.jpg
|   |   
|   +---version 2 - Windows_Multi_threading_enable
|   |   |   CMakeLists.txt
|   |   |   Compile_and_Build.bat
|   |   |   gba_output.png
|   |   |   main.cpp
|   |   |   test.jpg
|   |   |   
|   |   \---build
|   |       |   CMakeCache.txt
|   |       |   cmake_install.cmake
|   |       |   Makefile
|   |       |   OpenCVExample.exe
|   |       |   
|   |       \---CMakeFiles
|   |           |   cmake.check_cache
|   |           |   CMakeConfigureLog.yaml
|   |           |   CMakeDirectoryInformation.cmake
|   |           |   InstallScripts.json
|   |           |   Makefile.cmake
|   |           |   Makefile2
|   |           |   progress.marks
|   |           |   TargetDirectories.txt
|   |           |   
|   |           +---4.2.1
|   |           |   |   CMakeCCompiler.cmake
|   |           |   |   CMakeCXXCompiler.cmake
|   |           |   |   CMakeDetermineCompilerABI_C.bin
|   |           |   |   CMakeDetermineCompilerABI_CXX.bin
|   |           |   |   CMakeRCCompiler.cmake
|   |           |   |   CMakeSystem.cmake
|   |           |   |   
|   |           |   +---CompilerIdC
|   |           |   |   |   a.exe
|   |           |   |   |   CMakeCCompilerId.c
|   |           |   |   |   
|   |           |   |   \---tmp
|   |           |   \---CompilerIdCXX
|   |           |       |   a.exe
|   |           |       |   CMakeCXXCompilerId.cpp
|   |           |       |   
|   |           |       \---tmp
|   |           +---CMakeScratch
|   |           +---OpenCVExample.dir
|   |           |       build.make
|   |           |       cmake_clean.cmake
|   |           |       compiler_depend.make
|   |           |       compiler_depend.ts
|   |           |       depend.make
|   |           |       DependInfo.cmake
|   |           |       flags.make
|   |           |       includes_CXX.rsp
|   |           |       link.txt
|   |           |       linkLibs.rsp
|   |           |       main.cpp.obj
|   |           |       main.cpp.obj.d
|   |           |       objects.a
|   |           |       objects1.rsp
|   |           |       progress.make
|   |           |       
|   |           \---pkgRedirects
|   +---version_1 - No_threading
|   |   |   CMakeLists.txt
|   |   |   Compile_and_Build.bat
|   |   |   gba_output.png
|   |   |   main.cpp
|   |   |   test.jpg
|   |   |   
|   |   \---build
|   |       |   CMakeCache.txt
|   |       |   cmake_install.cmake
|   |       |   Makefile
|   |       |   OpenCVExample.exe
|   |       |   
|   |       \---CMakeFiles
|   |           |   cmake.check_cache
|   |           |   CMakeConfigureLog.yaml
|   |           |   CMakeDirectoryInformation.cmake
|   |           |   InstallScripts.json
|   |           |   Makefile.cmake
|   |           |   Makefile2
|   |           |   progress.marks
|   |           |   TargetDirectories.txt
|   |           |   
|   |           +---4.2.1
|   |           |   |   CMakeCCompiler.cmake
|   |           |   |   CMakeCXXCompiler.cmake
|   |           |   |   CMakeDetermineCompilerABI_C.bin
|   |           |   |   CMakeDetermineCompilerABI_CXX.bin
|   |           |   |   CMakeRCCompiler.cmake
|   |           |   |   CMakeSystem.cmake
|   |           |   |   
|   |           |   +---CompilerIdC
|   |           |   |   |   a.exe
|   |           |   |   |   CMakeCCompilerId.c
|   |           |   |   |   
|   |           |   |   \---tmp
|   |           |   \---CompilerIdCXX
|   |           |       |   a.exe
|   |           |       |   CMakeCXXCompilerId.cpp
|   |           |       |   
|   |           |       \---tmp
|   |           +---CMakeScratch
|   |           +---OpenCVExample.dir
|   |           |       build.make
|   |           |       cmake_clean.cmake
|   |           |       compiler_depend.make
|   |           |       compiler_depend.ts
|   |           |       depend.make
|   |           |       DependInfo.cmake
|   |           |       flags.make
|   |           |       includes_CXX.rsp
|   |           |       link.txt
|   |           |       linkLibs.rsp
|   |           |       main.cpp.obj
|   |           |       main.cpp.obj.d
|   |           |       objects.a
|   |           |       objects1.rsp
|   |           |       progress.make
|   |           |       
|   |           \---pkgRedirects
|   \---version_3 - Video_Multi_Threadings
|       |   CMakeLists.txt
|       |   Compile_and_Build.bat
|       |   gba_output.mp4
|       |   main.cpp
|       |   silk_song.gif
|       |   
|       \---build
|           |   CMakeCache.txt
|           |   cmake_install.cmake
|           |   Makefile
|           |   OpenCVExample.exe
|           |   
|           \---CMakeFiles
|               |   cmake.check_cache
|               |   CMakeConfigureLog.yaml
|               |   CMakeDirectoryInformation.cmake
|               |   InstallScripts.json
|               |   Makefile.cmake
|               |   Makefile2
|               |   progress.marks
|               |   TargetDirectories.txt
|               |   
|               +---4.2.1
|               |   |   CMakeCCompiler.cmake
|               |   |   CMakeCXXCompiler.cmake
|               |   |   CMakeDetermineCompilerABI_C.bin
|               |   |   CMakeDetermineCompilerABI_CXX.bin
|               |   |   CMakeRCCompiler.cmake
|               |   |   CMakeSystem.cmake
|               |   |   
|               |   +---CompilerIdC
|               |   |   |   a.exe
|               |   |   |   CMakeCCompilerId.c
|               |   |   |   
|               |   |   \---tmp
|               |   \---CompilerIdCXX
|               |       |   a.exe
|               |       |   CMakeCXXCompilerId.cpp
|               |       |   
|               |       \---tmp
|               +---CMakeScratch
|               +---OpenCVExample.dir
|               |       build.make
|               |       cmake_clean.cmake
|               |       compiler_depend.make
|               |       compiler_depend.ts
|               |       depend.make
|               |       DependInfo.cmake
|               |       flags.make
|               |       includes_CXX.rsp
|               |       link.txt
|               |       linkLibs.rsp
|               |       main.cpp.obj
|               |       main.cpp.obj.d
|               |       objects.a
|               |       objects1.rsp
|               |       progress.make
|               |       
|               \---pkgRedirects
\---python
        main.py
        output_gba.png
        test.jpg
```





