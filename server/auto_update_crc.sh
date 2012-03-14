#!/bin/bash
if [ -e /home/ace/cvs/mtp-target/user_texture/update_crc_flag.txt ]
then
   echo "crc update";
   wget -O update_crc.log http://mtp-target.dyndns.org/mtp-target/crc.php
   rm -f /home/ace/cvs/mtp-target/user_texture/update_crc_flag.txt
else
   echo "no crc update";
fi
