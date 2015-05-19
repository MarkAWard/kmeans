#!/bin/bash
#PBS -l nodes=1:ppn=1
#PBS -l walltime=15:00:00
#PBS -l mem=20GB
#PBS -N seq2
#PBS -M $USER@nyu.edu
#PBS -j oe

cd HPC/kmeans/
touch output/seq2_times.txt

./time_seq2.sh >> output/seq2_times.txt
