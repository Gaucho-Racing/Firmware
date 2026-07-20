# configure_cubemx_paths.ps1
# Updates STM32CubeMX template paths using the current repository location.
#
# Expected repository structure:
# Firmware/
# ├── tools/configure_cubemx_paths.ps1
# ├── CUBEMXTESTING/CUBEMXTESTING.ioc
# └── Lib/CubeMXTemplates/Autogen/Inc/can_cfg_h.ftl

$ErrorActionPreference = "Stop"

# Script location: Firmware/tools
$toolsDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path

# Repository root: Firmware
$repoRoot = Split-Path -Parent $toolsDirectory

$iocPath = Join-Path $repoRoot "CUBEMXTESTING\CUBEMXTESTING.ioc"
$projectPath = Join-Path $repoRoot "CUBEMXTESTING"
$templatePath = Join-Path $repoRoot "Lib\CubeMXTemplates"

if (!(Test-Path -LiteralPath $iocPath)) {
    Write-Error "Could not find CubeMX project file: $iocPath"
    exit 1
}

if (!(Test-Path -LiteralPath $templatePath)) {
    Write-Error "Could not find CubeMX template folder: $templatePath"
    exit 1
}

$templateFile = Join-Path $templatePath "Autogen\Inc\can_cfg_h.ftl"
if (!(Test-Path -LiteralPath $templateFile)) {
    Write-Error "Could not find CubeMX template file: $templateFile"
    exit 1
}

# CubeMX .ioc escaping:
# C:\Users\... becomes C\:\\Users\\...
function Convert-ToCubeMXPath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    return $Path.Replace("\", "\\").Replace(":", "\:")
}

$escapedProjectPath = Convert-ToCubeMXPath -Path $projectPath
$escapedTemplatePath = Convert-ToCubeMXPath -Path $templatePath

$desiredDestinationLine = "ProjectManager.TemplateDestinationPath=$escapedProjectPath"
$desiredSourceLine = "ProjectManager.TemplateSourcePath=$escapedTemplatePath"

# CubeMX expects a leading escaped backslash and a trailing comma here.
$desiredTemplatesLine = "ProjectManager.TemplatesList=\\Autogen\\Inc\\can_cfg_h.ftl,"

$content = Get-Content -LiteralPath $iocPath

function Set-OrAddLine {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Content,

        [Parameter(Mandatory = $true)]
        [string]$Key,

        [Parameter(Mandatory = $true)]
        [string]$Replacement
    )

    $pattern = "^" + [regex]::Escape($Key) + "=.*$"
    $found = $false

    $updated = foreach ($line in $Content) {
        if ($line -match $pattern) {
            $found = $true
            $Replacement
        }
        else {
            $line
        }
    }

    if (!$found) {
        $updated += $Replacement
    }

    return ,$updated
}

$content = Set-OrAddLine `
    -Content $content `
    -Key "ProjectManager.TemplateDestinationPath" `
    -Replacement $desiredDestinationLine

$content = Set-OrAddLine `
    -Content $content `
    -Key "ProjectManager.TemplateSourcePath" `
    -Replacement $desiredSourceLine

$content = Set-OrAddLine `
    -Content $content `
    -Key "ProjectManager.TemplatesList" `
    -Replacement $desiredTemplatesLine

# Preserve a normal text encoding without adding a UTF-8 BOM.
[System.IO.File]::WriteAllLines(
    $iocPath,
    $content,
    [System.Text.UTF8Encoding]::new($false)
)

Write-Host ""
Write-Host "CubeMX template configuration updated successfully."
Write-Host ""
Write-Host $desiredDestinationLine
Write-Host $desiredSourceLine
Write-Host $desiredTemplatesLine