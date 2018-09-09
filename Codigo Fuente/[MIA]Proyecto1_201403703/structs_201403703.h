/* 
 * File:   structs_201403703.h
 * Author: sinozuke
 *
 * Created on 8 de diciembre de 2017, 05:20 PM
 */

#ifndef STRUCTS_201403703_H
#define	STRUCTS_201403703_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stdio.h"


typedef struct MBR{
    int mbr_cantidad_nodos; //contiene la cantidad de DataNodes
    char mbr_numero_magico[10];
    int mbr_tabla_nodos; //contiene donde inicia la tabla de nodos
    int mbr_tabla_nombres; //contiene el byte donde inicia la tabla de nombres
} MBR;


typedef struct TABLA_NODOS{
    char nodo_ruta_nodo[128]; // contiene la ruta del archivo del Datanode
    int nodo_tamanio_nodo; // Contiene el tamanio actual del DataNode sin metadatos
    int nodo_tamanio_nodo_m; // Contiene el tamanio actual del DataNode con metadatos.
}TABLA_NODOS;

typedef struct TABLA_NOMBRES{
    char nombre_nombre[20]; //contiene el nombre del archivo o directorio
    int nombre_tipo; // 1 archivo  0 carpeta
    char nombre_fecha_creacion[250]; // fecha de la creacion
    int nombre_bloque_inicial; // numero de la estructura donde inicial el bloque de datos que contiene la informacion del archivo o numero de la estructura de tabla de nombres que almacena la carpeta
    int nombre_datanode; //Numero de DataNode donde se almacena el bloque inicial, nulo si es una carpeta
    int nombre_padre; //numero de la estructura de la tabla de nombres del directorio padre que contiene esta carpeta o archivo
    int nombre_estado; //estado del registro (ocupado o libre)   
}TABLA_NOMBRES;

typedef struct TABLA{
    char nombre_carpeta[20];
    int tabla_padre;
    TABLA_NOMBRES structura_tabla_nombre[9];
}TABLA;

typedef struct BLOQUE_DATO{
    char bd_data[51]; // contiene la informacion del archivo
    int bd_numero; // numero de bloques de datos
    int bd_siguiente; //numero de bloque siguiente que contiene la informacion
    int bd_datanode; //numero de DataNode donde se encuentra localizado el siguiente bloque
    int bd_estado; // estado del bloque (ocupado o libre)
}BLOQUE_DATO;

typedef struct colaPadres{
    char *padre;
    struct colaPadres *siguiente;
}colaPadres;

typedef struct colaDiscos{
    FILE * disco;
    struct colaDiscos * siguiente;
}colaDiscos;
#endif	/* STRUCTS_201403703_H */

