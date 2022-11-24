#define GHOSTS_STUD
#ifdef GHOSTS_STUD


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ghosts.h"
#include "pacman.h"
#include "pacman.c"
#include "matrix.h"
#include "global.h"
#include "main.c"

static const struct position UNK_POSITION = {-1,-1}; // Set unknown position to (UINT_MAX,UINT_MAX)

struct ghost { 
    int id;  //questo è l’id del fantasma 
    enum ghost_status status; 
    enum direction dir; 
    struct position pos; 
}; 

struct ghosts { 
    char **A; 
    //char** path;
    unsigned int nrow; 
    unsigned int ncol; 
    unsigned int n; 
    struct ghost *ghost; 
};

/* Create the ghosts data structure */
struct ghosts *ghosts_setup(unsigned int num_ghosts) { 
    struct ghosts *G = (struct ghosts *)malloc(sizeof(struct ghosts));
    srand(time(NULL));
    if(G != NULL) {  //se G non punta a NULL
        unsigned int i;
        G->n = num_ghosts;  // al campo n di G assegniamo num_ghosts
        G->ghost = (struct ghost *)calloc(num_ghosts,sizeof(struct ghost)); //al campo ghost di G assegniamo dinamicamente la memoria
        for(i = 0; i < G->n; i++) { //per ogni ghost presente assegniamo una UNK_POSITION e una direzione casuale
            G->ghost[i].id = i;     //id per il fantasma
            G->A = A->arena;         //nel char di arena inserisco il arena.txt preso da main.c
            G->ghost[i].dir = rand() % 3;   //random da zero a 3
            G->ghost[i].status = UNK_GHOST_STATUS;  //status sconosciuto preso da una enum
            int l=1; // fa andare il ciclo
            while(l){
                struct position tmp1 = G->ghost[i].pos; //tmp1 assume la posizione del fantasma
                struct position tmp =  {rand() % G->nrow,rand() % G->ncol}; //tmp assume due coordinate casuali
                if (!(IS_WALL(G->A,tmp) && IS_PACMAN(G->A,tmp) && IS_GHOST(G->A,tmp))){ //confronto le coordinate casuali con delle define prodotte in global.h
                    G->ghost[i].pos.i = tmp.i; //se le coordinate sono libere asseggno le coordinate al fantasma
                    G->ghost[i].pos.j = tmp.j;
                    l=0; //termina il ciclo    
                }
            }
            
        }
    }
    return G;
}

/* Destroy the ghost data structure */
void ghosts_destroy(struct ghosts *G) {
    if (G != NULL){
        int i;
        for (i=0;i<G->n;i++)
            free(G->ghost[i].id);
        free(G);
    }
return;
}

/* Set the arena (A) matrix */
void ghosts_set_arena(struct ghosts *G, char **A, unsigned int nrow,unsigned int ncol) {
    if (G != NULL) {
        G->A = A;
        G->ncol = ncol;
        G->nrow = nrow;
    }
    return;                                                      
}

/* Set the position of the ghost with that id. */
void ghosts_set_position(struct ghosts *G, unsigned int id, struct position pos) {
    if(G != NULL && id < G->n) G->ghost[id].pos = pos; // se G non punta a  NULLe il suo identificativo è valido assegniamo al fantasma con quell’id la posizione
    return;
}

/* Set the status of the ghost id. */
void ghosts_set_status(struct ghosts *G, unsigned int id, enum ghost_status status) {
    if(G != NULL) G->ghost[id].status = status;
    return;
}

/* Return the number of ghosts */
unsigned int ghosts_get_number(struct ghosts *G) {
    if(G != NULL) {
        unsigned int numGhost = G->n;
    return numGhost;
    }
}

/* Return the position of the ghost id. */
struct position ghosts_get_position(struct ghosts *G, unsigned int id) {
    struct position p; 
    if(G != NULL && id < G->n)   p= G->ghost[id].pos;
                            else p = UNK_POSITION;
    return p;

}

/* Return the status of the ghost id. */
enum ghost_status ghosts_get_status(struct ghosts *G, unsigned int id) {
    enum ghost_status stat;
    if(G != NULL && id < G->n)   stat = G->ghost[id].status;
                            else stat = UNK_GHOST_STATUS;
    return stat;
}

static int legal_position(struct ghosts *G, struct pacman*P, struct position pos, enum ghost_status status) { 
    if(IS_WALL(G->A,pos) || IS_GHOST(G->A,pos))  return 0;  
    else { 
        //unsigned int i;
        //struct position p = pacman_get_position(P); 
        //check pacmanintersect … 
        //check ghost intersect … 
    }
    return 1; 
}

/* Move the ghost id (according to its status). Returns the new position */
struct position ghosts_move(struct ghosts *G, struct pacman *P, unsigned int id) {
    struct position p;
    if(P && G && UNK_GHOST_STATUS) return p = UNK_POSITION;
    unsigned int pacPi,pacPj,ghostPi,ghostPj;
    pacPi = P->pos.i;
    pacPj = P->pos.j;
    ghostPi = G->ghost[id].pos.i;
    ghostPj = G->ghost[id].pos.j;

    switch (G->ghost[id].status){
        case NORMAL:
            
            break;
        case SCARED_NORMAL:
            /* code */
            break;
        case SCARED_BLINKING:
            /* code */
            break;
        case EYES:
            /* code */
            break;
        default:
            break;
    }
    return p;
}

#endif


