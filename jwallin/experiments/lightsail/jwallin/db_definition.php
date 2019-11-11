<?php
$data = array();

$data["Log"] = array(
  array("id", "I", "long", "", ""),
  array("macid", "macid", "string", "", ""),
  array("ipaddress", "ip", "string", "", ""),
  array("devicetime", "int", "long", "", ""),
  array("image1", "name", "string", "", ""),
  array("image1x", "float", "float", "", ""),
  array("image1y", "float", "float", "", ""),
  array("image1z", "float", "float", "", ""),
  array("image2", "name", "string", "", ""),
  array("image2x", "float", "float", "", ""),
  array("image2y", "float", "float", "", ""),
  array("image2z", "float", "float", "", ""),
  array("servertime", "I", "long", "", ""),
  array("reg_date", "I", "string", "", "")
);

$data["Devices"] = array(
  array("id", "I", "long", "", ""),
  array("macid", "macid", "string", "", ""),
  array("ipaddress", "ip", "string", "", ""),
  array("school", "name", "string", "", ""),
  array("reg_date", "I", "string", "", "")
);

$data["Schools"] = array(
  array("id", "I", "long", "", ""),
  array("name", "name", "string", "", ""),
  array("city", "name", "string", "", ""),
  array("state", "name", "string", "", ""),
  array("country", "name", "string", "", ""),
  array("reg_date", "I", "string", "", "")
);


?>
