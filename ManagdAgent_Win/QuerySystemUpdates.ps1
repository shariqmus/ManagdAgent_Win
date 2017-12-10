$session = New-Object -ComObject "Microsoft.Update.Session"
$updatesearcher = $session.CreateUpdateSearcher()
$count = $updatesearcher.GetTotalHistoryCount()
$updates = $updatesearcher.QueryHistory(0, $count)

foreach ($update in $updates) {
  $out = New-Object -Type PSObject -Prop @{
    #‘KB’ = [regex]::match($update.Title,’KB(\d+)’)
    ‘Title’ = $update.Title
    #‘Description’ = $update.Description
    #‘Id’ = $update.UpdateIdentity.UpdateId
    #‘RevisionNumber’ = $update.UpdateIdentity.RevisionNumber
    ‘Date’ = $update.Date
  }
  Clear-Host
  Write-Output $out | Sort-Object -Property Date |  ConvertTo-CSV -NoTypeInformation | Select-Object -Skip 1 # |ft Date -HideTableHeaders | ConvertTo-CSV -NoTypeInformation
}

