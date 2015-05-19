#!/bin/bash
#PBS -l nodes=1:ppn=20
#PBS -l walltime=15:00:00
#PBS -l mem=30GB
#PBS -N mpi1000_20
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi1000_times20.txt

./time_mpi1000.sh 20 >> output/mpi1000_times20.txt
