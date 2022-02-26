echo $1
cat job.sh > wrapper.sh
echo ./run.sh $1 $2 >> wrapper.sh
qsub -N mpi$1_$2 -m bea -M $USER@iitd.ac.in -P cse -l select=$1:ncpus=1:mpiprocs=1 -l walltime=08:00:00 wrapper.sh
#qsub -N mpi$1 -m bea -M $USER@iitd.ac.in -P cse -l select=$1:ncpus=1:mpiprocs=1 -l place=scatter -l walltime=02:00:00 wrapper.sh
