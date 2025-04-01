#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

// funcion de bcast con arbol binomial para un long long int
void mpi_binomial_bcast_long_long(long long int *value, int root, MPI_Comm comm) {
    int rank, size, d;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    // se asume que el root es 0, para simplificar
    for (d = 0; (1 << d) < size; d++) {
        if (rank < (1 << d)) {
            int dest = rank + (1 << d);
            if (dest < size) {
                MPI_Send(value, 1, MPI_LONG_LONG_INT, dest, 0, comm);
            }
        } else if (rank < (1 << (d + 1))) {
            int source = rank - (1 << d);
            MPI_Recv(value, 1, MPI_LONG_LONG_INT, source, 0, comm, MPI_STATUS_IGNORE);
        }
    }
}

// funcion de reduction en arbol plano (flattree) para un long long int
long long int mpi_flattree_reduce_long_long(long long int local_val, int root, MPI_Comm comm) {
    int rank, size, i;
    long long int total;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    
    if (rank == root) {
        total = local_val;
        long long int tmp;
        // el proceso root recibe de todos los demas procesos
        for (i = 0; i < size; i++) {
            if (i != root) {
                MPI_Recv(&tmp, 1, MPI_LONG_LONG_INT, i, 0, comm, MPI_STATUS_IGNORE);
                total += tmp;
            }
        }
        return total;
    } else {
        // los demas procesos envian su valor al proceso root
        MPI_Send(&local_val, 1, MPI_LONG_LONG_INT, root, 0, comm);
        return 0; // valor no util para los demas
    }
}

int main(int argc, char *argv[]) {
    int rank, size, i;
    long long int n, count = 0;
    double x, y, z, pi, PI25DT = 3.141592653589793238462643;
    
    // inicializacion de mpi
    MPI_Init(&argc, &argv);
    // obtener el id del proceso
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // obtener el numero total de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // el proceso 0 lee la entrada
    if (rank == 0) {
        printf("enter the number of points: (0 quits) \n");
        scanf("%lld", &n);
    }
    
    // distribuir n a todos los procesos usando bcast con arbol binomial
    mpi_binomial_bcast_long_long(&n, 0, MPI_COMM_WORLD);
    
    // si n es 0, se termina la ejecucion
    if (n == 0) {
        MPI_Finalize();
        return 0;
    }
    
    // distribucion de la carga de trabajo:
    // cada proceso calcula los puntos asignados segun su rank, usando un bucle for con incremento de size
    for (i = rank + 1; i <= n; i += size) {
        // generar puntos aleatorios en el intervalo [0,1]
        x = ((double) rand()) / ((double) RAND_MAX);
        y = ((double) rand()) / ((double) RAND_MAX);
        // calcular la distancia al origen
        z = sqrt((x * x) + (y * y));
        // verificar si el punto esta dentro del circulo de radio 1
        if (z <= 1.0)
            count++;
    }
    
    // recolectar los resultados parciales usando reduction flattree
    long long int total_count = mpi_flattree_reduce_long_long(count, 0, MPI_COMM_WORLD);
    
    // el proceso 0 calcula la aproximacion de pi y muestra el resultado
    if (rank == 0) {
        pi = ((double) total_count / n) * 4.0;
        printf("pi is approx. %.16f, error is %.16f\n", pi, fabs(pi - PI25DT));
    }
    
    // finalizar mpi
    MPI_Finalize();
    return 0;
}
