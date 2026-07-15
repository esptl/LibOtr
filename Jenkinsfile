// Jenkins pipeline for the Esp.ThirdParty.LibOtr NuGet package.
//
// Job setup: New Item -> Pipeline -> "Pipeline script from SCM" -> this repo, script path "Jenkinsfile".
// The final stage pushes the nuspec version bump back to master, so the job's git
// credential needs push rights. To avoid retrigger loops, exclude the Jenkins
// committer (or the "[ci skip]" message) from SCM polling.
//
// Agent prerequisites:
//  - Visual Studio 2026 (18.x) with C++/CLI (MSVC v145)
//  - .NET 10 SDK (for the Esp.ThirdParty.LibOtr.Managed net10.0-windows wrapper)
//  - access to https://svw-esp-nuge.internal.esptl.com

pipeline {
    agent { label 'BuildAgent2022' }
    options { disableConcurrentBuilds() }
    stages {
        stage('Build, pack, push') {
            steps {
                bat 'BuildNuget.bat'
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
