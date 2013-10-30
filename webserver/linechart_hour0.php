<?php
//header('Content-type: text/plain');
echo "<html>";
echo "<head>";
echo "</head>";
echo "<body>" .	"<h1>AVR Temperatur und Luftfeuchte per Stunde</h1>";

require 'dbhelp.php';

$DEBUG=false;
//$DEBUG=true;

function showLineChart($channelID){
    global $DEBUG;
    global $names;

    //Get the data from the database
	$sqlQuery=
	   "SELECT * FROM ( ".
	   "SELECT * FROM ( ".
	   "SELECT channel, ".
	   "DATE_FORMAT(date_time,'%H') as hour1, ".
	   "DAY(date_time) as day1,  ".
	   "AVG(temp)*.1 as temp1, ".
	   "date_time ".
	   "FROM `avrdb`.`avrtemp`  ".
	   "WHERE channel=".$channelID." ".
	   "GROUP BY ".
	   "day1, hour1 ".
	   "WITH ROLLUP ".
	   ") as myroundup ".
	   "WHERE day1 is not null AND hour1 is NOT NULL ".
	   "ORDER BY date_time DESC LIMIT 24 ".
	   ") as sorted_hours ".
	   "order by date_time asc; ";
    $sqlResult = mysql_query($sqlQuery);
	$dataResult = mysql_fetch_assoc($sqlResult);

    while ($row = mysql_fetch_assoc($sqlResult)) {
        $dataResult[] = $row;
    }
	if($DEBUG)
		displayArray($dataResult);

	$countData = count($dataResult);
	//Set chd parameter to no value
	$chd = '';
	//Limit 
	$limit = 10;
	 
    /*
    DataArrayTable [
		["time", Temperature1, Temperature2, Temperature3],
		["2013-08-31 14:43:59", 19.9, 20.3, 21.0],
		...
		] 
    */
//	$chd.="['Zeit','Temperatur','Luftfeuchte'],";
	$chd.="['Zeit','Temperatur'],";
	//Start to compile the data
	for ($row = 0; $row < $countData; $row++) {
		//Check for a value if one exists, add to $chd
		if(isset($dataResult[$row]['hour1']) &&
		   isset($dataResult[$row]['temp1']) )
		{
			$chd .= "['".$dataResult[$row]['hour1']."',".
				$dataResult[$row]['temp1']."],\n";
//				$dataResult[$row]['humidity']."],\n";
		}
	}

	if($DEBUG){
		echo "<p>DEBUG</p>";
		echo "<p>".$chd."</p>";
	}


// see https://developers.google.com/chart/interactive/docs/gallery/linechart
	echo "    <script type='text/javascript' src='https://www.google.com/jsapi'></script>";
	echo "    <script type='text/javascript'>";
	echo "      google.load('visualization', '1', {packages:['corechart']});";
	echo "      google.setOnLoadCallback(drawChart);";
	echo "      function drawChart() {";
	echo "        var data = google.visualization.arrayToDataTable([";
	echo $chd;
//	echo "          ['Year', 'Sales', 'Expenses'],";
//	echo "          ['2004',  1000,      400],";
//	echo "          ['2005',  1170,      460],";
//	echo "          ['2006',  660,       1120],";
//	echo "          ['2007',  1030,      540]";
	echo "        ]);";
	echo "";
	echo "        var options = {";
//	echo "          title: 'Temperatur / Stunde'";
/*
	echo "          vAxes:[";
	echo "             {title:'Temperatur',textStyle:{color: 'blue'}}, //  axis 0";
	echo "             {title:'Luftfeuchte',textStyle:{color: 'red'}} // Axis 1";
	echo "          ]";
//multiple vAxes
           series:{
               0:{targetAxisIndex:0},
               1:{targetAxisIndex:1}
           },
           vAxes:  {
           		0: {title:'Temperatur',textStyle:{color: 'blue'}, maxValue: 10}, // axis 0
               	1: {title:'Losses',textStyle:{color: 'red'}} // Axis 1
           }

*/
	echo "          title: 'Temperatur von ".$names[$channelID]."', ";
	echo "			vAxis: {maxValue: 25, minValue: -10}";
	echo "        };";
	echo "";
	echo "        var chart = new google.visualization.LineChart(document.getElementById('chart_div".$channelID."'));";
	echo "        chart.draw(data, options);";
	echo "      }";
	echo "    </script>";

	return;

}

openDB();
showLineChart(1);
showLineChart(2);
showLineChart(3);
echo "<div id='chart_div1' style='width: 900px; height: 500px;'></div>";
echo "<div id='chart_div2' style='width: 900px; height: 500px;'></div>";
echo "<div id='chart_div3' style='width: 900px; height: 500px;'></div>";

echo "<p>v0.7" . "</body>" . "</html>";
?>

