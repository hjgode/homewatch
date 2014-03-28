<?
class gauge {
	var $img, $center, $legend, $min, $maxred, $maxyellow, $max, $pos;
	var $white, $black, $red, $yellow, $green; 
    
    function gauge(){
		// Prepare image
		$this->img = imagecreate(100, 100);
        $this->white  = imagecolorallocate($this->img, 255, 255, 255);
        $this->black  = imagecolorallocate($this->img, 0, 0, 0);
        $this->red    = imagecolorallocate($this->img, 150, 0, 0);
        $this->yellow = imagecolorallocate($this->img, 150, 150, 0);
        $this->green  = imagecolorallocate($this->img, 0, 150, 0);
		
		// Set parameters
		$this->center=50;
		$this->min=0;
		$this->maxred=50;
		$this->maxyellow=80;
		$this->max=100;
		$this->pos=90;
		$this->legend="";
		
		// Draw Gauge
		imagearc($this->img, $this->center, $this->center, 100, 100, 0, 360, $this->black);
		imagearc($this->img, $this->center, $this->center, 10, 10, 0, 360, $this->black);
		imagearc($this->img, $this->center, $this->center, 86, 86, 0+180, 180+180, $this->black);
		imagearc($this->img, $this->center, $this->center, 56, 56, 0+180, 180+180, $this->black);
		$this->addLine($this->min);
		$this->addLine($this->maxred);
		imagefill ( $this->img, 25, 25, $this->red); //check this point if you move the arcs
		$this->addLine($this->maxyellow);
		imagefill ( $this->img, 55, 15, $this->yellow); //check this point if you move the arcs
		$this->addLine($this->max);
		imagefill ( $this->img, 85, 30, $this->green); //check this point if you move the arcs
		imagefill ( $this->img, 50, 75, $this->black);
    }
	
	function setPos($pos){
		// Only draw up to +-5% of the gauge scale
		if ($pos>$this->max+($this->max*0.05))$pos=$this->max+($this->max*0.05);
		if ($pos<$this->min-($this->max*0.05))$pos=$this->min-($this->max*0.05);
		$this->pos=$pos;
	}
	

	function addLine($div){
		// Draw black divisions
        $degrees=($div*180/$this->max)+180;
        $y=43*sin(deg2rad($degrees));
        $x=43*cos(deg2rad($degrees));
        $y2=28*sin(deg2rad($degrees));
        $x2=28*cos(deg2rad($degrees));
        imageline ($this->img, $this->center+$x2, $this->center+$y2, $this->center+$x, $this->center+$y, $this->black);
	}

	function setLegend($legend){
		$this->legend=$legend;
	}

	function plot(){
		//Wait for plot to avoid drawing more than one legend or hand
        $long=46;
        $degrees=($this->pos*180/$this->max)+180;
        $y=$long*sin(deg2rad($degrees));
        $x=$long*cos(deg2rad($degrees));
        imageline ($this->img, $this->center, $this->center, $this->center+$x, $this->center+$y, $this->white);
        imagestring($this->img, 3, $this->center+1-strlen($this->legend)*7/2, 58, $this->legend, $this->white);
		header("Content-type: image/png");
		imagepng($this->img);
		imagedestroy($this->img);
	}
}

?>
