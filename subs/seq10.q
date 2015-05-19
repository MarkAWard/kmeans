#!/bin/bash
#PBS -l nodes=1:ppn=1
#PBS -l walltime=15:00:00
#PBS -l mem=20GB
#PBS -N seq1
#PBS -M $USER@nyu.edu
#PBS -j oe

cd HPC/kmeans/
touch output/seq10_times.txt

./time_seq10.sh >> output/seq10_times.txt
