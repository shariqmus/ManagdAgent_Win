#####################################################
# Note: terminate the status with a ;
#####################################################

Clear-Host

Try
{
    $AV=Get-Service ds_agent -EA Stop | Select Status 
    Write-Host "$AV.Status;"
}
Catch
{
     Write-Host "NotFound;"
}
