// b) Escriba un programa conformado por n procesos que se ejecuten en paralelo que
// escriba por pantalla la frase:
// Hola Mundo! soy el proceso <X> de <TOTAL_PROCESOS> corriendo en la
// máquina <nombre de la máquina> IP= <direccion_IP>
// donde:
// ● <X> es el número ID del proceso.
// ● <TOTAL_PROCESOS> es el total de procesos.
// ● <dirección_IP> es la dirección IP de la máquina donde corre el proceso (debe
// ser la IP a través de la cual se accede a Internet, no la localhost).

// Ejecute el programa creado en una computadora y luego en varias computadoras.

// Nota: Un método para saber la IP de la máquina donde corre un proceso es crear un
// socket TCP y conectarlo a un socket que provea algún servicio conocido (por
// ejemplo, el servidor web de la UNCuyo). Luego obtener la IP local a la cual está
// conectado dicho socket. Por último, no olvidar cerrar el socket.
// En el aula abierta se proveen como ejemplos los archivos direccion_IP.cpp y
// direccion_IP.py con programas que permiten obtener la dirección IP en C++ y
// Python. Dichos códigos pueden agregarse como librerías a sus programas.

#include <iostream>
#include <mpi.h>
#include "direccion_IP.cpp"

using namespace std;

int main(int argc, char **argv) {
    int rank, total_procs, hostname_len;
    char hostname[MPI_MAX_PROCESSOR_NAME];  // MPI_MAX_PROCESSOR_NAME =
    char ip_address[40];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Identificador del proceso, se almacena en rank
    MPI_Comm_size(MPI_COMM_WORLD, &total_procs);  // Cantidad total de procesos, se almacena en total_procs
    MPI_Get_processor_name(hostname, &hostname_len);  // Nombre de la máquina donde corre el proceso, se almacena en hostname

    obtener_IP(ip_address);

    MPI_Bcast(ip_address, 40, MPI_CHAR, 0, MPI_COMM_WORLD);  // Se envía la dirección IP a todos los procesos

    cout << "Hola Mundo! Soy el proceso " << rank << " de " << total_procs
         << " corriendo en la máquina " << hostname << " IP = " << ip_address << endl;

    MPI_Finalize();

    return 0;
}
