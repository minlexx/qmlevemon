pipeline {
    agent {
        label 'qt5 && windows && android'
    }

    stages {
        stage('Build') {
            agent { label 'windows' }
            steps {
                echo 'Building...'
                checkout scm
                bat '''
                    rem echo PATH: %PATH%
                    rem echo LIB: %LIB%
                    rem echo INCLUDE: %INCLUDE%
                    rem echo QT_PREFIX: %QT_PREFIX%
                    where cl
                    if exist build rmdir /s /q build
                    mkdir build
                    cd build
                    echo CMake options: -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_PREFIX% -DBUILD_TESTING=OFF
                    cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_PREFIX% -DBUILD_TESTING=OFF ../
                    cmake --build . --target all
                '''
            }
        }
        stage('windeployqt') {
            agent { label 'windows' }
            steps {
                bat '''
                    if exist build\\out rmdir /s /q build\\out
                    mkdir build\\out
                    cd build\\out
                    copy /y ..\\QMLEVEMon.exe .
                    %QT_PREFIX%\\bin\\windeployqt.exe --release --qmldir ..\\..\\qml --compiler-runtime QMLEVEMon.exe
                    copy /y %windir%\\system32\\libeay32.dll .
                    copy /y %windir%\\system32\\ssleay32.dll .
                '''
                archiveArtifacts 'build/out/'
            }
        }
        stage('Build (android)') {
            agent { label 'android' }
            steps {
                bat '''
                    if exist build_android rmdir /s /q build_android
                    mkdir build_android
                    cd build_android
                    "%QT_ANDROID_PREFIX%\\bin\\qmake.exe" ..\\qmlevemon.pro -spec android-g++ "CONFIG+=release"
                    "%ANDROID_NDK_ROOT%\\prebuilt\\windows-x86_64\\bin\\make.exe" qmake_all
                    "%ANDROID_NDK_ROOT%\\prebuilt\\windows-x86_64\\bin\\make.exe"
                '''
            }
        }
        stage('androiddeployqt') {
            agent { label 'android' }
            steps {
                bat '''
                    cd build_android
                    "%QT_ANDROID_PREFIX%\\bin\\qmake.exe" -install qinstall -exe libqmlevemon.so .\\android-build\\libs\\armeabi-v7a\\libqmlevemon.so
                    "%QT_ANDROID_PREFIX%\\bin\\androiddeployqt.exe" --input ./android-libqmlevemon.so-deployment-settings.json --output ./android-build --android-platform android-27 --jdk "%JDK_ROOT%" --gradle
                '''
            }
        }
    }
}
