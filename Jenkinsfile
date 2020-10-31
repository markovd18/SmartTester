#!/usr/bin/env groovy

pipeline {
    agent none

    stages {
        stage('Build UNIX') {
            agent {
                docker { image: 'kitware/cmake:ci-debian10-x86_64-2020-10-03'}
            }
            steps {
                echo 'Running UNIX build...'
                sh 'cmake CMakeLists.txt'
            }
        }

        stage ('Build Windows') {
            agent {
                docker { image: 'kitware/cmake:ci-debian10-x86_64-2020-04-27'}
            }
            steps {
                echo 'Running Windows build...'
                bat 'cmake CMakeLists.txt'
            }
        }
    }
}
