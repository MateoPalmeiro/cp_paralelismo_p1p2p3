#!/bin/bash
# script para ejecutar un programa mpi (por ejemplo, pi o pi2) con distintos valores de n
# el script usa la variable global EXECUTABLE para definir el nombre del ejecutable,
# y para cada valor de n se imprime:
#   - "ejecutando <EXECUTABLE> con n = <n>"
#   - la salida del programa
#   - "tiempo total (wall-clock, date) = <tiempo_formateado>"
#
# se redirige stderr de mpirun a /dev/null para ocultar mensajes de hwloc.
#
# para usar el script:
#   chmod +x run_pi.sh
#   ./run_pi.sh

# variable global con el nombre del ejecutable (se asume que esta en el directorio actual)
EXECUTABLE="pi2"

# funcion para formatear el tiempo (en ms) de forma automatica:
# si el tiempo es mayor o igual a 1000 ms, se muestra en segundos con 2 decimales;
# si es menor a 1 ms, se muestra en microsegundos;
# de lo contrario, se muestra en ms sin decimales.
format_time() {
    local t_ms=$1
    if (( $(echo "$t_ms >= 1000" | bc -l) )); then
        t_sec=$(echo "scale=2; $t_ms / 1000" | bc -l)
        echo "${t_sec}s"
    elif (( $(echo "$t_ms < 1" | bc -l) )); then
        t_us=$(echo "scale=2; $t_ms * 1000" | bc -l)
        echo "${t_us}µs"
    else
        t_int=$(printf "%.0f" "$t_ms")
        echo "${t_int}ms"
    fi
}

# lista de valores de n a probar
n_values=(1 10 100 1000 10000 100000 1000000 10000000 100000000)

for n in "${n_values[@]}"
do
    # se imprime la linea de ejecucion
    echo "ejecutando ${EXECUTABLE} con n = ${n}"
    
    # medir el tiempo wall-clock usando date (en ms)
    start=$(date +%s%3N)
    
    # ejecutar el programa con mpirun y redirigir stderr a /dev/null para ocultar mensajes
    output=$(mpirun -np 4 ./"${EXECUTABLE}" 2>/dev/null <<< "$n")
    
    end=$(date +%s%3N)
    wall_ms=$(echo "$end - $start" | bc -l)
    wall_formatted=$(format_time "$wall_ms")
    
    # imprimir la salida del programa y el tiempo medido
    echo "$output"
    echo "tiempo total (wall-clock, date) = ${wall_formatted}"
    echo ""
done
