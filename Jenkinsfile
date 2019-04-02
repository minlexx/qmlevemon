pipeline {
    agent none

    stages {
        stage('Build (Windows)') {
            agent {
                label 'windows && qt5'
            }
            steps {
                echo 'Building...'
                checkout scm
                bat '''
                    rem echo PATH: %PATH%
                    rem echo LIB: %LIB%
                    rem echo INCLUDE: %INCLUDE%
                    rem echo QT_PREFIX: %QT_PREFIX%
                    where cl
                    where jom
                    where cmake
                    where git
                    if exist build rmdir /s /q build
                    mkdir build
                    cd build
                    echo CMake options: -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_PREFIX% -DBUILD_TESTING=OFF
                    cmake -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_PREFIX% -DBUILD_TESTING=OFF ../
                    cmake --build . --target all
                '''
            }
        }
        stage('windows-installer') {
            agent {
                label 'windows && qt5'
            }
            steps {
                bat '''
                    cd build
                    cmake --build . --target windows-installer
                '''
                archiveArtifacts 'build/qmlevemon-setup.exe'
            }
        }
        stage('Build (Android)') {
            agent {
                label 'android && qt5'
            } 
            steps {
                bat '''
                    set NDK_TOOLCHAIN_PATH=%ANDROID_NDK_ROOT%/toolchains/%ANDROID_NDK_TOOLCHAIN_PREFIX%-%ANDROID_NDK_TOOLCHAIN_VERSION%/prebuilt/%ANDROID_NDK_HOST%
                    echo ANDROID_NDK_ROOT: %ANDROID_NDK_ROOT%
                    echo ANDROID_NDK_HOST: %ANDROID_NDK_HOST%
                    echo ANDROID_NDK_PLATFORM: %ANDROID_NDK_PLATFORM%
                    echo NDK_TOOLCHAIN_PATH: %NDK_TOOLCHAIN_PATH%
                    echo QT_ANDROID_PREFIX: %QT_ANDROID_PREFIX%
                    echo JDK_ROOT: %JDK_ROOT%
                    if exist build_android rmdir /s /q build_android
                    mkdir build_android
                    cd build_android
                    "%QT_ANDROID_PREFIX%\\bin\\qmake.exe" ..\\qmlevemon.pro -spec android-g++ "CONFIG+=release"
                    "%ANDROID_NDK_ROOT%\\prebuilt\\%ANDROID_NDK_HOST%\\bin\\make.exe" qmake_all
                    "%ANDROID_NDK_ROOT%\\prebuilt\\%ANDROID_NDK_HOST%\\bin\\make.exe" -j%NUMBER_OF_PROCESSORS%
                '''
            }
        }
        stage('androiddeployqt') {
            agent {
                label 'android && qt5'
            }
            steps {
                bat '''
                    cd build_android
                    "%QT_ANDROID_PREFIX%\\bin\\qmake.exe" -install qinstall -exe libqmlevemon.so .\\android-build\\libs\\armeabi-v7a\\libqmlevemon.so
                    "%QT_ANDROID_PREFIX%\\bin\\androiddeployqt.exe" --input ./android-libqmlevemon.so-deployment-settings.json --output ./android-build --android-platform %ANDROID_NDK_PLATFORM% --jdk "%JDK_ROOT%" --gradle
                    copy /y android-build\\build\\outputs\\apk\\android-build-debug.apk .\\
                    ren android-build-debug.apk qmlevemon-debug.apk
                '''
                archiveArtifacts 'build_android/qmlevemon-debug.apk'
            }
        }
        stage('Build (Linux)') {
            agent {
                label 'linux && qt5'
            }
            steps {
                sh '''
                echo QT_PREFIX=$QT_PREFIX
                rm -rf build_linux/
                mkdir -p build_linux/
                cd build_linux/
                echo CMake options: -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$QT_PREFIX -DBUILD_TESTING=OFF
                cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$QT_PREFIX -DBUILD_TESTING=OFF ../
                make -j$(($(nproc)+1))
                '''
            }
        }
        stage('Build (Linux flatpak)') {
            agent {
                label 'linux && qt5'
            }
            steps {
                sh '''
                cd build_linux/
                cmake --build . --target flatpak-package
                '''
                archiveArtifacts 'build_linux/qmlevemon.flatpak'
            }
        }
        stage('Build (Android-cmake)') {
            agent {
                label 'android && qt5'
            }
            steps {
                bat '''
                    echo ======================== ANDROID-CMAKE ===============================
                    rem echo PATH: %PATH%
                    echo QT_ANDROID_PREFIX: %QT_ANDROID_PREFIX%
                    where cmake
                    set MAKE_PROGRAM=%ANDROID_NDK_ROOT%/prebuilt/%ANDROID_NDK_HOST%/bin/make.exe
                    set TOOLCHAIN_FILE=%ANDROID_NDK_ROOT%/build/cmake/android.toolchain.cmake
                    if exist build_android_cmake rmdir /s /q build_android_cmake
                    mkdir build_android_cmake
                    cd build_android_cmake
                    cmake ^
                      -G "Unix Makefiles" ^
                      -DCMAKE_MAKE_PROGRAM=%MAKE_PROGRAM% ^
                      -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% ^
                      -DANDROID_PLATFORM=%ANDROID_NDK_PLATFORM% ^
                      -DCMAKE_PREFIX_PATH=%QT_ANDROID_PREFIX% ^
                      -DCMAKE_FIND_ROOT_PATH=%QT_ANDROID_PREFIX% ^
                      -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH ^
                      -DCMAKE_BUILD_TYPE=Release ^
                      -DBUILD_TESTING=OFF ^
                      -DBUILD_FOR_ANDROID=ON ^
                      -DCMAKE_VERBOSE_MAKEFILE=ON ^
                      ../
                    cmake --build . --target all
                '''
                //archiveArtifacts 'build_android_cmake/qmlevemon.flatpak'
            }
        }
    }
}
