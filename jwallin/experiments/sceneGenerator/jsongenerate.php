<?php


$myObj->projectedEarthSize = 0.07;
$myObj->projectedEarthRotation = 5.0;
$myObj->projectedDistanceScale = 0.02;
$myObj->moonScale = 4;
$myObj->shape = "Sphere";

$myJSON = json_encode($myObj);


$myfile = fopen("./jupiterParameters.json","w");
fwrite($myfile, $myJSON);
fclose($myfile);

echo $myJSON;
?>
