echo $1
cat job.sh > wrapper.sh
echo ./run.sh $1 >> wrapper.sh
qsub -N mpi -m bea -M $USER@iitd.ac.in -P cse -l select=4:ncpus=1:mpiprocs=1 -l place=scatter -l walltime=02:00:00 wrapper.sh
