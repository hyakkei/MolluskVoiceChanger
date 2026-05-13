<#
.SYNOPSIS
    アプリアイコンを差し替えるスクリプト。
    assets/icons/app_icon.png から ICO を生成し、ビルドに反映してキャッシュを更新する。

.PARAMETER Config
    ビルド構成。"Debug"（デフォルト）または "Release"。

.EXAMPLE
    .\scripts\update_icon.ps1
    .\scripts\update_icon.ps1 -Config Release
#>
param(
    [string]$Config = "Debug"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$Root     = Split-Path -Parent $PSScriptRoot
$PngPath  = Join-Path $Root "assets\icons\app_icon.png"
$IcoPath  = Join-Path $Root "assets\icons\app_icon.ico"
$BuildDir = Join-Path $Root "build"
$JuceIco  = Join-Path $BuildDir "MolluskVoiceChanger_artefacts\JuceLibraryCode\icon.ico"

# ---- 前提チェック ----
if (-not (Test-Path $PngPath)) {
    Write-Error "app_icon.png が見つかりません: $PngPath"
    exit 1
}

# ---- アプリ停止 ----
Stop-Process -Name "Mollusk Voice Changer" -Force -ErrorAction SilentlyContinue

# ---- PNG → ICO 生成（透過保持、PNG 内包形式） ----
Write-Host "[1/4] ICO 生成中..."
Add-Type -AssemblyName System.Drawing

$sizes       = @(16, 32, 48, 256)
$sourceImg   = [System.Drawing.Image]::FromFile((Resolve-Path $PngPath).Path)
$pngDataList = @()

foreach ($size in $sizes) {
    $bmp = New-Object System.Drawing.Bitmap($size, $size)
    $g   = [System.Drawing.Graphics]::FromImage($bmp)
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.SmoothingMode     = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.DrawImage($sourceImg, 0, 0, $size, $size)
    $g.Dispose()
    $ms = New-Object System.IO.MemoryStream
    $bmp.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png)
    $pngDataList += , $ms.ToArray()
    $ms.Dispose()
    $bmp.Dispose()
}
$sourceImg.Dispose()

$stream = [System.IO.File]::Open($IcoPath, [System.IO.FileMode]::Create)
$writer = New-Object System.IO.BinaryWriter($stream)
$writer.Write([uint16]0)
$writer.Write([uint16]1)
$writer.Write([uint16]$sizes.Count)

$dataOffset = [uint32](6 + 16 * $sizes.Count)
for ($i = 0; $i -lt $sizes.Count; $i++) {
    $sz  = $sizes[$i]
    $dat = $pngDataList[$i]
    if ($sz -ge 256) { $wh = [byte]0 } else { $wh = [byte]$sz }
    $writer.Write($wh)
    $writer.Write($wh)
    $writer.Write([byte]0)
    $writer.Write([byte]0)
    $writer.Write([uint16]1)
    $writer.Write([uint16]32)
    $writer.Write([uint32]$dat.Length)
    $writer.Write($dataOffset)
    $dataOffset += [uint32]$dat.Length
}
foreach ($dat in $pngDataList) { $writer.Write($dat) }
$writer.Close()
Write-Host "    -> $IcoPath ($((Get-Item $IcoPath).Length) bytes)"

# ---- 1回目ビルド（juceaide に icon.ico を生成させる） ----
Write-Host "[2/4] 1回目ビルド ($Config)..."
cmake --build $BuildDir --config $Config 2>&1 | Select-Object -Last 1

# ---- juceaide 生成の icon.ico を透過 ICO で上書き ----
Write-Host "[3/4] JUCE icon.ico を差し替え..."
if (-not (Test-Path $JuceIco)) {
    Write-Error "JUCE の icon.ico が見つかりません。先にフルビルドを行ってください: $JuceIco"
    exit 1
}
Copy-Item $IcoPath $JuceIco -Force
Write-Host "    -> $JuceIco"

# ---- 2回目ビルド（差し替えた icon.ico を EXE に埋め込む） ----
Write-Host "[4/4] 2回目ビルド ($Config)..."
cmake --build $BuildDir --config $Config 2>&1 | Select-Object -Last 1

# ---- Windows エクスプローラーのアイコンキャッシュをクリア ----
Write-Host "[Done] アイコンキャッシュをクリア..."
Stop-Process -Name explorer -Force -ErrorAction SilentlyContinue
Start-Sleep -Milliseconds 500
Remove-Item "$env:LOCALAPPDATA\Microsoft\Windows\Explorer\iconcache*" -Force -ErrorAction SilentlyContinue
Remove-Item "$env:LOCALAPPDATA\Microsoft\Windows\Explorer\thumbcache*" -Force -ErrorAction SilentlyContinue
Start-Process explorer.exe

Write-Host ""
Write-Host "完了。EXE: $BuildDir\MolluskVoiceChanger_artefacts\$Config\Mollusk Voice Changer.exe"
