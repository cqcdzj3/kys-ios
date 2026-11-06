param(
    [Parameter(Mandatory = $true)][string]$Ipa,
    [Parameter(Mandatory = $true)][string]$GameDir,
    [string]$Output = ""
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem

if (-not (Test-Path -LiteralPath $Ipa)) { throw "IPA 不存在: $Ipa" }
if (-not (Test-Path -LiteralPath $GameDir)) { throw "游戏目录不存在: $GameDir" }
if (-not $Output) {
    $Output = Join-Path (Split-Path -Parent (Resolve-Path -LiteralPath $Ipa)) 'kys-with-data.ipa'
}

$tmp = Join-Path $env:TEMP ("kysipa-" + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $tmp | Out-Null
try {
    Write-Host "[1/4] 解压 IPA ..."
    [System.IO.Compression.ZipFile]::ExtractToDirectory($Ipa, $tmp)

    $app = Get-ChildItem -LiteralPath $tmp -Recurse -Directory -Filter '*.app' |
           Where-Object { $_.Parent.Name -eq 'Payload' } | Select-Object -First 1
    if (-not $app) { throw "IPA 里找不到 Payload/*.app" }

    $gameOut = Join-Path $app.FullName 'game'
    New-Item -ItemType Directory -Force -Path $gameOut | Out-Null

    Write-Host "[2/4] 复制游戏数据 -> $gameOut"
    Copy-Item -Path (Join-Path $GameDir '*') -Destination $gameOut -Recurse -Force

    $size = (Get-ChildItem -LiteralPath $gameOut -Recurse -File | Measure-Object Length -Sum).Sum / 1MB
    Write-Host ("      数据大小: {0:N1} MB" -f $size)

    Write-Host "[3/4] 重新打包 ..."
    $payload = Join-Path $tmp 'Payload'
    if (Test-Path -LiteralPath $Output) { Remove-Item -LiteralPath $Output -Force }
    [System.IO.Compression.ZipFile]::CreateFromDirectory(
        $tmp, $Output,
        [System.IO.Compression.CompressionLevel]::NoCompression,
        $false)

    Write-Host "[4/4] 完成: $Output"
    Write-Host ("      {0:N1} MB" -f ((Get-Item -LiteralPath $Output).Length / 1MB))
}
finally {
    Remove-Item -LiteralPath $tmp -Recurse -Force -ErrorAction SilentlyContinue
}
