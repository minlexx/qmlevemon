pipeline {
    agent {
        label 'windows && qt5'
    }

    stages {
        stage('Build') {
            steps {
                echo 'Building...'
                checkout scm
                cmakeBuild buildDir: 'build', buildType: 'Release', cleanBuild: true, generator: 'NMake Makefiles', installation: 'InSearchPath', sourceDir: '.', steps: [[args: 'all', withCmake: true]]
            }
        }
    }
}
