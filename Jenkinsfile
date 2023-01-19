/* Requires the Docker Pipeline plugin */
pipeline {
    agent { docker { image 'gcc:4.9' } }
    stages {
        stage('build') {
            steps {
                sh 'make'
            }
        }
    

	stage('Test') {
            steps {
                echo 'Testing'
            }
        }

 	stage('Sanity check') {
            steps {
                input "Does the staging environment look ok?"
            }
        }

        stage('Deploy') {
            steps {
                echo 'Deploying'
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
