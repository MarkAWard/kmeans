#!/bin/bash
#PBS -l nodes=1:ppn=8
#PBS -l walltime=15:00:00
#PBS -l mem=20GB
#PBS -N mpi10_8
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi10_times8.txt

./time_mpi10.sh 8 >> output/mpi10_times8.txt
