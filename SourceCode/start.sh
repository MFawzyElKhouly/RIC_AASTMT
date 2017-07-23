#!/bin/bash


NUM_PLAYERS=11
host="localhost"
port=3100 

if [ $# -gt 0 ]
then
host=$1
fi

export LD_LIBRARY_PATH=$LIBS_DIR:$LD_LIBRARY_PATH

opt=" --host=${host} --port ${port} --paramsfile paramfiles/defaultParams.txt "

for ((i=1;i<=$NUM_PLAYERS;i++)); do
    case $i in
	1|2)
	    echo "Running agent No. $i "
		./RICAASTMT $opt --unum $i --type 4  --paramsfile paramfiles/defaultParams_t4.txt &#> /dev/null &
	    ;;
	3|4)
	    echo "Running agent No. $i "
		./RICAASTMT $opt --unum $i --type 4   --paramsfile paramfiles/defaultParams_t4.txt  &#>  /dev/null &
	    ;;
	5|6)
	    echo "Running agent No. $i "
		./RICAASTMT $opt --unum $i --type 2  --paramsfile paramfiles/defaultParams_t2.txt  &#> /dev/null &
	    ;;
	7|8)
	    echo "Running agent No. $i "
		./RICAASTMT  $opt --unum $i --type 0  --paramsfile paramfiles/defaultParams_t0.txt &#> /dev/null &
	    ;;
	*)
	    echo "Running agent No. $i "
		./RICAASTMT $opt --unum $i --type 0  --paramsfile paramfiles/defaultParams_t0.txt &#> /dev/null &
	    ;;
	
    esac
    sleep 1
done
