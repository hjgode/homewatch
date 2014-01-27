<?php
/*bChart - Simple PHP chart v3.2 - 2011-08-02
* http://bchart.sf.net

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
//If you want to use fmt=sql*, please add your server connection here===========
//@mysql_connect('localhost','user','password');
//@mysql_select_db('db_name');
@session_start();
@bChart_create();
function bChart_create(){
	if ($_GET['ses']) $init=$_SESSION[$_GET['ses']];
	else $init=($_GET['data'] ? $_GET : $_POST);
	if (!$init['data']) return;
	$data=$init['data'];
	unset($init['data']);
	if (!isset($init['sepRow'])) $init['sepRow']=';';
	$img=new bChart($data,$init);
	$img->cout();
}
class bChart{
function bChart($data,$init){
	$this->init=$init;
	$this->initInit($init,$data);
	$w=$this->init['w']; $h=$this->init['h'];
	$this->w=$w;//used for public access
	$this->img=imagecreatetruecolor($w,$h);
	$fff=imagecolorallocate($this->img,255,255,255);
	imagefilledrectangle($this->img,0,0,$w,$h,$fff);
	imagecolortransparent($this->img,$fff);
	$this->drawRectRoundFill(3,3,$w-3,$h-3,5,240,240,240);
	$this->drawRectRound(1,1,$w-1,$h-1,5,230,230,230);
	$this->drawGraphArea(255,255,255);
	$this->drawScale(120,120,120);
	if ($this->init['legend']) $this->drawLegend();
	$this->drawChart($this->data);
	if (isset($this->data2)) $this->drawChart($this->data2,2);
}
function cout($url=0){
	if ($url) imagepng($this->img,$url);
	else{
		header('Content-type: image/png');
		imagepng($this->img);
		imagedestroy($this->img);
	}
}
function drawGraphArea($R,$G,$B){
	$color=imagecolorallocate($this->img,$R,$G,$B);
	$x1=$this->init['x1']; $y1=$this->init['y1'];
	$x2=$this->init['x2']; $y2=$this->init['y2'];
	imagefilledrectangle($this->img,$x1,$y1,$x2,$y2,$color);
	$color2=imagecolorallocate($this->img,$R-40,$G-40,$B-40);
	imagerectangle($this->img,$x1,$y1,$x2,$y2,$color2);
	if (!$this->init['bgStripe']) return;
	$color3=imagecolorallocate($this->img,$R-15,$G-15,$B-14);
	$imgH=$y2-$y1-1;
	for($i=$x1-$imgH;$i<=$x2;$i=$i+4){
		$a1=$i; $b1=$y2; $a2=$i+$imgH; $b2=$y1;
		if ($a1<$x1){
			$a1=$x1; $b1=$y1+$a2-$x1+1;
		}
		if ($a2>=$x2){
			$b2=$y1+$a2-$x2+1; $a2=$x2-1;
		}
		imageline($this->img,$a1,$b1,$a2,$b2+1,$color3);
	}
}
function drawRectRound($x1,$y1,$x2,$y2,$r,$R,$G,$B){
	$color=imagecolorallocate($this->img,$R,$G,$B);
	imageline($this->img,$x1+$r,$y1,$x2-$r,$y1,$color);
	imageline($this->img,$x2,$y1+$r,$x2,$y2-$r,$color);
	imageline($this->img,$x1+$r,$y2,$x2-$r,$y2,$color);
	imageline($this->img,$x1,$y1+$r,$x1,$y2-$r,$color);
	imagearc($this->img,$x1+$r,$y1+$r,$r*2,$r*2,180,270,$color);
	imagearc($this->img,$x2-$r,$y1+$r,$r*2,$r*2,270,0,$color);
	imagearc($this->img,$x2-$r,$y2-$r,$r*2,$r*2,0,90,$color);
	imagearc($this->img,$x1+$r,$y2-$r,$r*2,$r*2,90,180,$color);
}
function drawRectRoundFill($x1,$y1,$x2,$y2,$r,$R,$G,$B){
	$color=imagecolorallocate($this->img,$R,$G,$B);
	imagefilledrectangle($this->img,$x1,$y1+$r,$x2,$y2-$r,$color);
	imagefilledrectangle($this->img,$x1+$r,$y1,$x2-$r,$y2,$color);
	imagefilledarc($this->img,$x1+$r,$y1+$r,$r*2,$r*2,180,270,$color,IMG_ARC_PIE);
	imagefilledarc($this->img,$x2-$r,$y1+$r,$r*2,$r*2,270,0,$color,IMG_ARC_PIE);
	imagefilledarc($this->img,$x2-$r,$y2-$r,$r*2,$r*2,0,90,$color,IMG_ARC_PIE);
	imagefilledarc($this->img,$x1+$r,$y2-$r,$r*2,$r*2,90,180,$color,IMG_ARC_PIE);
}
function drawText($x,$y,$angle=0,$txt,$color,$pos=7,$x1=null,$y2=null){
//pos:1 top left 2 top center 3 top right
//pos:4 mid left 5 mid center 6 mid right
//pos:7 btm left 8 btm center 9 btm right
	$font=$this->init['font'];
	$fontSize=$this->init['fontSize'];
	if ($x1<>null && ($pos==2 || $pos==5 || $pos==8)) $x=$x+($x1-$x)/2;
	if ($y1<>null && ($pos==4 || $pos==5 || $pos==6)) $y=$y+($y1-$y)/2;
	if ($pos<>7){
		$box=imagettfbbox($fontSize,0,$font,$txt);
		$Y=$box[5]-$box[3]; $X=$box[2]-$box[0];
		if ($angle==90 || $angle==-90) $this->swap($X,$Y);
		if ($pos<4) $y=$y-$Y;//top
		elseif ($pos<7) $y=$y-$Y/2;//center
		if ($pos==2 || $pos==5 || $pos==8) $x=$x-$X/2;//center
		elseif ($pos==3 || $pos==6 || $pos==9) $x=$x-$X;//right
	}
	imagettftext($this->img,$fontSize,$angle,$x,$y,$color,$font,$txt);
}
function drawLegend(){
	$legend=@substr($this->init['legend'],0,1);
	if (@strlen($this->init['legend'])==2){
		foreach ($this->init['serial'] as $sn) $str .=$sn;
		$box=imagettfbbox($this->init['fontSize'],0,$this->init['font'],$str);
		$X=$box[2]-$box[0];
		$x=($this->w-$X-count($this->init['serial'])*15)/2;
	}elseif ($legend<>1) $x=10;
	if ($legend=='T') $y=$this->init['y1']-20;
	elseif ($legend=='B') $y=$this->init['h']-15;
	else{
		$x=$this->init['x2']+10;
		$y=$this->init['y1'];
	}
	$colorN=count($this->init['colors']);
	foreach ($this->init['serial'] as $i=>$sn){
		$c=$this->init['colors'][$i%$colorN];
		$c=imagecolorallocate($this->img,$c[0],$c[1],$c[2]);
		if ($legend==1){
			if ($i) $y +=$this->init['fontSize']+5;
			imagefilledrectangle($this->img,$x,$y,$x+8,$y+8,$c);
			$this->drawText($x+15,$y+8,0,$sn,$c);
		}else{
			imagefilledrectangle($this->img,$x,$y,$x+8,$y+8,$c);
			$this->drawText($x+15,$y+8,0,$sn,$c);
			$box=imagettfbbox($this->init['fontSize'],0,$this->init['font'],$sn);
			$X=$box[2]-$box[0];
			$x +=($X+20);
		}
	}
}
function drawScale($R,$G,$B){
	$color=imagecolorallocate($this->img,$R,$G,$B);
	$x1=$this->init['x1']; $y1=$this->init['y1'];
	$x2=$this->init['x2']; $y2=$this->init['y2'];
	imageline($this->img,$x1,$y1,$x1,$y2,$color);
	imageline($this->img,$x1,$y2,$x2,$y2,$color);
	$color2=imagecolorallocate($this->img,$R+100,$G+100,$B+100);
	//y axis
	$yDiv=count($this->init['yVal']);
	for ($i=0;$i<$yDiv;$i++){
		$y=$i*$this->init['step']+$y1;
		imageline($this->img,$x1-3,$y,$x1,$y,$color);
		$this->drawText($x1-8,$y,0,$this->formatVal($this->init['yVal'][$i]).' '.$this->init['yUnit'],$color,6);
		if ($i && $i<$yDiv-1){
			for ($j=$x1;$j<$x2;$j=$j+10) imageline($this->img,$j,$y,$j+4,$y,$color2);
		}
	}
	if ($this->init['yTitle']) $this->drawText(15,$this->init['y2'],90,$this->init['yTitle'],$color,8,null,$this->init['y1']);
	//y2 axis
	if (isset($this->data2)){
		$yDiv=count($this->init['yVal2']);
		for ($i=0;$i<$yDiv;$i++){
			$y=$i*$this->init['step2']+$y1;
			imageline($this->img,$x2,$y,$x2+3,$y,$color);
			$this->drawText($x2+7,$y,0,$this->formatVal($this->init['yVal2'][$i]).' '.$this->init['yUnit'],$color,4);
		}
	}
	//x axis
	$is_bar=(substr($this->init['chart'],0,3)=='bar');
	$xNum=count($this->init['col']);
	$xDiv=($x2-$x1)/($is_bar ? $xNum : $xNum-1);
	if ($is_bar){
		$fm=$xDiv/2;
		$tickFm=0; $tickTo=$xNum;
	}else{
		$tickFm=1; $tickTo=$xNum-2;
	}
	for ($i=0;$i<$xNum;$i++){
		$x=$i*$xDiv+$fm+$x1;
		$this->init['xVal'][$i]=$x;
		imageline($this->img,$x,$y2,$x,$y2+3,$color);
		if (!$this->init['xSkip'] || $i==($this->init['xSkip']+1)*$skip){
			$skip++;
			if (!$this->init['xAngle']){
				$a=0; $b=18; $align=8;
			}elseif ($this->init['xAngle']==-45){
				$a=0; $b=8; $align=7;
			}elseif ($this->init['xAngle']==-90){
				$a=-8; $b=8; $align=8;
			}
			$this->drawText($x+$a,$y2+$b,$this->init['xAngle'],$this->formatVal($this->init['col'][$i],1),$color,$align);
		}
		if ($i>=$tickFm && $i<=$tickTo){
			for ($j=$y1;$j<$y2;$j=$j+10) imageline($this->img,$x,$j,$x,$j+4,$color2);
		}
	}
	if ($this->init['xTitle']) $this->drawText($this->init['x1'],$this->init['y2']+25,0,$this->init['xTitle'],$color,2,$this->init['x2']);
	if ($this->init['title']) $this->drawText(10,8,0,$this->init['title'],$color,$this->init['titleAlign'],$this->init['w']-10);
}
function formatVal($v,$x=0){
	if ($x && !$this->init['xFormat']) return $v;
	if (!$x && !$this->init['yFormat']) return $v;
	$fmt=($x ? $this->init['xFormat'] : $this->init['yFormat']);
	$fmt=@explode("|",$fmt,4);
	if ($fmt[0]=='format') return @number_format($v,$fmt[1],$fmt[2],$fmt[3]);
	if ($fmt[0]=='substr'){
		if ($fmt[2]<>'') return @substr($v,$fmt[1],$fmt[2]);
		return @substr($v,$fmt[1]);
	}
	$id=($x ? $this->init['xID'] : $this->init['yID']);
	if ($id[$v]<>'') return $id[$v];
	return $v;
}
function drawChart($data,$mode=''){
	if (count($this->init['xVal'])<2) $W=$this->init['x2']-$this->init['x1'];
	else $W=$this->init['xVal'][1]-$this->init['xVal'][0]-2;
	if (count($data)==2 && isset($data['bChart_SumUp_Original'])){
		$is_sum=1; $w=$W;
	}else $w=$W/count($data);
	if (isset($this->data2)) $w=$W/2;
	$lw=$this->init['lineWidth']/2;//for line chart
	$colorN=count($this->init['colors']);
	foreach ($this->init['xVal'] as $i=>$x){
		$j=($mode ? 1 : 0);
		foreach ($data as $sn=>$arr){
			$c=$this->init['colors'][$j%$colorN];
			$c=imagecolorallocate($this->img,$c[0],$c[1],$c[2]);
			$y=($this->init['yVal'.$mode][0]-$arr[$i])*$this->init['step'.$mode]/($this->init['yVal'.$mode][0]-$this->init['yVal'.$mode][1]);
			//draw bar
			if ($this->init['chart']=="barV" || $is_sum){
				$barX=$x-$W/2;
				$barX2=$barX+$W;
				if ($is_sum){//same copy from bar
					$ya=$this->init['y1']+$y;
					$yb=$this->init['y1']+$this->init['yVal'][0]*$this->init['step']/($this->init['yVal'][0]-$this->init['yVal'][1]);
				}elseif (!$j){
					$ya=$this->init['y1']+$y;
					$yb=$this->init['y1']+$this->init['yVal'][0]*$this->init['step']/($this->init['yVal'][0]-$this->init['yVal'][1]);
				}else{
					$yb=$ya;
					$ya -=$arr[$i]*$this->init['step']/($this->init['yVal'][0]-$this->init['yVal'][1]);
				}
			}else{
				$barX=$x-$W/2+$j*$w;
				$barX2=$barX+$w;
				$ya=$this->init['y1']+$y;
				$yb=$this->init['y1']+$this->init['yVal'.$mode][0]*$this->init['step'.$mode]/($this->init['yVal'.$mode][0]-$this->init['yVal'.$mode][1]);
			}
			if ($ya>$yb) $this->swap($ya,$yb);
			if ($ya<$this->init['y1']) $ya=$this->init['y1'];
			if ($yb>$this->init['y2']) $yb=$this->init['y2'];
			imagefilledrectangle($this->img,$barX,$ya,$barX2,$yb,$c);
			//draw value
			if ($this->init['chart']<>"barV" && $this->init['valShow']) $this->drawText($barX,$ya-2,$this->init['valAngle'],$arr[$i],$c,8,$barX+$w);
			$j++;
		}
		if ($this->init['chart']=="barV" && $this->init['valShow']) $this->drawText($barX,$ya-2,$this->init['valAngle'],$this->init['ttl'][$i],$c,8,$barX+$W);
	}
}
function initInit($init,$data){
	$this->initInitDefa('h',230);
	$this->initInitDefa('chart','bar');
	$this->initInitDefa('bgStripe',1);
	$this->initInitDefa('fmt','str');
	$this->initInitDefa('sepCol',',');
	$this->initInitDefa('sepRow',chr(10));
	//font dir might need a fix? fix later
	$this->initInitDefa('font',@dirname(__FILE__).'/DejaVuSansCondensed.ttf');
	$this->initInitDefa('fontSize',8);
	$this->initInitDefa('x1',($this->init['yTitle'] ? 70 : 55));
	$this->initInitDefa('y1',20+($this->init['title'] ? 15 : 0));
	$this->initInitDefa('y2',$this->init['h']-($this->init['xTitle'] ? 45 : 30));
	$this->initInitDefa('lineWidth',1);
	if ($init['titleAlign']<>1) $this->init['titleAlign']=2;
	$data_fmt=@substr($init['fmt'],0,3);
	if ($data_fmt=='csv') $this->initDataCSV($data);
	elseif ($data_fmt=='sql') $this->initDataSQL($data);
	elseif ($data_fmt=='arr') $this->initDataArr($data);
	else $this->initDataStr($data);
	if ($init['gapT']) $this->init['y1'] +=$init['gapT'];
	if ($init['gapB']) $this->init['y2'] -=$init['gapB'];
	if ($init['gapL']) $this->init['x1'] +=$init['gapL'];
	$legend=@substr($this->init['legend'],0,1);
	if ($legend=='T') $this->init['y1'] +=15;
	if ($legend=='B') $this->init['y2'] -=15;
	if ($init['yAxis']==2 && count($this->data)==2){
		$this->init['chart']='bar';
		$i=0;
		foreach ($this->data as $k=>$v){
			if (!$i) $tmp[$k]=$v;
			else $this->data2[$k]=$v;
			$i++;
		}
		$this->data=$tmp;
	}
	$this->auditData($this->data);
	if (!isset($this->init['legend']))
		$this->init['legend']=(count($this->init['serial'])>1 ? 1 : 0);
	if ($init['w']=='fix'){
		$num=count($this->init['col']);
		if ($this->init['chart']=='bar') $num=$num*count($this->init['serial'])*20;
		else $num=$num*21;
		$this->init['w']=$this->init['x1']+20+$num+($this->init['legend'] ? 95 : 0);
	}else $this->initInitDefa('w',448);
	$this->init['x2']=$this->init['w']-20-($this->init['legend']==1 ? 95 : 0);
	if ($init['gapR']) $this->init['x2'] -=$init['gapR'];
	if (isset($this->data2)){
		$this->init['x2'] -=35;
		$this->auditData($this->data2,2);
	}
	$this->initColor();
}
function initColor(){//ttl 22 colors - should be enough
	$color="94,48,0;51,102,153;247,143,1;90,181,110;201,34,0;238,208,181;153,102,51;153,51,51;153,153,102;102,204,204;102,102,153;51,153,153;102,204,102;153,102,102;153,153,153;51,153,51;204,102,102;51,204,153;102,153,102;153,153,255;102,153,153;204,153,102";
	if ($this->init['color']) $color=$this->init['color'].";".$color;//add at front
	if ($this->init['colorAdd']) $color .=";".$this->init['colorAdd'];//add at end
	$arr=@explode(';',$color);
	foreach ($arr as $v){
		$arr2=@explode(',',$v);
		$R=@ceil($arr2[0]);
		$G=@ceil($arr2[1]);
		$B=@ceil($arr2[2]);
		if ($R>-1 && $R<256 && $G>-1 && $G<256 && $B>-1 && $B<256) $colors[]=@array($R,$G,$B);
	}
	if ($this->init['colorDel']<>''){
		$arr=@explode(",",$this->init['colorDel']);
		foreach ($arr as $v) unset($colors[$v]);
		foreach ($colors as $v) $this->init['colors'][]=$v;
	}else $this->init['colors']=$colors;
}
function initInitDefa($name,$var){if (!isset($this->init[$name])) $this->init[$name]=$var;}
function initDataCSV($file){$this->initDataStr(@file_get_contents($file));}
function initDataStr($str){
/*csv,str,sql,arr are in same format: line1=header
Year,Jan,Feb,Mar,... csv|str|sql|arr
2000,123,234,345,...
2003,333,444,555,...

Year,Mon,Amt -- csvV2|strV2|sqlV2|arrV2
2002,Jan,123
2002,Feb,234
2003,May,122

Mon,Amt,IP,Hit -- csvV|strV|sqlV|arrV
Jan,100,22,333
Feb,200,44,555*/
	$str=@trim($str);
	$arr=@explode($this->init['sepRow'],$str);
	foreach ($arr as $i=>$line) $data[]=@explode($this->init['sepCol'],$line);
	$this->initDataArr($data);
}
function initDataSQL($sql){
	$res=@mysql_query($sql);
	$num=@mysql_num_fields($res);
	for ($i=0;$i<$num;$i++) $data[0][]=@mysql_field_name($res,$i);
	while ($row=@mysql_fetch_row($res)) $data[]=$row;
	$this->initDataArr($data);
}
function initDataArr($data){
	$style=@substr($this->init['fmt'],3);//'',V,V2
	$head=@array_shift($data);
	$num=count($head);
	if ($style=='V2' && $num<3) $style='V';
	if ($num==1){
		foreach ($data as $arr) $this->data[$head[0]][]=$arr[0];
	}elseif ($style=='V'){
		$sn=$head;
		@array_shift($sn);
		foreach ($data as $arr){
			$this->init['col'][]=$arr[0];
			foreach ($sn as $k=>$v) $this->data[$v][]=$arr[$k+1];
		}
	}elseif ($style=='V2'){
		foreach ($data as $arr){
			if (!@in_array($arr[0],$sn)) $sn[]=$arr[0];
			if (!@in_array($arr[1],$col)) $col[]=$arr[1];
			$tmp[$arr[0]][$arr[1]]=$arr[2];
		}
		$this->init['col']=$col;
		foreach ($sn as $s){
			foreach ($col as $v) $this->data[$s][]=$tmp[$s][$v];
		}
	}else{
		$this->init['col']=$head;
		@array_shift($this->init['col']);
		foreach ($data as $arr) $this->data[@array_shift($arr)]=$arr;
	}
}
function auditData(&$data,$mode=''){
	$SUM=$this->init['sum'];
	if ($SUM && ($mode || count($data)==1)) $data_hold=$data;
	foreach ($data as $k=>$v){
		$this->init['serial'][]=$k;
		foreach ($v as $k1=>$v1){
			if ($colNum<$k1) $colNum=$k1;
			if ($SUM){
				if ($SUM=='all' || @substr($this->init['col'][$k1],0,$SUM)==$hold_k) $sum[$k] +=$v1;
				else{
					$sum[$k]=$v1;
					$hold_k=@substr($this->init['col'][$k1],0,$SUM);
				}
				$v1=$data[$k][$k1]=$sum[$k];
			}
			if ($this->init['chart']=='barV') $this->init['ttl'][$k1] +=$v1;
			else{
				if (!isset($max) || $max<$v1) $max=$v1;
				if (!isset($min) || $min>$v1) $min=$v1;
			}
		}
	}
	if (isset($data_hold)) $data['bChart_SumUp_Original']=$data_hold[$k];
	if ($this->init['chart']=='barV'){
		foreach ($this->init['ttl'] as $v1){
			if (!isset($max) || $max<$v1) $max=$v1;
			if (!isset($min) || $min>$v1) $min=$v1;
		}
	}
	$this->init['max'.$mode]=$max;
	$this->init['min'.$mode]=$min;
	for ($i=0;$i<=$colNum;$i++){
		if (!$this->init['col'][$i]) $this->init['col'][$i]=$i;
	}
	//calculat y-axis
	if ($max<0 && $min<0) $ttl=$min;
	elseif ($max<0 || $min<0) $ttl=$max-$min;
	else $ttl=$max;
	if ($ttl<0) $ttl=abs($ttl);
	$this->init['zoom'.$mode]=$zoom=pow(10,floor(log10($ttl)));
	$ttl=$ttl/$zoom;
	if ($ttl==1) $step=0.2;
	elseif ($ttl>6) $step=2;
	elseif ($ttl>5) $step=1.2;
	elseif ($ttl>4) $step=1;
	elseif ($ttl>3) $step=0.8;
	elseif ($ttl>2) $step=0.6;
	else $step=0.4;
	$this->init['step'.$mode]=$step;
	if ($max<=0 && $min<0){
		for ($i=0;$i<6;$i++) $this->init['yVal'.$mode][]=0-$i*$zoom*$step;
	}else{
		for ($i=5;$i>-1;$i--) $this->init['yVal'.$mode][]=$i*$zoom*$step;
		if ($max<=0 || $min<0) $this->auditDataYval($max,$min,$step,0,$mode);
	}
	if ($max>0 && $this->init['yVal'.$mode][1]>=$max) array_shift($this->init['yVal'.$mode]);
	elseif ($min<0 && $this->init['yVal'.$mode][4]<=$min) array_pop($this->init['yVal'.$mode]);
	$step=count($this->init['yVal'.$mode]);
	if ($min<$this->init['yVal'.$mode][$step-1]) $this->init['yVal'.$mode][]=$this->init['yVal'.$mode][$step-1]-abs($this->init['yVal'.$mode][$step-2]-$this->init['yVal'.$mode][$step-3]);
	$this->init['step'.$mode]=($this->init['y2']-$this->init['y1'])/(count($this->init['yVal'.$mode])-1);
}
function auditDataYval($max,$min,$step,$count=0,$mode){
	if ($count>5) return;
	$count++;
	if ($this->init['yVal'.$mode][1]>$max && $this->init['yVal'.$mode][5]>$min){
		$this->init['yVal'.$mode][]=$this->init['yVal'.$mode][5]-$this->init['zoom'.$mode]*$step;
		array_shift($this->init['yVal'.$mode]);
		$this->auditDataYval($max,$min,$step,$count,$mode);
	}
}
function swap(&$a,&$b){$c=$a;$a=$b;$b=$c;}
}
?>
