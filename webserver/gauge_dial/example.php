<?php
/*  
	PHP5 Class - PHPDialGauge	(August 15th 2007)
	Author: Stephen Powis (spowis@fightjudge.com)
	Purpose: A class to display a simple dial gauge (Not much to explain here right?)
	
	Notes:  Edit the Config Section in phpdial_gauge.php to Suit your Environment
*/
require("phpdial_gauge.php");

if (!isset($_POST["value"]))
{
	// Display Form asking for input to create Gauge
	?>
	<form method="POST" action="example.php">
	<table>
		<tr>
			<td>
				Min value in Gauge:
			</td>
			<td>
				<input type="text" name="low" value="0">
			</td>
		</tr>
		<tr>
			<td>
				Max value in Gauge:
			</td>
			<td>
				<input type="text" name="high" value="100">
			</td>
		</tr>
		<tr>
			<td>
				Value on Gauge:
			</td>
			<td>
				<input type="text" name="value">
			</td>
		</tr>
		<tr>
			<td>
				Display Value:
			</td>
			<td>
				<select name='display_value'>
					<option value='true'>Yes</option>
					<option value='false'>No</option>
				</select>
			</td>
		</tr>
		<tr>
			<td>
				Output Image:
			</td>
			<td>
				<select name='image_type'>
					<option value='png'>PNG</option>
					<option value='gif'>GIF</option>
				</select>
			</td>
		</tr>
	</table>
	<input type="submit" value="Create Gauge">
	</form>
	<?
}
else 
{
	// Create Dial Gauge from POSTed Data
	if ($_POST["display_value"] == "true")
		$disp = true;
	else 
		$disp = false;
		
	$my_gauge = new dial_gauge($_POST["value"],$disp,$_POST["low"],$_POST["high"]);
	
	
	// Determine which type of output
	if ($_POST["image_type"] == 'png')
	{
		header("Content-Type: image/png");	// png output
		$my_gauge->display_png();
	}
	else 
	{
		header("Content-Type: image/gif");	// gif output
		$my_gauge->display_gif();
	}
}





?>