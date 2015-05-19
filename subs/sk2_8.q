#!/bin/bash
#PBS -l nodes=1:ppn=8
#PBS -l walltime=15:00:00
#PBS -l mem=20GB
#PBS -N sk2_8
#PBS -M $USER@nyu.edu
#PBS -j oe

module unload python/intel/2.7.9
module load scikit-learn/intel/0.16.1

cd HPC/kmeans/
touch output/sk2_times8.txt

./time_sklearn2.sh 8 >> output/sk2_times8.txt
