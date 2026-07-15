param($NuSpecFile = $(throw "Mandatory parameter -NuSpecFile not supplied"), $MajorVersion, $MinorVersion, $RevisionVersion)

# Nuspec-aware variant of Core\IncrementNugetVersionNumber.ps1: matches the <version>
# element regardless of casing/xmlns and publishes plain versions (no -a suffix).

$NuSpecFile = Resolve-Path $NuSpecFile
[xml]$fileContents = Get-Content -Path $NuSpecFile

$element = $fileContents.SelectSingleNode("//*[local-name()='version']")
if ($null -eq $element) { throw "No <version> element found in $NuSpecFile" }

$segments = $element.InnerText.Trim() -split "\."
$v1 = if ($segments.Length -gt 0) { $segments[0] } else { "1" }
$v2 = if ($segments.Length -gt 1) { $segments[1] } else { "0" }
$v3 = if ($segments.Length -gt 2) { $segments[2] } else { "0" }

# Preserve a prerelease suffix (e.g. 1.0.100-beta) if one is ever used
$suffix = ""
if ($v3 -match "^(\d+)(-.*)$") { $v3 = $Matches[1]; $suffix = $Matches[2] }

if ($MajorVersion)    { $v1 = $MajorVersion }
if ($MinorVersion)    { $v2 = $MinorVersion }
if ($RevisionVersion) { $v3 = $RevisionVersion } else { $v3 = ([int]$v3) + 1 }

$newVersion = "$v1.$v2.$v3$suffix"
Write-Host "Setting Version to $newVersion"
$element.InnerText = $newVersion
$fileContents.Save($NuSpecFile)
