#!/bin/bash
#PBS -l nodes=1:ppn=16
#PBS -l walltime=15:00:00
#PBS -l mem=30GB
#PBS -N mpi100_16
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi100_times16.txt

./time_mpi100.sh 16 >> output/mpi100_times16.txt
