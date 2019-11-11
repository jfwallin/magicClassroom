<?php 
include "validate.php";


$stuff = array();
$stuff["Log"] = array(
  array("id", "I", "", "", ""),
  array("macid", "macid", "s", "", ""),
  array("ipaddress", "ip", "s", "", ""),
  array("devicetime", "int", "", "", ""),
  array("image1", "name", "s", "", ""),
  array("image1x", "float", "s", "", ""),
  array("image1y", "float", "s", "", ""),
  array("image1z", "float", "s", "", ""),
  array("image2", "name", "s", "", ""),
  array("image2x", "float", "s", "", ""),
  array("image2y", "float", "s", "", ""),
  array("image2z", "float", "s", "", ""),
  array("servertime", "I", "", "", ""),
  array("reg_date", "I", "s", "", "")
);



//Devices
$stuff["Log"] = array(
  array("id", "I", "", "", ""),
  array("macid", "macid", "s", "", ""),
  array("ipaddress", "ip", "s", "", ""),
  array("school", "name", "s", "", ""),
  array("reg_date", "I", "s", "", "")
);



//Schools
$stuff["Schools"] = array(
  array("id", "I", "", "", ""),
  array("name", "name", "s", "", ""),
  array("city", "name", "s", "", ""),
  array("state", "name", "s", "", ""),
  array("country", "name", "s", "", ""),
  array("reg_date", "I", "s", "", "")
);




print_r($stuff);
  //echo phpinfo();

?>
