#!/bin/bash
#PBS -l nodes=1:ppn=4
#PBS -l walltime=15:00:00
#PBS -l mem=30GB
#PBS -N mpi1000_4
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi1000_times4.txt

./time_mpi1000.sh 4 >> output/mpi1000_times4.txt
