<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>mtp-target</title>
<link rel="stylesheet" type="text/css" href="http://www.mtp-target.org/mtptarget.css">
</head>
<body>

<table border="0">
<tr><td>
</tr></td>

<?php

        $fp = fopen("../adv_server/connection.stat", "r");
        while (!feof($fp))
	{
                $line = fgets($fp, 2048);
                $out = substr($line,0,strlen($line)-1); //remove \n
		$array = explode(" ", $out);
		if(count($array)>=9 && $array[7] == '+')
		{
			$ut = str_replace("'","",$array[9]);
			if(strlen($ut)>0)
			{
				$namecount[$ut] = $namecount[$ut] + 1;
			}
		}
	}

	arsort($namecount, SORT_NUMERIC);
	reset($namecount);
        echo "<h3>Most used user textures:</h3>";
	$i = 0;	
	while (list($key, $val) = each($namecount))
	{
		echo "<a href=\"../user_texture/ping_ball_$key.tga\"><img width=64 src=\"../user_texture/ping_ball_$key.jpg\" ALT=\"file\"></a>";
		echo "Name: ping_ball_$key.tga Used: $val<br>\n";
		$i=$i+1;
		if($i==5) break;
	}
	echo "<h3>All user textures available</h3>";

		$dirname = "../user_texture/";
		$d = dir($dirname);
		while (false !== ($entry = $d->read())) 
		{
			$f = $dirname . $entry ;
			$t = filetype($f);
			if($t == "file")
			{
				if(strstr($entry,"ping_ball_")!="" && substr($entry,-3)=="tga" )
				{
					$modified = stat("../user_texture/$entry");
					$moddate = date("D M j G:i:s T Y",$modified[9]);
					printf("<tr>");
					$entryjpg = str_replace(".tga", ".jpg", $entry);
					$cfgEntry = strstr($entry,"ping_ball_");
					$cfgEntry = substr($cfgEntry,strlen("ping_ball_"),strlen($cfgEntry)-4-strlen("ping_ball_"));
					printf("<td><a href=\"../user_texture/$entry\"><img src=\"../user_texture/$entryjpg\" ALT=\"file\"></a></td>");
					echo "<td><table>";
					echo "<tr><td>Name: $entry</td></tr>";
					echo "<tr><td>Uploaded: $moddate</td></tr>";
					echo "<tr><td>Add the following line in mtp_target.cfg</td></tr>";
					echo "<tr><td><b>EntityTexture = \"$cfgEntry\";</b></td></tr>";
					echo "</table>";
					printf("</td></tr>");
					printf("\n");
				}
			}
		}
		$d->close();
	?>
</table>

</body>
</html>
