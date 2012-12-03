#!/bin/sh
ROOT_DIR="/home/004/a/ax/axj107420/aos/replicated_fs"
total=`cat $ROOT_DIR/config/common.h | grep TOTAL_SERVERS | cut -f3 -d" "`
#echo "-------  :file:  ---- :stdout:"
echo "Cleaning up..."
#cd $ROOT_DIR && ./scripts/kill.sh
cd $ROOT_DIR && ./scripts/setup.sh
i=1
for line in `cat $ROOT_DIR/config/file_servers.conf`
do
	echo "ssh to $line and run ./server"
	ssh $line "cd $ROOT_DIR; nohup ./server > output/s_$i 2>&1 &"
	if [ $i -eq $total ]; then
		break;
	fi
	i=$(( $i + 1 ))
done
