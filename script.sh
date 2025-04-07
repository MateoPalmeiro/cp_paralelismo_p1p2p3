#!/bin/bash
# Script para ejecutar dos programas MPI (pi y pi2) con distintos valores de n.
# Para cada valor de n se imprime:
#   - "Ejecutando <EXECUTABLE> con n = <n>"
#   - La salida del programa.
#   - "Tiempo total (wall-clock, date) = <tiempo_formateado>"
# Al finalizar ambos, se calcula y muestra la diferencia de tiempos:
#   diferencia = tiempo(pi) - tiempo(pi2)
#
# Se redirige stderr de mpirun a /dev/null para ocultar mensajes de hwloc.
#
# Para usar el script:
#   chmod +x script.sh
#   ./script.sh
# 1 10 100 1000 10000 100000 1000000 10000000 100000000 1000000000 10000000000 100000000000 1000000000000
# Variables globales con los nombres de los ejecutables (se asumen en el directorio actual)
EXECUTABLE1="pi"
EXECUTABLE2="pi2"

# Función para formatear el tiempo (en ms) automáticamente:
# - Si el tiempo es mayor o igual a 1000 ms, se muestra en segundos con 2 decimales.
# - Si es menor a 1 ms, se muestra en microsegundos.
# - En otro caso, se muestra en ms sin decimales.
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

# Lista de valores de n a probar
n_values=(1 10 100 1000 10000 100000 1000000 10000000 100000000 1000000000)

for n in "${n_values[@]}"
do
    echo "=============================="
    echo "Valor de n: ${n}"
    echo "------------------------------"

    # Ejecutar el primer ejecutable (pi)
    echo "Ejecutando ${EXECUTABLE1} con n = ${n}"
    start1=$(date +%s%3N)
    output1=$(mpirun -np 4 ./"${EXECUTABLE1}" 2>/dev/null <<< "$n")
    end1=$(date +%s%3N)
    wall_ms1=$(echo "$end1 - $start1" | bc -l)
    wall_formatted1=$(format_time "$wall_ms1")
    echo "$output1"
    echo "Tiempo total (wall-clock, date) para ${EXECUTABLE1} = ${wall_formatted1}"
    echo ""

    # Ejecutar el segundo ejecutable (pi2)
    echo "Ejecutando ${EXECUTABLE2} con n = ${n}"
    start2=$(date +%s%3N)
    output2=$(mpirun -np 4 ./"${EXECUTABLE2}" 2>/dev/null <<< "$n")
    end2=$(date +%s%3N)
    wall_ms2=$(echo "$end2 - $start2" | bc -l)
    wall_formatted2=$(format_time "$wall_ms2")
    echo "$output2"
    echo "Tiempo total (wall-clock, date) para ${EXECUTABLE2} = ${wall_formatted2}"
    echo ""

    # Calcular la diferencia de tiempo: tiempo(pi) - tiempo(pi2)
    diff_ms=$(echo "$wall_ms1 - $wall_ms2" | bc -l)
    diff_formatted=$(format_time "$diff_ms")
    echo "Diferencia de tiempo (pi - pi2) = ${diff_formatted}"
    echo "=============================="
    echo ""
done
