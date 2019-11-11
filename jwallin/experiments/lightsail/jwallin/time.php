
<?php





$milliseconds = round(microtime(true) * 1000);
echo $milliseconds;


$mseconds = (microtime(true) );
$fs = $mseconds - floor($mseconds);

$dd = date('Y/m/d H:i:s');
echo "<p>" . $dd . "  " . $fs;

?>
