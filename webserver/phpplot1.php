<?php
//Include the code
require_once 'phplot.php';
require_once 'dbhelp.php';

$DEBUG=false;

openDB();

function getData($channel, $mystart, $myend){
    global $DEBUG;
    //$mystart="201402070000";
    //$myend="201402072359";
    //Get the data from the database
    /*
    $sqlQuery=
       "SELECT dtime, temp1, humi FROM ".
       "(SELECT DATE_FORMAT(date_time,'%Y%m%d%H%i') as dtime, temp/10 as temp1, humidity as humi, channel FROM avrtemp ".
       " where channel=".$channel." ORDER BY dtime ASC) as bad ".
       " WHERE dtime > ".$mystart." AND dtime < ".$myend." ".
       ";";
    */
    $sqlQuery=
       "SELECT '' as empty, UNIX_TIMESTAMP(date_time) as time1, temp1, humi FROM ".
//       "SELECT '' as empty, DATE_FORMAT(date_time,'%H:%i') as time1, temp1, humi FROM ".
       " (SELECT date_time, ".
	   "  DATE_FORMAT(date_time,'%Y%m%d%H%i') as dtime, ".
	   "  temp/10 as temp1, ".
	   "  humidity as humi, ".
	   "  channel FROM avrtemp ".
       "  where channel=".$channel." ORDER BY dtime ASC".
	   " ) as bad ".
       " WHERE dtime > ".$mystart." AND dtime < ".$myend." ".
       " ;";
	
	if($DEBUG)
	    echo "<p>".$sqlQuery."</p>";
    $sqlResult = mysql_query($sqlQuery);

    if($sqlResult){
        while($row = mysql_fetch_assoc($sqlResult)) {
            //$dataResult[] = $row;
            $dataResult[] = $row; 
        }
    }
    else{
        $dataResult[]= array(1,2,3) ;
    }
        
    return $dataResult;
}

function plotGraph($data){
	//Define the object
	$plot = new PHPlot();

	$example_data=$data;
	$plot->SetDataValues($example_data);
	$plot->SetDataType('data-data');
	
	//Set titles
	$plot->SetTitle("temp and humi");
	$plot->SetXTitle('time');
	$plot->SetYTitle('Y Data');
	$legend=array('temp','humi');
	$plot->SetLegend($legend);
	$plot->SetXDataLabelAngle(90);

	//$plot->SetXGridLabelType("time"); 
	$plot->SetXTickLabelPos('xaxis');
	$plot->SetXTickPos('plotdown');
	$plot->SetXLabelType('time', '%H:%M');
	$plot->TuneXAutoTicks(10, 'date');
//	$plot->SetXTickIncrement(.5);
	//$plot->SetXTickIncrement(60 * 24);
	
	$plot->SetPlotType('lines');
    //$plot->SetPlotAreaWorld(strtotime('00:00'), null, strtotime('23:59'), null);	
	$plot->SetDrawXGrid(true);
	//Draw it
	$plot->DrawGraph();
}

/*
    echo "<html><head></head><body>
    <img src=".showGraph(0,0)."'>";
*/
    if( isset($_GET['start']) && isset($_GET['end']) && isset($_GET['channel']) ) {
        $startValue=$_GET['start'];
        $endValue=$_GET['end'];
        $channel=$_GET['channel'];
        $data=getData($channel, $startValue, $endValue);
        plotGraph($data);
    }
    else{
        $currentscript=$_SERVER['PHP_SELF'];
        echo "<html><body><a href='";
        echo $currentScript . "?start=201402070000&end=201402072359";
        echo "'>test</a></body></html>";
    }	
?>
