<?php 

function validateMacId($inputTag)
{
  $macid = "";
  $macidErr = "";
  if (empty($_POST[$inputTag])) {
    $macidErr = "No Mac ID posted ";
  } else {
    $macid = test_input($_POST["macid"]);
    if ( 
      (!preg_match("/^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/", $macid)) &&
      (!preg_match("/^[0-9A-Fa-f]*$/" ,$macid)) )
    {
      $macidErr = " Invalid Mac ID : " . $macid ;
    }
  }
  return array ($macid, $macidErr); 
}

function validateIP($inputTag) {

  $ip = "";
  $ipErr  = "";
  if (empty($_POST[$inputTag])) {
    $ipErr = "IP address is required";
  } else {
    $ip = test_input($_POST["ip"]);
    if (!preg_match("/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/", $ip)) {
      $ipErr = "bad ip address";
    } 
    return array ($ip, $ipErr); 
  }
}

function validateInt($inputTag) {
  $intvalue = "";
  $intErr  = "";
  if (empty($_POST[$inputTag])) {
    $intErr = $inputTag . " is required";
  } else {
    $intvalue = test_input($_POST[$inputTag]);
    if (!preg_match("/^[0-9]*$/", $ip)) {
      $intErr = "bad " . $inputTag; //devicetime";
    } 
  }
  return array($intvalue, $intErr);
}

function validateName($inputTag) {

  $name = "";
  $nameErr = "";
  if (empty($_POST[$inputTag])) {
    $nameErr = $inputTag . " is required";
  } else {
    $name = test_input($_POST[$inputTag]);
    if (!preg_match("/^([-\.\w]+)$/", $name)) {
      $nameErr = "bad " . $name;
    } 
  }

  return array ($name, $nameErr);
}

function validateFloat($inputTag) {

  $x = "";
  $xErr = "";
  if (empty($_POST[ $inputTag])){
    $xErr = $inputTag . " is required";
  } else {
    $x = test_input($_POST[$inputTag]);
    if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $x)) {
      $xErr = $inputTag . " is not a real number ";
    } 
  }
  return array($x, $xErr);
}

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}


// ************************

// define variables and set to empty values
if ($_SERVER["REQUEST_METHOD"] == "POST") {

  //mac id
  list ($macid, $macidErr) = validateMacid("macid");
  

  list ($ip, $ipErr) = validateIP("ip");
  echo "IP " . $ip . "  " .$ipError ."\n";

  list ($devicetime, $devicetimeErr) = validateInt("devicetime");
  echo "devicetime " . $devicetime . "  " .$devicetimeError ."\n";


  list ($image1, $image1Err) = validateName("image1");
  echo "image1 ". $image1 . " " . $image1Err . "\n";

  list ($image1x, $image1xErr) = validateFloat("image1x");
  echo "image1x ". $image1x . " " . $image1xErr . "\n";

  list ($image1y, $image1yErr) = validateFloat("image1y");
  echo "image1y ". $image1y . " " . $image1yErr . "\n";

  list ($image1z, $image1zErr) = validateFloat("image1z");
  echo "image1z ". $image1z . " " . $image1zErr . "\n";



  list ($image2, $image2Err) = validateName("image2");
  echo "image2 ". $image2 . " " . $image2Err . "\n";

  list ($image2x, $image2xErr) = validateFloat("image2x");
  echo "image2x ". $image2x . " " . $image2xErr . "\n";

  list ($image2y, $image2yErr) = validateFloat("image2y");
  echo "image2y ". $image2y . " " . $image2yErr . "\n";

  list ($image2z, $image2zErr) = validateFloat("image2z");
  echo "image2z ". $image2z . " " . $image2zErr . "\n";

  //  echo phpinfo();
}

?>
