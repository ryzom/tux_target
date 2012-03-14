<html>
<head>
<title>mtp-target</title>
</head>
<body>

	<?php
		function exec_requete($requete)
		{
		
			$DBHost = "127.0.0.1";
			$DBUserName = "root";
			$DBPassword = "lgt3ki";
			$DBName = "nel";
	
			$MySqlLinkIdentifier = mysql_connect( $DBHost, $DBUserName, $DBPassword ) or die("cannot connect to $DBHost");
		
			mysql_select_db($DBName) or die("cannot select $DBName");
		
			$resultat = mysql_query($requete) or die("query failed(".mysql_error().") : $requete");
			$erreur = mysql_error();
			if ($erreur != "")
			{
		    		die("mysql_error : $erreur");
			}
			return $resultat;
		}
		
		function dbAddFile($srcName,$destDir,$destName)
		{
			$gzFilename = $destDir.$destName.".gz";
			$requete = "SELECT Crc,UNIX_TIMESTAMP(CrcDate) FROM data_file WHERE Name='$destName';";
			$result=exec_requete($requete);
			//echo "$srcName -> $gzFilename<br>";
			if($line = mysql_fetch_array($result))
			{
				$sha1 = $line[0];
				$crcDate = $line[1];
				$filedate = filemtime($srcName);
				if($crcDate>$filedate && file_exists($gzFilename))
				{
					echo "uptodate ";
					return $sha1;
				}
					
				if($crcDate<=$filedate)
					echo "crc not up to date ";
				if(!file_exists($gzFilename))
					echo "$gzFilename file not found ";
					
				echo "computing ... ";
				$sha1 = sha1_file($srcName);
				$command = "nice gzip -c ".$srcName." > ".$gzFilename;
				system($command);
				$requete = "UPDATE data_file SET Crc='$sha1', CrcDate=NOW() WHERE Name='$destName';";
				$result=exec_requete($requete);
				return $sha1;
			}
			else
			{
				echo "$srcName never build : computing ... ";
				$sha1 = sha1_file($srcName);
				$command = "nice gzip -c ".$srcName." > ".$gzFilename;
				system($command);
				$requete = "INSERT INTO data_file (Name,Crc,CrcDate) VALUES ('$destName','$sha1',NOW());";
				$result=exec_requete($requete);
				return $sha1;
			}
			return 0;
		}
		
		function emptyDir($dir)
		{
			if(!file_exists($dir))
			{
				mkdir($dir);
				return;
			}
			
			$handle = opendir($dir);
			if(!handle)
				die("failed to opendir : $dir");
				
			while (false!==($FolderOrFile = readdir($handle)))
			{
				if($FolderOrFile != "." && $FolderOrFile != "..")
				{ 
					if(is_dir("$dir/$FolderOrFile"))
					{ 
						//deldir("$dir/$FolderOrFile");   // recursive
					}
					else
					{ 
						unlink("$dir/$FolderOrFile"); 
					}
				} 
			}
			closedir($handle);
			/*
			if(rmdir($dir))
			{ 
				$success = true; 
			}
			return $success; 
			*/
		} 
		function crcAddDir($fp,$srcDir,$destDir,$validExt)
		{
			$handle = opendir($srcDir);
			if(!handle)
				die("failed to opendir : $srcDir");
			while (false!==($FolderOrFile = readdir($handle)))
			{
				if($FolderOrFile != "." && $FolderOrFile != "..")
				{
					$f = $srcDir . "/" . $FolderOrFile ;
					if(is_dir($f))
					{ 
						crcAddDir($fp,$f,$destDir,$validExt); //recursive
					}
					else
					{ 
						$arr=explode('.',$FolderOrFile);
						$arrSize=count($arr);
						if($arrSize>1)
						{
							$ext = $arr[$arrSize-1];
							if(in_array($ext, $validExt))
							{
								//printf("%s : %s<br>",$f,$ext);
								$sha1 = dbAddFile($f,$destDir,$FolderOrFile);
								printf("%s : %s<br>\n",$FolderOrFile,$sha1);
								fwrite($fp,sprintf("%s\n",$FolderOrFile));
								fwrite($fp,sprintf("%s\n",$sha1));
								//$command = "nice gzip -c ".$f." > ".$destDir.$FolderOrFile.".gz";
								//sleep(1);
								//echo "executing : ".$command."<br>";
								//system($command);
							}
						}
					}
				}
			}
			closedir($handle);		
		}

		$ext  = "tga lua dds ps shape xml";
		$extArray = explode(" ", $ext);
		$user_texture_ext  = "dds";
		$user_texture_extArray = explode(" ", $user_texture_ext);

		$sourceDirname = "/home/skeet/cvs/code/mtp-target/";
		$exportDirname = "./export/";
		$crcFilename = "crc.txt";
		$crcTempFilename = "crc_temp.txt";
		
		$fullCrcFilename = $exportDirname.$crcFilename;
		$fullCrcTempFilename = $exportDirname.$crcTempFilename;

		printf("building : '%s'<br>\n",$fullCrcFilename);
		$crcfp = fopen($fullCrcTempFilename,"wt");
		if(!$crcfp) die("error opening crc file : ".$fullCrcTempFilename);
		
		crcAddDir($crcfp,$sourceDirname."server/data/",$exportDirname,$extArray);
		crcAddDir($crcfp,$sourceDirname."user_texture/",$exportDirname,$user_texture_extArray);
		
		fclose($crcfp);
		if(file_exists($fullCrcFilename))
			unlink($fullCrcFilename);
		rename($fullCrcTempFilename,$fullCrcFilename);
		/*
		$command = "gzip ".$fullCrcFilename;
		echo "executing : ".$command."<br>";
		system($command);
		*/
	?>




</body>
</html>
