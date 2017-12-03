Clear-Host
$Average=Get-WmiObject win32_processor | Measure-Object -property LoadPercentage -Average | Select Average
Write-Host $Average.Average -NoNewline
