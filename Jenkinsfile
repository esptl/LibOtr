// Jenkins pipeline for the Esp.ThirdParty.LibOtr NuGet package.
//
// Our Jenkins has no "Pipeline script from SCM", so paste this into the Script box
// of a Pipeline job (this file in git stays the source of truth). The Checkout
// stage clones the repo itself - set gitCredentialsId to the ID of the "build
// agent" credential the standard jobs use (Manage Jenkins -> Credentials).
//
// Mirrored to the internal GitLab (git.internal.esptl.com/Softphone/libotr) so
// the job can use the same build-agent credential as the other package repos;
// github.com/esptl/LibOtr remains the upstream and is pushed to manually.
//
// Alternatively use a standard freestyle job (like NuGet_Core): Git SCM on this
// repo + label BuildAgent2022 + one batch step "call BuildNuget.bat" + a batch
// step with the git add/commit/push lines from the last stage below.
//
// Agent prerequisites:
//  - Visual Studio 2026 (18.x) with C++/CLI (MSVC v145)
//  - .NET 10 SDK (for the Esp.ThirdParty.LibOtr.Managed net10.0-windows wrapper)
//  - access to https://svw-esp-nuge.internal.esptl.com

def repoUrl = 'https://git.internal.esptl.com/Softphone/libotr'
def gitCredentialsId = 'build-agent'

pipeline {
    agent { label 'Buildagent2022' }
    options { disableConcurrentBuilds() }
    stages {
        stage('Checkout') {
            steps {
                git branch: 'master', credentialsId: gitCredentialsId, url: repoUrl
            }
        }
        stage('Build, pack, push') {
            steps {
                bat 'call BuildNuget.bat'
            }
        }
        stage('Commit version bump') {
            steps {
                bat '''git add NuGet.LibOtr/Package.nuspec
git -c user.name=Jenkins -c user.email=jenkins@esptl.com commit -m "Increment Esp.ThirdParty.LibOtr nuget version [ci skip]"
git push origin HEAD:master'''
            }
        }
    }
}
