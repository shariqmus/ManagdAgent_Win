Clear-Host

Try
{
    $AV=Get-Service ds_agent -EA Stop | Select Status 
    Write-Host $AV.Status -NoNewline
}
Catch
{
     Write-Host "NotFound" -NoNewline
}