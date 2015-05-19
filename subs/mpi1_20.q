#!/bin/bash
#PBS -l nodes=1:ppn=20
#PBS -l walltime=5:00:00
#PBS -l mem=20GB
#PBS -N mpi1_20
#PBS -M $USER@nyu.edu
#PBS -j oe

module load openmpi/intel

cd HPC/kmeans/
touch output/mpi_times20.txt

./time_mpi1.sh 20 >> output/mpi_times20.txt
