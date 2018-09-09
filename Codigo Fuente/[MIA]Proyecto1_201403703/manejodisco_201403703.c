#include "manejodisco_201403703.h"
#include "structs_201403703.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"
#define RED   "\x1B[31m"
#define GREEN "\033[32m"
#define BOLDCYAN    "\033[1m\033[36m"
void crearDirectorios(char path[128]){
    const char barra = '/'; //constante
    char * nombre; //guarda la parte final
    if(path[0] == '/'){
        nombre = strrchr(path, barra); //puntero a la parte final donde esta el nombre (no importa si no tiene nombre)
        char cmd[256]; //para el comando
        strcpy(cmd, "mkdir -p '");
        strncat(cmd, path, strlen(path)-strlen(nombre)); //concatena desde el inicio del path hasta la ultima /
        strcat(cmd, "'");
        system(cmd); //ejecuta
    }//fin if
}

void concatenarChar(char cadena1[128], char cadena2[128]){
    int tam_cadena2 = strlen(cadena2);
    int tam_cadena1 = strlen(cadena1);
    int i=tam_cadena1;
    for(i; i < tam_cadena1; i++){
         cadena1[i] = cadena2[tam_cadena2];
         tam_cadena2++;
    }

}
void crearDisco(char nombre_disco[100], char ruta[128], int cantidad_nodos, int tamanio_nodo){
    FILE * disco;
    int tamanio_disco=10;
    char ruta_maestro[128];
    int inicio_tabla_nodos=0;
  //  char ruta_sinDisco[128];
    strcpy(ruta_maestro,ruta);
    int i;
    crearDirectorios(ruta);
    unsigned char buffers[1024];
    for(i=0;i<1024;i++){
      buffers[i]='\0';
    }
    MBR mbr; // aqui se crea el MBR
    mbr.mbr_cantidad_nodos = cantidad_nodos;
    strcpy(mbr.mbr_numero_magico,"201403703");
    mbr.mbr_tabla_nodos = sizeof(mbr);
    mbr.mbr_tabla_nombres = sizeof(mbr)+10*(sizeof(TABLA_NODOS));

    strcat(ruta_maestro,nombre_disco);
    disco = fopen(ruta_maestro,"w+b");
    for(i = 0; i < (tamanio_disco*1024); i++){ //num de iteraciones = tamaño en bytes
        fwrite(buffers, sizeof(buffers), 1, disco);
    }

   fseek(disco, 0, SEEK_SET);//se manda a escribir el mbr al disco
   int tamanio_mbr = sizeof(mbr);   
   fwrite(&mbr, 1, sizeof(mbr), disco);
   int donde_quedo = ftell(disco);
   inicio_tabla_nodos = ftell(disco);
   fseek(disco, ftell(disco), SEEK_SET);
   for(i =0 ; i < 10; i++){
        TABLA_NODOS tn;
        tn.nodo_ruta_nodo[0]='\0';
        tn.nodo_tamanio_nodo = -1;
        tn.nodo_tamanio_nodo_m = -1;
        fwrite(&tn, 1, sizeof(tn), disco);
   }
   int posicion_actual = ftell(disco);
   fseek(disco, inicio_tabla_nodos, SEEK_SET); //posiciona el puntero despues del mbr para escribir sobre el numero de 
   for(i = 0; i < cantidad_nodos; i++){
       char ruta_temporal[128];
      
       char nombre[5];
       nombre[5] = '\0';
       char number[1];
       number[0] = i + '0';
       strcpy(nombre, "Nodo");
       strcat(nombre, number);
       strcpy(ruta_temporal, ruta);
       strcat(ruta_temporal,nombre_disco);
         strncat(ruta_temporal, nombre,5);
       TABLA_NODOS tn;
       fread(&tn, sizeof(tn), 1, disco);
       copiarCadenaRuta(tn.nodo_ruta_nodo,ruta_temporal,50);;
       tn.nodo_tamanio_nodo = tamanio_nodo*1024;
       tn.nodo_tamanio_nodo_m = tamanio_nodo*1024;
       int pos = ftell(disco);
       int retorno = pos - sizeof(tn);
       fseek(disco, retorno, SEEK_SET);
       fwrite(&tn, 1, sizeof(tn), disco);
       
   }
   
   int posicion_inicio_tnombres = posicion_actual;
   fseek(disco, posicion_inicio_tnombres, SEEK_SET);
   //aqui escribira la tabla de nombres en el archivo principal
   for(i = 0; i < 1000; i++){
       TABLA t;
       t.nombre_carpeta[0]='\0';
       int j;
       for(j=0;j < 10; j++){
          TABLA_NOMBRES tname;             
          tname.nombre_bloque_inicial = -1;
          tname.nombre_datanode = -1;
          tname.nombre_estado = -1;
          tname.nombre_padre = -1;
          tname.nombre_tipo = -1;   
          t.structura_tabla_nombre[j] = tname;
       }
       fwrite(&t, 1, sizeof(t), disco);
   }
   fclose(disco);
   //aqui abajo se crean los dataNodes dependiendo del tamanio que el usuario haya ingresado
   for(i = 0; i < cantidad_nodos; i++){
       char ruta_temporal[128];
       char nombre[5];

       nombre[5] = '\0';
       char number[1];
       number[0] = i + '0';
       strcpy(nombre, "Nodo");
       strcat(nombre, number);
       
       strcpy(ruta_temporal, ruta);
       strcat(ruta_temporal,nombre_disco);
  
       strncat(ruta_temporal, nombre,5);
       FILE * disco_nodo;       
       disco_nodo = fopen(ruta_temporal,"w+b");
       int j;
       for(j = 0; j < (tamanio_nodo*1024); j++){ //num de iteraciones = tamaño en bytes
            fwrite(buffers, sizeof(buffers), 1, disco_nodo);
       }
       fseek(disco_nodo, 0, SEEK_SET);
       int numero_bd = ((tamanio_nodo*1024) / sizeof(BLOQUE_DATO));
       int x;
       for(x = 0 ; x < numero_bd; x++){
            BLOQUE_DATO bd;
            bd.bd_datanode = -1;
            bd.bd_estado = 0; // 0 libre 1 ocupado
            bd.bd_numero  = -1;
            bd.bd_siguiente = numero_bd;
            fwrite(&bd, 1, sizeof(bd), disco_nodo);
       }
       
       fclose(disco_nodo);
   }
   printf(GREEN "****SE HA CREADO UN NUEVO DISCO EN %s*****\n" RESET, ruta);

}

void crear_mas_nodos(char path[128], char nombreMaestro[100]){
    MBR mbr;
    FILE *disco_maestro;
    char archivo_maestro[128];
    strcpy(archivo_maestro, path);
    strcat(archivo_maestro, nombreMaestro);
    disco_maestro = fopen(archivo_maestro,"r+b");
    fread(&mbr, sizeof(mbr), 1, disco_maestro);
    int cantidad_nodos;
    int cantidad_actual_nodos = mbr.mbr_cantidad_nodos;
    int tamanio_nodo;
    printf("~$ Ingrese la cantidad de Nodos que desea crear:  ");
    scanf("%i",&cantidad_nodos);
    getchar();
    printf("~$ Ingrese el tamanio de los Nodos [MB]:  ");
    scanf("%i",&tamanio_nodo);
    getchar();
    //variables para ciclos
    int i;
   //aqui abajo se crean los dataNodes dependiendo del tamanio que el usuario haya ingresado
   for(i = 0; i < cantidad_nodos; i++){
       char ruta_temporal[128];
        ruta_temporal[128]='\0';
       char nombre[5];
       nombre[5] = '\0';
       char number[1];
       number[0] = cantidad_actual_nodos + '0';
       cantidad_actual_nodos++;
       strcpy(nombre, "Nodo");
       strcat(nombre, number);
       memset(ruta_temporal, '\0',128);
       strcpy(ruta_temporal, path);
       strcat(ruta_temporal,nombreMaestro);
       strncat(ruta_temporal, nombre,5);
       strncat(ruta_temporal, nombre,5);
       unsigned char buffers[1024];
       for(i=0;i<1024;i++){
           buffers[i]='\0';
       }
       FILE * disco_nodo;       
       disco_nodo = fopen(ruta_temporal,"w+b");
       int j;
       for(j = 0; j < (tamanio_nodo*1024); j++){ //num de iteraciones = tamaño en bytes
            fwrite(buffers, sizeof(buffers), 1, disco_nodo);
       }
       fseek(disco_nodo, 0, SEEK_SET);
       int numero_bd = ((tamanio_nodo*1024) / sizeof(BLOQUE_DATO));
       int x;
       for(x = 0 ; x < numero_bd; x++){
            BLOQUE_DATO bd;
            bd.bd_datanode = -1;
            bd.bd_estado = 0; // 0 libre 1 ocupado
            bd.bd_numero  = -1;
            bd.bd_siguiente = numero_bd;
            fwrite(&bd, 1, sizeof(bd), disco_nodo);
       }
       
       fclose(disco_nodo);
   }
     mbr.mbr_cantidad_nodos = mbr.mbr_cantidad_nodos + cantidad_nodos;
    fseek(disco_maestro, 0, SEEK_SET);
    fwrite(&mbr,1, sizeof(mbr), disco_maestro);
    fclose(disco_maestro);
   printf("*-------------se han creado correctamente la cantidad de %i nodos de datos---------------*\n",cantidad_nodos);

}
void eliminarDisco(char dire[250]){
    int validacion=0;

        printf("~$ Esta seguro que quiere eliminar el Nodo? \n ~$ 1: si 2: no \n");
        scanf("%d",&validacion);
        getchar();


        if(validacion==1){
        int ret = remove(dire);

            if(ret == 0)
            {
                printf("*-------------se ha eliminado el nodo-----------------* \n");
            }
            else
            {
                printf("*---------------Error: no se pudo eliminar --------------------*\n");
            }
         }else{
                printf("**-------------------se ha cancelado la eliminacion de un NODO----------------*");
         }
}
void copiarCadenaRuta(char cadena1[125], char cadena2[125], int size){
    
    int i;
    for(i=0;i<size;i++){
        if(cadena2[i] == '\0'){
            cadena1[i] = '\0';
            break;
        }
        cadena1[i] = cadena2[i];
        
    }
    for(i;i<size;i++){
      cadena1[i] = '\0';
    }
    

}
