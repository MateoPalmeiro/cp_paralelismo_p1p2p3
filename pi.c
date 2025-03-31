#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, n, i, count = 0;
    double x, y, z, pi, PI25DT = 3.141592653589793238462643;
    
    // inicializacion de mpi
    MPI_Init(&argc, &argv);
    // obtener el id del proceso
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // obtener el numero total de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // proceso 0: lectura de la entrada y distribucion del valor n
    if (rank == 0) {
        printf("enter the number of points: (0 quits) \n");
        scanf("%d", &n);
        // enviar n a cada uno de los otros procesos
        for (int dest = 1; dest < size; dest++) {
            MPI_Send(&n, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    } else {
        // los demas procesos reciben n del proceso 0
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // si n es 0, se termina la ejecucion
    if (n == 0) {
        MPI_Finalize();
        return 0;
    }

    /* distribucion de la carga de trabajo:
       cada proceso calcula los puntos asignados segun su rank,
       utilizando un bucle for con incremento de size. */
    for (i = rank + 1; i <= n; i += size) {
        // generar puntos aleatorios en el intervalo [0,1]
        x = ((double) rand()) / ((double) RAND_MAX);
        y = ((double) rand()) / ((double) RAND_MAX);
        // calcular la raiz cuadrada de la suma de los cuadrados
        z = sqrt((x * x) + (y * y));
        // verificar si el punto se encuentra dentro del circulo de radio 1
        if (z <= 1.0)
            count++;
    }

    // recolectar los conteos parciales: el proceso 0 suma los resultados
    if (rank == 0) {
        int total_count = count;
        int temp_count;
        for (int src = 1; src < size; src++) {
            MPI_Recv(&temp_count, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += temp_count;
        }
        // calcular pi usando la proporcion de puntos dentro del circulo
        pi = ((double) total_count / n) * 4.0;
        printf("pi is approx. %.16f, error is %.16f\n", pi, fabs(pi - PI25DT));
    } else {
        // cada proceso envia su cuenta parcial al proceso 0
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    // finalizar mpi
    MPI_Finalize();
    return 0;
}
