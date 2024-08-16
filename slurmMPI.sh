#!/bin/bash -l

#SBATCH -D ./
#SBATCH --export=ALL
#SBATCH -p course


module load mpi/intel-mpi/2019u5/bin
module load compilers/intel/2019u5



procs=${SLURM_NTASKS}

cores=${SLURM_CPUS_PER_TASK}

export OMP_NUM_THREADS=$cores

mpiicc -qopenmp z -std=c99 -lm


# Loop for executing the program 10 times
 for i in {1..10}
 do
    echo "Execution $i"
    mpirun -np $procs ./a.out
  done

