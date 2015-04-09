<?php
//global dbhelp.php file

$server="localhost";
$user="root";
$passwd="chopper";
$names=array(
1 => "Aussen",
2 => "Schlaf",
3 => "Andreas",
4 => "Bad");
$DEBUG=false;

function openDB(){
	global $DEBUG;
	global $user;
	global $passwd;
	$link = mysql_connect("localhost", $user, $passwd);
	if(!$link){
		echo "SqlConnect failed";
		echo mysql_error();
	}
	else
	    if($DEBUG)
	        echo "SqlConnect OK";

	$query="CREATE database IF NOT EXISTS avrdb;";
		$result = mysql_query($query, $link);
	if(!$result){
		echo "CreateDB failed";
		echo mysql_error();
	}
	else
		if($DEBUG)
			echo "CreateDB OK";

	$result = mysql_select_db("avrdb", $link);
	if(!$result){
		echo "SelectDB failed";
		echo mysql_error();
	}
	else
		if($DEBUG)
			echo "SelectDB OK";

	$query="CREATE TABLE IF NOT EXISTS `avrtemp` (" .
	"`id` int(11) NOT NULL AUTO_INCREMENT, " .
	"`channel` int(11), " .
	"`temp` int(11), " .
	"`humidity` int(11), " .
	"`state` int(11), " .
	"`date_time` TIMESTAMP, " .
	"PRIMARY KEY (`id`) );";
	$result = mysql_query($query, $link);
	if(!$result){
		echo "CreateTable failed";
		echo mysql_error();
	}
	else
		if($DEBUG)
			echo "CreateTable OK";

	return true;	
}

function addData($c,$t,$h){
	global $DEBUG;
	$lastStoredDateTime=getLastStoredDateTime();
	if($DEBUG){
		echo "<p>" . $c . "</p>\n";
		echo "<p>LastDateTime=".$lastStoredDateTime."</p>\n";
	}
	//state HACK
	$state=0;
	if($c==4){
	    if($t % 10 == 5)
	        $state=1;
	}
	if($DEBUG){
		echo "<p>temp=" . $t . "</p>\n";
		echo "<p>state=".$state."</p>\n";
	}
	
	// add data but do not save seconds (use 00)
	$query="INSERT INTO avrtemp (`channel`,`temp`,`humidity`,`date_time`, `state`)".
              " VALUES ( $c,$t,$h,".
//			  " DATE_FORMAT(NOW()),".
			  " DATE_FORMAT('".$lastStoredDateTime."', ".
              " '%Y-%c-%d %H:%i'), $state )";
	if($DEBUG)
		echo "<p>" . $query;
	$result = mysql_query($query);
	if(!$result){
		echo "addData failed";
		echo mysql_error();
	}
	else
		if($DEBUG)
			echo "addData OK";
}

function listData(){
	global $DEBUG;
	global $names;
	$result = mysql_query("SELECT channel, temp, humidity, date_time FROM `avrtemp` order by date_time DESC LIMIT 10");
	if (!$result) {
	echo "query failed: " . mysql_error();
	return;
	}
	    echo "<table border='1' cellpadding='1'>";
	    echo "<tr><th>Kanal</th><th>Temp</th><th>Feuchte</th><th>Zeit</th></tr>";
	    $lasttemp;
	while ($row = mysql_fetch_array($result)) {
	$name = $names[$row["channel"]];
	$lastname = $name;
	echo "<tr>";
	$temperatur1 = $row["temp"] / 10;
	$lasttemp=$temperatur1;
	$lasthum=$row['humidity'];
	//$temperatur10 = $row["temp"] % 10;
	//$temperatur = $temperatur1 . "," . $temperatur10;
	echo "<td>" . $name . "</td>" .
	"<td align='right'>" . $temperatur1 . "</td>" .
	"<td align='right'>" . $row["humidity"] . "</td>" .
	"<td>" . $row["date_time"] . "</td>";
	echo "</tr>";
	}
	echo "</table>";
	// http://www.freeduino.de/node/2110

	echo "<table border='1' cellpadding='2'";
	echo "<tr><td valign='top'>" . $lastname . "</td><td>" . showChart("temp", $lasttemp) . "</td>";
	echo "<td>" . showChart("humi", $lasthum) . "</td></tr>";
	echo "</table>";
	// echo "<img src='http://chart.apis.google.com/chart".
	// "?chxr=0,-20,60".
	// "&chxt=y".
	// "&chs=200x120".
	// "&cht=gm".
	// "&chco=7A7A7A,0000FF|FFFF00|FF0000".
	// "&chds=-20,60".
	// "&chd=t:".$lasttemp.
	// "&chl=".$lasttemp.
	// "&chtt=Temp'>";
	// echo "<img src='http://chart.apis.google.com/chart".
	// "?chxr=0,0,100".
	// "&chxt=y".
	// "&chs=200x120".
	// "&cht=gm".
	// "&chco=7A7A7A,007E00|0000FF|FF0000".
	// "&chds=0,100".
	// "&chd=t:".$lasthum.
	// "&chl=".$lasthum.
	// "&chtt=Feuchte'>";
}

function diff75($wert1, $wert2){
   if($wert1==$wert2){
      return $wert1;
   }
   // 20 -> 10 normal=15, bei 75% nur 20-3.75
   if($wert1>$wert2){
      $diff=(($wert2-$wert1)/2)/100*75;
      return $wert1-$diff;
   }
   // 10 -> 20 normal=15, bei 75 nur 13.75
   if($wert1<$wert2){
      $diff=(($wert2-$wert1)/2)/100*75;
      return $wert1+$diff;
   }
}

function getLastStoredDateTime(){
    OpenDB();
	global $DEBUG;
	$lDate_time=date('i'); //retrun only minutes
	$dateTimeNow=date('Y-m-d H:i:00');
	if($DEBUG)
		echo "\n<p>".$lDate_time."</p>\n";
	//fetch minutes and full datetime
	$query="Select DATE_FORMAT(date_time, '%i') as dt, date_time FROM `avrtemp` ORDER by date_time DESC LIMIT 1;";
	$result = mysql_query($query);
	if ($result) {
		while ($row = mysql_fetch_array($result)) {
			//should only be one row
			$mValDB=$row['dt'];
			if($DEBUG)
				echo "\n<p>Minutes stored last: ".$row['dt']."</p>\n";
			if( intval($mValDB) - intval($lDate_time) != 0 )
			{
				$diffVal = abs(intval($mValDB) - intval($lDate_time));
				if( $diffVal <= 2 ){
					if($DEBUG)
						echo "\n<p>Data found and Diff <=2: date(now)=".$dateTimeNow.", last stored=".$row['date_time']."</p>\n";
					return $row['date_time']; //return db last datetime
				}
				else
					if($DEBUG)
						echo "\n<p>Diff > 2: date(now)=".$dateTimeNow.", last stored=".$row['date_time']."</p>\n";

			}
			else
				if($DEBUG)
					echo "\n<p>Data found and Diff = 0: date(now)=".$dateTimeNow.", last stored=".$row['date_time']."</p>\n";

		}
	}
	else
		if($DEBUG)
			echo "\n<p>getLastStoredDateTime(): query did not return result</p>\n";

	if($DEBUG)
		echo "\n<p>Data not found or Diff to large: ".$dateTimeNow."</p>\n";
	return $dateTimeNow;	//return current datetime
}

//return a html table with all charts
function showAllCharts(){
	global $names;
	//global $DEBUG;
	$DEBUG=true;
    $htmltext="";
	$images=Array(
		1 => "<img src='house.png' height='64'/>",
		2 => "<img src='bed.png' height='32'/>",
		3 => "<img src='teddy.png' height='48'/>'",
		4 => "<img src='bad.png' height='48'/>'"
		);

	$htmltext.="\n<table border='1'>";
	$aussentemp=0;
	$innentemp=0;
	$aussenfeuchte=0;
	$innenfeuchte=0;
	$innenfeuchteberechnet=0;
	$innentempberechnet=0;
	$lueftenjanein=0;
	for($id=1; $id<=4; $id++){
		$query="Select temp*.1 as temp1,humidity,channel,date_time, DATE_FORMAT(date_time, '%d.%m.%Y<p>%k:%i</p>') as date_timeStr FROM `avrtemp` WHERE channel=".$id." ORDER by date_time DESC LIMIT 1;";
		$result = mysql_query($query);
		if ($result) {
			while ($row = mysql_fetch_array($result)) {
				$name = $names[$row["channel"]];
				$image = $images[$row["channel"]];
				$temperature = $row["temp1"];
				$humidity = $row["humidity"];
				// see http://web-docs.gsi.de/~giese/luftfeuchtigkeit.php
				if($id==1){ //aussen
				   $aussentemp=$temperature;
				   $aussenfeuchte=$humidity;
				}
				if($id==2) { //schlafzimmer
				   $innentemp=$temperature;
				   if($innentemp>$aussentemp){
				      $innenfeuchteberechnet=
				      $aussenfeuchte-3*($innentemp-$aussentemp);
				   }else{
				      $innenfeuchteberechnet=
				      $aussenfeuchte-3*($aussentemp-$innentemp);
				   }
				   if($innenfeuchteberechnet<$innenfeuchte){
				      $lueftenjanein=1;
				   }else{
				      $lueftenjanein=0;
				   }
			                   $innentempberechnet=($innentemp+$aussentemp)/2;
				}
		//$innentempberechnet=diff75($innentempberechnet,$innentemp); //75% austausch
		//$innenfeuchteberechnet=diff75($innenfeuchteberechnet, $innenfeuchte);
				$htmltext.="\n<tr>";
				$htmltext.="<td align='center'>".$image."<p><small>".$row['date_timeStr']."</small></p></td>";
				$htmltext.="<td align='center'>".showChart("temp", $temperature);
				if($id==2){
				   $htmltext.="<br>lueften->".$innentempberechnet."</td>";
				}else{
				   $htmltext.="</td>";
				}
				if($id==2){
				   $htmltext.="<td align='center'>".showChart("humi", $humidity).
				   "<br>"."lueften?->".$innenfeuchteberechnet.
				   "</td>";
				}else{
				   $htmltext.="<td>".showChart("humi", $humidity)."</td>";
				}
				$htmltext.="\n</tr>";
			}
		}
		else{
			if($DEBUG){
				echo "\n<p>showAllCharts() no data!</p>\n";
				$htmltext.="\n<tr><td>showAllCharts() no data!</td></tr>\n";
			}
		}
	} 
	$htmltext.="</table>\n";
    return $htmltext;
}

function showChart($type, $val){
	$chartcolors;
	$charttitle;
	if($type=="temp"){
	$chartcolors="&chco=7A7A7A,0000FF|FFFF00|FF0000";
	$charttitle="&chtt=Temp";
	$chartscale="?chxr=0,-20,60";
	$chartdsize="&chds=-20,60";
	}
	else{ //($type=="humi")
	$chartscale="?chxr=0,0,100";
	$chartcolors="&chco=7A7A7A,007E00|0000FF|FF0000";
	$charttitle="&chtt=Feuchte";
	$chartdsize="&chds=0,100";
	}
	$chartimg = "<img src='http://chart.apis.google.com/chart".
	$chartscale. //"?chxr=0,0,100".
	"&chxt=y".
	"&chs=200x120".
	"&cht=gm".
	$chartcolors. // "&chco=7A7A7A,007E00|0000FF|FF0000".
	$chartdsize. // "&chds=0,100".
/*
chdl=30
chdls=0000CC,18
chdlp=bv
*/
"&chdl=".$val.
"&chdls=0000CC,18".
"&chdlp=bv".
"&chd=t:".$val.
// oder nur
//	"&chd=t:".$val.
//"&chl=".$val.
	$charttitle.
	"'>"; // "&chtt=Feuchte'>";
	return $chartimg;
}

function displayArray($val)
{
    echo "<pre>";
    print_r($val);
    echo "</pre>";
    return;
}

?>

