<?php

# This script is used to insert all users that is not on the database
# easy way to add the score if necessary

    $filename = "/home/ace/cvs/mtp-target/adv_server/mtp_target_service.cfg";

	$fp = fopen ($filename, "rb");
	if (!$fp)
	{
		echo "no info";
		return;
	}

	$DBHost = "mtp-target.dyndns.org";
	$DBUserName = "root";
	$DBPassword = "PlokiAqz";
	$DBName = "nel";

	$link = mysql_connect($DBHost, $DBUserName, $DBPassword) or die ("Can't connect to database host:$DBHost user:$DBUserName");
	mysql_select_db ($DBName) or die ("Can't access to the table dbname:$DBName");

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
				$login = trim($arr[$i]);
				$login = trim($login, "\"");

				$pass = trim($arr[$i+1]);
				$pass = trim($pass, "\"");

				$score = (int)(strtr($arr[$i+2],"\""," "));

				//echo $login." ".$score."<br>";

				//$query = "select * from user where Login='".$login."'";
				//$query = "update user set Score=Score+".$score." where Login='".$login."'";
				//echo $query."<br>";
				$result = mysql_query ($query) or die ("Can't execute the query: ".$query);
/*				$num = mysql_num_rows($result);
				if($num != 1)
				{
					$cpass = crypt($pass, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./");

					echo $login." ".$cpass."<br>\n";

					$query = "insert into user (Login, Password, Registered) values ('".$login."', '".$cpass."', NOW())";
					//$result = mysql_query ($query) or die ("Can't execute the query: ".$query);
					echo $query."<br>";

					$nb++;
				}
*/
				$i+=3;
			}
		}
	}
	fclose($fp);

//	echo "Number of row affected: ".$nb;
?>
