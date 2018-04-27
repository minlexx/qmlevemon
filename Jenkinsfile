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
                    call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
                    echo PATH: %PATH%
                    echo LIB: %LIB%
                    echo INCLUDE: %INCLUDE%
                    echo QT_PREFIX: %QT_PREFIX%
                    if exist build rmdir /s /q build
                    mkdir build
                    cd build
                    echo CMake options: -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_PREFIX% -DBUILD_TESTING=OFF
                    cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_PREFIX% -DBUILD_TESTING=OFF ../
                    cmake --build . --target all
                '''
            }
        }
    }
}
