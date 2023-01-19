/* Requires the Docker Pipeline plugin */
pipeline {
    agent { docker { image 'gcc:4.9' } }
    stages {
        stage('build') {
            steps {
                sh 'make'
            }
        }
    }
}
