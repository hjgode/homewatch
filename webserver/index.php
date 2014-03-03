<?php
require 'dbhelp.php';

//header('Content-type: text/plain');
echo "<html>";
echo "<head>";
echo "<meta name='viewport' content='width=device-width, user-scalable=false, initial-scale=1;'>";
echo "</head>";
echo "<body>" .	"<h2>Temperatur und Luftfeuchte</h2>";


// GET requests are sent as a query string on the URL:
// GET index.html?name1=value&name2=value HTTP/1.1
// Host: about.com
// http://192.168.0.40/homewatch/index.php?channel=1&temp=165&humidity=80&datetime=010120131234
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
  //listData();
  echo "<p>OK updated data for channel:" . $c . "</p>";
}
else
{
  if($DEBUG)
    echo "listData()";
  //listData();
  //echo "<p>missing arg";
}
echo "<p><a href='linechart_hour.php'>Stunden-&Uuml;bersicht</a></p>";
echo "<p><a href='barchart_days.php'>Tages-&Uuml;bersicht</a></p>";
echo "<p><a href='http://www.unwetterzentrale.de/uwz/getwarning_de.php?plz=41363&uwz=UWZ-DE&lang=de'>Unwetterwarnungen J&uuml;chen</a></p>";
echo "<p><a href='bad2.php'>Bad Daten</a></p>";

echo showAllCharts();
//#################################################################################
// see http://code.google.com/p/googlechartphplib/wiki/GettingStarted
//#################################################################################
// don't forget to update the path here
require './lib/GoogleChart.php';

$chart = new GoogleChart('lc', 500, 200);

// manually forcing the scale to [0,100]
$chart->setScale(0,100);

// add one line
$data = new GoogleChartData(array(49,74,78,71,40,39,35,20,50,61,45));
$chart->addData($data);

// customize y axis
$y_axis = new GoogleChartAxis('y');
$y_axis->setDrawTickMarks(false)->setLabels(array(0,50,100));
$chart->addAxis($y_axis);

// customize x axis
$x_axis = new GoogleChartAxis('x');
$x_axis->setTickMarks(5);
$chart->addAxis($x_axis);

echo $chart->toHtml();
//#################################################################################
//                                       END
//#################################################################################

// see http://sidu.sourceforge.net/chart/example-k.php
echo "<p>";
echo "<img src='bChart.php?data=id,color;A,1.5;B,2;C,3;D,4;".
  "E,3;F,2;G,1;H,2;I,3;J,4;K,3;L,2;M,1;N,2;O,3;P,4;Q,3;".
  "R,2;S,3;T,2;U,1;V,2;W,3;X,5".
  "&legend=1'>";
echo "</p>";
//#################################################################################

echo "<p>v0.92" . "</body>" . "</html>";
?>

