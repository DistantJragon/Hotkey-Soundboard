Get-ChildItem -Path "$PSScriptRoot\Public" -Recurse | ForEach-Object {
    . $_.FullName
}
