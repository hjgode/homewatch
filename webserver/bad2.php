<?php
//header('Content-type: text/plain');
echo "<html>";
echo "<head>";
echo "</head>";
echo "<body>" .	"<h1>AVR Temperatur und Luftfeuchte im Bad v0.2</h1>";

require_once 'dbhelp.php';
require_once 'phpplot2.php';

$DEBUG=false;

openDB();

//return options text for form list with days measured
function getDays(){
    global $DEBUG;
    $sqlQuery="select * from ".
    "(select DATE_FORMAT(date_time,'%Y%m%d') as days_only from avrtemp where channel=4) ".
    "as dtimes ".
    "group by days_only;";
    $sqlResult = mysql_query($sqlQuery);

    $options = '';

    if($sqlResult){
        //$filter=mysql_query($sqlQuery);
        while($row = mysql_fetch_array($sqlResult)) {
            $options .="<option>" . $row['days_only'] . "</option>";
        }
    }
    if($DEBUG)
      echo "<p>options: ".$options."</p>";
    return $options;
}

function dataStartEnd($mystart, $myend){
    global $DEBUG;
    //Get the data from the database
	$sqlQuery=
	   "SELECT * FROM ".
	   "(SELECT DATE_FORMAT(date_time,'%Y%m%d%H%i') as dtime, temp/10 as temp1, humidity, channel, state * 10 as state1 FROM avrtemp ".
	   " where channel=4 ".
	   " GROUP BY dtime ".
	   " ORDER BY dtime ASC) as bad ".
	   " WHERE dtime > ".$mystart." AND dtime < ".$myend." ".
	   ";";
	if($DEBUG)
	    echo "<p>".$sqlQuery."</p>";
	    
    $sqlResult = mysql_query($sqlQuery);
    if($sqlResult){
//	    $dataResult = mysql_fetch_assoc($sqlResult);

        while ($row = mysql_fetch_assoc($sqlResult)) {
            $dataResult[] = $row;
        }

        //draw line graph
        $countData = count($dataResult);
        //Set chd parameter to no value
        $chd = '';

	    $chd.="['Zeit','Temperatur','Luftfeuchte', 'state'],";
	    //Start to compile the data
	    for ($row = 0; $row < $countData; $row++) {
		    //Check for a value if one exists, add to $chd
		    if(isset($dataResult[$row]['dtime']) &&
		       isset($dataResult[$row]['temp1']) &&
		       isset($dataResult[$row]['humidity']) &&
		       isset($dataResult[$row]['state1']) 
		    )
		    {
			    $chd .= "[".
			        "'".$dataResult[$row]['dtime']."',".
				    $dataResult[$row]['temp1'].",".
				    $dataResult[$row]['humidity'].",".
				    $dataResult[$row]['state1'].
				    "],\r\n";
		    }
	    }
	    if($DEBUG)
	        echo "<p>".$chd."</p>";
	    $channelID=4; // see "chart_div".$channelID
	    // see https://developers.google.com/chart/interactive/docs/gallery/linechart
	    echo "    <script type='text/javascript' src='https://www.google.com/jsapi'></script>\r\n";
	    echo "    <script type='text/javascript'>\r\n";
	    echo "      google.load('visualization', '1', {packages:['corechart']});\r\n";
	    echo "      google.setOnLoadCallback(drawChart);\r\n";
	    echo "      var myStart;\r\n";
	    echo "      var myEnd;\r\n";
	    echo "      var bSetStart=true;\r\n";
	    echo "      function drawChart() {\r\n";
	    echo "        var data = google.visualization.arrayToDataTable([\r\n";
	    echo $chd."";
	    echo "        \r\n]);";
	    echo "\n\n";
	    echo "        var options = {\r\n";
	    echo "          title: 'Temperatur und Luftfeuchte von "."Bad"."',";
    //	echo "			vAxis: {maxValue: 25, minValue: -10}";
    //######## DO NOT USE COMMENTS inside options! ######
        echo "			series: {";
        echo "			   0:{targetAxisIndex:0, color: 'red'},";
        echo "			   1:{targetAxisIndex:1, color: 'blue'}";
        echo "			},\r\n";
        echo "			vAxes:  {";
        echo "			   0: {title:'Temperatur',textStyle:{color: 'red'}, maxValue: 25, minValue: -10},";
        echo "			   1: {title:'Luftfeuchte',textStyle:{color: 'blue'},maxValue: 100, minValue: 0}";
        echo "			}\r\n";

	    echo "        };\r\n";
	    echo "\n\n";
	    echo "        var chart = new google.visualization.LineChart(document.getElementById('chart_div".$channelID."'));\r\n";

	    echo "        function selectHandler(e) {\r\n";
//	    echo " alert('hello'); \r\n";
	    echo "          var selection = chart.getSelection();\r\n";
	    echo "          if(selection.length>0 && typeof selection[0].row != 'undefined') {\r\n";
	    echo "             //alert('That is row/column no. '+selection[0].row+'/'+selection[0].column);\r\n";
	    echo "             if(bSetStart){\r\n";
	    echo "               document.getElementById('start').value=data.getValue(selection[0].row,0);\r\n";
	    echo "               bSetStart= !bSetStart;\r\n";
	    echo "               //alert('data='+data.getValue(selection[0].row,0));\r\n";
	    echo "             }\r\n";
	    echo "             else{\r\n";
	    echo "               document.getElementById('end').value=data.getValue(selection[0].row,0);\r\n";
	    echo "               bSetStart= !bSetStart;\r\n";
	    echo "             }//if bSetStart\r\n";
	    echo "          }//if selection\r\n";
	    echo "          return;\r\n";
	    echo "        }//selectHandler\r\n";
	    //
	    echo "	      // Must wait for the ready event in order to";
        echo "        // request the chart and subscribe to 'onmouseover'.\r\n";
        echo "        google.visualization.events.addListener(chart, 'ready', onReady);\r\n";

	    echo "        function onReady() {\r\n";
                        // Every time the table fires the "select" event, it should call your
                        // selectHandler() function.
	    echo "          google.visualization.events.addListener(chart, 'select', selectHandler);\r\n";
	    echo "        }";
                      //Draw the chart
	    echo "        chart.draw(data, options);\r\n";
	    echo "      } //end of function drawChart\r\n";
        

	    echo "    </script>\r\n";
	    
	    if($DEBUG)
		    displayArray($dataResult);

	    $countData = count($dataResult);
	}
	else {
	    echo "<p>NO DATA</p>";
	}
	return $dataResult;
}

function showForm(){
    $options=getDays(); //get list of known measure days

    $currentscript=$_SERVER['PHP_SELF'];

    //print search form
    echo "<form id='form1' method='get' action='".$currentscript."'>";
    echo '<input id="start" type="text" name="start" >';//value="'."201401291027".'" >';
    echo '<input id="end" type="text" name="end" >';//value="'."201401301027".'" >';
    echo '<input type="submit" name="cmd" value="Anzeigen">';
    $days="<p><label>Tage</label>:
    <select name='days' id='days' >
      " . $options . "
    </select>";
    echo $days;
    $tag_changed="<script type='text/javascript'>
    function tag_changed(idx){
      //if(idx==-1)
      //   return;
      var tag = document.getElementById('days').value;
      document.getElementById('start').value=tag+'0000';
      document.getElementById('end').value=tag+'2359';
      //alert('you selected: '+tag);
    }
    </script>";
    echo $tag_changed;
    echo '<input type="button" name="cmd_tag" onclick="javascript:tag_changed();" value="&uuml;bernehmen"></p>';
    echo '</form>';
}

showForm();

echo "<div id='chart_div4' style='width: 900px; height: 500px;'>Line-Chart</div>";

if( isset($_GET['cmd']) ){
    $cmd=$_GET['cmd'];
    if(true){
        echo "<p>cmd=".$cmd."</p>";
    }
    if($cmd=="Anzeigen"){
        if( isset($_GET['start']) && isset($_GET['end'])  )
        {
            $startValue=$_GET['start'];
            $endValue=$_GET['end'];
            //
            $data = dataStartEnd($startValue,$endValue); //draws google chart
            $channel=4;
            $data=getData($channel, $startValue, $endValue);
            plotGraph($data); //draws phpplot2.png
            echo "<img src='./phpplot2.png'>";
			//echo "<img src='phpplot2.php?channel=4&start=".$startValue."&end=".$endValue."'>";
            if($data){
                //displayArray($data);
	            echo "    <script type='text/javascript'>\r\n";
                echo "      document.getElementById('start').value=".$_GET['start'].";\r\n";
                echo "      document.getElementById('end').value=".$_GET['end'].";\r\n";
                echo "    </script>\r\n";
            }
            else{
                echo "<p>NO selection DATA</p>";
            }
                
        }
        else{
            listDataOfChannel(4);
        }// is start end
    } // is Anzeigen
}// is cmd


echo "</body>";
?>
