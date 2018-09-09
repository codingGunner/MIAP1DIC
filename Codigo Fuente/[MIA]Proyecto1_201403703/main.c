/* 
 * File:   main.c
 * Author: sinozuke
 *
 * Created on 8 de diciembre de 2017, 04:03 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "manejodisco_201403703.h"
#include "manejoarchivosdirectorios_201403703.h"
#include <time.h>
#include <dirent.h>
/*
 * 
 */
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"
#define RED   "\x1B[31m"
#define GREEN "\033[32m"
#define BOLDCYAN    "\033[1m\033[36m"
 char name_disk[250]; 
 int contador_nodos;
 char nombre_disco[100];
 char ruta[128];
 char direccion_carpeta_[250];
 struct dirent *de =NULL;
 DIR *dr;
 void listarElementos(char path[250]){
    // Pointer for directory entry
 
    // opendir() returns a pointer of DIR type. 
    dr = opendir(path);
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return 0;
    }
    int i=0;
    while ((de = readdir(dr)) != NULL ){
          printf("%i. %s\n",i,de->d_name);
          i++;
    }
    closedir(dr);  
 
 }

 void MenuPrincipal(){
     memset(direccion_carpeta_,'\0',250);
         int salida =0;
         int select = 0;
         int selection=0;
         strcpy(direccion_carpeta_,"/home/sinozuke/Escritorio/CLUSTERS_MIA");
    printf("------------------------------------------------\n");
    printf("               SELECCIONE UNA OPCION \n");
    printf("------------------------------------------------\n"); 
    while(salida==0){
        printf("1. CREAR DISCO \n");
        printf("2. MONTAR DISCO \n");
        printf("3. FINALIZAR APLICACION \n");
        printf("~$ ");
        scanf("%i",&select);
        getchar();
        switch(select){
            case 1:
            CreacionDisco();
            break;           
            case 2:
                 
                  listarElementos(direccion_carpeta_);
                   printf("~$ Escriba el numero del disco que quiere montar: ");
                   scanf("%i",&selection);
                   getchar();
                  dr = opendir(direccion_carpeta_);
                    if (dr == NULL)  // opendir returns NULL if couldn't open directory
                    {
                        printf("Could not open current directory" );
                        return 0;
                    }
                    int i=0;
                    while ((de = readdir(dr)) != NULL ){
                        if(i == selection){
                            strcat(direccion_carpeta_,"/");
                            strcat(direccion_carpeta_, de->d_name);
                            break;
                        }
                          i++;
                    }
                    closedir(dr);  
                  MenuManejoDisco(de->d_name);
                  
                //MenuManejoDisco();
            break;
            case 3:
                printf("**** APLICACION FINALIZADA ****");
                salida =1;
            break;
                       
        }
    }
}
 void CreacionDisco(){
    
    int cantidad_nodos=0;
    int tamanio_nodo=0;
    char ruta_sinDisco[128];
    printf("~$ Ingrese el nombre del disco:  ");
    scanf(" %[^\n]",nombre_disco);
    getchar();
  //  printf("~$ Ingrese la ruta en la que desea crear el disco:  ");
  //  scanf(" %[^\n]",ruta);
  //  getchar();
    strcpy(ruta, "/home/sinozuke/Escritorio/CLUSTERS_MIA/");
    printf("~$ Ingrese la cantidad de Nodos que desea crear (un Minimo de 3):  ");
    scanf("%i",&cantidad_nodos);
    getchar();
    printf("~$ Ingrese el tamanio de los Nodos [MB]:  ");
    scanf("%i",&tamanio_nodo);
    getchar();
    crearDisco(nombre_disco, ruta, cantidad_nodos, tamanio_nodo);
 }
 void MenuManejoDisco(char nombre[250]){
    int salida =0;
    int select = 0;   
    printf(GREEN " ****** DISCO %s MONTADO EXITOSAMENTE SELECCIONE UNA OPCION ****** " RESET,nombre );
    printf("------------------------------------------------\n");
    printf("              MENU DE DISCO \n");
    printf("------------------------------------------------\n"); 
    while(salida==0){
        printf("1. CREAR NODOS DE DATOS \n");
        printf("2. ELIMINAR NODO \n");
        printf("3. MENU DE MANEJO DE CARPETAS \n");
        printf("4. MENU DE MANEJO DE ARCHIVOS \n");
        printf("5. MENU DE REPORTES \n");
        printf("6. DESMONTAR DISCO \n");
        printf("7. REGRESAR \n");
        printf("~$ ");
        scanf("%i",&select);
        getchar();
        switch(select){
            case 1:
                crear_mas_nodos(ruta,nombre_disco);
            break;
            case 2:
                eliminarDisco(direccion_carpeta_);
            break;
            case 3:
                MenuManejoDirectorios();
            break;
            case 4:
                MenuManejoArchivos();
                break;
            case 5:
                MenuReportes();
            break;
            case 6:
                memset(direccion_carpeta_,'\0',250);
                MenuPrincipal();
                break;
            case 7:
                salida = 1;
                MenuPrincipal();
           break;
                       
        }
    }
 
 }
    
 void MenuManejoDirectorios(){
    int salida =0;
    int select = 0;  
    char direccion_carpeta[250];
    char destino[250];
    char objetivo[250];
    printf("------------------------------------------------\n");
    printf("              MENU DE CARPETAS \n");
    printf("------------------------------------------------\n"); 
    while(salida==0){
        printf("1. CREAR CARPETA \n");
        printf("2. ELIMINAR CARPETA \n");
        printf("3. COPIAR CARPETA \n");
        printf("4. BUSCAR CARPETA \n");
        printf("5. REGRESAR \n");
        printf("~$ ");
        scanf("%i",&select);
        getchar();
        switch(select){
            case 1:
                   printf("~$ Escriba la ruta de la carpeta que desea crear: ");
                   scanf(" %[^\n]", direccion_carpeta);
                   getchar();
                   crearCarpeta(direccion_carpeta,direccion_carpeta_);
            break;
            case 2:
                eliminarDirectorio(direccion_carpeta_);
            break;
            case 3:
                 copiarCarpeta(direccion_carpeta_);
            break;
            case 4:
                buscarCoincidencias(direccion_carpeta_);
            break;
            case 5:
                salida  = 1;
            
            break;           
        }
    }
     
     
 
 }
 char nombreArchivo_[20];
  void MenuManejoArchivos(){
    int salida =0;
    int select = 0; 
    char nombre[20];
    char direccion_carpeta[250];
    char contenido[512];
    printf("------------------------------------------------\n");
    printf("              MENU DE ARCHIVOS \n");
    printf("------------------------------------------------\n"); 
    while(salida==0){
        printf("1. CREAR ARCHIVO \n");
        printf("2. ELIMINAR ARCHIVO \n");
        printf("3. EDITAR ARCHIVO \n");
        printf("4. COPIAR ARCHIVO \n");
        printf("5. BUSCAR ARCHIVO \n");
        printf("6. REGRESAR \n");
        printf("~$ ");
        scanf("%i",&select);
        getchar();
        switch(select){
            case 1:
                    printf("Escriba la ruta donde quiere crear el archivo:  ");
                    scanf(" %[^\n]",&direccion_carpeta);
                    getchar();
                    printf("Escriba la informacion que lleve el archivo: \n");
                    scanf(" %[^\n]",&contenido);
                    printf("********presione ENTER para continuar"); 
                    while(getchar()!='\n'); // option TWO to clean stdin
                    getchar();
                    crearArchivo(direccion_carpeta,direccion_carpeta_,contenido);
            break;
            case 2:
                 eliminarDirectorio(direccion_carpeta_);
            break;
            case 3:
                modificarArchivo(direccion_carpeta_);
            break;
            case 4:
             copiarCarpeta(direccion_carpeta_);
            break;
            case 5:
                buscarCoincidenciasArchivo(direccion_carpeta_);
                break;
            case 6:
                salida =1;
            break;           
        }
    }
     
     
 
 }
  void MenuReportes(){
      int salida = 0;
      int select = 0; 
      char path[250];
    printf("------------------------------------------------\n");
    printf("              MENU DE REPORTES \n");
    printf("------------------------------------------------\n"); 
      while(salida==0){
        printf("1. REPORTE DE DATANODE \n");
        printf("2. REPORTE DE ARCHIVOS \n");
        printf("3. REPORTE TABLA DE NOMBRES \n");
        printf("4. REGRESAR \n");
        printf("~$ ");
        scanf("%i",&select);
        getchar();
        switch(select){
            case 1:
                reporteNodos(direccion_carpeta_);
            break;
            case 2:
             generandoReporteArchivos(direccion_carpeta_);
            break;
            case 3:
                reporteTablaNombres(direccion_carpeta_);
            break;
            case 4:
                salida =1;
            break;
            
        }
      }
  
  }
  
  
int main(int argc, char** argv) {
     srand(time(NULL));
    MenuPrincipal();
    return (EXIT_SUCCESS);
}

