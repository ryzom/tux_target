<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>mtp-target</title>
<link rel="stylesheet" type="text/css" href="http://www.mtp-target.org/mtptarget.css">
</head>
<body>

<?php
function generateHtml($server)
{

	if (!function_exists('fprintf')) {
	   if (function_exists('vsprintf')) { // >= 4.1.0
	       function fprintf() {
	           $args = func_get_args();
	           $fp = array_shift($args);
	           $format = array_shift($args);
	           return fwrite($fp, vsprintf($format, $args));
	       }
	   } else { // < 4.1.0
	       function fprintf() {
	           $args = func_get_args();
	           $fp = array_shift($args);
	           $format = array_shift($args);
	           $code = '';
	           for ($i = 0; $i < count($args); ++$i) {
	               if ($code) {
	                   $code .= ',';
	               }
	               $code .= '$args[' . $i . ']';
	           }
	           $code = 'return sprintf($format, ' . $code . ');';
	           $rv = eval($code);
	           return $rv ? fwrite($fp, $rv) : false;
	       }
	   }
	}
	
	$html_fp = fopen("stats_$server.html", "wt");

	if($server == "easy")
	{
	    $filename = "/home/ace/cvs/mtp-target/server/connection.stat";
		$servername = "easy";
	}
	else if($server == "adv")
	{
    	$filename = "/home/ace/cvs/mtp-target/adv_server/connection.stat";
		$servername = "advanced";
	}
	else
	{
		die("ERROR: Unknown server $server");
	}

	$fp = fopen($filename, "r");
	if (!$fp) {echo "<p>Unable to open remote file.</p>"; exit;}
	
	$userCountPerHourTotal;
	$userCountPerHourToday;
	$userCountPerHourTodayMax;
	$userCountPerMinuteTodayMax;
	$lastHour = 0;
	$lastDay = 0;
	$lastMinute = 0;
	$once = 0;
	$firstDate = mktime();
	$lastRestartTime = mktime();
	
	for($i=0;$i<24;$i++)
	{
		$userCountPerHourTotal[$i]=0;
		$userCountPerHourToday[$i]=0;
		$userCountPerHourTodayMax[$i]=0;
	}
	$userCountPerMinuteTodayMax = array_fill(0,60,-1);
	
	$playerCount = 0;
	while (!feof($fp)):
		$line = fgets($fp, 2048);
		$out = array(substr($line,0,strlen($line)-1));//remove \n
		list ($ttime, $syear, $smonth, $sday, $shour, $smin, $ssec, $inout, $sname)= split (" ", $out[0]);

		$name = str_replace("'","",$sname);

		//fprintf($html_fp,"[%s] -> [%s]<br>",$sname,$name);
		
		$hour = intval($shour);
		$minute = intval($smin);
		$sec = intval($ssec);
		$day = intval($sday);
		$month = intval($smonth);
		$year = intval($syear);
		if(strlen($line)!=0 && $ttime!="c")
		{		
			if($lastHour!=$hour)
			{
				//fprintf($html_fp,"%dh0 = %d<br>",$lastHour,$userCountPerHourTotal[$lastHour]);
				$lastHour=$hour;
				$userCountPerMinuteTodayMax = array_fill(0,60,-1);
				$userCountPerMinuteTodayMax[0]=$playerCount;
				/*
				for($i=0;$i<60;$i++)
				{
					fprintf($html_fp,"%d<br>",$userCountPerMinuteTodayMax[$i]);
				}
				fprintf($html_fp,"<hr>");
				*/
			}
			
			if($lastDay!=$day)
			{
				$lastDay=$day;
				for($i=0;$i<24;$i++)
				{
					$userCountPerHourToday[$i]=0;
					$userCountPerHourTodayMax[$i]=0;
				}
				//fprintf($html_fp,"<hr>");
			}
			
			if($inout=='+')
			{
				$lastMinute = $minute;
				$playerCount++;
				$userCountPerHourTotal[$hour]++;
				$userCountPerHourToday[$hour]++;
				if($playerCount>$userCountPerHourTodayMax[$hour])
					$userCountPerHourTodayMax[$hour] = $playerCount;
				if($playerCount>$userCountPerMinuteTodayMax[$minute])
					$userCountPerMinuteTodayMax[$minute] = $playerCount;
				//fprintf($html_fp,"%d:%d [%s] comes(%d,%d,%d) in<br>",$hour,$minute,$name,$playerCount,$userCountPerHourToday[$hour],$userCountPerMinuteTodayMax[$minute]);
			}
			else if($inout=='#')
			{
				$lastRestartTime = mktime($hour,$minute,$sec,$month,$day,$year);
				if($once==0)
				{
					$firstDate = $lastRestartTime;
					$once = 0;
				}
				$playerCount = 0;
				//fprintf($html_fp,"<hr>",$name);
			}
			else if($inout=='?')
			{
				if(strlen($name)>0)
					$playerCount++;
				//fprintf($html_fp,"invalid login(%d) %s(%d)<br>",$playerCount,$name,strlen($name));
			}
			else if($inout=='-')
			{
				if(strlen($name)>0)
					$playerCount--;
				//fprintf($html_fp,"[%s](%d) leaves(%d)<br>",$name,strlen($name),$playerCount);
			}
		}
		$fp++;
	endwhile;
	
	fclose($fp);

	$t1 = strtotime(date("Y-m-d H:i:s"));
	$t2 = strtotime(date("Y-m-d H:i:s",$lastRestartTime));
	$t3 = $t1-$t2;
	
	$upSec = $t3%60;
	$upMin = ($t3/60)%60;
	$upHour = (($t3/60)/60)%24;
	$upDay = ((($t3/60)/60)/24)%100;

	fprintf($html_fp,"Server : %s<br>\n",$servername);
	fprintf($html_fp,"Stats generated : %s<br>\n",date("l dS of F Y H:i:s"));
	
	if($upDay == 1)
		fprintf($html_fp,"Server uptime : %d day %02d:%02d:%02d <br>",$upDay,$upHour,$upMin,$upSec);
	else if($upDay > 1)
		fprintf($html_fp,"Server uptime : %d days %02d:%02d:%02d <br>",$upDay,$upHour,$upMin,$upSec);
	else
		fprintf($html_fp,"Server uptime : %02d:%02d:%02d <br>",$upHour,$upMin,$upSec);
	fprintf($html_fp,"Current logged user : %d<br>\n",$playerCount);
	fprintf($html_fp,"Last restart : %s<br>\n",date("l dS of F Y H:i:s",$lastRestartTime));
	

	fprintf($html_fp,"<br>\n");
	fprintf($html_fp,"<br>\n");
	
	fprintf($html_fp,"<b>Total</b> login :<br>\n");
	fprintf($html_fp,"Since %s\n",date("l dS of F Y h:i:s A",$firstDate));
	
	fprintf($html_fp,"<table bgcolor=\"#FFFAEA\">\n");
	fprintf($html_fp,"<tr valign=\"bottom\">\n");

	$maxTotal = 0;
	
	for($i=0;$i<24;$i++)
	{
		if($userCountPerHourTotal[$i]>$maxTotal)
			$maxTotal = $userCountPerHourTotal[$i];
	}
	
	for($i=0;$i<24;$i++)
	{
		//fprintf($html_fp,"=>%dh0 : %d<br>",$i,$userCountPerHourTotal[$i]);
		fprintf($html_fp,"<td valign=\"bottom\">\n");
		if($maxTotal!=0)
			fprintf($html_fp,"\t<img align=\"bottom\" src=\"./img/vp.png\" height=\"%d\" width=\"6\" alt='total login : %d' title='total login : %d' />\n",$userCountPerHourTotal[$i]*100/$maxTotal,$userCountPerHourTotal[$i],$userCountPerHourTotal[$i]);
		fprintf($html_fp,"</td>\n");
	}
	fprintf($html_fp,"</tr>\n");
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td width=20>%d</td>\n",$userCountPerHourTotal[$i]);
	}
	fprintf($html_fp,"</tr>\n");	

	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td><img src=\"./img/hr%d.png\" width=\"10\"/></td>\n",($i%12)+1);
	}
	fprintf($html_fp,"</tr>\n");	
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td>%d</td>\n",$i);
	}
	fprintf($html_fp,"</tr>\n");	

	fprintf($html_fp,"</table>\n");
	fprintf($html_fp,"<br>\n");
	fprintf($html_fp,"<br>\n");
	
	fprintf($html_fp,"Today login :\n");
	fprintf($html_fp,"<table bgcolor=\"#FFFAEA\">\n");
	fprintf($html_fp,"<tr valign=\"bottom\">\n");
	
	$maxToday = 0;
	
	for($i=0;$i<24;$i++)
	{
		if($userCountPerHourToday[$i]>$maxToday)
			$maxToday = $userCountPerHourToday[$i];
		
	}
	
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td valign=\"bottom\">\n");
		if($maxToday!=0)
			fprintf($html_fp,"\t<img align=\"bottom\" src=\"./img/vh.png\" height=\"%d\" width=\"6\" alt='today login : %d' title='today login : %d' />\n",$userCountPerHourToday[$i]*100/$maxToday,$userCountPerHourToday[$i],$userCountPerHourToday[$i]);
		fprintf($html_fp,"</td>\n");
	}
	fprintf($html_fp,"</tr>\n");
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td width=20>%d</td>\n",$userCountPerHourToday[$i]);
	}
	fprintf($html_fp,"</tr>\n");	
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td><img src=\"./img/hr%d.png\" width=\"10\"/></td>\n",($i%12)+1);
	}
	fprintf($html_fp,"</tr>\n");	

	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td>%d</td>\n",$i);
	}
	fprintf($html_fp,"</tr>\n");	
	
	fprintf($html_fp,"</table>\n");
	fprintf($html_fp,"<br>\n");
	fprintf($html_fp,"<br>\n");
	
	
	fprintf($html_fp,"Today Max simultaneous user :\n");
	fprintf($html_fp,"<table bgcolor=\"#FFFAEA\">\n");
	fprintf($html_fp,"<tr valign=\"bottom\">\n");

	$maxTodayMax = 0;
	
	for($i=0;$i<24;$i++)
	{
		if($userCountPerHourTodayMax[$i]>$maxTodayMax)
			$maxTodayMax = $userCountPerHourTodayMax[$i];
		
	}
	
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td valign=\"bottom\">\n");
		if($maxTodayMax!=0)
			fprintf($html_fp,"\t<img align=\"bottom\" src=\"./img/vh.png\" height=\"%d\" width=\"6\" alt='today simultaneous user : %d' title='today simultaneous user : %d' />\n",$userCountPerHourTodayMax[$i]*100/$maxTodayMax,$userCountPerHourTodayMax[$i],$userCountPerHourTodayMax[$i]);
		fprintf($html_fp,"</td>\n");
	}
	fprintf($html_fp,"</tr>\n");
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td width=20>%d</td>\n",$userCountPerHourTodayMax[$i]);
	}
	fprintf($html_fp,"</tr>\n");	
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td><img src=\"./img/hr%d.png\" width=\"10\"/></td>\n",($i%12)+1);
	}
	fprintf($html_fp,"</tr>\n");	

	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td>%d</td>\n",$i);
	}
	fprintf($html_fp,"</tr>\n");	
	
	fprintf($html_fp,"</table>\n");
	fprintf($html_fp,"<br>\n");
	fprintf($html_fp,"<br>\n");
	
	fprintf($html_fp,"Last hour max simultaneous user :\n");
	fprintf($html_fp,"<table bgcolor=\"#FFFAEA\">\n");
	fprintf($html_fp,"<tr valign=\"bottom\">\n");

	$maxHourMax = 0;
	
	for($i=0;$i<60 && $i<$lastMinute;$i++)
	{
		if($userCountPerMinuteTodayMax[$i]==-1 && $i>0 && $i<$lastMinute)
			$userCountPerMinuteTodayMax[$i]=$userCountPerMinuteTodayMax[$i-1];
		if($userCountPerMinuteTodayMax[$i]>$maxHourMax)
			$maxHourMax = $userCountPerMinuteTodayMax[$i];
		
	}
	
	for($i=0;$i<60;$i++)
	{
		fprintf($html_fp,"<td valign=\"bottom\">\n");
		if($maxHourMax!=0)
			fprintf($html_fp,"\t<img align=\"bottom\" src=\"./img/vp.png\" height=\"%d\" width=\"6\" alt='today simultaneous user : %d' title='today simultaneous user : %d' />\n",$userCountPerMinuteTodayMax[$i]*100/$maxHourMax,$userCountPerMinuteTodayMax[$i],$userCountPerMinuteTodayMax[$i]);
		fprintf($html_fp,"</td>\n");
	}
	fprintf($html_fp,"</tr>\n");
	
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<60 && $i<=$lastMinute;$i++)
	{
		fprintf($html_fp,"<td>%d</td>\n",$userCountPerMinuteTodayMax[$i]);
	}
	fprintf($html_fp,"</tr>\n");	
	
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<60 && $i<=$lastMinute;$i++)
	{
		fprintf($html_fp,"<td>%02d</td>\n",$i);
	}
	fprintf($html_fp,"</tr>\n");	
	
	
	/*
	fprintf($html_fp,"<tr>\n");
	for($i=0;$i<24;$i++)
	{
		fprintf($html_fp,"<td><img src=\"./img/hr%d.png\" width=\"10\"/></td>\n",($i%12)+1);
	}
	fprintf($html_fp,"</tr>\n");
	*/
	fprintf($html_fp,"</table>\n");
	fclose($html_fp);
}


if(!isset($server)) $server = "easy";

//$lastModeTime = strtotime(date("Y-m-d H:i:s")) - filemtime("stats_$server.html");
//printf("%d<br>",$lastModeTime);

if (!file_exists("stats_$server.html") || (strtotime(date("Y-m-d H:i:s")) - filemtime("stats_$server.html"))>60*5)
	generateHtml($server);

$html_fp = fopen("stats_$server.html", "rt");
while (!feof($html_fp)):
	$line = fgets($html_fp, 2048);
	echo $line;
	$html_fp++;
endwhile;
fclose($html_fp);

?>
</body>
</html>
