cmake ^
  -G "Unix Makefiles" ^
  -DCMAKE_MAKE_PROGRAM=V:/bin/Android/android-ndk-r15c/prebuilt/windows-x86_64/bin/make.exe ^
  -DCMAKE_TOOLCHAIN_FILE=V:/bin/Android/android-ndk-r15c/build/cmake/android.toolchain.cmake ^
  -DANDROID_PLATFORM=android-24 ^
  -DCMAKE_PREFIX_PATH=C:/qt/5.11.3/android_armv7 ^
  -DCMAKE_FIND_ROOT_PATH=C:/qt/5.11.3/android_armv7 ^
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH ^
  -DBUILD_FOR_ANDROID=ON ^
  -DBUILD_TESTING=OFF ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_VERBOSE_MAKEFILE=ON ^
  ../qmlevemon/
