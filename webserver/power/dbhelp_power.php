<?php
//global dbhelp.php file

$server="localhost";
$user="root";
$passwd="chopper";

$pwrnames=array(
"22F0" => "Bad",
"1E0E" => "K&uuml;che",
"1B67" => "Andreas",
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
/* GET /homewatch/power/index.php?id=1E0E&ticks=1056852848&total=37071698&ontime=37057879&usedws=9332520423&currentw=1.2	&maxw=2080.8&numresets=2&status=True
		HTTP/1.1
*/

	$query="CREATE TABLE IF NOT EXISTS `powerusage` (" .
	"`id` int(11) NOT NULL AUTO_INCREMENT, " .
	"`pwrid` CHAR(4), " .
	"`ticks` BIGINT(16), " .
	"`total` int(11), " .
	"`ontime` int(11), " .
	"`usedws` BIGINT (16), " .
	"`currentw` float, " .
	"`maxw` FLOAT, " .
	"`numresets` int(11), " .
	"`status` int(11), " .

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


/* GET /homewatch/power/index.php?
		id=1E0E
		&ticks=1056852848
		&total=37071698
		&ontime=37057879
		&usedws=9332520423
		&currentw=1.2
		&maxw=2080.8
		&numresets=2
		&status=True
		HTTP/1.1
*/

function addPowerData($pwrid,$ticks,$total,$ontime,$usedws,$currentw,$maxw,$numresets,$status){
	global $DEBUG;
	if($DEBUG){
		echo "<p>pwrid=" . $pwrid."</p>\n";
		echo "<p>status=".$status."</p>\n";
	}
	$lastStoredDateTime=new DateTime();
	// add data but do not save seconds (use 00)
	$query="INSERT INTO powerusage (`pwrid`,`ticks`,`total`,`ontime`, `usedws`,
	                                `currentw`, `maxw`, `numresets`, `status`, 
	                                `date_time` )".
              " VALUES ( '$pwrid', $ticks, $total, $ontime, $usedws, 
                         $currentw, $maxw, $numresets, $status, ".
			  "          DATE_FORMAT(NOW(),".
                         
//			  "          DATE_FORMAT('".$lastStoredDateTime."', '%Y-%c-%d %H:%i')
              " '%Y-%c-%d %H:%i')". 
			  "  )"; //end query
	if($DEBUG)
		echo "<br>" . $query . "<br>";
	$result = mysql_query($query);
	if(!$result){
		echo "<br>addPowerData failed:<br>";
		echo mysql_error();
	}
	else
		if($DEBUG)
			echo "addPowerData OK";
}

function listData(){
	global $DEBUG;
	global $pwrnames;
//	$result = mysql_query("SELECT channel, temp, humidity, date_time FROM `avrtemp` order by date_time DESC LIMIT 10");
	$result = mysql_query(
	            "select pwrid, 
                ROUND(usedws/60/60/1000) as usedkwh,
                ontime*100/total as ontimes,
                ROUND(total/60/60/24) as totaldays  
                from powerusage 
                GROUP BY pwrid 
                order by pwrid, date_time limit 40;"); //gives pwrid, usedkwh, ontimes, totaldays
	if (!$result) {
	echo "query failed: " . mysql_error();
	return;
	}
	    echo "<table border='1' cellpadding='1'>";
	    echo "<tr><th>Meter ID</th><th>used KWh</th><th>ON times %</th><th>counted days</th></tr>";
	    while ($row = mysql_fetch_array($result)) {
	        $name = $pwrnames[$row["pwrid"]]; //$names[$row["channel"]];
	        //$pwrid = $name;
	        echo "<tr>";
	        $usedkwh = $row["usedkwh"];
	        $ontimes = $row["ontimes"];
	        $totaldays = $row["totaldays"];
	        echo "<td>" . $name . "</td>" .
	        "<td align='right'>" . $usedkwh . "</td>" .
	        "<td align='right'>" . $ontimes . "</td>" .
	        "<td>" . $totaldays . "</td>";
	        echo "</tr>";
	    }
	    echo "</table>";
	    
//show latest 3 meters	    
	$result = mysql_query(
	            "select pwrid,currentw,date_time 
                from powerusage 
                order by date_time DESC LIMIT 3;"); //gives pwrid, currentw, date_time
	if (!$result) {
	    echo "query failed: " . mysql_error();
	    return;
	}
	    echo "<br><table border='1' cellpadding='1'>";
	    echo "<tr><th>Meter ID</th><th>currentw</th><th>date time</th></tr>";
	    while ($row = mysql_fetch_array($result)) {
	        $name = $pwrnames[$row["pwrid"]]; //$names[$row["channel"]];
	        //$pwrid = $name;
	        echo "<tr>";
	        $currentw = $row["currentw"];
	        $date_time = $row["date_time"];
	        echo "<td>" . $name . "</td>" .
	        "<td align='right'>" . $currentw . "</td>" .
	        "<td align='right'>" . $date_time . "</td>";
	        echo "</tr>";
	    }
	    echo "</table>";

/*
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
*/
}

function getLastStoredDateTime(){
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
	for($id=1; $id<=4; $id++){
		$query="Select temp*.1 as temp1,humidity,channel,date_time, DATE_FORMAT(date_time, '%d.%m.%Y<p>%k:%i</p>') as date_timeStr FROM `avrtemp` WHERE channel=".$id." ORDER by date_time DESC LIMIT 1;";
		$result = mysql_query($query);
		if ($result) {
			while ($row = mysql_fetch_array($result)) {
				$name = $names[$row["channel"]];
				$image = $images[$row["channel"]];
				$temperature = $row["temp1"];
				$humidity = $row["humidity"];
				$htmltext.="\n<tr>";
				$htmltext.="<td align='center'>".$image."<p><small>".$row['date_timeStr']."</small></p></td>";
				$htmltext.="<td>".showChart("temp", $temperature)."</td>";
				$htmltext.="<td>".showChart("humi", $humidity)."</td>";
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

