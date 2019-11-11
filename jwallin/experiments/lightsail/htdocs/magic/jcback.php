


<?php
include "validate.php";

echo "<!DOCTYPE HTML>  ";
echo "<html>";
echo "<head>";
echo "<style>";
echo ".error {color: #FF0000;}";
echo "</style>";
echo "</head>";
echo "<body>  ";


// read the defintion file for the script
// This includes names, field labels, data types, lower/upper limits and a default value.
// Convert them into the base object and into an array.

$defname = "./jupiterDefinition.csv";
if (file_exists($defname) ) {

	// read in the Definition file
	$nstring = file_get_contents($defname);

	// break the file up into lines
	$lines = explode(PHP_EOL, $nstring);

	// parse the csv
	$inputs = array();
	foreach ($lines as $line) {
		$inputs[] = str_getcsv($line);
	}
	array_pop($inputs);
	//print_r($inputs);

	$myArray = array();
	for ($ii = 0; $ii < count($inputs)  ; $ii++) {
		$key = $inputs[$ii][1];
		$defaultValue = $inputs[$ii][5];
		//echo $key . " --- " . $defaultValue . "<br>\n";
		$myArray[$key] = $defaultValue;
	}

	$myObj = (object) $myArray;
} else {
	echo "<H1> NO DEFINTION FILE</H1>\n";
}


// Read the current simulation values.  If there is not a file,
// create the current value file.
$fname = "./jupiterParameters.json";
if (file_exists($fname)) {
	$myfile = fopen($fname,"r");
	$myJSON = fread($myfile, filesize($fname) ) ;;
	fclose($myfile);
	$myObj = json_decode($myJSON);
} else {
	$myJSON = json_encode($myObj);
	// create the initial file
	$myfile = fopen($fname, "w"); 
	fwrite($myfile, $myJSON);	
	fclose($myfile);
}

$myArray = (array) $myObj;




// Process the posted data - if any
if ($_SERVER["REQUEST_METHOD"] == "POST") {

	echo "posting stuff <br>\n";

	$errorArray = array();
	// loop through input fields
	for ($ii = 0; $ii< count($inputs); $ii++) {

		// identify the keys, keyType, and default values
		$key = $inputs[$ii][1];
		$keyType = $inputs[$ii][3];
		$value = $myArray[$key];
		//echo "KEY = " .$key . "<br>\n";


		if (empty($_POST[$key])) {
			$pvalue = "";
			//echo "EMPTY <p>\n";
		} else {
			$pvalue = test_input($_POST[$key]);
			//echo "old value = " . $value . "<br>\n";
//			echo "new value = " .$pvalue . "<br>\n";
			list ($qvalue, $err) = validateEntry($pvalue, $keyType);
			//echo "validated value = " . $qvalue . "<br>\n";
			//echo "error " . $err . "<br>\n";
			// if there is no error, update the value
			// if there is an error, keep the old value
			if ($err == "") {			
				$value = $qvalue;
				$errorArray[$key] = "";
			} else {
				$errorArray[$key] = $pvalue . "  " . $err;
			} 
		}
		$myArray[$key] = $value;
	}
} else {

	echo "no post<br> \n\n";
} 



$myObj = (object) $myArray;
$myJSON = json_encode($myObj);


echo "<form action=\"jc.php\" method=\"post\"> \n\n";
for ($ii = 0; $ii< count($inputs); $ii++){

	$title =  $inputs[$ii][0];
	$type =  $inputs[$ii][2];
	$name =  $inputs[$ii][1];
	$value = $myArray[$name];

	echo $title . ":<input type=\"" . $type . "\" name=\"" . $name  . "\" value=\"" . $value . "\">";
	if ($errorArray[$name] == ""){
	echo "";
	} else {
		echo " <span class=\"error\">*  " . $errorArray[$name] . "\"</span>\n";
	}
	echo "<br><br>\n";
};


echo "<input type=\"submit\" name=\"submit\" value=\"Submit\">  ";
echo "</form>";
echo "</body>";
echo "</html>";
echo "";


echo "before <br>\n";
echo file_get_contents($fname) . "<br>\n";


$myJSON = json_encode($myObj);
echo "yyyy = " . $myJSON . "<br>\n";

echo $fname .  "  getting ready to writei<br> \n";
echo $myJSON . "  json <br>\n";
file_put_contents($fname, $myJSON);


echo file_get_contents($fname) . "<br>\n";


$myfile = fopen($fname, "w"); //"./jupiterParameters.json","w");
fwrite($myfile, $myJSON);	
fclose($myfile);


echo file_get_contents($fname) . "<br>\n";


?>
