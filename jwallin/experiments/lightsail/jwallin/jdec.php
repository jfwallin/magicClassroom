<?php

$s= '[{"id":"19","macid":"88:e9:fe:60:d2:f4","ipaddress":"192.168.1.24","devicetime":"1568516714083","image1":"i1.jpg","image1x":"2.43","image1y":"2.5","image1z":"1.3","image2":"i2.jpg","image2x":"-3.1","image2y":"2.3","image2z":"-0.3","servertime":"1568516714281","reg_date":"2019-09-15 03:05:14"},{"id":"20","macid":"88:e9:fe:60:d2:f4","ipaddress":"192.168.1.24","devicetime":"1568516767838","image1":"i1.jpg","image1x":"2.43","image1y":"2.5","image1z":"1.3","image2":"i2.jpg","image2x":"-3.1","image2y":"2.3","image2z":"-0.3","servertime":"1568516768048","reg_date":"2019-09-15 03:06:08"}]
';

echo $s . "n";

$d= json_decode($s);
print_r($d);

$la = count($d);
for ($l = 0; $l< $la; $l++) {
  $dd = $d[$l];
//  print_r($dd);

foreach($dd as $key => $val) {
  echo "KEY IS: " .$key . "\n";
  echo "VALUE is " . ((array)$dd)[$key] . "\n";
  /*
  foreach(((array)$dd)[$key] as $val2) {
        echo "VALUE IS: $val2\n";
    }
   */
}
/*  foreach($dd->entries as $row) {
    foreach($row as $key => $val) {
      echo $key . ': ' . $val;
      echo '<br>';
    }
  }
*/
}


?>
