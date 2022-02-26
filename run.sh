if [ "$2"=="1" ]; then
    mpirun -np $1 ./wtf data/8717_31525/edges.dat 8717 31525 0.1 5000 30 20 369 > $1time1.txt
    echo $PBS_JOBID >> time$2_$1.txt
    cat $1time1.txt >> time$2_$1.txt
    mv converted.txt output_$PBS_JOBID.txt
elif [ "$2"=="2" ]; then
    mpirun -np $1 ./wtf data/81867_545671/edges.dat 81867 545671 0.1 5000 30 20 369 > $1time2.txt
    echo $PBS_JOBID >> time$2_$1.txt
    cat $1time1.txt >> time$2_$1.txt
    mv converted.txt output_$PBS_JOBID.txt
else
    mpirun -np $1 ./wtf data/82168_870161/edges.dat 82168 870161 0.1 5000 30 20 369 > $1time3.txt
    echo $PBS_JOBID >> time$2_$1.txt
    cat $1time1.txt >> time$2_$1.txt
    mv converted.txt output_$PBS_JOBID.txt
fi