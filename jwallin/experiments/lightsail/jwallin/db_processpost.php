<?php

function searchfor($array, $column, $target) {
  $thelength = count($array);
  $findex = -1;

  for ($ii =0; $ii < $thelength; $ii++) {
    if ($array[$ii][$column] == $target) {
      $findex = $ii;
    }
  }
  return $findex;
}

function formSql($tablename, $array) {

  /**/

  // we always ignore the id number since it is
  // automatically generated, so we start with index 1
  // We set up the first value, and then add other values 
  // with the appropriate commas.
  //

  $firstvalue = 1;
  $name = $array[$firstvalue][0];
  $value = $array[$firstvalue][3];
  $isstring = $array[$firstvalue][2];

  $nstring = $name;
  if ($isstring == "string") { 
    $vstring = "\"" . $value . "\"" ;
  } else {
    $vstring = $value;
  }

  $ll = count($array);
  for ($i = $firstvalue + 1; $i < $ll; $i++) {
    $name = $array[$i][0];
    $value = $array[$i][3];
    $isstring = $array[$i][2];

    if ($isstring == "string") { 
      $vstring = $vstring . ", " . "\"" . $value . "\"";
    } else {
      $vstring = $vstring . ", " . $value;
    }
    $nstring = $nstring . ", " . $name;

  }

  $sql = "INSERT INTO ". $tablename .  " (" . $nstring . " ) VALUES (". $vstring . " );";
  return $sql;
}

function processPostedData($inputTag, $data) {

  // see if the inputtag was posted for this database
  if (empty($_POST[$inputTag])) {
    $Err = "No value for " . $inputTag;
    echo $Err . " EXISTING \n";
    exit();
  } else {
    $value = test_input($_POST[$inputTag]);
  }

  // see if the tag has a value that exists in the 
  // current set of tables
  if (!array_key_exists($value, $data)) {
    $Err = "No such database ". $value;
    echo $Err . " EXITING \n";
    exit();
  } else {
    $tableName = $value;
  } 

  // if that table exists, process the data 
  $postedData = $data[$tableName];
  $ll = count($postedData);

  for ($i = 0; $i < $ll; $i++) {
    $ignoreFlag = $postedData[$i][1];
    if ($ignoreFlag <> "I") {
      list($a, $b) = validateEntry($postedData[$i][0], $postedData[$i][1]);
      $postedData[$i][3] = $a;
      $postedData[$i][4] = $b;
    }
  }

  return array($postedData, $tableName);

}


?>
