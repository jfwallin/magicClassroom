<?php
$servername = "localhost";
$username = "myuser";
$password = "mysqlUsernamePassword";

$dbname = "myDB";
$createDB = 0;

$tablename = "Devices";
$createTable = 1;

// Create connection
$conn = new mysqli($servername, $username, $password);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 


$dbAction = array("SELECT", "CREATE", "DROP");

// Create database
$sql = $dbAction[$createDB] . " DATABASE " . $dbname;
echo "\n\n " . $sql . "\n\n";
if ($createDB > 0 ) {
if ($conn->query($sql) === TRUE) {
    echo "Database " . $dbAction .  " successfully";
} else {
    echo "Error " . $dbAction . " database: " . $conn->error;
}
}


$conn->close();

//#################
//

 // Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 



// sql to create table

$sql = $dbAction[$createTable] . " TABLE " . $tablename . " (
id INT(6) UNSIGNED AUTO_INCREMENT, 
macid VARCHAR(30) NOT NULL PRIMARY KEY,
ipaddress VARCHAR(30) NOT NULL,
school VARCHAR(50),
reg_date TIMESTAMP
)";


echo "\n\n " . $sql . "\n\n";
if ($conn->query($sql) === TRUE) {
    echo "Table " . $tablename . " " .  $dbAction[$createTable] . " successfully";
} else {
    echo "Error " . $dbAction[$createTable] .  " table: " . $conn->error;
}

$conn->close();









?>
