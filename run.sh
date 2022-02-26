if [ "$2"=="1" ]; then
    mpirun -np $1 ./wtf data/8717_31525/edges.dat 8717 31525 0.1 5000 30 20 369 > $1time1.txt
    echo break >> $1time1.txt
    mpirun -np $1 ./wtf data/8717_31525/edges.dat 8717 31525 0.1 5000 30 20 369 >> $1time1.txt
    echo break >> $1time1.txt
    mpirun -np $1 ./wtf data/8717_31525/edges.dat 8717 31525 0.1 5000 30 20 369 >> $1time1.txt
elif [ "$2"=="2" ]; then
    mpirun -np $1 ./wtf data/81867_545671/edges.dat 81867 545671 0.1 5000 30 20 369 > $1time2.txt
    echo break >> $1time2.txt
    mpirun -np $1 ./wtf data/81867_545671/edges.dat 81867 545671 0.1 5000 30 20 369 >> $1time2.txt
    echo break >> $1time2.txt
    mpirun -np $1 ./wtf data/81867_545671/edges.dat 81867 545671 0.1 5000 30 20 369 >> $1time2.txt
else
    mpirun -np $1 ./wtf data/82168_870161/edges.dat 82168 870161 0.1 5000 30 20 369 > $1time3.txt
    echo break >> $1time3.txt
    mpirun -np $1 ./wtf data/82168_870161/edges.dat 82168 870161 0.1 5000 30 20 369 >> $1time3.txt
    echo break >> $1time3.txt
    mpirun -np $1 ./wtf data/82168_870161/edges.dat 82168 870161 0.1 5000 30 20 369 >> $1time3.txt
fi