pipeline {
    agent {
        label 'windows && qt5'
    }

    stages {
        stage('Build') {
            steps {
                echo 'Building...'
                checkout scm
                bat 'call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"'
                bat 'echo PATH: %PATH%'
                bat 'echo LIB: %LIB%'
                bat 'echo INCLUDE: %INCLUDE%'
                bat 'echo QT_PREFIX: %QT_PREFIX%'
                bat 'rmdir /s /q build'
                bat 'mkdir build'
                bat 'cd build'
                bat 'cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_PREFIX% -DBUILD_TESTING=OFF ../'
            }
        }
    }
}
