#include "manejoarchivosdirectorios_201403703.h"
#include "structs_201403703.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"
#define RED   "\x1B[31m"
#define GREEN "\033[32m"
#define BOLDCYAN    "\033[1m\033[36m"
struct colaPadres *inicio = NULL;
struct colaPadres *fin = NULL;

struct colaDiscos *inicio_discos = NULL;
struct colaDiscos *fin_discos = NULL;
struct colaPadres *root = NULL;


void copiarCadena(char cadena1[25], char cadena2[25], int size){
    
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


void insertarCola(colaPadres *nuevo){
    if(inicio==NULL){
        inicio = nuevo;
        fin = nuevo;
    }else{
        fin->siguiente = nuevo;
        fin = nuevo;
        fin->siguiente = NULL;
    
    }

}

void insertarColaDiscos(colaDiscos *nuevo){
    if(inicio_discos==NULL){
        inicio_discos = nuevo;
        fin_discos = nuevo;
    }else{
        fin_discos->siguiente = nuevo;
        fin_discos = nuevo;
        fin_discos->siguiente = NULL;
    
    }

}
colaPadres *pop(){
    colaPadres *tempo = inicio;
    if(inicio != NULL){
      inicio = inicio->siguiente; 
      return tempo;
    }else{
      return NULL;
    }
    
}

colaDiscos * pop_disco(){
    colaDiscos *tempo = inicio_discos;
    if(inicio_discos != NULL){
      inicio_discos = inicio_discos->siguiente; 
      return tempo;
    }else{
      return NULL;
    }
    
}
colaPadres *peek(){
    return inicio;
}
void crearCarpeta(char path[250], char ruta[128]){
    char padre[25];
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path, "/");
            while (token != NULL){
                //printf("The token is:  %s\n", token);
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();
    FILE * disco;
    disco = fopen(ruta, "r+b");
    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS); 
    int temanio_ = sizeof(TABLA_NODOS);
   int creacion =  revisarTablaNombres(temporal, inicio_tablaNombres, disco, inicio_tablaNombres, 0);
    if(creacion == 1){
        printf("**** CARPETA CREADA EXITOSAMENTE **** \n");
    }else{
        printf( RED "**** ALGUN ERROR EN LA CREACION DE CARPETAS **** \n" RESET);
    }
   fclose(disco);
}

int revisarTablaNombres(colaPadres* temporal, int posicion, FILE * disco, int inicial, int tablaPadre){
//revisar siempre la primer parte de la tabla 
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    fseek(disco, posicion, SEEK_SET);
    TABLA t;
    fread(&t,sizeof(t),1,disco);
    int i;
   if(strcmp(temporal->padre, "/")==0){
       copiarCadena(t.nombre_carpeta, temporal->padre,19);
   }
    colaPadres * father = temporal;
       temporal = pop();
       
        if(temporal != NULL){
           TABLA_NOMBRES tn;  
           int found = buscarCarpeta(t, temporal->padre);
           if(found == -1){ // esto quiere decir que la carpeta no se encontro entonces no existe
               int posicionInsertar = buscarEspacioVacio(t);
                 tn.nombre_estado = 1;
                 strcpy(tn.nombre_nombre, temporal->padre);
                 tn.nombre_tipo = 0;
                 tn.nombre_padre= tablaPadre;
                 strcpy(tn.nombre_fecha_creacion,asctime(timeinfo));
                 t.structura_tabla_nombre[posicionInsertar] = tn;
                 fseek(disco, posicion, SEEK_SET);
                 fwrite(&t, 1, sizeof(t), disco);
                 crearTableName(posicion,disco, temporal->padre, tablaPadre);               
                 return 1; //carpeta creada
             
           }else{ //aqui quiere decir que si existe la carpeta se procede a verificar si hay mas direcciones que revisar o  notificar que ya existe
               if(temporal->siguiente == NULL){
                    printf(RED "*** LA CARPETA YA EXISTE *** \n" RESET);
               return -1;
               }else{
                   TABLA_NOMBRES tnomb;
                   tnomb = t.structura_tabla_nombre[found];
                   int padre = tnomb.nombre_padre;
                    int ppadre= buscarPadre(inicial,disco,temporal->padre,padre); // este devueleve un numero pequeno de numero de table name
                    int post = inicial + (sizeof(t)*ppadre);
                    int c =   revisarTablaNombres(temporal, post, disco, inicial, ppadre);
                    if(c == 1){
                        return 1;
                    }else{
                        return -1;
                    }
                
               }            
           }
      
        }

     
    return 0;
}

int buscarCarpeta(TABLA tnuevo, char nombre[20])
{
    int i;
    for(i = 0; i < 10; i++){
        if(strcmp(tnuevo.structura_tabla_nombre[i].nombre_nombre, nombre)==0){
            return i;
        }
    }
    return -1;
}

int buscarEspacioVacio(TABLA tnuevo){
    int i;
    for(i = 0; i < 10; i++){
        if(tnuevo.structura_tabla_nombre[i].nombre_estado == -1){
            return i;
        }
    }
}

int crearTableName(int posicion, FILE * disco, char nombre[20], int tabla_padre){
    int s=0;
    while(s==0){
        fseek(disco, posicion, SEEK_SET);
        TABLA t;
        fread(&t, 1, sizeof(t), disco);
        if(t.nombre_carpeta[0] == '\0'){
            copiarCadena(t.nombre_carpeta, nombre, 19);
            t.tabla_padre = tabla_padre;
            fseek(disco, posicion, SEEK_SET);
            fwrite(&t, 1, sizeof(t), disco);
            s = 1;
        }else{
            posicion = posicion + sizeof(t);
        }
        
    }
    return 1;
}



int buscarPadre(int posicion, FILE * disco,char nombre[20], int padre){
    int contadorPadres=0;
    int s=0;
    while(s==0){
        fseek(disco, posicion, SEEK_SET);
        TABLA t;
        TABLA_NOMBRES tnombre;
        fread(&t, 1, sizeof(t), disco);
        if(strcmp(t.nombre_carpeta,nombre) == 0 && t.tabla_padre == padre){
            return contadorPadres;
           // s = 1;
        }else{
            contadorPadres++;
            posicion = posicion + sizeof(t);
        }
        
    }
    return contadorPadres;

}


///////////********************AQUI ABAJO SE MANEJA LA BUSQUEDA DE CARPETAS
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

char rutaBusqueda[250];


void buscarCoincidencias(char path[250]){
    char nombre[30];
    char ptr[100];
    printf("Ingrese el nombre de la carpeta que desea buscar:  ");
    scanf(" %[^\n]",nombre);
    getchar();
    
     FILE * disco;
    disco = fopen(path, "r+b");
    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS);
     printf("*** COINCIDENCIAS PARA LA CARPETA %s *** \n", nombre);
    int resp = buscarCoincidenciasEnCarpetas(nombre, inicio_tablaNombres, disco, inicio_tablaNombres, 0 );

}


int  buscarCoincidenciasEnCarpetas(char nombre[30], int posicion, FILE * disco, int inicial, int tablaPadre){

    int s=0;
    while(s < 50){
        fseek(disco, posicion, SEEK_SET);
        TABLA t;
        fread(&t, 1, sizeof(t), disco);
        if(compararCadenaSubString(t.nombre_carpeta, nombre) == 0){
            armarRuta(t.tabla_padre, inicial, disco);
            strcat(rutaBusqueda,"/");
            strcat(rutaBusqueda, t.nombre_carpeta);
            printf(GREEN "%s\n" RESET, rutaBusqueda);
            memset(rutaBusqueda, '\0',250);
            posicion = posicion + sizeof(t);
            //return 1;
        }else{
           posicion = posicion + sizeof(t);
        }
        s++;
    }
    return 0;

}

int armarRuta(int tablaCarpeta, int posicion,FILE * disco){

 int post = posicion + (sizeof(TABLA)*tablaCarpeta);
 fseek(disco, post, SEEK_SET);
 TABLA t;
 fread(&t, 1, sizeof(t), disco);
 if(strcmp(t.nombre_carpeta,"/") == 0){
     return 1; //llego satifactoriamente a la raiz
 }else{
     armarRuta(t.tabla_padre, posicion, disco);
     strcat(rutaBusqueda,"/");
     strcat(rutaBusqueda, t.nombre_carpeta);
 }
 return 0; // algun error al armar la ruta
}

int compararCadenaSubString(char cadena1[20], char cadena2[20]){
    int i=0;
    int v=0;
    int posicion =0;
    while(i<strlen(cadena1)){
        int a = (int)cadena1[i];
        int b = (int)cadena2[posicion];
        if(a == b || a-b == 32 || b-a == 32){
            i++;
            posicion++;
            v = 0;
            if(posicion == strlen(cadena2)){
                v =0 ;
                break;
            }
        }else{
            i++;
            posicion = 0;
            v =-1;
        }
        //i++;
    }
    if(i == strlen(cadena1) && posicion < strlen(cadena2)){
        v= -1;
    }
    return v;
}
////////////////////////////// AQUI ABAJO SE MANEJAN LOS ARCHIVOS ////////////////////////////////////////////////
////////////////////////////// AQUI ABAJO SE MANEJAN LOS ARCHIVOS ////////////////////////////////////////////////
////////////////////////////// AQUI ABAJO SE MANEJAN LOS ARCHIVOS ////////////////////////////////////////////////
void crearArchivo(char path[250], char ruta[128], char contenido[512]){
    char padre[25];
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path, "/");
            while (token != NULL){
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();
    FILE * disco;
    disco = fopen(ruta, "r+b");
    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS); 
    int temanio_ = sizeof(TABLA_NODOS);
   int creacion =  revisarTablaNombresArchivos(temporal, inicio_tablaNombres, disco, inicio_tablaNombres, 0, contenido);
    if(creacion == 1){
        printf(GREEN "**** ARCHIVO CREADO EXITOSAMENTE **** \n" RESET);
    }else{
        printf( RED "**** ALGUN ERROR EN LA CREACION DE ARCHIVOS **** \n" RESET);
    }
   fclose(disco);
}

int revisarTablaNombresArchivos(colaPadres* temporal, int posicion, FILE * disco, int inicial, int tablaPadre,char contenido[512]){
    //revisa el mbr para ver cuantos nodos hay
    
    MBR mbr;
    fseek(disco, 0, SEEK_SET);
    fread(&mbr, sizeof(mbr),1,disco);
    int cantidadNodos = mbr.mbr_cantidad_nodos;
    
    //esto calcula el tiempo actual
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    fseek(disco, posicion, SEEK_SET);
    TABLA t;
    fread(&t,sizeof(t),1,disco);
    int i;
   if(strcmp(temporal->padre, "/")==0){
       copiarCadena(t.nombre_carpeta, temporal->padre,19);
   }
    colaPadres * father = temporal;
       temporal = pop();
       
        if(temporal != NULL){
           TABLA_NOMBRES tn;  
           int found = buscarCarpeta(t, temporal->padre);
           if(found == -1){ // esto quiere decir que la carpeta no se encontro entonces no existe
               int posicionInsertar = buscarEspacioVacio(t);
                 tn.nombre_estado = 1;
                 strcpy(tn.nombre_nombre, temporal->padre);
                 tn.nombre_tipo = 1;
                 tn.nombre_padre= tablaPadre;
                 int r = rand() % cantidadNodos;
                 tn.nombre_datanode = r;                
                 int numeroBloque = llenarNodoInformacion(-1,-1,r, contenido, disco, mbr );
                 tn.nombre_bloque_inicial = numeroBloque;
                 strcpy(tn.nombre_fecha_creacion,asctime(timeinfo));
                 t.structura_tabla_nombre[posicionInsertar] = tn;
                 fseek(disco, posicion, SEEK_SET);
                 fwrite(&t, 1, sizeof(t), disco);
                // crearTableName(posicion,disco, temporal->padre, tablaPadre);               
                 return 1; //carpeta creada
             
           }else{ //aqui quiere decir que si existe la carpeta se procede a verificar si hay mas direcciones que revisar o  notificar que ya existe
               if(temporal->siguiente == NULL){
                   printf(RED "*** EL ARCHIVO YA EXISTE *** \n" RESET);
                   return -1;
               }else{
                   TABLA_NOMBRES tnomb;
                   tnomb = t.structura_tabla_nombre[found];
                   int padre = tnomb.nombre_padre;
                int ppadre= buscarPadre(inicial,disco,temporal->padre, padre); // este devueleve un numero pequeno de numero de table name
                 //  temporal = pop();
                int post = inicial + (sizeof(t)*ppadre);
                int c =   revisarTablaNombresArchivos(temporal, post, disco, inicial, ppadre,contenido);
                if(c == 1){
                    return 1;
                }else{
                    return -1;
                }
                
               }            
           }
      
        }

    
    
}

int llenarNodoInformacion(int pos_b_anterior,int n_anterior,int numeroNodo, char contenido[512],FILE * disco, MBR mbr){
    TABLA_NODOS tn;
    int tabla_nodos = mbr.mbr_tabla_nodos + (sizeof(tn)*numeroNodo);
    fseek(disco, tabla_nodos, SEEK_SET);
    fread(&tn, sizeof(tn),1,disco);
    FILE * disco_nodo;
    disco_nodo = fopen(tn.nodo_ruta_nodo,"r+b");
    
    int s=0;
    int pos =0;
    int contadorBloques =0;
    char contenido_corto[50];
    while(s==0){
        BLOQUE_DATO bd;
        fseek(disco_nodo, pos, SEEK_SET);
        fread(&bd, sizeof(bd), 1, disco_nodo);
        
        if(bd.bd_estado == 0){ //quiere decir que el bloque esta vacio
            if(strlen(contenido) < 50){
                bd.bd_estado = 1; //el bloque del nodo se ocupa
                strncpy(bd.bd_data, contenido, 50); //copia los datos al bloque
                bd.bd_datanode = -1;
                bd.bd_siguiente = -1;
                bd.bd_numero = 1;
                fseek(disco_nodo, pos, SEEK_SET);
                fwrite(&bd, 1, sizeof(bd), disco_nodo);
                s = 1;
                fclose(disco_nodo);
                return contadorBloques; //informacion creada
              
                
            }else{
                bd.bd_estado =  1;
                strncpy(contenido_corto, contenido, 50);
                strcpy(bd.bd_data, contenido_corto);
                bd.bd_data[50]='\0';
                bd.bd_numero = 1;
                int sum = mbr.mbr_cantidad_nodos;
                int r = rand() % sum;
                bd.bd_datanode = r;
                quitarCaracteresChar(contenido);
                fseek(disco_nodo, pos, SEEK_SET);
                fwrite(&bd, 1, sizeof(bd), disco_nodo);
                fclose(disco_nodo);
                int siguienteposicion = llenarNodoInformacion(pos,numeroNodo,r, contenido, disco, mbr);
                
              FILE *  disco_nodos = fopen(tn.nodo_ruta_nodo, "r+b");
                fseek(disco_nodos, pos, SEEK_SET);
                BLOQUE_DATO bdn;
                fread(&bdn, sizeof(bdn),1, disco_nodos);
                bdn.bd_siguiente = siguienteposicion;
                fseek(disco_nodos, pos,SEEK_SET);
                fwrite(&bdn, 1, sizeof(bdn), disco_nodo);
                fclose(disco_nodos);
                return contadorBloques;

                break;               
                //agregar siguiente nodo
                //agregar siguiente numero de bloque
                 
            }
         
        }else{
            pos = pos + sizeof(bd);
            contadorBloques++;
        }
    
    
    }
  // fclose(disco_nodo);

}

void quitarCaracteresChar(char contenido[512]){
    int i;
    int pos=0;
    char auxiliar[512];
    memset(auxiliar,'\0',512);
    for(i=50; i < strlen(contenido); i++){
        auxiliar[pos] = contenido[i];
        pos++;
    }
    memset(contenido,'\0',512);
    strcpy(contenido, auxiliar);
    int b;
    int a = 1;
    b= a;



}





void generarReporteArchivo(char path[120], FILE * disco, FILE * reporteArchivo){
    
     char padre[25];
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path, "/");
            while (token != NULL){
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();
    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS); 
    int temanio_ = sizeof(TABLA_NODOS);
   int revision =  revisarTablaNombresArchivosNoCrear(temporal, inicio_tablaNombres, disco, inicio_tablaNombres, 0, reporteArchivo);
   if(revision == -1){
       printf(RED " ERROR AL BUSCAR ARCHIVO \n " RESET);
   }

}

int revisarTablaNombresArchivosNoCrear(colaPadres* temporal, int posicion, FILE * disco, int inicial, int tablaPadre, FILE * reporteArchivo){

    MBR mbr;
    fseek(disco, 0, SEEK_SET);
    fread(&mbr, sizeof(mbr),1,disco);
    int cantidadNodos = mbr.mbr_cantidad_nodos +1;
    
    //esto calcula el tiempo actual
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    
    fseek(disco, posicion, SEEK_SET);
    TABLA t;
    fread(&t,sizeof(t),1,disco);
    int i;
   if(strcmp(temporal->padre, "/")==0){
       copiarCadena(t.nombre_carpeta, temporal->padre,19);
   }
    colaPadres * father = temporal;
       temporal = pop();
       
        if(temporal != NULL){
           TABLA_NOMBRES tn;  
           int found = buscarCarpeta(t, temporal->padre);
           if(found == -1){ // esto quiere decir que la carpeta no se encontro entonces no existe
                    //el archivo no existe
                 return -1; //carpeta creada
             
           }else{ //aqui quiere decir que si existe la carpeta se procede a verificar si hay mas direcciones que revisar o  notificar que ya existe
               if(temporal->siguiente == NULL){
                  tn = t.structura_tabla_nombre[found];
                  if(tn.nombre_tipo == 1){
                      int bloqueinicialarchivo = tn.nombre_bloque_inicial;
                      int numeronodo = tn.nombre_datanode;
                      buscarArchivo(numeronodo,bloqueinicialarchivo, disco, reporteArchivo);
                  
                  }
                   
                   return 1;
               }else{
                    TABLA_NOMBRES tnomb;
                   tnomb = t.structura_tabla_nombre[found];
                   int padre = tnomb.nombre_padre;
                int ppadre= buscarPadre(inicial,disco,temporal->padre, padre); // este devueleve un numero pequeno de numero de table name
                int post = inicial + (sizeof(t)*ppadre);
                int c =   revisarTablaNombresArchivosNoCrear(temporal, post, disco, inicial, ppadre,reporteArchivo);
                if(c == 1){
                    return 1;
                }else{
                    return -1;
                }
                
               }            
           }
      
        }


}






void buscarArchivo(int numeroNodo, int numeroBloque, FILE * disco, FILE * reporteArchivo){
    

    int pos_t_nodo = sizeof(MBR)+ (sizeof(TABLA_NODOS)*numeroNodo);
    fseek(disco, pos_t_nodo, SEEK_SET);
    TABLA_NODOS tnodo;
    fread(&tnodo, sizeof(tnodo), 1, disco);
    FILE * disco_nodo;
    disco_nodo = fopen(tnodo.nodo_ruta_nodo, "r+b");
    buscarEntreNodos(numeroBloque, disco_nodo, reporteArchivo, numeroNodo, disco);
    fclose(disco_nodo);


}

int  buscarEntreNodos(int numeroBloque, FILE * disco_nodo, FILE * reporteArchivo, int numeroNodo, FILE * disco){
    char nombreNodo[5];
    nombreNodo[0] = '\0';
    BLOQUE_DATO bd;
    int pos_b_inicial = (numeroBloque*(sizeof(bd)));
    fseek(disco_nodo, pos_b_inicial, SEEK_SET);
    fread(&bd, sizeof(bd), 1, disco_nodo);
    if(bd.bd_siguiente != -1){// si siguiente es diferente de -1 es porque hay mas donde buscar
        strcpy(nombreNodo, "Nodo");
        nombreNodo[4] = numeroNodo + '0';
        fwrite(nombreNodo,sizeof(nombreNodo),1,reporteArchivo);
        fwrite(":",sizeof(char),1,reporteArchivo);
        fputs(bd.bd_data,reporteArchivo);
        fwrite("\n", sizeof(char),1,reporteArchivo);
        int numerobloquesiguiente = bd.bd_siguiente;
        int numeronodosiguiente = bd.bd_datanode;
        if(numeronodosiguiente != numeroNodo){
            TABLA_NODOS tn;
            FILE * nodo_siguiente;
            int pos_s_nodo = sizeof(MBR)+ (sizeof(TABLA_NODOS)*numeronodosiguiente);
            fseek(disco, pos_s_nodo, SEEK_SET);
            fread(&tn, sizeof(tn), 1, disco);
            nodo_siguiente = fopen(tn.nodo_ruta_nodo, "r+b");
            buscarEntreNodos(numerobloquesiguiente, nodo_siguiente, reporteArchivo,numeronodosiguiente, disco);
           
            
        }else{
             buscarEntreNodos(numerobloquesiguiente, disco_nodo, reporteArchivo,numeronodosiguiente, disco);
        
        }
        
        return 1; //hay un cambio
    }else { //es el ultimo nodo
        strcpy(nombreNodo, "Nodo");
        nombreNodo[4] = numeroNodo + '0';
        fwrite(nombreNodo,sizeof(nombreNodo),1,reporteArchivo);
        fwrite(":",sizeof(char),1,reporteArchivo);
        fputs(bd.bd_data,reporteArchivo);
        fwrite("\n", sizeof(char),1,reporteArchivo);
        return 1; //llego hasta el final
    }
    //aqui se imprime el archivo 
    return -1;
}


void generandoReporteArchivos(char r[128]){
    char en1[] = "------------------------------------ \n";
    char en2[] = "         REPORTE DE ARCHIVO  \n";
    char en3[] = "------------------------------------ \n";
    FILE * reporteArchivo;
    reporteArchivo = fopen("/home/sinozuke/Escritorio/reporteArchivos.txt","w+b");
    fputs(en1,reporteArchivo);
    fputs(en2,reporteArchivo);
    fputs(en3,reporteArchivo);
    
    FILE * disco;
    disco = fopen(r, "r+b");
    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS);
       
    buscarCarpetaParaReporte(inicio_tablaNombres, disco, inicio_tablaNombres, 0, reporteArchivo);
    fclose(disco);
    fclose(reporteArchivo);
    printf(GREEN "**** REPORTE GENERADO  **** \n" RESET);
    if(!fork())
    execlp("gedit", "gedit", "/home/sinozuke/Escritorio/reporteArchivos.txt", NULL);

}
char rutaBusquedaArchivo[120];
char auxiliaruta[120];
void buscarCarpetaParaReporte(int posicion, FILE * disco, int inicial, int tablaPadre, FILE * reporteArchivo){
   int s=0;
    while(s < 100){
        fseek(disco, posicion, SEEK_SET);
        TABLA t;
        fread(&t,sizeof(t),1, disco);
      //  int respuesta = buscarEnCarpeta(t, nombre);
       // if(respuesta != -1){
    int i;
    for(i=0; i<10; i++){
        TABLA_NOMBRES tn;
        tn= t.structura_tabla_nombre[i];
            if(tn.nombre_tipo == 1){
                // return i;    // si se encontro y si coincide , devuelve la posicion   
                armarRutaArchivo(t.tabla_padre, inicial, disco);
                strcat(rutaBusquedaArchivo,"/");
                strcat(rutaBusquedaArchivo, t.nombre_carpeta);
                strcat(rutaBusquedaArchivo,"/");
                strcat(rutaBusquedaArchivo, t.structura_tabla_nombre[i].nombre_nombre);
                fputs("\n", reporteArchivo);
                fputs("\n", reporteArchivo);
                fputs("\n", reporteArchivo);
                fputs(rutaBusquedaArchivo,reporteArchivo);
                fwrite("\n",1, sizeof(char),reporteArchivo);
                generarReporteArchivo(rutaBusquedaArchivo, disco,reporteArchivo);
                memset(rutaBusquedaArchivo, '\0',120);


            }
    }
            posicion = posicion + sizeof(t);

        
        s++;
    }


}

void concatenarCadenas(char cadena1[120], char cadena2[120]){
//concatenar cadena 2 a cadena 1
    
    int tamanio_cadena1= strlen(cadena1);
    int i = tamanio_cadena1;
    int pos_final = i+strlen(cadena2);
    int posCadena2=0;
    for(i; i < pos_final; i++){
        cadena1[i]= cadena2[posCadena2];
        posCadena2++;    
    }


}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////PARTE DE LA BUSQUEDA DE ARCHIVOS///////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void buscarCoincidenciasArchivo(char direcc[250]){
    char nombre[30];
    char ptr[100];
    printf("Ingrese el nombre del archivo que desea buscar:  ");
    scanf(" %[^\n]",nombre);
    getchar();
    
     FILE * disco;
    disco = fopen(direcc, "r+b");
    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS);
     printf("*** COINCIDENCIAS PARA EL ARCHIVO %s *** \n", nombre);
     
    int resp = buscarCoincidenciasEnArchivos(nombre, inicio_tablaNombres, disco, inicio_tablaNombres, 0 );


    
}


int  buscarCoincidenciasEnArchivos(char nombre[30], int posicion, FILE * disco, int inicial, int tablaPadre){

    int s=0;
    while(s < 100){
        fseek(disco, posicion, SEEK_SET);
        TABLA t;
        fread(&t, sizeof(t),1, disco);
      //  int respuesta = buscarEnCarpeta(t, nombre);
       // if(respuesta != -1){
    int i;
    for(i=0; i<10; i++){
        TABLA_NOMBRES tn;
        tn= t.structura_tabla_nombre[i];
        if(tn.nombre_tipo == 1){
            if(compararCadenaSubString(tn.nombre_nombre,nombre)==0){
               // return i;    // si se encontro y si coincide , devuelve la posicion   
            armarRutaArchivo(t.tabla_padre, inicial, disco);
            strcat(rutaBusquedaArchivo,"/");
            strcat(rutaBusquedaArchivo, t.nombre_carpeta);
            strcat(rutaBusquedaArchivo,"/");
            strcat(rutaBusquedaArchivo, t.structura_tabla_nombre[i].nombre_nombre);
            printf(GREEN "%s\n" RESET, rutaBusquedaArchivo);
            memset(rutaBusquedaArchivo, '\0',250);
            }
        
        }
    }
            posicion = posicion + sizeof(t);

        

        s++;
    }
    return 0;

}

int buscarEnCarpeta(TABLA t, char nombre[20]){
    int i;
    for(i=0; i<10; i++){
        TABLA_NOMBRES tn;
        tn= t.structura_tabla_nombre[i];
        if(tn.nombre_tipo == 1){
            if(compararCadenaSubString(tn.nombre_nombre,nombre)==0){
                return i;    // si se encontro y si coincide , devuelve la posicion       
            }
        
        }
    
    }
    return -1; // no se encontro, seguir buscando
}


int armarRutaArchivo(int tablaCarpeta, int posicion, FILE * disco){

 int post = posicion + (sizeof(TABLA)*tablaCarpeta);
 fseek(disco, post, SEEK_SET);
 TABLA t;
 fread(&t,sizeof(t),1, disco);
 if(strcmp(t.nombre_carpeta,"/") == 0){
     return 1; //llego satifactoriamente a la raiz
 }else{
     armarRutaArchivo(t.tabla_padre, posicion, disco);
     strcat(rutaBusquedaArchivo,"/");
     strcat(rutaBusquedaArchivo, t.nombre_carpeta);
 }
 return 0; // algun error al armar la ruta
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////      REPORTE DE NODOS          ///////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void reporteNodos(char path[250]){
    MBR mbr;

    FILE * disco;
    disco = fopen(path,"r+b");
    FILE * reporteNodos;
    reporteNodos = fopen("/home/sinozuke/Escritorio/reporteNodos.txt","w+");
    char en1[] = "------------------------------------ \n";
    char en2[] = "         REPORTE DE NODOS  \n";
    char en3[] = "------------------------------------ \n";
    fputs(en1,reporteNodos);
    fputs(en2,reporteNodos);
    fputs(en3,reporteNodos);
    fseek(disco, 0, SEEK_SET);
    fread(&mbr,sizeof(mbr),1,disco);
    int cantidad_nodos = mbr.mbr_cantidad_nodos;
    int s=0;
    
    int pos_i_tn=sizeof(mbr);
    while(s < cantidad_nodos){
       char nombre[6];
       nombre[6] = '\0';
       char number[1];
       number[0] = s + '0';
       strcpy(nombre, "Nodo");
       nombre[4] = number[0];
       nombre[5] = ':';
           fputs(nombre, reporteNodos);
           fputs("\n", reporteNodos);
           fseek(disco, pos_i_tn, SEEK_SET);
           TABLA_NODOS tn; 
           int numero_bloques = (tn.nodo_tamanio_nodo/sizeof(BLOQUE_DATO));
           fread(&tn,sizeof(tn), 1, disco);
           FILE * disco_nodo;
           disco_nodo = fopen(tn.nodo_ruta_nodo, "r+b");
           buscar_imprimir(disco_nodo,reporteNodos);
           fclose(disco_nodo);
             fputs("\n", reporteNodos);
             fputs("\n", reporteNodos);
             fputs("\n", reporteNodos);
           pos_i_tn += sizeof(tn);
           s++;
    }
    
    fclose(disco);
    fclose(reporteNodos);
    printf(GREEN " **** REPORTE NODOS GENERADO *** \n " RESET);
    printf(GREEN "**** REPORTE GENERADO  **** \n" RESET);
    if(!fork())
    execlp("gedit", "gedit", "/home/sinozuke/Escritorio/reporteNodos.txt", NULL);
}

void buscar_imprimir(FILE * disco_nodo, FILE * reporteNodo, int numero_bloques){
   
    int pos_n_bd = 0;
    
    int bloque=0;
    while(bloque < 100){
     BLOQUE_DATO bd;
     fseek(disco_nodo, pos_n_bd, SEEK_SET);
     fread(&bd, sizeof(bd), 1, disco_nodo);
     if(bd.bd_estado == 1){
       char nombre[8];
       nombre[8] = '\0';
       char number[1];
       number[0] = bloque + '0';
       strcpy(nombre, "Bloque");
       nombre[6] = number[0];
       nombre[7] = ':';
        fputs(nombre, reporteNodo);
        fputs(bd.bd_data, reporteNodo);
        fputs("\n", reporteNodo);
     
     } 

     pos_n_bd += sizeof(bd);
     bloque++;
    }
    


}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////      REPORTE DE TABLA NOMBRES       ///////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void reporteTablaNombres(char path[250]){
    FILE * disco;
    disco = fopen(path,"r+b");
    FILE * reporteNombres;
    reporteNombres = fopen("/home/sinozuke/Escritorio/reporteTablaNombres.txt","w+");
    char en1[] = "------------------------------------ \n";
    char en2[] = "         REPORTE DE TABLAS DE NOMBRE  \n";
    char en3[] = "------------------------------------ \n";
    fputs(en1,reporteNombres);
    fputs(en2,reporteNombres);
    fputs(en3,reporteNombres);
    int inicio_t_n = sizeof(MBR) + 10*sizeof(TABLA_NODOS);
    //posicionarme en el inicio tabla nombres
    int s=0;
    while(s < 100){
        fseek(disco, inicio_t_n, SEEK_SET);
        TABLA t;
        fread(&t, sizeof(t),1,disco);
        if(t.nombre_carpeta[0] != '\0'){
            char nombre[6];
            nombre[6] = '\0';
            char number[1];
            number[0] = s + '0';
            strcpy(nombre, "Tabla");
            nombre[5] = number[0];
           fputs(nombre, reporteNombres);
        fputs(": Nombre---->",reporteNombres);  
        fputs(t.nombre_carpeta, reporteNombres);  
        fputs("\n", reporteNombres);
        //   fputs(" | ", reporteNombres);
           
        }
        int i;
        for(i=0;i<10;i++){
            TABLA_NOMBRES tnombre;
            tnombre = t.structura_tabla_nombre[i];
            if(tnombre.nombre_estado == 1 && tnombre.nombre_tipo ==0){
                fputs(tnombre.nombre_nombre, reporteNombres);
                fputs(" | ", reporteNombres);
                fputs("carpeta", reporteNombres);
                 fputs(" | ", reporteNombres);
                  char m[120];
                strcpy(m,tnombre.nombre_fecha_creacion);
                int fin =strlen(m)-1;
                m[fin]='\0';
                fputs(m, reporteNombres);
                fputs(" | ", reporteNombres);
                fputs(" 1 | - | - | ocupado", reporteNombres);
                fputs("\n", reporteNombres);
            
            }
            else if(tnombre.nombre_estado == 1 && tnombre.nombre_tipo ==1){
                fputs(tnombre.nombre_nombre, reporteNombres);
                fputs(" | ", reporteNombres);
                fputs("archivo", reporteNombres);
                 fputs(" | ", reporteNombres);
                  char m[120];
                strcpy(m,tnombre.nombre_fecha_creacion);
                int fin =strlen(m)-1;
                m[fin]='\0';
                fputs(m, reporteNombres);
                fputs(" | ", reporteNombres);
                fputs(" 1 | - | - | ocupado", reporteNombres);
                fputs("\n", reporteNombres);
            
            }
        }
        if(t.nombre_carpeta[0] != '\0'){
             fputs("\n", reporteNombres);
             fputs("\n", reporteNombres);
             fputs("\n", reporteNombres);
        }
        s++;
        inicio_t_n+=sizeof(t);
    }
    fclose(disco);
    fclose(reporteNombres);
    printf(GREEN "**** REPORTE GENERADO  **** \n" RESET);
    if(!fork())
    execlp("gedit", "gedit", "/home/sinozuke/Escritorio/reporteTablaNombres.txt", NULL);
}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////      ELIMINAR CARPETAS Y ARCHIVOS     ////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void eliminarDirectorio(char roota[250]){
    char path[120];
   
    printf("Ingrese el directorio que desea eliminar:  ");
    scanf(" %[^\n]",path);
    getchar();
    FILE * disco;
    disco = fopen(roota,"r+b");
    int i_t_nombres = sizeof(MBR)+10*(sizeof(TABLA_NODOS));
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path, "/");
            while (token != NULL){
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();
    int respuesta = revisarTablaNombresEliminar(temporal, i_t_nombres, disco, i_t_nombres, 0);
    if(respuesta == -2){
        printf(RED "***EL ARCHIVO NO EXISTE NO SE PUEDE ELIMINAR**** \n" RESET);
    }else if(respuesta == 1){
        printf(GREEN "***DIRECTORIO Y SUBDIRECTORIOS ELIMINADOS **** \n" RESET);
    
    }
    fclose(disco);
}

int revisarTablaNombresEliminar(colaPadres* temporal, int posicion, FILE * disco, int inicial, int tablaPadre){
    fseek(disco, posicion, SEEK_SET);
    TABLA t;
    fread(&t,sizeof(t),1,disco);
    int i;
   if(strcmp(temporal->padre, "/")==0){
       copiarCadena(t.nombre_carpeta, temporal->padre,19);
   }
    colaPadres * father = temporal;
       temporal = pop();
       
        if(temporal != NULL){
           TABLA_NOMBRES tn;  
           int found = buscarCarpeta(t, temporal->padre);
           if(found == -1){ // esto quiere decir que la carpeta no se encontro entonces no existe
            
                 return -2; //carpeta NO ENCONTRADA
             
           }else{ //aqui quiere decir que si existe la carpeta se procede a verificar si hay mas direcciones que revisar o  notificar que ya existe
               if(temporal->siguiente == NULL){
                  
                        int posc = buscarCarpeta(t,temporal->padre);
                        int tipo=-1;
                        int bloqueinicialarchivo=-1;
                        int numeronodo=-1;
                        if(posc != -1){
                            tn = t.structura_tabla_nombre[posc];
                            bloqueinicialarchivo = tn.nombre_bloque_inicial ;
                            numeronodo = tn.nombre_datanode;
                            tn.nombre_bloque_inicial = -1;
                            tn.nombre_datanode = -1;
                            tn.nombre_estado = -1;
                            tn.nombre_padre = -1;
                            tipo = tn.nombre_tipo;
                            tn.nombre_tipo = -1;  
                            memset(tn.nombre_fecha_creacion,'\0',250);
                            memset(tn.nombre_nombre,'\0',20);
                            t.structura_tabla_nombre[posc] = tn;    
                            fseek(disco, posicion, SEEK_SET);
                            fwrite(&t, 1, sizeof(t), disco);
                        }

                        if(tipo ==0){
                          deleteTableName(inicial,disco, temporal->padre, tablaPadre );
                        }else if(tipo == 1){
                            buscarArchivoEliminar(numeronodo,bloqueinicialarchivo, disco);  
                        
                        }
                        
                   //AQUI SE BORRA EL TABLE NAME ASOCIADO 
                   
                   
                   
               return 1;
               }else{
                   TABLA_NOMBRES tnomb;
                   tnomb = t.structura_tabla_nombre[found];
                   int padre = tnomb.nombre_padre;
                    int ppadre= buscarPadre(inicial,disco,temporal->padre, padre); // este devueleve un numero pequeno de numero de table name
                    int post = inicial + (sizeof(t)*ppadre);
                    int c =   revisarTablaNombresEliminar(temporal, post, disco, inicial, ppadre);
                    if(c == 1){
                        return 1;
                    }else{
                        return -2;
                    }
                
               }            
           }
      
        }

     
    return 0;
}

 
int deleteTableName(int posicion, FILE * disco, char nombre[20], int padre){
    int s=0;
    while(s==0){
        fseek(disco, posicion, SEEK_SET);
        TABLA t;
        fread(&t, 1, sizeof(t), disco);
        if(strcmp(t.nombre_carpeta,nombre)==0 && t.tabla_padre == padre){
            memset(t.nombre_carpeta,'\0',20);
            t.tabla_padre = -1;
            int i;
            for(i = 0; i < 10; i++){
              if(t.structura_tabla_nombre[i].nombre_estado == 1 && t.structura_tabla_nombre[i].nombre_tipo == 0){
                  deleteTableName(posicion, disco,t.structura_tabla_nombre[i].nombre_nombre, t.structura_tabla_nombre[i].nombre_padre);
                  TABLA_NOMBRES tn;     
                        tn.nombre_bloque_inicial = -1;
                        tn.nombre_datanode = -1;
                        tn.nombre_estado = -1;
                        tn.nombre_padre = -1;
                        tn.nombre_tipo = -1;  
                        memset(tn.nombre_fecha_creacion,'\0',250);
                        memset(tn.nombre_nombre,'\0',20);
                        t.structura_tabla_nombre[i] = tn;
              }else if(t.structura_tabla_nombre[i].nombre_estado == 1 && t.structura_tabla_nombre[i].nombre_tipo == 1){
                  
                  TABLA_NOMBRES tn;    
                  tn = t.structura_tabla_nombre[i];
                      int bloqueinicialarchivo = tn.nombre_bloque_inicial;
                      int numeronodo = tn.nombre_datanode;
                      buscarArchivoEliminar(numeronodo,bloqueinicialarchivo, disco);    
                      tn.nombre_bloque_inicial = -1;
                      tn.nombre_datanode = -1;
                      tn.nombre_estado = -1;
                      tn.nombre_padre = -1;
                      tn.nombre_tipo = -1;  
                      memset(tn.nombre_fecha_creacion,'\0',250);
                      memset(tn.nombre_nombre,'\0',20);
                      t.structura_tabla_nombre[i] = tn;
                        

              }
            }  
            fseek(disco, posicion, SEEK_SET);
            fwrite(&t, 1, sizeof(t), disco);
            s = 1;
        }else{
            posicion = posicion + sizeof(t);
        }
        
    }
    return 1;
}

void buscarArchivoEliminar(int numeroNodo, int numeroBloque, FILE * disco){
    int pos_t_nodo = sizeof(MBR)+ (sizeof(TABLA_NODOS)*numeroNodo);
    fseek(disco, pos_t_nodo, SEEK_SET);
    TABLA_NODOS tnodo;
    fread(&tnodo, sizeof(tnodo), 1, disco);
    FILE * disco_nodo;
    disco_nodo = fopen(tnodo.nodo_ruta_nodo, "r+b");
   
    buscarEntreNodosEliminar(numeroBloque, disco_nodo, numeroNodo, disco);
    fclose(disco_nodo);
    colaDiscos * temporal = pop_disco();
    while(temporal != NULL){
        fclose(temporal->disco);
        temporal = pop_disco();
    }


}

int buscarEntreNodosEliminar(int numeroBloque, FILE * disco_nodo, int numeroNodo, FILE * disco){

    BLOQUE_DATO bd;
    int pos_b_inicial = (numeroBloque*(sizeof(bd)));
    fseek(disco_nodo, pos_b_inicial, SEEK_SET);
    fread(&bd, sizeof(bd), 1, disco_nodo);
    if(bd.bd_siguiente != -1){// si siguiente es diferente de -1 es porque hay mas donde busca
        int numerobloquesiguiente = bd.bd_siguiente;
        int numeronodosiguiente = bd.bd_datanode;
        if(numeronodosiguiente != numeroNodo){
            TABLA_NODOS tn;
            FILE * nodo_siguiente;
            int pos_s_nodo = sizeof(MBR)+ (sizeof(TABLA_NODOS)*numeronodosiguiente);
            fseek(disco, pos_s_nodo, SEEK_SET);
            fread(&tn, sizeof(tn), 1, disco);
            nodo_siguiente = fopen(tn.nodo_ruta_nodo, "r+b");
            buscarEntreNodosEliminar(numerobloquesiguiente, nodo_siguiente,numeronodosiguiente, disco);
           
          //  disco_nodo = fopen(tn.nodo_ruta_nodo, "r+b");
            memset(bd.bd_data,'\0',51);
              bd.bd_datanode =-1;
              bd.bd_estado =0;
              bd.bd_siguiente = -1;
              fseek(disco_nodo, pos_b_inicial, SEEK_SET);
              fwrite(&bd, 1, sizeof(bd),disco_nodo);
    colaDiscos *d =malloc(sizeof(struct colaDiscos));
    d->disco = nodo_siguiente;  
    insertarColaDiscos(d);
            
        }else{
            
             buscarEntreNodosEliminar(numerobloquesiguiente, disco_nodo,numeronodosiguiente, disco);
              memset(bd.bd_data,'\0',51);
              bd.bd_datanode =-1;
              bd.bd_estado =0;
              bd.bd_siguiente = -1;
              fseek(disco_nodo, pos_b_inicial, SEEK_SET);
              fwrite(&bd, 1, sizeof(bd),disco_nodo);
            
        }

        
        return 1; //hay un cambio
    }else { //es el ultimo nodo
           memset(bd.bd_data,'\0',51);
           bd.bd_datanode =-1;
           bd.bd_estado =0;
           bd.bd_siguiente = -1;
           fseek(disco_nodo, pos_b_inicial, SEEK_SET);
           fwrite(&bd, 1, sizeof(bd),disco_nodo);
         
        return 1; //llego hasta el final
    }
    //aqui se imprime el archivo 
    return -1;

}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////    COPIAR CARPETAS Y ARCHIVOS     ////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//recomendacion: ir de derecha a izquierda creando, primero buscar hasta la ultima
//carpeta que existe

//si tiene archivos dentro tengo que recorrer la informacion y guardarla temporalmente para 
char contenidoTemporal[512];
void crearArchivoCopiar(char path[350], FILE * disco, char contenido[512]){
    char padre[25];
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path, "/");
            while (token != NULL){
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();
    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS); 
    int temanio_ = sizeof(TABLA_NODOS);
   int creacion =  revisarTablaNombresArchivos(temporal, inicio_tablaNombres, disco, inicio_tablaNombres, 0, contenido);
    if(creacion == 1){
        printf(GREEN "**** ARCHIVO CREADO EXITOSAMENTE **** \n" RESET);
    }else{
        printf( RED "**** ALGUN ERROR EN LA CREACION DE ARCHIVOS **** \n" RESET);
    }
   fclose(disco);
}
void copiarCarpeta(char rut[250]){
    char path1[320];
    char path2[320];
    printf("Ingrese la direccion que desea copiar:  ");
    scanf(" %[^\n]",path1);
    getchar();
    printf("En donde la desea copiar? [escriba la ruta]: ");
    scanf(" %[^\n]",path2);
    getchar();
    FILE * disco;
    disco = fopen(rut,"r+b");
    int i_t_nombres = sizeof(MBR)+10*(sizeof(TABLA_NODOS));
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path1, "/");
            while (token != NULL){
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();
   int respuesta = revisarTablaNombresCopiar(temporal, i_t_nombres, disco, i_t_nombres, 0, path2,rut);

}

int revisarTablaNombresCopiar(colaPadres* temporal, int posicion, FILE * disco, int inicial, int tablaPadre, char path2[320], char rrr[250]){
    fseek(disco, posicion, SEEK_SET);
    TABLA t;
    fread(&t,sizeof(t),1,disco);
    int i;
   if(strcmp(temporal->padre, "/")==0){
       copiarCadena(t.nombre_carpeta, temporal->padre,19);
   }
    colaPadres * father = temporal;
       temporal = pop();
       
        if(temporal != NULL){
           TABLA_NOMBRES tn;  
           int found = buscarCarpeta(t, temporal->padre);
           if(found == -1){ // esto quiere decir que la carpeta no se encontro entonces no existe
            
                 return -2; //carpeta NO ENCONTRADA
             
           }else{ //aqui quiere decir que si existe la carpeta se procede a verificar si hay mas direcciones que revisar o  notificar que ya existe
               if(temporal->siguiente == NULL){
                  //llego a la carpeta final que es la que deseamos copiar
                        int posc = buscarCarpeta(t,temporal->padre);
                        int tipo=-1;
                        int bloqueinicialarchivo=-1;
                        int numeronodo=-1;
                        if(posc != -1){
                            tn = t.structura_tabla_nombre[posc];
                            bloqueinicialarchivo = tn.nombre_bloque_inicial ;
                            numeronodo = tn.nombre_datanode;
                            tipo = tn.nombre_tipo;
                            
                        }
                        char path3[320];
                        if(tipo ==0){
                            strcat(path2, "/");
                            strcat(path2,temporal->padre);
                            
                            memset(path3, '\0',320);
                            strcpy(path3,path2);
                            crearCarpetaCopiar(path3, disco);
                            disco  = fopen(rrr,"r+b"); 
                            cTableName(inicial,disco, temporal->padre, path2,rrr);
                            //aqui el metodo para copiar
                        }else if(tipo == 1){
                            //buscarArchivoEliminar(numeronodo,bloqueinicialarchivo, disco);  
                            //copiara aqui para el contenido temporal
                            strcat(path2, "/");
                            strcat(path2,temporal->padre);
                            //char path3[320];
                            memset(path3, '\0',320);
                            strcpy(path3,path2);
                            buscarArchivoCopiar(numeronodo, bloqueinicialarchivo,disco);
                            crearArchivoCopiar(path3, disco, contenidoTemporal);
                           
                            disco = fopen(rrr,"r+b");
                            
                            
                        }
   
                   
               return 1;
               }else{
                   TABLA_NOMBRES tnomb;
                   tnomb = t.structura_tabla_nombre[found];
                   int padre = tnomb.nombre_padre;
                    int ppadre= buscarPadre(inicial,disco,temporal->padre, padre); // este devueleve un numero pequeno de numero de table name
                    int post = inicial + (sizeof(t)*ppadre);
                    int c =   revisarTablaNombresCopiar(temporal, post, disco, inicial, ppadre, path2,rrr);
                    if(c == 1){
                        return 1;
                    }else{
                        return -2;
                    }
                
               }            
           }
      
        }

     
    return 0;
}

int buscarEntreNodosCopiar(int numeroBloque, FILE * disco_nodo, int numeroNodo, FILE * disco){

    BLOQUE_DATO bd;
    int pos_b_inicial = (numeroBloque*(sizeof(bd)));
    fseek(disco_nodo, pos_b_inicial, SEEK_SET);
    fread(&bd, sizeof(bd), 1, disco_nodo);
    if(bd.bd_siguiente != -1){// si siguiente es diferente de -1 es porque hay mas donde busca
        int numerobloquesiguiente = bd.bd_siguiente;
        int numeronodosiguiente = bd.bd_datanode;
        if(numeronodosiguiente != numeroNodo){
            TABLA_NODOS tn;
            FILE * nodo_siguiente;
            int pos_s_nodo = sizeof(MBR)+ (sizeof(TABLA_NODOS)*numeronodosiguiente);
            fseek(disco, pos_s_nodo, SEEK_SET);
            fread(&tn, sizeof(tn), 1, disco);
            nodo_siguiente = fopen(tn.nodo_ruta_nodo, "r+b");
            buscarEntreNodosCopiar(numerobloquesiguiente, nodo_siguiente,numeronodosiguiente, disco);
            strcat(contenidoTemporal, bd.bd_data);
            
            
        }else{
            
             buscarEntreNodosCopiar(numerobloquesiguiente, disco_nodo,numeronodosiguiente, disco);
             strcat(contenidoTemporal, bd.bd_data);
            
        }

        
        return 1; //hay un cambio
    }else { //es el ultimo nodo
            strcat(contenidoTemporal, bd.bd_data);
         
        return 1; //llego hasta el final
    }
    //aqui se imprime el archivo 
    return -1;

}

void buscarArchivoCopiar(int numeroNodo, int numeroBloque, FILE * disco){
    int pos_t_nodo = sizeof(MBR)+ (sizeof(TABLA_NODOS)*numeroNodo);
    fseek(disco, pos_t_nodo, SEEK_SET);
    TABLA_NODOS tnodo;
    fread(&tnodo, sizeof(tnodo), 1, disco);
    FILE * disco_nodo;
    disco_nodo = fopen(tnodo.nodo_ruta_nodo, "r+b");
    buscarEntreNodosCopiar(numeroBloque, disco_nodo, numeroNodo, disco);
    fclose(disco_nodo);

}
int cTableName(int posicion, FILE * disco, char nombre[20], char path2[320], char r[250]){
    int s=0;
    char path3[320];
    while(s==0){
        fseek(disco, posicion, SEEK_SET);
        TABLA t;
        fread(&t, 1, sizeof(t), disco);
        if(strcmp(t.nombre_carpeta,nombre)==0){
            int i;
            for(i = 0; i < 10; i++){
              if(t.structura_tabla_nombre[i].nombre_estado == 1 && t.structura_tabla_nombre[i].nombre_tipo == 0){
                  memset(path3, '\0', 320);
                  strcpy(path3, path2);
                  TABLA_NOMBRES tnombre;
                  tnombre = t.structura_tabla_nombre[i];
                  strcat(path3, "/");
                  strcat(path3, tnombre.nombre_nombre);      
                  crearCarpetaCopiar(path3, disco);
                  disco = fopen(r, "r+b");               
              }else if(t.structura_tabla_nombre[i].nombre_estado == 1 && t.structura_tabla_nombre[i].nombre_tipo == 1){

                   memset(path3, '\0', 320);
                  strcpy(path3, path2);
                  
                  TABLA_NOMBRES tn;    
                  tn = t.structura_tabla_nombre[i];
                    //memset(path3, '\0', 320);
                  strcat(path3, "/");             
                  strcat(path3, tn.nombre_nombre);
                  buscarArchivoCopiar(tn.nombre_datanode, tn.nombre_bloque_inicial,disco);
                  crearArchivoCopiar(path3, disco, contenidoTemporal);
                  disco = fopen(r,"r+b");
                    //  buscarArchivoEliminar(numeronodo,bloqueinicialarchivo, disco);    

                      //aqui ira a buscar el documento y lo almacenara en una variable temporal
              }
            }  

            s = 1;
        }else{
            posicion = posicion + sizeof(t);
        }
        
    }
    return 1;
}



void crearCarpetaCopiar(char path[320], FILE  * disco){
    char padre[25];
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path, "/");
            while (token != NULL){
                //printf("The token is:  %s\n", token);
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();

    int inicio_tablaNombres = sizeof(MBR) + 10*sizeof(TABLA_NODOS); 
    int temanio_ = sizeof(TABLA_NODOS);
   int creacion =  revisarTablaNombres(temporal, inicio_tablaNombres, disco, inicio_tablaNombres, 0);
    if(creacion == 1){
       // printf("**** CARPETA CREADA EXITOSAMENTE **** \n");
    }else{
        printf( RED "**** ALGUN ERROR EN LA CREACION DE CARPETAS **** \n" RESET);
    }
 fclose(disco);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////  MODIFICAR ARCHIVO                    ////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int revisarTablaNombresModificar(colaPadres* temporal, int posicion, FILE * disco, int inicial, int tablaPadre, char path[320], char rrr[250]){
    fseek(disco, posicion, SEEK_SET);
    TABLA t;
    fread(&t,sizeof(t),1,disco);
    int i;
   if(strcmp(temporal->padre, "/")==0){
       copiarCadena(t.nombre_carpeta, temporal->padre,19);
   }
    colaPadres * father = temporal;
       temporal = pop();
       
        if(temporal != NULL){
           TABLA_NOMBRES tn;  
           int found = buscarCarpeta(t, temporal->padre);
           if(found == -1){ // esto quiere decir que la carpeta no se encontro entonces no existe
            
                 return -2; //carpeta NO ENCONTRADA
             
           }else{ //aqui quiere decir que si existe la carpeta se procede a verificar si hay mas direcciones que revisar o  notificar que ya existe
               if(temporal->siguiente == NULL){
                  //llego a la carpeta final que es la que deseamos copiar
                    
                        int posc = buscarCarpeta(t,temporal->padre);
                        int tipo=-1;
                        int bloqueinicialarchivo=-1;
                        int numeronodo=-1;
                        if(posc != -1){
                            tn = t.structura_tabla_nombre[posc];
                            bloqueinicialarchivo = tn.nombre_bloque_inicial ;
                            numeronodo = tn.nombre_datanode;
                            tn.nombre_bloque_inicial = -1;
                            tn.nombre_datanode = -1;
                            tn.nombre_estado = -1;
                            tn.nombre_padre = -1;
                            tipo = tn.nombre_tipo;
                            tn.nombre_tipo = -1;  
                            memset(tn.nombre_fecha_creacion,'\0',250);
                            memset(tn.nombre_nombre,'\0',20);
                            t.structura_tabla_nombre[posc] = tn;    
                            fseek(disco, posicion, SEEK_SET);
                            fwrite(&t, 1, sizeof(t), disco);
                        }
                        
                        
                        char path3[320];
                        if(tipo ==0){

                            //aqui el metodo para copiar
                        }else if(tipo == 1){
                            // 

                            buscarArchivoCopiar(numeronodo, bloqueinicialarchivo,disco);
                            FILE * archivo;
                            archivo = fopen("archivo.txt","w+");
                            fputs(contenidoTemporal, archivo);
                            fclose(archivo);
                            memset(contenidoTemporal,'\0',512);
                            
                            buscarArchivoEliminar(numeronodo,bloqueinicialarchivo, disco); 
                            //aqui abir el editor de texto para modificar
                             if(!fork())
                             execlp("gedit", "gedit", "archivo.txt", NULL);
                             printf(" PRESIONE ENTER PARA CONTINUAR \n");
                             getchar();
                            
                            archivo = fopen("archivo.txt","rb");
                            fseek(archivo, 0, SEEK_END);
                            long fsize = ftell(archivo);
                            fseek(archivo, 0, SEEK_SET);  //same as rewind(f);
                            char *string = malloc(fsize + 1);
                            fread(string, fsize, 1, archivo);
                            fclose(archivo);
                            crearArchivoCopiar(path, disco, string);
                            disco = fopen(rrr,"r+b");
                            
                            
                        }
   
                   
               return 1;
               }else{
                   TABLA_NOMBRES tnomb;
                   tnomb = t.structura_tabla_nombre[found];
                   int padre = tnomb.nombre_padre;
                    int ppadre= buscarPadre(inicial,disco,temporal->padre, padre); // este devueleve un numero pequeno de numero de table name
                    int post = inicial + (sizeof(t)*ppadre);
                    int c =   revisarTablaNombresModificar(temporal, post, disco, inicial, ppadre,path,rrr);
                    if(c == 1){
                        return 1;
                    }else{
                        return -2;
                    }
                
               }            
           }
      
        }

     
    return 0;
}

void modificarArchivo(char rrr[250]){
    char path[320];
    char path1[320];
    printf("Ingrese la direccion del archivo que desea modificar:  ");
    scanf(" %[^\n]",path);
    getchar();
    strcpy(path1, path);
    FILE * disco;
    disco = fopen(rrr,"r+b");
    int i_t_nombres = sizeof(MBR)+10*(sizeof(TABLA_NODOS));
    char *token;
    char *root="/";
    colaPadres *raiz =malloc(sizeof(struct colaPadres));
    raiz->padre = root;  
    insertarCola(raiz);
    token = strtok(path, "/");
            while (token != NULL){
                colaPadres *retVal = malloc (sizeof (struct colaPadres));
                retVal->padre=token;
                insertarCola(retVal);
                token = strtok(NULL,"/");
            }
    colaPadres *temporal = pop();
   int respuesta = revisarTablaNombresModificar(temporal, i_t_nombres, disco, i_t_nombres, 0,path1, rrr);

}