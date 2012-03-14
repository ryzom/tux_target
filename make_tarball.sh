#!/bin/sh -

VERSION=1.2.0

TAGVERSION=`echo $VERSION | tr \. _`

DEST=tmp/mtp-target
CDEST=$DEST/client
SDEST=$DEST/server

# create directories
rm -rf tmp
mkdir tmp
mkdir $DEST
mkdir $CDEST
mkdir $SDEST
mkdir $DEST/lib

# copy client files
cp -a client/src/client $CDEST
strip $CDEST/client
cp -a client/data $CDEST
cp -a client/cache $CDEST
cp -a client/mtp_target_default.cfg $CDEST
echo "#!/bin/sh -" >>$CDEST/launch.sh
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:../lib" >>$CDEST/launch.sh
echo "./client" >>$CDEST/launch.sh
chmod 700 $CDEST/launch.sh

# copy server files
cp -a server/src/mtp_target_service $SDEST
strip $SDEST/mtp_target_service
cp -a server/data $SDEST
cp -a server/mtp_target_service_default.cfg $SDEST
echo "#!/bin/sh -" >>$SDEST/launch.sh
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:../lib" >>$SDEST/launch.sh
echo "./mtp_target_service" >>$SDEST/launch.sh
chmod 700 $SDEST/launch.sh

# copy libs
strip $HOME/install/release/lib/*.so
cp -a $HOME/install/release/lib/libnel3d.so.0.0.4 $DEST/lib
cp -a $HOME/install/release/lib/libnel3d.so.0 $DEST/lib
cp -a $HOME/install/release/lib/libnelnet.so.0.0.4 $DEST/lib
cp -a $HOME/install/release/lib/libnelnet.so.0 $DEST/lib
cp -a $HOME/install/release/lib/libnelmisc.so.0.0.4 $DEST/lib
cp -a $HOME/install/release/lib/libnelmisc.so.0 $DEST/lib
cp -a $HOME/install/release/lib/libnel_drv_opengl.so.0.0.0 $DEST/lib
cp -a $HOME/install/release/lib/libnel_drv_opengl.so.0 $DEST/lib

cp /usr/lib/libfreetype.so.6 $DEST/lib
cp /usr/lib/liblua50.so.5.0 $DEST/lib
cp /usr/lib/libxml2.so.2 $DEST/lib
cp /usr/lib/libz.so.1 $DEST/lib
cp /usr/lib/libstlport_gcc.so.4.6 $DEST/lib
cp /usr/lib/libjpeg.so.62 $DEST/lib
cp /lib/libcrypt.so.1 $DEST/lib

cvs -z3 -q tag -F release_$TAGVERSION

# tar and send
rm -f mtp-target-*.tar.bz2
cd tmp
tar jcvf ../mtp-target-$VERSION.tar.bz2 mtp-target
cd ..
ftp ftpperso.free.fr
