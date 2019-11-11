
<?php
// define variables and set to empty values


$macid = "33a";

if (!preg_match("/^[a-zA-Z]*$/",$macid)) {
  echo "ok\n";
}


echo $macid;
echo "\n";

$ip = "192.168.1.23";
$ip = "192.168.1.2333";
#if (!preg_match("/^[0-9]*\.[0-9]*\.[0-9]*\.[0-9]$/", $ip))
//if (!preg_match("/^[0-9]{1,3}$/", $ip))
if (!preg_match("/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/", $ip))
{
echo "duck";
}


?>

