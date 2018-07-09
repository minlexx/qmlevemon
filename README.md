# qmlevemon
Qt5/QML based EVE Online character monitor


## Build for desktop (Windows / Linux)
You need only Qt5 >= 5.9 installed.

Open in Qt Creator, select toolkit, build. Or, run in a separate build directory:

Linux:
```
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DCMAKE_PREFIX_PATH=/path/to/qt/5.11.1/gcc_64 -DCMAKE_INSTALL_PREFIX=/usr ../qmlevemon
cmake --build . --target install  (or make -j8 && sudo make install)
```
Windows + MSVC:
```
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DCMAKE_PREFIX_PATH=C:\path\to\qt\5.11.1\msvc2017_x64 ..\qmlevemon
cmake --build . -- target all
```

## Build for Android platform
Before that, have an Android SDK & NDK installed.

example command:
```
cmake -DCMAKE_TOOLCHAIN_FILE=../qmlevemon/cmake/toolchain/Android.cmake \
      -DBUILD_FOR_ANDROID=ON \
      -DBUILD_TESTING=OFF \
      -DCMAKE_ANDROID_NDK=/path/to/android-ndk-r16 \
      -DANDROID_SDK_ROOT=/path/to/Android/Sdk \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_VERBOSE_MAKEFILE=ON \
      -DCMAKE_PREFIX_PATH=/path/to/qt/5.11.1/android_armv7 \
      -DANDROID_API_LEVEL=16 \
      -DQTANDROID_EXPORTED_TARGET=QMLEVEMon \
      -DANDROID_EXTRA_LIBS="/path/to/qmlevemon/android/arm-linux-androideabi-4.9/libcrypto.so;/path/to/qmlevemon/android/arm-linux-androideabi-4.9/libssl.so" \
      ../qmlevemon/

make -j8 create-apk

```


## CCP Copyright Notice
EVE Online, the EVE logo, EVE and all associated logos and designs are the intellectual property of CCP hf. All artwork, screenshots, characters, vehicles, storylines, world facts or other recognizable features of the intellectual property relating to these trademarks are likewise the intellectual property of CCP hf. EVE Online and the EVE logo are the registered trademarks of CCP hf. All rights are reserved worldwide. All other trademarks are the property of their respective owners. CCP hf. has granted permission to use EVE Online and all associated logos and designs for promotional and information purposes on its website but does not endorse, and is not in any way affiliated with, this software. CCP is in no way responsible for the content on or functioning of this program, nor can it be liable for any damage arising from the use of this program.
