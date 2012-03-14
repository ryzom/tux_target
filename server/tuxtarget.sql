-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.1.40-community


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema tuxtarget
--

CREATE DATABASE IF NOT EXISTS tuxtarget;
USE tuxtarget;

--
-- Definition of table `ban`
--

DROP TABLE IF EXISTS `ban`;
CREATE TABLE `ban` (
  `BanId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Ip` varchar(45) NOT NULL,
  `Date` datetime NOT NULL,
  `Duration` int(10) unsigned NOT NULL,
  PRIMARY KEY (`BanId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `ban`
--

/*!40000 ALTER TABLE `ban` DISABLE KEYS */;
/*!40000 ALTER TABLE `ban` ENABLE KEYS */;


--
-- Definition of table `shard`
--

DROP TABLE IF EXISTS `shard`;
CREATE TABLE `shard` (
  `ShardId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `WSAddr` varchar(45) NOT NULL,
  `State` enum('Offline','Online') NOT NULL,
  `NbPlayers` int(10) unsigned NOT NULL,
  `InternalId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`ShardId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `shard`
--

/*!40000 ALTER TABLE `shard` DISABLE KEYS */;
/*!40000 ALTER TABLE `shard` ENABLE KEYS */;


--
-- Definition of table `user`
--

DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `UId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Login` varchar(45) NOT NULL,
  `Password` varchar(45) NOT NULL,
  `ShardId` int(10) unsigned NOT NULL,
  `State` enum('Offline','Online','Waiting','Authorized') DEFAULT 'Offline',
  `Registered` datetime NOT NULL,
  `Cookie` varchar(45) NOT NULL,
  `Score` int(10) unsigned NOT NULL,
  `UserTexture` varchar(45) NOT NULL,
  `UserColor` varchar(45) NOT NULL,
  `UserTrace` varchar(45) NOT NULL,
  `UserMesh` varchar(45) NOT NULL,
  PRIMARY KEY (`UId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `user`
--

/*!40000 ALTER TABLE `user` DISABLE KEYS */;
/*!40000 ALTER TABLE `user` ENABLE KEYS */;




/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
