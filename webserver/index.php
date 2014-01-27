<?php
//header('Content-type: text/plain');
echo "<html>";
echo "<head>";
echo "</head>";
echo "<body>" .	"<h1>AVR Temperatur und Feuchte Server</h1>";

$server="localhost";
$user="root";
$names=array(
	1 => "Aussen",
	2 => "Schlaf",
	3 => "Andreas");
$DEBUG=false;

function openDB(){
	global $DEBUG;
	$link = mysql_connect("localhost", "root");
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

	$query="CREATE TABLE  IF NOT EXISTS `avrtemp` (" .
	"`id` int(11) NOT NULL AUTO_INCREMENT, " .
	"`channel` int(11), " .
	"`temp` int(11), " .
	"`humidity` int(11), " .
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
	if($DEBUG)
		echo "<p>" . $c;
	$query="INSERT INTO avrtemp (`channel`,`temp`,`humidity`,`date_time`) VALUES ( $c,$t,$h,NOW() )";
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
	   $chartscale.  //"?chxr=0,0,100".
	   "&chxt=y".
	   "&chs=200x120".
	   "&cht=gm".
	   $chartcolors. // "&chco=7A7A7A,007E00|0000FF|FF0000".
	   $chartdsize.  // "&chds=0,100".
	   "&chd=t:".$val.
	   "&chl=".$val.
	   $charttitle.
	   "'>";         // "&chtt=Feuchte'>";
	return $chartimg;
}

function listData(){
	global $DEBUG;
	$result = mysql_query("SELECT channel, temp, humidity, date_time FROM `avrtemp` order by date_time");
	if (!$result) {
	   echo "query failed: " . mysql_error();
	   return;
	}
    echo "<table border='1' cellpadding='1'>";
    echo "<tr><th>Kanal</th><th>Temp</th><th>Feuchte</th><th>Zeit</th></tr>";
    $lasttemp;
	while ($row = mysql_fetch_array($result)) {
		global $names;
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
	//    "?chxr=0,-20,60".
	//    "&chxt=y".
	//    "&chs=200x120".
	//    "&cht=gm".
	//    "&chco=7A7A7A,0000FF|FFFF00|FF0000".
	//    "&chds=-20,60".
	//    "&chd=t:".$lasttemp.
	//    "&chl=".$lasttemp.
	//    "&chtt=Temp'>";
	// echo "<img src='http://chart.apis.google.com/chart".
	//    "?chxr=0,0,100".
	//    "&chxt=y".
	//    "&chs=200x120".
	//    "&cht=gm".
	//    "&chco=7A7A7A,007E00|0000FF|FF0000".
	//    "&chds=0,100".
	//    "&chd=t:".$lasthum.
	//    "&chl=".$lasthum.
	//    "&chtt=Feuchte'>";
}

// GET requests are sent as a query string on the URL:
//    GET index.html?name1=value&name2=value HTTP/1.1
//    Host: about.com 
// http://192.168.0.40:8081/index.php?channel=1&temp=165&humidity=80&datetime=010120131234
	if($DEBUG){
       print("<pre>");
	   print_r($_GET);
	   print("</pre>");
	}

	openDB();

	if( isset($_GET['channel']) && isset($_GET['temp']) && isset($_GET['humidity']) )
	{
		if($DEBUG)
			echo "<p>addData()";
		$c=$_GET['channel'];
		$t=$_GET['temp'];
		$h=$_GET['humidity'];
		addData($c, $t, $h);
		if($DEBUG)
       		echo "<p>all done";
       	listData();
	}
	else
	{
		if($DEBUG)
			echo "listData()";
		listData();
		//echo "<p>missing arg";
	}
echo "<p>v0.5" . "</body>" . "</html>";
?>