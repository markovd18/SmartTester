#!/usr/bin/env groovy

pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Building with cmake...'
                sh 'cmake CMakeLists.txt'
            }
        }
    }
}