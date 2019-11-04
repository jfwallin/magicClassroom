<?php 
include "validate.php";
include "db_definition.php";
include "db_insert.php";

include "db_processpost.php";
include "db_recent.php";


// ************************

// define variables and set to empty values
if ($_SERVER["REQUEST_METHOD"] == "POST") {

  // process the posted data and put it into the array
  // defined in the python program.  This should be associated
  // with a database table generated in the python script.
  // dbtable is the name of the database table we will be inserting
  // data into.
  $inputTag = "dbtable";
  list($postedData, $tableName) = processPostedData($inputTag, $data);
  
  // add the two additional fields we need to add:
  // servertime
  // timestamp
  //
  // These fields are filled in using data from the server rather
  // than from the device.  We need to find their index in the
  // database array, and then update them.
  $rid =    searchfor($postedData, 0, "servertime") ;
  $milliseconds = round(microtime(true) * 1000);
  $postedData[$rid][3] = $milliseconds;

  $rid =    searchfor($postedData, 0, "reg_date") ;
  $timestamp = date('Y-m-d G:i:s');
  $postedData[$rid][3] = $timestamp;

  // Once we have all the data in an array that needs to be posted,
  // we need to create an sql query to put the data into the table.
  $sqlinsert = formSql($tableName, $postedData);

  //echo $sqlinsert . "\n";

  // with the sqlinsert query assembled, we can post
  // the data into the server and check the device into the logfile
  $servername = "localhost";
  $username = "myuser";
  $password = "mysqlUsernamePassword";
  $mydb = "duckduck";
  db_insert($servername, $username, $password, $mydb, $sqlinsert);


  // we now will findout who has checked in the last
  // $recentinterval millisecond and the MACID of the device is not 
  // $mymac and the target images have the same name.

  $macid = "duck";  // testing only
  $recentinterval = 1000 * 60;
  $rid = searchfor($postedData, 0, "image1");
  $i1 = $postedData[$rid][3];
  $rid = searchfor($postedData, 0, "image2");
  $i2 = $postedData[$rid][3];

  // do the query based on these parameters
  $neighbors = db_recent($servername, $username, $password, $mydb, $tableName, $recentinterval, $mymac, $i1, $i2);

  // turn this into a json string
  $json = json_encode($neighbors);
  echo $json ;


  //echo phpinfo();
}

?>
