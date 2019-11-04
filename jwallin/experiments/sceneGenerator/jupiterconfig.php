<!DOCTYPE HTML>  
<html>
<head>
<style>
.error {color: #FF0000;}
</style>
</head>
<body>  



<?php
include "valiidate.php";

  
  
$stuff = array();
$stuff["Jupiter"] = array(
  array("projectedEarthSize","float", 0.0, 0.07, 2.0),
  array("projectedEarthRotation", "float", 0.0, 5.0, 30.0),
  array("projectedDistanceScale", "float", 0.0, 0.02, 0.3),
  array("moonScale", "float", 0.0, 4.0, 10.0),
  array/("shape", "name", "", "", "")
);





if ($_SERVER["REQUEST_METHOD"] == "POST") {

  for ($ii = 0; $ji < count($stuff), $ii++) {

      
  }

}


$myObj->projectedEarthSize = 0.07;
$myObj->projectedEarthRotation = 5.0;
$myObj->projectedDistanceScale = 0.02;
$myObj->moonScale = 4;
$myObj->shape = "Sphere";

$myJSON = json_encode($myObj);


$myfile = fopen("./jupiterParameters.json","w");
fwrite($myfile, $myJSON);
fclose($myfile);

echo $myJSON;

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}
?>

<h2>PHP Form Validation Example</h2>
<?php
for 
?>

<h2>PHP Form Validation Example</h2>
<p><span class="error">* required field</span></p>
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">  
  Name: <input type="text" name="name" value="<?php echo $name;?>">
  <span class="error">* <?php echo $nameErr;?></span>
  <br><br>
  E-mail: <input type="text" name="email" value="<?php echo $email;?>">
  <span class="error">* <?php echo $emailErr;?></span>
  <br><br>
  Website: <input type="text" name="website" value="<?php echo $website;?>">
  <span class="error"><?php echo $websiteErr;?></span>
  <br><br>
  Comment: <textarea name="comment" rows="5" cols="40"><?php echo $comment;?></textarea>
  <br><br>
  Gender:
  <input type="radio" name="gender" <?php if (isset($gender) && $gender=="female") echo "checked";?> value="female">Female
  <input type="radio" name="gender" <?php if (isset($gender) && $gender=="male") echo "checked";?> value="male">Male
  <input type="radio" name="gender" <?php if (isset($gender) && $gender=="other") echo "checked";?> value="other">Other  
  <span class="error">* <?php echo $genderErr;?></span>
  <br><br>
  <input type="submit" name="submit" value="Submit">  
</form>

</body>
</html>
