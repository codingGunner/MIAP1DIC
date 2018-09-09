/* 
 * File:   manejodisco_201403703.h
 * Author: sinozuke
 *
 * Created on 8 de diciembre de 2017, 04:25 PM
 */

#ifndef MANEJODISCO_201403703_H
#define	MANEJODISCO_201403703_H

#ifdef	__cplusplus
extern "C" {
#endif

    void crearDisco(char nombre_disco[100], char ruta[128], int cantidad_nodos, int tamanio_nodo);
    void crearDirectorios(char path[128]);
    void crear_mas_nodos(char path[128], char nombreMaestro[100]);
    void eliminarDisco();

#ifdef	__cplusplus
}
#endif

#endif	/* MANEJODISCO_201403703_H */

