# Copy or slice wordlists for Mr. CrackBot AI SD cards — authorized lab use only.
param(
    [ValidateSet("kali-classic", "lab-subset", "subset")]
    [string]$Source = "lab-subset",
    [string]$Dest = "",
    [string]$SubsetInput = "",
    [int]$MaxLines = 50000
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$LabSubset = Join-Path $Root "data\wordlists\rockyou_lab_subset.txt"

function Write-Usage {
    Write-Host @"
Usage:
  .\scripts\fetch_wordlists.ps1 -Source lab-subset -Dest D:\sd\rockyou_lab.txt
  .\scripts\fetch_wordlists.ps1 -Source kali-classic -Dest D:\sd\rockyou.txt
  .\scripts\fetch_wordlists.ps1 -Source subset -SubsetInput C:\bench\rockyou2024.txt -Dest D:\sd\rockyou2024.txt -MaxLines 50000
"@
}

if (-not $Dest) { Write-Usage; exit 1 }

switch ($Source) {
    "lab-subset" {
        Copy-Item $LabSubset $Dest -Force
        Write-Host "Copied lab subset -> $Dest"
    }
    "kali-classic" {
        $kali = "/usr/share/wordlists/rockyou.txt"
        if (Test-Path $kali) {
            Copy-Item $kali $Dest -Force
        } else {
            Write-Error "Kali path not found. Use WSL/Kali VM or copy rockyou.txt manually. See docs/WORDLISTS.md"
        }
    }
    "subset" {
        if (-not (Test-Path $SubsetInput)) { Write-Error "SubsetInput not found: $SubsetInput" }
        $count = 0
        $out = New-Object System.Collections.Generic.List[string]
        Get-Content $SubsetInput | ForEach-Object {
            $line = $_.Trim()
            if ($line.Length -lt 8 -or $line.Length -gt 40) { return }
            if ($line -notmatch '^[\x20-\x7E]+$') { return }
            [void]$out.Add($line)
            $script:count++
            if ($count -ge $MaxLines) { break }
        }
        $out | Set-Content $Dest -Encoding utf8
        Write-Host "Wrote $count lines -> $Dest"
    }
}
