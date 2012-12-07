#!/bin/sh
ROOT_DIR="/home/004/a/ax/axj107420/aos/replicated_fs"
total=`cat $ROOT_DIR/config/config.h | grep TOTAL_SERVERS | cut -f3 -d" "`
i=1
for line in `cat $ROOT_DIR/config/file_servers.conf`
do
	echo "Killing on - $line"
	ssh -n -f $line "pkill -9 server"
	if [ $i -eq $total ]; then
		break;
	fi
	i=$(( $i + 1 ))
done
