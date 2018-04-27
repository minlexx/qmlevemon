pipeline {
    agent {
        label 'windows && qt5'
    }

    stages {
        stage('Build') {
            steps {
                echo 'Building...'
                checkout scm
                bat 'call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"'
                cmakeBuild buildDir: 'build', buildType: 'Release', cleanBuild: true, generator: 'NMake Makefiles', installation: 'InSearchPath', sourceDir: '.', steps: [[args: 'all', withCmake: true]]
            }
        }
    }
}
