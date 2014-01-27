<?php
//header('Content-type: text/plain');
echo "<html>";
echo "<head>";
echo "</head>";
echo "<body>" .	"<h1>AVR Temperatur und Luftfeuchte per Tag</h1>";

require 'dbhelp.php';

$DEBUG=false;

function showBarDays($channelID){
    global $DEBUG;
    global $names;

    // #query average temp for hours
    $sqlQuery = 
    	"SELECT * FROM (".
		"	SELECT * FROM ( ".
		"		SELECT channel, ".
		"		DAY(date_time) as day1,  ".
		"		AVG(temp)*.1 as temp1, ".
		"		date_time, ".
		"		DATE_FORMAT(date_time, '%j') as dayofyear ".
		"		FROM `avrdb`.`avrtemp`  ".
		"		WHERE channel=".$channelID." ".
		"		GROUP BY ".	
		"		day1, dayofyear ".
		"		WITH ROLLUP ".
		"		) as myroundup ".
		"	WHERE day1 is not null ".
		"	AND DATE_FORMAT(now(), '%j') - dayofyear < 14  ".
		"	ORDER BY date_time ASC) as mydays ".
		"order by date_time ASC".
		";";
    $sqlResult = mysql_query($sqlQuery);
	$dataResult = mysql_fetch_assoc($sqlResult);
    while ($row = mysql_fetch_assoc($sqlResult)) {
        $dataResult[] = $row;
    }
    // 'month1', 'day1', 'hour1', 'temp1'
    // '9', '4', '18', '16.95000'

	if($DEBUG)
		displayArray($dataResult);

	$countData = count($dataResult);	//count number of data
	//Set chd parameter to no value
	$chd = '';

//	$chd.="['month1', 'day1', 'hour1', 'temp1'],";
	$chd.="['day1', 'temp1'],";
	//Start to compile the data
	for ($row = 0; $row < $countData; $row++) {
		//Check for a value if one exists, add to $chd
		if(
//			isset($dataResult[$row]['month1']) &&
//		   	isset($dataResult[$row]['day1'])   
//		   	&&
		   	isset($dataResult[$row]['day1']) )
		{
			$chd .= 
				"['".
//				$dataResult[$row]['month1']."',".
//				$dataResult[$row]['day1'].",".
				$dataResult[$row]['day1']."'".",".
				$dataResult[$row]['temp1'].
				"],\n";
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
*/
	echo "          title: 'Temperatur von ".$names[$channelID]."', ";
	echo "          vAxis: {maxValue: 30, minValue: -10}";
	echo "        };";
	echo "";
//	echo "        var chart = new google.visualization.LineChart(document.getElementById('chart_div".$channelID."'));";
	echo "        var chart = new google.visualization.ColumnChart(document.getElementById('chart_div".$channelID."'));";
	echo "        chart.draw(data, options);";
//	echo "        chart.draw(data, {title:'Temperatur/Stunde', vAxis: {maxValue=}});";
	echo "      }";

	echo "    </script>";

	return;
}

//html code
openDB();
showBarDays(1);
showBarDays(2);
showBarDays(3);
echo "<div id='chart_div1' style='width: 900px; height: 500px;'></div>";
echo "<div id='chart_div2' style='width: 900px; height: 500px;'></div>";
echo "<div id='chart_div3' style='width: 900px; height: 500px;'></div>";

echo "<p>v0.8" . "</body>" . "</html>";



?>
