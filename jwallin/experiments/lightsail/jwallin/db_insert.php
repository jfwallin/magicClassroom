<?php 
function db_insert($servername, $username, $password, $mydb, $query) {

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

  if ($conn->query($query) === FALSE) {
    echo "Error on insert: " . $conn->error;
    echo "\n";
  }
  /* else {
    echo "Insert successful";
    echo "\n";
  }
   */
  
  $conn->close();

}
?>
