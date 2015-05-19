#!/bin/bash
#PBS -l nodes=1:ppn=12
#PBS -l walltime=15:00:00
#PBS -l mem=30GB
#PBS -N mpi1000_12
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi1000_times12.txt

./time_mpi1000.sh 12 >> output/mpi1000_times12.txt
