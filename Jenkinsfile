/* Requires the Docker Pipeline plugin */
pipeline {
    agent { docker { image 'gcc:4.9' } }
    stages {
        stage('build') {
            steps {
                sh 'make'
            }
        }
        
        stage('Deploy') {
            steps {
                timeout(time: 3, unit: 'MINUTES') {
                    retry(5) {
                        sh 'echo "should be deployed"'
                    }
                }
            }
        }
        
        stage('Test') {
            steps {
                sh 'echo "Success!"; exit 0'
            }
        }
    }
    
    post {
        always {
            echo 'This will always run'
        }
        success {
            echo 'This will run only if successful'
        }
        failure {
            echo 'This will run only if failed'
        }
        unstable {
            echo 'This will run only if the run was marked as unstable'
        }
        changed {
            echo 'This will run only if the state of the Pipeline has changed'
            echo 'For example, if the Pipeline was previously failing but is now successful'
        }
    }
}
