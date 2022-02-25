#!/bin/sh
### Set the job name (for your reference)
#PBS -N mpi
### Set the project name, your department code by default
#PBS -P cse
### Request email when job begins and ends, don't change anything on the below line
#PBS -m bea
### Specify email address to use for notification, don't change anything on the below line
#PBS -M $USER@iitd.ac.in
#### Request your resources, just change the numbers
#PBS -l select=1:ncpus=1:mpiprocs=1
### Specify "wallclock time" required for this job, hhh:mm:ss
#PBS -l place=scatter
#PBS -l walltime=06:20:00

# After job starts, must goto working directory.
# $PBS_O_WORKDIR is the directory from where the job is fired.
echo "==============================="
echo $PBS_JOBID
cat $PBS_NODEFILE
echo "==============================="
cd $PBS_O_WORKDIR
echo $PBS_O_WORKDIR

#job
./job.sh 1