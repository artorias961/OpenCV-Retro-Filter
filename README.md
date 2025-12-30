# OpenCV Retro Filter

A C++ computer vision project that converts images and GIFs into a **retro Game Boy Advance–style aesthetic** using **OpenCV only**.  
The pipeline is fully deterministic (no ML), pixel-based, and optimized using **Windows multithreading**.



## ✨ Features

- 🎮 GBA-inspired retro visual style
- 🧩 Pixelation via low-resolution resampling
- 🎨 Color palette reduction using K-means
- 🟦 Ordered Bayer dithering (8×8 matrix)
- 🧵 **4-way Windows threading** for per-pixel dithering
- 🖼️ Image input support (`.jpg`, `.png`)
- 🎞️ GIF input support (processed frame-by-frame)
- 📹 MP4 output for animated content (OpenCV-only)



## 🧠 Technical Overview

The processing pipeline is:

1. Contrast enhancement (YCrCb color space)
2. Downscale to low internal resolution (pixelation)
3. Optional edge hinting (Canny-based)
4. **Ordered dithering (parallelized into 4 regions)**
5. K-means color quantization
6. Nearest-neighbor upscale
7. Light sharpening pass

Multithreading is applied only where safe and deterministic
(per-pixel dithering), matching real-world CV best practices.



## 🧵 Multithreading

- Uses **Windows native threads** (`CreateThread`)
- Image divided into **4 quadrants**
- Each thread operates on an independent region
- No locks or synchronization required
- Inspired by classic Sobel multi-threading assignments



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




