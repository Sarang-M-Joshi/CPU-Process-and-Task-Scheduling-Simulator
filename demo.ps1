$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot

Write-Host 'Building DOOMSDAY // TEMPORAL CORE...'
& g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp scheduler/Scheduler.cpp -o cpu_scheduler.exe
if ($LASTEXITCODE -ne 0) { throw 'Application build failed.' }

Write-Host 'Building and running the DSA, storage, and scheduling checks...'
& g++ -std=c++17 -Wall -Wextra -Wpedantic tests/structures_tests.cpp scheduler/Scheduler.cpp -o structures_tests.exe
if ($LASTEXITCODE -ne 0) { throw 'Check executable build failed.' }
& .\structures_tests.exe
if ($LASTEXITCODE -ne 0) { throw 'Project checks failed.' }

Write-Host 'Running the sample comparison and saved-event replay...'
$commands = @(
    '2', # Temporal Simulation
    '6', # Load sample workload
    '4', # Compare policies; this archives all three completed runs
    '2', # Comparison Round Robin quantum
    '2', # Select Round Robin for the next run
    '2', # Round Robin quantum
    '2', # Reduced animation
    '5', # Replay latest archived run
    '1', # Timed replay
    '0', # Leave Temporal Simulation
    '0'  # Shut down
)
$output = $commands | & .\cpu_scheduler.exe 2>&1 | Out-String
Write-Output $output
if ($LASTEXITCODE -ne 0) { throw 'Demonstration executable failed.' }
if ($output -notmatch 'MULTIVERSE COMPARISON' -or
    $output -notmatch 'TIMELINE ARCHIVE // REPLAY' -or
    $output -notmatch 'FINAL SAVED GANTT' -or
    $output -notmatch 'SAVED METRICS') {
    throw 'Demonstration did not show comparison and event replay output.'
}

$oldAnimationSetting = $env:DOOMSDAY_ANIMATION
$env:DOOMSDAY_ANIMATION = 'reduced'
$stepCommands = [System.Collections.Generic.List[string]]::new()
$stepCommands.Add('4') # Open Timeline Archives
$stepCommands.Add('2') # Step-controlled replay
for ($i = 0; $i -lt 6; ++$i) { $stepCommands.Add('') } # Resume each recorded segment
$stepCommands.Add('0') # Shut down
$stepOutput = $stepCommands.ToArray() | & .\cpu_scheduler.exe 2>&1 | Out-String
if ($null -eq $oldAnimationSetting) { Remove-Item Env:DOOMSDAY_ANIMATION -ErrorAction SilentlyContinue }
else { $env:DOOMSDAY_ANIMATION = $oldAnimationSetting }
$stepCount = ([regex]::Matches($stepOutput, 'Replay controls')).Count
if ($LASTEXITCODE -ne 0 -or $stepCount -ne 6 -or $stepOutput -notmatch 'SAVED SUMMARY') {
    throw 'Step-controlled replay did not complete with all pause/resume controls.'
}

$oldAnimationSetting = $env:DOOMSDAY_ANIMATION
$oldNoColorSetting = $env:NO_COLOR
$env:DOOMSDAY_ANIMATION = 'off'
$env:NO_COLOR = '1'
$plainOutput = @('0') | & .\cpu_scheduler.exe 2>&1 | Out-String
$plainExit = $LASTEXITCODE
$dsaCommands = @('3', '1', '1', '42', '0', '0', '0')
$dsaOutput = $dsaCommands | & .\cpu_scheduler.exe 2>&1 | Out-String
$dsaExit = $LASTEXITCODE
if ($null -eq $oldAnimationSetting) { Remove-Item Env:DOOMSDAY_ANIMATION -ErrorAction SilentlyContinue }
else { $env:DOOMSDAY_ANIMATION = $oldAnimationSetting }
if ($null -eq $oldNoColorSetting) { Remove-Item Env:NO_COLOR -ErrorAction SilentlyContinue }
else { $env:NO_COLOR = $oldNoColorSetting }
if ($plainExit -ne 0 -or $dsaExit -ne 0 -or -not $plainOutput.Contains('CORE SHUTDOWN') -or $plainOutput.Contains([char]27) -or
    $dsaOutput -notmatch 'SLL // PROCESS CHAIN INSPECTOR' -or $dsaOutput -notmatch 'Inserted at beginning\.') {
    throw 'Animation-off plain-text startup did not complete cleanly.'
}

Write-Host 'Day 5 demonstration completed. The replayable event log is simulation_events.log.'
