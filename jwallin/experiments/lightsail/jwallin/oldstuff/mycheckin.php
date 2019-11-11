<!DOCTYPE HTML>  
<html>
<head>
<style>
.error {color: #FF0000;}
</style>
</head>
<body>  

<?php
// define variables and set to empty values
$nameErr = $emailErr = $genderErr = $websiteErr = "";
$name = $email = $gender = $comment = $website = "";
$macid = $ip = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {

  $macidErr = "";  
  if (empty($_POST["macid"])) {
    $macidErr = "MACID is required";
  } else {
    $macid = test_input($_POST["macid"]);
    // check if name only contains letters and whitespace
    if (!preg_match("/^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/" ,$macid)) {
      echo "Ok <br><br>\n";
      $macidErr = "error 1"; 
      
    }

    if (!preg_match("/^[0-9A-Fa-f]*$/" ,$macid)) {
      $macidErr = "error 2"; 
    }
  }
  

  $ipErr  = "";
  if (empty($_POST["ip"])) {
    $ipErr = "IP address is required";
  } else {
    $ip = test_input($_POST["ip"]);
    //if (!preg_match("/^[0-9]*\.[0-9]*\.[0-9]*\.[0-9]$/", $ip)) {
    if (!preg_match("/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/", $ip)) {
      $ipErr = "bad ip address";
    } 
  }
   

  $devicetimeErr  = "";
  if (empty($_POST["devicetime"])) {
    $devicetimeErr = "devicetime is required";
  } else {
    $ip = test_input($_POST["devicetime"]);
    if (!preg_match("/^[0-9]*$/", $ip)) {
      $devicetimeErr = "bad devicetime";
    } 
  }

  $image1Err = "";
  if (empty($_POST["image1"])) {
    $image1Err = "image1 name is required";
  } else {
    $image1 = test_input($_POST["image1"]);
    if (!preg_match("/^([-\.\w]+)$/", $image1)) {
      $image1Err = "bad image1 name";
    } 
  }


  $image1xErr = "";
  if (empty($_POST["image1x"])) {
    $image1xErr = "image1x is required";
  } else {
    $image1x = test_input($_POST["image1x"]);
    if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $image1x)) {
      $image1xErr = "bad image1x";
    } 
  }

  $image1yErr = "";
  if (empty($_POST["image1y"])) {
    $image1yErr = "image1y is required";
  } else {
    $image1y = test_input($_POST["image1y"]);
    if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $image1y)) {
      $image1yErr = "bad image1y";
    } 
  }
  
  $image1zErr = "";
  if (empty($_POST["image1z"])) {
    $image1zErr = "image1z is required";
  } else {
    $image1z = test_input($_POST["image1z"]);
    if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $image1z)) {
      $imageizErr = "bad image1z";
    } 
  }






  $image2Err = "";
  if (empty($_POST["image2"])) {
    $image2Err = "image2 name is required";
  } else {
    $image2 = test_input($_POST["image2"]);
    if (!preg_match("/^([-\.\w]+)$/", $image2)) {
      $image2Err = "bad image2 name";
    } 
  }

  $image2xErr = "";
  if (empty($_POST["image2x"])) {
    $image2xErr = "image2x is required";
  } else {
    $image2x = test_input($_POST["image2x"]);
    if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $image2x)) {
      $image2xErr = "bad image2x";
    } 
  }

  $image2yErr = "";
  if (empty($_POST["image2y"])) {
    $image2yErr = "image2y is required";
  } else {
    $image2y = test_input($_POST["image2y"]);
    if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $image2y)) {
      $image2yErr = "bad image2y";
    } 
  }
  
  $image2zErr = "";
  if (empty($_POST["image2z"])) {
    $imageizErr = "image2z is required";
  } else {
    $image2z = test_input($_POST["image2z"]);
    if (!preg_match("/^-?(?:\d+|\d*\.\d+)$/", $image2z)) {
      $imageizErr = "bad image2z";
    } 
  }

/*
  if (empty($_POST["email"])) {
    $emailErr = "Email is required";
  } else {
    $email = test_input($_POST["email"]);
    // check if e-mail address is well-formed
    if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
      $emailErr = "Invalid email format"; 
    }
  }

  if (empty($_POST["website"])) {
    $website = "";
  } else {
    $website = test_input($_POST["website"]);
    // check if URL address syntax is valid
    if (!preg_match("/\b(?:(?:https?|ftp):\/\/|www\.)[-a-z0-9+&@#\/%?=~_|!:,.;]*[-a-z0-9+&@#\/%=~_|]/i",$website)) {
      $websiteErr = "Invalid URL"; 
    }    
  }
  
  if (empty($_POST["comment"])) {
    $comment = "";
  } else {
    $comment = test_input($_POST["comment"]);
  }

  if (empty($_POST["gender"])) {
    $genderErr = "Gender is required";
  } else {
    $gender = test_input($_POST["gender"]);
  }
 */


  echo phpinfo();
}


function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}
?>

<h2>PHP Form Validation Example</h2>
<p><span class="error">* required field</span></p>
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">  

  Mac ID: <input type="text" name="macid">
  <span class="error">* <?php echo $macidErr;?></span>
  <br><br>

   IP: <input type="text" name="ip">
  <span class="error">* <?php echo $ipErr;?></span>
  <br><br>


   devicetime: <input type="text" name="devicetime">
  <span class="error">* <?php echo $devicetimeErr;?></span>
  <br><br>




  image1: <input type="text" name="image1">
  <span class="error">* <?php echo $image1Err;?></span>
  <br><br>
   image1x: <input type="text" name="image1x">
  <span class="error">* <?php echo $image1xErr;?></span>
  <br><br>

   image1y: <input type="text" name="image1y">
  <span class="error">* <?php echo $image1yErr;?></span>
  <br><br>

   image1z: <input type="text" name="image1z">
  <span class="error">* <?php echo $image1zErr;?></span>
  <br><br>



  image2: <input type="text" name="image2">
  <span class="error">* <?php echo $image2Err;?></span>
  <br><br>

   image2x: <input type="text" name="image2x">
  <span class="error">* <?php echo $image2xErr;?></span>
  <br><br>

   image2y: <input type="text" name="image2y">
  <span class="error">* <?php echo $image2yErr;?></span>
  <br><br>

   image2z: <input type="text" name="image2z">
  <span class="error">* <?php echo $image2zErr;?></span>
  <br><br>



  <br><br>
  <input type="submit" name="submit" value="Submit">  
</form>

<?php
echo "<h2>Your Input:</h2>";
echo $macid;
echo "<br>";

echo $ip;
echo "<br>";

echo $email;
echo "<br>";
echo $website;
echo "<br>";
echo $comment;
echo "<br>";
echo $gender;
?>

</body>
</html>
