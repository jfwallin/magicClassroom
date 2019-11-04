<?php 

function validateEntry($inputTag, $validationType) {
  $value  = "";
  $Err = "";

  if (empty($_POST[$inputTag])) {
    $Err = "No value for " . $inputTag;
  } else {

    $value = test_input($_POST[$inputTag]);
    switch ($validationType) 
    {
    case "macid":
      if ( 
        (!preg_match("/^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/", $value)) &&
        (!preg_match("/^[0-9A-Fa-f]*$/" ,$value)) )
      {
        $Err = " Invalid  MAC" ;
      }
      break;

    case "ip":
      if (!preg_match("/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/", $value)) {
        $Err = "bad ip address";
      } 
      break;

    case "int":
      if (!preg_match("/^[0-9]*$/", $value)) {
        $Err = "bad integer " ; 
      } 
      break;

    case "name":
      if (!preg_match("/^([-\.\w]+)$/", $value)) {
        $Err = "bad name" ;
      } 
      break;

    case "float":
      if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $value)) {
        $Err = "not a real number";
      } 
      break;    

    case "email":
      // check if e-mail address is well-formed
      if (!filter_var($value, FILTER_VALIDATE_EMAIL)) {
        $Err = "Invalid email format"; 
      }
      break;

    case "website":
      // check if URL address syntax is valid (this regular expression also allows dashes in the URL)
      if (!preg_match("/\b(?:(?:https?|ftp):\/\/|www\.)[-a-z0-9+&@#\/%?=~_|!:,.;]*[-a-z0-9+&@#\/%=~_|]/i",$value)) {
        $Err = "Invalid URL"; 
      }
      break;


    }
  }
  return array($value, $Err);
}

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}

?>
