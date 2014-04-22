<?
/*
	PHP5 Class - PHPDialGauge	(August 15th 2007)
	Author: Stephen Powis (spowis@fightjudge.com)
	Purpose: A class to display a simple dial gauge,
			 I put this together quick n' dirty for a project I was working on,
			 since I didn't see anything else out there freely like it, I decided
			 to stick it up for public use.
			 
			 You are free to use this class for any purpose you desire, but if you do 
			 decide to use it, please leave the credits, and shoot me an email to let me
			 know I helped someone out :)
	
	How to Use:
	1. Pass the construct the following Values
	$value 			(Integer)		-	The Value to display on the gauge
	$display_value	(Boolean)		-	Turns on/off displaying the value in the gauge
	$range_bottom	(Integer)		-	Min Value displayed on the gauge
	$range_top		(Integer)		-	Max Value displayed on the gauge
	
	2. Send the proper Headers for displaying an image ie: <?php header("Content-Type: image/png"); ?>
	
	3. Call one of the two output methods, display_gif or display_png
	
	Notes:  This only works properly in PHP5, as PHP4's imagerotate strips out the transparency info.
*/

/* Config Section */
// Define these to the local filesystem path to the following included image files
define(BLANK_DIAL,"gauge_blank.png");	// A blank gauge Image
define(BLANK_IMG,"blank.png");			// A blank PNG Image
define(NEEDLE,"gauge_needle.png");		// Our Needle Image
define(DISPLAY_RANGE_VALUES,true);		// Turns on/off displaying the min/max values on the gauge
define(Y_VAL_POS,110);					// Defines the vertical position to display the value on the gauge

/* Don't Edit Below */

class dial_gauge{
	/* Private Vars */
	var $gaugeIMG;
	
	function dial_gauge($value,$display_value = true,$range_bottom=0, $range_top=100)
	{
		// Basic check of input
		if ($range_top == 0) {
			$range_bottom = 0;
			$range_top = 100;
		}
		if ($range_bottom > $range_top) {
			$tmp = $range_bottom;
			$range_top = $range_bottom;
			$range_bottom = $tmp;
			unset($tmp);
		}
			
		// Load Blank Gauge Image
		$this->gaugeIMG = imagecreatefrompng(BLANK_DIAL);
		imageAlphaBlending($this->gaugeIMG, true);
		imageSaveAlpha($this->gaugeIMG, true);
		
		// Determine where to position Value Number on Gauge based on the number of digits it has to keep it centered
		// This only really works for Integer values (no decimal places)
		if ($display_value)		
		{
			if ($value < 10)
				$x_pos = 80;	// 1 Digit
			elseif ($value < 100 && $value >= 10)
				$x_pos = 76;	// 2 Digits
			elseif ($value >= 100 && $value < 1000)
				$x_pos = 73;	// 3 Digits
			elseif ($value >= 1000 && $value < 10000)
				$x_pos = 70;

			// Write Value onto Gauge
			imagestring($this->gaugeIMG,3,$x_pos,Y_VAL_POS,$value,imagecolorallocate($this->gaugeIMG,0,0,0));
		}
		
		if (DISPLAY_RANGE_VALUES == true)
		{
			// Write Min/Max Values onto Gauge
			imagestring($this->gaugeIMG,1,44,117,$range_bottom,imagecolorallocate($this->gaugeIMG,0,0,0));
			imagestring($this->gaugeIMG,1,105,117,$range_top,imagecolorallocate($this->gaugeIMG,0,0,0));
		}
		
		// Prevent Gauge from Looping
		if ($value > $range_top)
			$value = $range_top;	
		
		if ($value < $range_bottom)
			$value = $range_bottom;
			
		// Determine angle to rotate the needle based on the range and value
		$angle = (($value - $range_bottom) * 260)/($range_top - $range_bottom);
		
		// Pull in the Needle Image, enabling
		$needleIMG = imagecreatefrompng(NEEDLE);
		imageAlphaBlending($needleIMG, true);
		imageSaveAlpha($needleIMG, true);

		// get original Width and Height
		$needle_x = imagesx($needleIMG);
		$needle_y = imagesy($needleIMG);

		// Rotate the Needle as needed
		$needleIMG = imagerotate($needleIMG,-$angle,-1);		// max range from 0 -> -260

		// Get new width and height
		$new_x = imagesx($needleIMG);
		$new_y = imagesy($needleIMG);

		// Create blank Image from png
		$new_img = imagecreatefrompng(BLANK_IMG);
		imageAlphaBlending($new_img, true);
		imageSaveAlpha($new_img, true);

		// Crop the image, just leaving the center of it.
		imagecopy($new_img,$needleIMG,0,0,round(($new_x-$needle_x)/2)+33,round(($new_y-$needle_y)/2)+33,$needle_x,$needle_y);

		// Position it over the proper 'Pin Point' for the needle
		imagecopy($this->gaugeIMG,$new_img,0,0,0,0,165,165);

		return(true);
	}
	
	function display_gif()
	{
		// Display Completed Gauge
		imagecolortransparent($this->gaugeIMG,imagecolorat($this->gaugeIMG,1,1));
		imagegif($this->gaugeIMG);
	}
	
	function display_png()
	{
		// Display Completed Gauge
		imagepng($this->gaugeIMG);
	}
}
?>