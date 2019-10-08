
<?php

$date = new DateTime();
echo $date->getTimestamp();
echo "\n";
  
$tz = $date->getTimezone();
echo $tz->getName();
echo "\n";

$timestamp = strtotime('22-09-2008');  
echo "timestemp = ".$timestamp;
echo "\n";
  
$d=mktime(11, 14, 54, 8, 12, 2014);
echo "Created date is " . date("Y-m-d h:i:sa", $d);
echo "\n";
  
$now = strtotime("Now");
echo "Now = ". $now . "\n";

echo "current time = " .date("Y-m-d h:i:sa", $now);
echo "\n";

date_default_timezone_set("America/Chicago");
$now = strtotime("Now");
echo "current time = " .date("Y-m-d h:i:sa", $now);
echo "\n";

$tnow =  time() ;
echo "unix time ". $tnow;
echo "\n";

  ;?>
