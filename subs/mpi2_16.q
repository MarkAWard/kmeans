#!/bin/bash
#PBS -l nodes=1:ppn=16
#PBS -l walltime=5:00:00
#PBS -l mem=20GB
#PBS -N mpi2_16
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi2_times16.txt

./time_mpi2.sh 16 >> output/mpi2_times16.txt
