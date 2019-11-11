<?php
header('Content-Type: text/event-stream');
header('Cache-Control: no-cache');

$time = date('r');
echo "data: ";
echo "<p> The server time is: {$time} </p>";
echo "clap clap clap\n";
//echo "stuff: FRED\n";
flush();
?>

