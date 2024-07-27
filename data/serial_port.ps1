[System.IO.Ports.SerialPort]::GetPortNames();
$portNumber = Read-Host -Prompt "provide COM <number>";
$port = New-Object System.IO.Ports.SerialPort ("COM" + $portNumber), 9600, None, 8, one;
$port.Open();
while (1) {
    $command = Read-Host;
    $port.WriteLine($command)
    $port.ReadExisting();
}