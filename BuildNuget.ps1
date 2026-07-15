# Builds and publishes the Esp.ThirdParty.LibOtr NuGet package
# (C++/CLI wrapper + C# managed layer over libotr/libgcrypt/libgpg-error).
#
#   .\BuildNuget.ps1                 # bump version, build, pack, push to the ESP feed
#   .\BuildNuget.ps1 -NoPush         # local test: build + pack only
#   .\BuildNuget.ps1 -NoBump -NoPush # rebuild the current nuspec version without pushing
#
# The solution has no ProjectDependencies for the C++/CLI DLL or the C# wrapper
# (the wrapper uses <Reference>+HintPath, not ProjectReference), and
# ConsoleApplication1 (net452) can't reference the net10 wrapper - so the projects
# are built individually in dependency order instead of via libotr-vs.sln.
# Requires Visual Studio 2026 (18.x) with C++/CLI support and the .NET 10 SDK.
param(
    [switch]$NoPush,
    [switch]$NoBump,
    [string]$FeedUrl = "https://svw-esp-nuge.internal.esptl.com/v3/index.json",
    [string]$ApiKey = $env:NUGET_API_KEY
)
$ErrorActionPreference = "Stop"
if (-not $ApiKey) { $ApiKey = "f9214bc8c8cee05f434990c0c77719b2" }  # same key as Core\BuildNuget.bat

$RepoRoot = $PSScriptRoot
$NuGetDir = Join-Path $RepoRoot "NuGet.LibOtr"
$NuGet    = Join-Path $NuGetDir "NuGet.exe"

# Locate VS 2026 MSBuild via vswhere
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw "vswhere.exe not found - install Visual Studio with C++/CLI support" }
$VSRoot  = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
if (-not $VSRoot) { throw "No Visual Studio installation with MSBuild found" }
$MSBuild = Join-Path $VSRoot "MSBuild\Current\Bin\MSBuild.exe"

function Run([string]$exe, [string[]]$argv) {
    Write-Host ">> $exe $($argv -join ' ')"
    & $exe @argv
    if ($LASTEXITCODE -ne 0) { throw "$exe exited with $LASTEXITCODE" }
}

# 1. Bump the package version (Jenkins commits the nuspec back after a green build)
if (-not $NoBump) {
    & "$RepoRoot\IncrementNugetVersionNumber.ps1" -NuSpecFile "$NuGetDir\Package.nuspec"
}

# 2. Build in dependency order, skipping ConsoleApplication1/OTRTest
$common = @("-p:Configuration=Release", "-p:Platform=x64", "-p:SolutionDir=$RepoRoot\", "-v:minimal", "-nologo")
Run $MSBuild (@("$RepoRoot\libgpg-error\libgpg-error.vcxproj") + $common)
Run $MSBuild (@("$RepoRoot\libgcrypt\libgcrypt.vcxproj") + $common)
Run $MSBuild (@("$RepoRoot\libotr\libotr.vcxproj") + $common)
Run $MSBuild (@("$RepoRoot\Esp.ThirdParty.LibOtr\Esp.ThirdParty.LibOtr.vcxproj") + $common)
Run $MSBuild (@("$RepoRoot\Esp.ThirdParty.LibOtr.Managed\Esp.ThirdParty.LibOtr.Managed.csproj", "-restore") + $common)

# 3. Stage the DLLs into the package layout
# (vcxprojs output to SolutionDir\x64\Release\ when SolutionDir is passed)
$NativeDll  = Join-Path $RepoRoot "x64\Release\Esp.ThirdParty.LibOtr.dll"
$ManagedDll = Join-Path $RepoRoot "Esp.ThirdParty.LibOtr.Managed\bin\x64\Release\net10.0-windows\Esp.ThirdParty.LibOtr.Managed.dll"
if (-not (Test-Path $NativeDll))  { throw "C++/CLI DLL not found at $NativeDll" }
if (-not (Test-Path $ManagedDll)) { throw "Managed DLL not found at $ManagedDll" }
# nuget pack fails on folders referenced by the nuspec that don't exist; git can't track empty dirs
foreach ($d in "lib", "tools", "content") { New-Item -ItemType Directory -Force (Join-Path $NuGetDir $d) | Out-Null }
Copy-Item $NativeDll  (Join-Path $NuGetDir "lib") -Force
Copy-Item $ManagedDll (Join-Path $NuGetDir "lib") -Force

# 4. Pack + push
Push-Location $NuGetDir
try {
    Get-ChildItem *.nupkg -ErrorAction SilentlyContinue | Remove-Item -Force
    Run $NuGet @("pack", "Package.nuspec", "-NoPackageAnalysis")
    $pkg = Get-ChildItem *.nupkg | Select-Object -First 1
    if (-not $pkg) { throw "No .nupkg produced" }
    Write-Host "Packed $($pkg.Name)"
    if (-not $NoPush) {
        Run $NuGet @("push", $pkg.Name, $ApiKey, "-Source", $FeedUrl, "-NonInteractive")
        Write-Host "Pushed $($pkg.Name) to $FeedUrl"
    }
} finally { Pop-Location }
