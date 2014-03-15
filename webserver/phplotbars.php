<?php
require_once 'dbhelp.php';
openDB();

$DEBUG=true;

require_once 'phplot.php';

$DEBUG=false;

function getPlotBarData($channelID){
    global $DEBUG;
    global $names;

    // #query average temp for days
    $sqlQuery = 
//    	"SELECT day1,temp1, humi1 FROM (".
//    	"SELECT day1, min1, avg1-min1 as avg2, max1-avg1 as max2 FROM (".
    	"SELECT day1, min1, avg1, max1 FROM (".
		"	SELECT * FROM ( ".
		"		SELECT channel, ".
		"		DAY(date_time) as day1,  ".
		"		MAX(temp)*.1 as max1, ".
		"		AVG(temp)*.1 as avg1, ".
		"		MIN(temp)*.1 as min1, ".
		"		AVG(humidity) as humi1, ".
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

    if($sqlResult){
        while($row = mysql_fetch_assoc($sqlResult)) {
            $dataResult[] = $row; 
        }
    }
    else{
        $dataResult[]= array(array(1,2),array(3,4)) ;
    }
 
    if($DEBUG)
        displayArray($dataResult);
    return $dataResult;
}

function plotBarData($dataArray, $title){

    $data = array(
      array('Jan', 40, 2, 4), array('Feb', 30, 3, 4), array('Mar', 20, 4, 4),
      array('Apr', 10, 5, 4), array('May',  3, 6, 4), array('Jun',  7, 7, 4),
      array('Jul', 10, 8, 4), array('Aug', 15, 9, 4), array('Sep', 20, 5, 4),
      array('Oct', 18, 4, 4), array('Nov', 16, 7, 4), array('Dec', 14, 3, 4)
    );

    $data=$dataArray;
    
    $plot = new PHPlot(400, 300);
    $plot->SetIsInline(true);
    $plot->SetOutputFile($title.'.png');
    
    $plot->SetImageBorderType('plain');

    $plot->SetPlotType('bars');
    //$plot->SetPlotType('stackedbars');
    //$plot->SetPlotType('lines');
    $plot->SetDataType('text-data');
    //$plot->SetDataType('data-data');

    $plot->SetPlotAreaWorld(NULL, -10, NULL, 35);
    $plot->SetDataValues($data);
    $plot->SetDataColors(array('red','blue','green','yellow'));
    
    # Main plot title:
    $plot->SetTitle($title);

    # No 3-D shading of the bars:
    $plot->SetShading(0);

    # Make a legend for the 3 data sets plotted:
    $plot->SetLegend(array('min','avg','max'));
    //$plot->SetLegendPosition(0, 0, 'image', 0, 0, 35, 5);

    # Turn off X tick labels and ticks because they don't apply here:
    $plot->SetXTickLabelPos('none');
    $plot->SetXTickPos('none');

    $plot->DrawGraph();
}

function testBars(){
    # PHPlot Example: Bar chart, 3 data sets, unshaded

    $data = array(
      array('Jan', 40, 2, 4), array('Feb', 30, 3, 4), array('Mar', 20, 4, 4),
      array('Apr', 10, 5, 4), array('May',  3, 6, 4), array('Jun',  7, 7, 4),
      array('Jul', 10, 8, 4), array('Aug', 15, 9, 4), array('Sep', 20, 5, 4),
      array('Oct', 18, 4, 4), array('Nov', 16, 7, 4), array('Dec', 14, 3, 4),
    );

    $plot = new PHPlot(800, 600);
    
    $plot->SetIsInline(true);
    
    $plot->SetImageBorderType('plain');

    $plot->SetPlotType('bars');
    $plot->SetDataType('text-data');
    $plot->SetDataValues($data);

    # Main plot title:
    $plot->SetTitle('Unshaded Bar Chart with 3 Data Sets');

    # No 3-D shading of the bars:
    $plot->SetShading(0);

    # Make a legend for the 3 data sets plotted:
    $plot->SetLegend(array('Engineering', 'Manufacturing', 'Administration'));

    # Turn off X tick labels and ticks because they don't apply here:
    $plot->SetXTickLabelPos('none');
    $plot->SetXTickPos('none');

    $plot->DrawGraph();
}

function plotBarsImage($channel){
    global $names;
    $title=$names[$channel];
    $data=getPlotBarData($channel);
    plotBarData($data, $title);//output to file

    echo "<img src='".$title.".png'>";

}

//testBars();

?>

