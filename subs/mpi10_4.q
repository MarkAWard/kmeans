#!/bin/bash
#PBS -l nodes=1:ppn=4
#PBS -l walltime=15:00:00
#PBS -l mem=20GB
#PBS -N mpi10_4
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi10_times4.txt

./time_mpi10.sh 4 >> output/mpi10_times4.txt
