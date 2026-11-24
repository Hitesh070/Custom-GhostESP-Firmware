param(
    [string]$Port = "COM5",
    [int]$Baud = 460800
)

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$espProfile = "C:\Espressif\tools\Microsoft.v6.1.1.PowerShell_profile.ps1"
if (Test-Path $espProfile) {
    . $espProfile
}

Write-Host "[+] Flashing GhostESP God Mode V3 to $Port at ${Baud} baud..." -ForegroundColor Cyan

python -m esptool --chip esp32s3 -p $Port -b $Baud `
  --before default-reset --after hard-reset write-flash `
  --flash-mode dio --flash-size 16MB --flash-freq 80m `
  0x0000 "$scriptDir\binaries\v3\bootloader.bin" `
  0x8000 "$scriptDir\binaries\v3\partition-table.bin" `
  0x10000 "$scriptDir\binaries\v3\Ghost_ESP_IDF.bin"
