//Universidad del Valle de Guatemala
// Sara Zavala 18893
//Laboratorio 3
//Sistemas Operativos

//Referencias para guia
//https://www.geeksforgeeks.org/check-if-given-sudoku-board-configuration-is-valid-or-not/
//https://programmercave0.github.io/blog/2020/07/13/SUDOKU-VALIDATOR-CodinGame-C++-Implementation

#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <omp.h>
#define tamanio 82
#define dimensiones 9
// ---------------------------------------------------
char tope[tamanio];
char cont[dimensiones][dimensiones];
// ---------------------------------------------------
void lector(){
    omp_set_num_threads(tamanio - 1);
    omp_set_nested(1);
    FILE * fl = fopen("SudokuValidator.txt", "r");
    fgets(tope, tamanio, (FILE*)fl);
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (size_t x = 0; x < dimensiones; x++){
        for (size_t y = 0; y < dimensiones; y++){
            cont[x][y] = tope[(9 * x) + y];
        }
    } fclose(fl);
}
// ---------------------------------------------------
int mostrarSubs(int c, int r){
    omp_set_num_threads(tamanio - 1);
    omp_set_nested(1);
    int numsver[dimensiones] = {0,0,0,0,0,0,0,0,0};
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (size_t x = c; x < c + 3; x++){
        for(size_t y = r; y < r + 3; y++){
            if (cont[x][y] > 48 && cont[x][y] < 58){
                numsver[cont[x][y]-49] = 1;
            } else {
                printf("El ingreso es erroneo");
                exit(-1);
            }
        }
    }
    for (size_t x = 0; x < dimensiones; x++){
        if (numsver[x] == 0){
            return 0;
        }
    }
    return 1;
}
// ---------------------------------------------------
typedef struct fThread {
    int ver_Col;
    int resulta2ado;
} fThread;
void *confirmarFoC(void *arg){
    omp_set_num_threads(dimensiones);
    omp_set_nested(1);
    fThread *tdata = (fThread *)arg;
    int ver_Col = tdata->ver_Col;
    int colver[dimensiones] = {1,1,1,1,1,1,1,1,1};
    #pragma omp parallel for 
    for (int x = 0; x < dimensiones; x++){
        if (ver_Col == 1) {
        pid_t ttid;
        ttid = syscall(SYS_gettid);
        printf("ID método de columnas: %d\n", ttid);}
        int numsver[dimensiones] = {0,0,0,0,0,0,0,0,0};
        for (int y = 0; y < dimensiones; y++){
            if (ver_Col == 1){
                if (cont[y][x] > 48 && cont[y][x] < 58) numsver[cont[y][x]-49] = 1;
                else {
                    printf("El ingreso es erroneo");
                    exit(-1);
                }
            } else {
                if (cont[x][y] > 48 && cont[x][y] < 58){numsver[cont[x][y]-49] = 1;} 
                else {
                    printf("El ingreso es erroneo");
                    exit(-1);}
            }           
        }
        for (int y = 0; y < dimensiones; y++){
            if (numsver[x] == 0){colver[x] = 0; 
            	break;} 
        }
    }
    for (size_t x = 0; x < dimensiones; x++){
        if (colver[x] == 0){
            tdata -> resulta2ado = 0;
            pthread_exit(0);
        }
    }
    tdata->resulta2ado = 1;
    pthread_exit(0);
}
// ---------------------------------------------------
int main(int argc, char const *argv[]){   
    omp_set_num_threads(dimensiones);
    omp_set_nested(1);
// -------------------
    lector();   
    int resulta2 = 1; 
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (size_t x = 0; x < 3; x++){
        for (size_t y = 0; y < 3; y++){
            if (mostrarSubs(y * 3, x * 3) == 0) resulta2 = 0;
        }        
    }

    pid_t pid = getpid();
    pid_t pid2 = fork();
    char res[10];
    if (pid2 == 0) {
        sprintf(res, "%d", pid);
        execlp("ps", "ps", "-p", res, "-lLf", NULL);

    } else {        
        pthread_t tid;
        fThread tdata, tdata2;
        tdata.ver_Col = 1;
        pthread_create(&tid, NULL, confirmarFoC, (void *)&tdata);
        pthread_join(tid, NULL);
// ------------------------------------------------------------------------------------
        pid_t ttid;
        ttid = syscall(SYS_gettid);
        printf("ID de main: %d\n", ttid);
        // -------------------
        wait(NULL);
        tdata2.ver_Col = 0;
        pthread_create(&tid, NULL, confirmarFoC, (void *)&tdata2);
        pthread_join(tid, NULL);
        if (resulta2 == 1 && tdata.resulta2ado == 1 && tdata2.resulta2ado == 1){
            printf("Sodoku válido\n");
        } else {
            printf("Sodoku inválido\n");
        }
        
        pid_t grandid = getpid();
        pid_t grandid2 = fork();
        char res2[10];
// -------------------
        if (grandid2 == 0) {
            sprintf(res2, "%d", grandid);
            execlp("ps", "ps", "-p", res2, "-lLf", NULL);
        } else {
            wait(NULL);
        }
        
    }
     
    return 0;
}