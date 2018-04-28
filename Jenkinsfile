pipeline {
    agent {
        label 'windows && qt5'
    }

    stages {
        stage('Build') {
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
                archiveArtifacts 'build/out'
            }
        }
    }
}
