#!/bin/bash
#PBS -l nodes=1:ppn=20
#PBS -l walltime=5:00:00
#PBS -l mem=20GB
#PBS -N mpi2_20
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi2_times20.txt

./time_mpi2.sh 20 >> output/mpi2_times20.txt
