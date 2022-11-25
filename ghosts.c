#define GHOSTS_STUD
#ifdef GHOSTS_STUD


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ghosts.h"
#include "pacman.h"
#include "matrix.h"
#include "global.h"

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
             //nel char di arena inserisco il arena.txt preso da main.c
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
        /*int i;
        for (i=0;i<G->n;i++)
            free(G->ghost[i]);*/
        free(G);
    }
return;
}

/* Set the arena (A) matrix */
void ghosts_set_arena(struct ghosts *G, char **A, unsigned int nrow,unsigned int ncol) {
    if (G != NULL) {
        G->A = matrix_read(**a,nrow,ncol);
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
    if(G != NULL && id < G->n) G->ghost[id].status = status;
    return;
}

/* Return the number of ghosts */
unsigned int ghosts_get_number(struct ghosts *G) {
    if(G != NULL && id < G->n) {
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
    
    switch (status){
        case NORMAL:
            if(IS_WALL(G->A,pos) || IS_GHOST(G->A,pos))  return 1; break;
        default:
            if(IS_WALL(G->A,pos) || IS_GHOST(G->A,pos) || IS_PACMAN(G->A,pos))  return 1; break;
    }
     
    /*else { 
        //unsigned int i;
        //struct position p = pacman_get_position(P); 
        //check pacmanintersect … 
        //check ghost intersect … 
    }*/
    return 0; 
}
static struct position follow(struct ghosts *G,struct pacman *P, unsigned int id){
    enum direction dir;
    struct position pacP,ghostP;
    pacP.i = P->pos.i;
    pacP.j = P->pos.j;
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j= G->ghost[id].pos.j;

    if(pacP.i > ghostP.i){
        dir = RIGHT;
        ghostP.i++;
        if (legal_position(G,P,ghostP,G->ghost[id].status)) G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.i < ghostP.i) {
        dir = LEFT;
        ghostP.i--;
        if (legal_position(G,P,ghostP,G->ghost[id].status)) G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.j > ghostP.j){
        dir = DOWN;
        ghostP.j++;
        if (legal_position(G,P,ghostP,G->ghost[id].status))  G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.j < ghostP.j){
        dir = UP;
        ghostP.j--;
        if (legal_position(G,P,ghostP,G->ghost[id].status))  G->ghost[id].pos = ghostP;
        return ghostP;
    }
    dir = UNK_DIRECTION;
    return ghostP;    
}
static struct position escape(struct ghosts *G,struct pacman *P, unsigned int id){
    enum direction dir;
    struct position pacP,ghostP;
    pacP.i = P->pos.i;
    pacP.j = P->pos.j;
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j= G->ghost[id].pos.j;

    if(pacP.i > ghostP.i){
        dir = LEFT;
        ghostP.i--;
        if (legal_position(G,P,ghostP,G->ghost[id].status)) G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.i < ghostP.i) {
        dir = RIGHT;
        ghostP.i++;
        if (legal_position(G,P,ghostP,G->ghost[id].status)) G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.j > ghostP.j){
        dir = UP;
        ghostP.j--;
        if (legal_position(G,P,ghostP,G->ghost[id].status))  G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.j < ghostP.j){
        dir = DOWN;
        ghostP.j++;
        if (legal_position(G,P,ghostP,G->ghost[id].status))  G->ghost[id].pos = ghostP;
        return ghostP;
    }
    dir = UNK_DIRECTION;
    return ghostP;    
}
static struct position wayhome(struct ghosts *G,struct pacman *P, unsigned int id){
 char **arena = G->A;
 struct position ghostP;
 char dir = arena[ghostP.i][ghostP.j];
    switch (dir){
        case 'U':
            G->ghost[id].pos.j--;
        break;
        case 'D':
            G->ghost[id].pos.j++;
        break;
        case 'R':
            G->ghost[id].pos.i++;
        break;
        case 'L':
            G->ghost[id].pos.i--;
        break;
        default: G.ghost[id].pos = {-1,-1};
        break;
    }
    return G->ghost[id].pos;
}

/* Move the ghost id (according to its status). Returns the new position */
struct position ghosts_move(struct ghosts *G, struct pacman *P, unsigned int id) {
    struct position p = {-1,-1};
    enum direction dir;
    if(P && G && UNK_GHOST_STATUS) return p;
    switch (G->ghost[id].status){
        //Se lo stato del fanstama `e NORMAL, lo spostamento in una cella pu`o essere effettuato solo se tale cella `e libera: 
        //*la cella non `e occupata da un altro fantasma ∗ la cella non `e occupata da un muro
        case NORMAL: 
                p = follow(G,P,id); break;
        case SCARED_NORMAL:                   //Se lo stato del fantasma non `e NORMAL, 
                p = escape(G,P,id); break;    //lo spostamento non pu`o essere effettuato in una cella occupata da un muro, da un altro fantasma e da pacman.
        case SCARED_BLINKING:
                p = escape(G,P,id); break;
        case EYES:
                p = wayhome(G,P,id);break;
        default:
                p = UNK_POSITION;   break;
    }
    return p;
}

#endif


