#!/bin/bash
#PBS -l nodes=1:ppn=20
#PBS -l walltime=15:00:00
#PBS -l mem=20GB
#PBS -N mpi10_20
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi10_times20.txt

./time_mpi10.sh 20 >> output/mpi10_times20.txt
