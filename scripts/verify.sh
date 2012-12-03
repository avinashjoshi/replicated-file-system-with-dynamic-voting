#!/bin/sh
ROOT_DIR="/home/004/a/ax/axj107420/aos/ragam"
clients=`cat $ROOT_DIR/config/commons.h | grep MAX_CLIENTS | cut -f3 -d" "`
servers=`cat $ROOT_DIR/config/commons.h | grep MAX_SERVERS | cut -f3 -d" "`

a=( $( cat $ROOT_DIR/config/client.conf ) )

cd data_centers
echo "== FOLDER: ./data_centers =="
echo "DIFFING various datacenters..."
echo "1. s_0 with s_1"
diff -u s_0 s_1

echo "2. s_0 with s_2"
diff -u s_0 s_2

echo "3. s_1 with s_2"
diff -u s_1 s_2

echo "Number of lines in each data center"
echo ",---------+----+----+----."
echo "|         | DATA CENTERS |"
echo "| CLIENT# +----+----+----|"
echo "|         | s0 | s1 | s2 |"
echo "|---------+----+----+----|"
for ((i=0;i<$clients;i+=1)); do
	#s=`printf "%02d" $i`;
	echo "|  ${a[$i]}  | `cat s_0 | grep "<$i" | wc -l` | `cat s_1 | grep "<$i" | wc -l` | `cat s_2 | grep "<$i" | wc -l` |"
done
echo "\`---------+----+----+----'"
