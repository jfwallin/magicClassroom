<?php
function db_recent($servername, $username, $password, $mydb, $tablename, $recentinterval, $mymac, $i1, $i2) {

  $conn = new mysqli($servername, $username, $password, $mydb);
  // Check connection
  if ($conn->connect_error) {
    die("Connection failed: recent" . $conn->connect_error);
  } 

  $milliseconds = round(microtime(true) * 1000);
  $targettime = $milliseconds - $recentinterval;
  $query = "SELECT * FROM " . $tablename . " where (servertime >  " . $targettime . " AND macid <> \"". $mymac . "\" AND image1 = \"" . $i1 . "\" AND image2 = \"" . $i2 . "\" );";

  //echo "\n\n " . $query , "\n";
  $results = $conn->query($query);
  /*
  if ($results) {
    echo "select successful    ";
    echo $results->num_rows;
  } else {
    echo "Error on select: " . $conn->error;
  }
  echo "\n\n";
   */

  $groupmembers = array();
  if ($results->num_rows > 0) {
    while($row = $results->fetch_assoc()) {
      $groupmembers[] = $row;
    }
  }


  return $groupmembers;
}


?>
