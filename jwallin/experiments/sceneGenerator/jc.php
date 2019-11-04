


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
  
$stuff = array();
$stuff["Jupiter"] = array(
  array("Projected Earth Size (meters)", "projectedEarthSize", "text", "float", 0.0, 0.07, 2.0),
  array("Projected Earth Rotation Time (seconds)", "projectedEarthRotation", "text", "float", 0.0, 5.0, 30.0),
  array("Projected Orbital Distance Scaling", "projectedDistanceScale", "text", "float", 0.0, 0.02, 0.3),
  array("Moon Scale Factor", "moonScale", "text",  "float", 0.0, 4.0, 10.0),
  array("shape", "name", "", "", "")
);

$planet = "Jupiter";

for ($ii = 0; $ii< count($stuff[$planet]); $ii++){

  $title =  $stuff[$planet][$ii][0];
  $type =  $stuff[$planet][$ii][2];
  $name = $stuff[$planet][$ii][1];

  echo $title . ":<input type=\"" . $type . "\" name=\"" . $name; // . "\" value=\"" . $value"\">";
  echo " <span class=\"error\">* invalid " . $name . "\"</span>\n";
  echo "<br><br>\n";
};


echo "<input type=\"submit\" name=\"submit\" value=\"Submit\">  ";
echo "</form>";
echo "</body>";
echo "</html>";
echo "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {

  for ($ii = 0; $ji < count($stuff); $ii++) {
     echo "xxxx";
  }

}


$myObj->projectedEarthSize = 0.07;
$myObj->projectedEarthRotation = 5.0;
$myObj->projectedDistanceScale = 0.02;
$myObj->moonScale = 4;
$myObj->shape = "Sphere";

$myJSON = json_encode($myObj);


//$myfile = fopen("./jupiterParameters.json","w");
//fwrite($myfile, $myJSON);
//fclose($myfile);

//echo $myJSON;





?>
