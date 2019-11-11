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


function db_checkin($servername, $username, $password)  {

  $mydb = "duckduck";

  // Create connection
  $conn = new mysqli($servername, $username, $password);

  // Check connection
  if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
  } 

  $conn->select_db($mydb);
  if (mysqli_connect_errno()) {
    printf("Connect failed: %s\n", mysqli_connect_error());
    exit();
  }

  $tnow =  time() ;
  $IP          = "192.168.1.73";
  $MAC         = "88E9FE60D2F4";
  $DEVICETIME = time();
  $image1 = "i1";
  $image1x = 2.3;
  $image1y = 0.2;
  $image1z = -0.3;
  $image2 = "i2";
  $image2x = -1.2;
  $image2y = -0.3;
  $image2z = 0.5;
  $reg_date = $tnow + 32346;
  $timestamp = date('Y-m-d G:i:s');

/*$v = mt_rand() / mt_getrandmax();
if ($v > 0.5) {
  $MAC = strval($v);
}*/

  $val = implode(", ", ["\"" . $MAC . "\"", "\"" . $IP . "\"", $DEVICETIME, "\"". $image1 . "\"", strval($image1x), strval($image1y), strval($image1z), "\"" .$image2 . "\"", strval($image2x), strval($image2y), strval($image2z), "\"". $timestamp. "\""]);

  echo "\n " . $val . "\n\n";
  $query = "INSERT INTO log (macid, ipaddress, devicetime, image1, image1x, image1y, image1z, image2, image2x, image2y, image2z, reg_date ) VALUES ( " . $val . ")";

  echo "\n";
  echo $query;
  echo "\n";


  if ($conn->query($query) === TRUE) {
    echo "Insert successful";
  } else {
    echo "Error on insert: " . $conn->error;
  }
  echo "\n";

  $conn->close();

}

function db_recent($servername, $username, $password)  {

  $mydb = "duckduck";

  $conn = new mysqli($servername, $username, $password, $mydb);
  // Check connection
  if ($conn->connect_error) {
    die("Connection failed: recent" . $conn->connect_error);
  } else 
  {
    echo "connected to " . $mydb . " in recent\n\n";
  }
  echo "\n\n\n";
  $query = "SELECT * FROM log";
  $query = "SELECT * FROM log where devicetime >  " . strval($tnow - 1000) ;
  $query = "SELECT * FROM log where (devicetime >  " . strval($tnow - 1000) . " AND macid <> \"". $MAC . "\")" ;
  echo "\n\n " . $query , "\n";
  $results = $conn->query($query);
  if ($results) {
    echo "select successful    ";
    echo $results->num_rows;

  } else {
    echo "Error on select: " . $conn->error;
  }
  echo "\n\n";

  if ($results->num_rows > 0) {
    // output data of each row
    while($row = $results->fetch_assoc()) {
      echo "time stamps = " . $row['reg_date'] . "\n";
      echo "devicetime = " . $row['devicetime'] . "\n";
      echo "MAC = " . $row['macid'] . "\n";
      echo "\n";
    }
  }
}


// ************************

// define variables and set to empty values
if ($_SERVER["REQUEST_METHOD"] == "POST") {

  $myarray = array(
    array("macid", "macid", "", ""),
    array("ip", "ip", "", ""),
    array("devicetime", "int", "",""),
    array("image1","name", "",""),
    array("image1x","float", "",""),
    array("image1y","float", "",""),
    array("image1z","float", "",""),
    array("image2","name", "",""),
    array("image2x","float", "",""),
    array("image2y","float", "",""),
    array("image2z","float", "","")
  );

  $ll = count($myarray);

  for ($i = 0; $i < $ll; $i++) {
    list($a, $b) = validateEntry($myarray[$i][0], $myarray[$i][1]);
    $myarray[$i][2] = $a;
    $myarray[$i][3] = $b;
    echo $myarray[$i] . ": " . $a . "   ->" . $b . "\n";
  }
  /**/


  $servername = "localhost";
  $username = "myuser";
  $password = "mysqlUsernamePassword";
  db_checkin($servername, $username, $password);

  db_recent($servername, $username, $password);
  //  echo phpinfo();
}

?>
