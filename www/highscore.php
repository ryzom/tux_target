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

	if(!isset($server)) $server = "easy";

	if($server == "easy")
	{
	    $filename = "/home/ace/cvs/mtp-target/server/mtp_target_service.cfg";
		$servername = "easy";
	}
	else if($server == "adv")
	{
    	$filename = "/home/ace/cvs/mtp-target/adv_server/mtp_target_service.cfg";
		$servername = "advanced";
	}
	else
	{
		die("ERROR: Unknown server $server");
	}

	echo "<h2>Official $servername server high scores</h2>\n";

	$fp = fopen ($filename, "rb");
	if (!$fp)
	{
		echo "no info";
		return;
	}

	while (!feof($fp))
	{
		$line = fgets($fp, 10000);
		if (strstr ($line, "Accounts"))
		{
			$line = "";

			while (!feof($fp))
			{
				$nl = fgets($fp, 10000);
				if(strstr($nl,"};")) break;
				$line = $line.$nl;
			}
			$line = strtr($line, "=,{};","     ");
			$line = ereg_replace (' +', ' ', $line);
			list($user,$pass,$score) = explode(" ",$line);
			$arr = explode(" ",$line);
			$p=0;

			for ($i = 0; $i+2 < sizeof($arr);)
			{
				$login = strtr($arr[$i],"\""," ");
				$score = (int)(strtr($arr[$i+2],"\""," "));
				// uncomment this line for debug 
				//echo $login." ".$score."<br>\n";

				if($server == "easy" && $score < 10000 || $server == "adv")
				{
					$my1[$p] = $login;
					$my2[$p] = $score;

					$b = strpos($my1[$p],'[');
					$e = strpos($my1[$p],']');
					if($b !== false && $e !== false)
					{
						$team = substr($my1[$p],$b+1,$e-$b-1);
						$team_score[$team] = $team_score[$team] + $my2[$p];
						$team_numbers[$team] = $team_numbers[$team] + 1;
					}
					$p++;
				}
				$i+=3;
			}
			arsort($team_score, SORT_NUMERIC);
			reset($team_score);

			array_multisort($my2, SORT_DESC, $my1);

			echo "<h3>Registered:</h3>";
			echo sizeof($my1)." players</p>";

			echo "<h3>10 best players high scores ";
			if($server=="easy") echo "(on the easy server, only score < than 10000 are displayed)";
			echo ":</h3>";

			for ($i = 0; $i < 1; $i++)
				echo "<font style=\"color:red\"><b>$my1[$i]</b> : $my2[$i]</font><br>";

			for (; $i < 3; $i++)
				echo "<b>$my1[$i]</b> : $my2[$i]<br>";

			for (; $i < 10; $i++)
				echo "$my1[$i] : $my2[$i]<br>";

                        echo "<h3>3 best teams high scores :</h3>";
			$i = 0;	
			while (list($key, $val) = each($team_score))
			{
                                echo "$key : $val ($team_numbers[$key] members)<br>";
				$i=$i+1;
				if($i==3) break;
			}

		}
		else if (strstr ($line, "LevelStats"))
		{
			$line = "";

			while (!feof($fp))
			{
				$nl = fgets($fp, 10000);
				if(strstr($nl,"};")) break;
				$line = $line.$nl;
			}
			$line = strtr($line, "=,{};","     ");
			$line = ereg_replace (' +', ' ', $line);
			$LevelStats = explode(" ",$line);
		}
	}
	fclose($fp);

	echo "<h3>Highscores by level :</h3>";
	if (isset($LevelStats))
	{
	 for ($i=0; $i < sizeof($LevelStats); $i++)
	 {
		$LevelStats[$i] = strtr($LevelStats[$i],"\""," ");
	}

	 for ($i=0; $i+4 < sizeof($LevelStats); $i+=5)
	 {
		echo "- <b>$LevelStats[$i] :</b><br>";
		echo "Best time with ".$LevelStats[$i+2]." seconds : ".$LevelStats[$i+1]."<br>";
		echo "Best score with ".$LevelStats[$i+4]." points : ".$LevelStats[$i+3]."<br>";
	}
}

?>
</body>
</html>
