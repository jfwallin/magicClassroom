


<?php
 
$fname = "./jupiterParameters.json";
//if (file_exists($fname)) {
	echo "DDDDDDD";
	$myfile = fopen("./jupiterParameters.json","r");
	$myJSON = fread($myfile, filesize($fname) ) ;;
	fclose($myfile);
	$myObj = json_decode($myJSON);
	echo $myJSON;

$myArray = (array) $myObj;

$myArray["shape"] = "dog";

$myObj = (object) $myArray;

$myJSON = json_encode($myObj);
echo "xxxxx \n";
echo $myJSON . "<br>\n";

echo "------------------\n";
//$fname = "./jupiterParameters.json";
echo file_get_contents($fname) . "<br>\n";
$myfile = fopen($fname, "w"); //"./jupiterParameters.json","w");
file_put_contents($myfile, $myJSON);
echo file_get_contents($fname) . "<br>\n";
//fclose($myfile);


//echo $myJSON;





?>
