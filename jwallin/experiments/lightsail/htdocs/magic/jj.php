<?php 

$command = escapeshellcmd('./generateCelestial.py');
$output = shell_exec($command);
echo $output;

?>
