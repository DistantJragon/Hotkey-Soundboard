# Licenses / Third-party notices

This `licenses/` folder contains license texts and short notices for third-party components
that may be redistributed with the application.

---

## Manifest of shipped third-party components

### Application

- **File:** `HotkeySoundboard.exe`  
  **Component:** Hotkey Soundboard  
  **License:** GPLv3  
  **License File:** `/LICENSE`  
  **Notes:** This is our program binary.  
  **Source:** https://github.com/DistantJragon/HotkeySoundboard

### Qt libraries

- **Files:**
  - `generic/qtuiotouchplugin.dll`
  - `iconengines/qsvgicon.dll`
  - `imageformats/qgif.dll`
  - `imageformats/qico.dll`
  - `imageformats/qjpeg.dll`
  - `imageformats/qsvg.dll`
  - `multimedia/ffmpegmediaplugin.dll`
  - `multimedia/windowsmediaplugin.dll`
  - `networkinformation/qnetworkmanager.dll`
  - `platforms/qwindows.dll`
  - `styles/qmodernwindowsstyle.dll`
  - `tls/qcertonlybackend.dll`
  - `tls/qschannelbackend.dll`
  - `Qt6Core.dll`
  - `Qt6Gui.dll`
  - `Qt6Multimedia.dll`
  - `Qt6Network.dll`
  - `Qt6Svg.dll`
  - `Qt6Widgets.dll`  
    **Component:** Qt 6.9.2  
    **License:** LGPLv3  
    **License File:** `qt6.txt`  
    **Source:** https://download.qt.io/

### MinGW runtime libraries

- **File:** `libstdc++-6.dll`  
  **Component:** libstdc++ (mingw-w64)  
  **License:** GPLv3 with GCC Runtime Library Exception  
  **License File:** `mingw-stdc++.txt`  
  **Source:** https://www.mingw-w64.org/  
  **Notes:** Copied from Qt 6.9.2 MinGW distribution.

- **File:** `libgcc_s_seh-1.dll`  
  **Component:** libgcc (mingw-w64)  
  **License:** GPLv3 with GCC Runtime Library Exception  
  **License File:** `mingw-gcc.txt`  
  **Notes:** Copied from Qt 6.9.2 MinGW distribution.

- **File:** `libwinpthread-1.dll`  
  **Component:** winpthreads (mingw-w64)  
  **License:** MIT and BSD  
  **License File:** `mingw-winpthread.txt`  
  **Notes:** Copied from Qt 6.9.2 MinGW distribution.

### Mesa llvmpipe

- **File:** `opengl32sw.dll`  
  **Component:** Qt-provided Mesa llvmpipe  
  **License:** MIT License Boost Software License  
  **License File:** `mesa-llvmpipe.txt`  
  **Notes:** Copied from Qt 6.9.2 MinGW distribution.

### FFmpeg

- **Files:** `avcodec-61.dll`, `avformat-61.dll`, `avutil-59.dll`, `swresample-5.dll`, `swscale-8.dll`  
  **Component:** FFmpeg 7.1.1  
  **License:** LGPLv2.1+  
  **License File:** `ffmpeg.txt`  
  **Source:** https://ffmpeg.org/  
  **Notes:** Copied from Qt 6.9.2 MinGW distribution.

## Other Third-party components

### FlowLayout example

- **File:** flowlayout.cpp / flowlayout.h  
  **Component:** Qt FlowLayout example  
  **License:** BSD-3-Clause  
  **License File:** `flowlayout.txt`  
  **Source:** https://doc.qt.io/qt-6/qtwidgets-layouts-flowlayout-example.html  
  **Notes:** We include the FlowLayout source in our repopsitory
