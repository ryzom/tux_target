<?php
include_once("config.php");

function validInput($input)
{
  if(strlen($input)>20) return false;
  $res = (ereg("^[\-_\.A-Za-z0-9([)(|\]|])]+$",$input));
  //$res = (ereg("^[_\.A-Za-z0-9\-]+$",$input));
  /*
  if(!$res)
  	echo "not a valid input : ".$input."<br>";
  */
  return $res;
}

function textureName2Id($texture_name)
{
	  $requete = "SELECT Id FROM texture WHERE Name='".$texture_name."';";
	  $resultat=exec_game_db_requete($requete);
	
	  if($ligne = mysql_fetch_array($resultat))
	  {
	  	return $ligne[0];
	  }
	  echo "texture not found : ".$texture_name;
	  return -1;	
}



if(!isset($user_login) || !validInput($user_login))
	$user_login = "Anonymous";

$uploaddir = '/home/skeet/cvs/code/mtp-target/user_texture/';
$webdir = '/var/www/';
$uploadfilename = strtolower($_FILES['userfile']['name']);
$uploadcrcflagname = 'update_crc_flag.txt';
$uploadcrcflagfilename = $uploaddir.$uploadcrcflagname;
$uploadfile = $uploaddir . $uploadfilename;
$maxfilesize = 300000; // some tga are bigger because they add some dumb extra infos

if($_FILES['userfile']['size'] > $maxfilesize)
{
   die("FAILED! The file you tried to upload is too big (can't be more than ".maxfilesize." bytes)");
}

if(file_exists($uploadfile) || $uploadfilename == "ping_ball_blue.tga")
{
   die("FAILED! The file you tried to upload already exists, please, use another name");
}

if(!ereg("ping_ball_([[:alnum:]]+).tga", $uploadfilename, $regs))
{
   die("FAILED! The file you tried to upload doesn't follow the file name rulez (for example: ping_ball_ryzom.tga), please, use another name");
}

if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile))
{
   system("chmod 644 ".$uploaddir."ping_ball_".$regs[1].".tga");
   system("convert ".$uploaddir."ping_ball_".$regs[1].".tga ".$webdir."user_texture/ping_ball_".$regs[1].".jpg");
   system($webdir."/mtp-target/devilconvert ".$uploaddir."ping_ball_".$regs[1].".tga ".$uploaddir."ping_ball_".$regs[1].".dds");

   $fp = fopen($uploadcrcflagfilename,"at");
   if(!$fp)
	   echo "<p>cannot open ".$uploadcrcflagname;
   else
   {
   	fwrite($fp,sprintf("%s\n",$uploadfilename));
   	fclose($fp);
   }

/*
   echo "<p>Thank you $user_login !";
   echo "<p>The file '".$uploadfilename."' was uploaded(will be available on servers within 15 min).";
   echo "<p>Don't forget  to add the following line in your mtp_target.cfg: ";
   echo '<pre>EntityTexture = "'.$regs[1].'";</pre></p>';
*/   
   $requete = sprintf("INSERT INTO texture (Name,Date,UploadBy) VALUES('%s','%s','%s');",$regs[1],date("Y-m-d H:i:s"),$user_login);
   $resultat=exec_game_db_requete($requete);
   $user_texture_id = textureName2Id($regs[1]);
   header("Location: http://www.mtp-target.org/index.php?page=user_texture_upload_done.php&user_texture_id=$user_texture_id");
}
else
{
   die("FAILED! Possible file upload attack!");
}

?>
