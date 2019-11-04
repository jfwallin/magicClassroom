<?php
$servername = "localhost";
$username = "myuser";
$password = "mysqlUsernamePassword";

$mydb = "duckduck";




// Create connection
$conn = new mysqli($servername, $username, $password);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$conn->select_db($mydb);
if (mysqli_connect_errno()) {
    die("Connect failed: %s\n". mysqli_connect_error());
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

$v = mt_rand() / mt_getrandmax();
echo "VVVV = " . strval($v) . "\n";

if ($v > 0.5) {
  $MAC = strval($v);
}

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


$conn->close();




echo "\n";
$conn = new mysqli($servername, $username, $password, $mydb);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: p2" . $conn->connect_error);
} else 
{
  echo "connected to " . $mydb . "\n\n";
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

/*
if ($results->num_rows > 0) {
    // output data of each row
    while($row = $results->fetch_assoc()) {
      $keys = array_keys($row);
      foreach ($keys as $k) {
        echo $k.  "=> ". $row[$k] . "\n";
      }
        //echo "id: " . $row["id"] , "\n";  //. " - Name: " . $row["firstname"]. " " . $row["lastname"]. "<br>";
    }
} else {
    echo "0 results";
}
 */


/*
while ($row = mysql_fetch_array($results, MYSQL_ASSOC))
{
  echo $row;
/*  foreach ($row as $name=>$col) {
    echo $name . "   " . $col . "\n";
}
}

 */


$conn->close();


/*$d=mktime(0, 0, 0, 1, 1, 1970);
echo "Created date is " . date("Y-m-d h:i:sa", $d);
echo "\n";
  
echo time($d);


//$tz = $d->getTimezone();
//echo $tz->getName();
echo "\n";


$tt = microtime(1);
echo "microtime " . $tt . "\n";

echo "mt = " . strval( round($tt * 1000)) . "\n";


$dd = $tt - time($d);
echo "djjjjj = " . strval($dd) , "\n";
 */

/// Create database
//$sql = "SELECT DATABASE " . $mydb;  #myDB";
//if ($conn->query($sql) === TRUE) {
//    echo "Database created successfully";
//} else {
//    echo "Error creating database: " . $conn->error;
//}


?>
