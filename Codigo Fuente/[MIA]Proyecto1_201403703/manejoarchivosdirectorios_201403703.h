/* 
 * File:   manejoarchivosdirectorios_201403703.h
 * Author: sinozuke
 *
 * Created on 10 de diciembre de 2017, 05:56 PM
 */

#ifndef MANEJOARCHIVOSDIRECTORIOS_201403703_H
#define	MANEJOARCHIVOSDIRECTORIOS_201403703_H


void crearCarpeta(char path[250], char ruta[128]);
int revisarTablaNombres();
int buscarCarpeta();
int buscarEspacioVacio();
void copiarCadena();
int crearTableName();
int buscarPadre();
void buscarCoincidencias();
int  buscarCoincidenciasEnCarpetas();
int armarRuta();
int compararCadenaSubString();
void generarReporteArchivo();
void buscarCoincidenciasArchivo();
void generandoReporteArchivos();
int armarRutaArchivo();
void reporteNodos();
void reporteTablaNombres();
int revisarTablaNombresEliminar();
void eliminarDirectorio();
void copiarCarpeta();
void buscarArchivoCopiar();
int buscarEntreNodosCopiar();
void modificarArchivo();
#endif	/* MANEJOARCHIVOSDIRECTORIOS_201403703_H */

