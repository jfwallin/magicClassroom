


<?php
include "validate.php";


function createSlider2($sliderID, $sliderDescription, $key, $xmin, $xmax, $xstep, $xvalue) {
$sliderValue = "slider" . $sliderID;
$slideID = "slideID" . $sliderID;
$rangeSliderID = "rangeslider". $sliderID;
$outputID = "output" . $sliderID;

$sclass = "rangeslider";
$cname = "myslider";

echo $sliderDescription . ": <span id=\"" . $sliderValue . "\"></span>\n";
echo "<div class=\"" . $sclass . "\"> \n"; // this is the class name in the css file
echo "  <input type=\"range\" min=\"" . $xmin . "\" max=\"" . $xmax . "\" step=\"" . $xstep . "\" value=\"" . $xvalue . "\"\n";
echo "  class=\"" . $cname . "\" id=\"" . $slideID. "\" name=\"" . $key . "\">\n ";
echo "</div>\n";
echo "\n ";


echo "<script>\n ";
echo "var " . $rangeSliderID . " = document.getElementById(\"" . $slideID . "\");\n ";
echo "var " . $outputID . " = document.getElementById(\"" . $sliderValue . "\"); \n";
echo $outputID . ".innerHTML = " . $rangeSliderID . ".value; \n";
echo $rangeSliderID . ".oninput = function() { \n";
echo $outputID . ".innerHTML = this.value; \n";
echo "}\n";
echo "</script>\n";
}




echo "<!DOCTYPE HTML>\n  ";
echo "<html>\n";
echo "<head>\n";

echo "<link href=\"sliderstyle.css\" rel=\"stylesheet\" type=\"text/css\">\n";
echo "</head>\n";
echo "<body>\n  ";
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
			list ($qvalue, $err) = validateEntry($pvalue, $keyType);
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
}// else { } 



$myObj = (object) $myArray;
$myJSON = json_encode($myObj);


echo "<form action=\"jc.php\" method=\"post\"> \n\n";


for ($ii = 0; $ii< count($inputs); $ii++){

	$title =  $inputs[$ii][0];
	$name =  $inputs[$ii][1];
	$type =  $inputs[$ii][2];
	$dataType = $inputs[$ii][3];	
	$xmin  =  strval(floatval($inputs[$ii][4]));
	$xmax  =  strval(floatval($inputs[$ii][6]));
	$xstep = strval(floatval( $inputs[$ii][7]));

	$value = $myArray[$name];
	$xvalue = strval(floatval($value));
	$key = $name;
	$sliderID = $ii;
	$sliderDescription = $title;
	if ($dataType == "float") {
		createSlider2($sliderID, $sliderDescription, $key, $xmin, $xmax, $xstep, $xvalue);
	}
	else {


	echo $title . ":<input type=\"" . $type . "\" name=\"" . $name  . "\" value=\"" . $value . "\">";
	if ($errorArray[$name] == ""){
	echo "";
	} else {
		echo " <span class=\"error\">*  " . $errorArray[$name] . "\"</span>\n";
	}
	echo "<br><br>\n";
	}
};


echo "<input type=\"submit\" name=\"submit\" value=\"Submit\">  ";
echo "</form>";
echo "</body>";
echo "</html>";
echo "";

//echo file_get_contents($fname) . "<br>\n";


$myJSON = json_encode($myObj);
file_put_contents($fname, $myJSON);



?>
