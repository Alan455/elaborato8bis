#define GHOSTS_STUD
#ifdef GHOSTS_STUD


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ghosts.h"
#include "pacman.h"
#include "matrix.h"

struct ghost { 
    int id;  //questo è l’id del fantasma 
    enum ghost_status status; 
    enum direction dir; 
    struct position pos; 
}; 

struct ghosts { 
    char **A;
    unsigned int nrow; 
    unsigned int ncol; 
    unsigned int n; 
    struct ghost *ghost; 
};

/* Create the ghosts data structure */
struct ghosts *ghosts_setup(unsigned int num_ghosts) { 
    struct ghosts *G = (struct ghosts *)malloc(sizeof(struct ghosts));
    if(G != NULL) {  //se G non punta a NULL
        unsigned int i;
        G->n = num_ghosts;
        G->ghost = (struct ghost *)calloc(num_ghosts,sizeof(struct ghost));
        for(i = 0; i < G->n; i++) {
            G->ghost[i].id = i;                                         
            G->ghost[i].dir = LEFT;
            G->ghost[i].status = UNK_GHOST_STATUS;  //status sconosciuto preso da una enum
            G->ghost[i].pos.i = -1; //se le coordinate sono libere asseggno le coordinate al fantasma
            G->ghost[i].pos.j = -1;
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
        G->ncol = ncol;
        G->nrow = nrow;
        G->A = A;
    }
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
    if(G != NULL) {
        unsigned int numGhost = G->n;
    return numGhost;
    }
}

/* Return the position of the ghost id. */
struct position ghosts_get_position(struct ghosts *G, unsigned int id) {
    struct position p; 
    if(G != NULL && id < G->n)   p= G->ghost[id].pos;
                            else {p.i = -1; p.j = -1;}
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
            printf("STATUS NORMAL\n");
            printf("%c\n",G->A[15]);
            if(IS_WALL(G->A,pos) || IS_GHOST(G->A,pos)) return 0; break;
        case SCARED_NORMAL:       
            if(IS_WALL(G->A,pos) || IS_GHOST(G->A,pos) || IS_PACMAN(G->A,pos)) return 0; break;
        case SCARED_BLINKING:
            if(IS_WALL(G->A,pos) || IS_GHOST(G->A,pos) || IS_PACMAN(G->A,pos))  return 0; break;
        case EYES:
            if(IS_WALL(G->A,pos) || IS_GHOST(G->A,pos) || IS_PACMAN(G->A,pos))  return 0; break;
    }
    return 1;
}

static struct position follow(struct ghosts *G,struct pacman *P, unsigned int id){
    enum direction dir;
    struct position pacP,ghostP;
    pacP = pacman_get_position(P);
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j= G->ghost[id].pos.j;
    printf("Le coordinate di pacman sono: >>%d:%d<<",pacP.i,pacP.j);
    if(pacP.j > ghostP.j){
        if ((ghostP.i == 14) && ((pacP.j-ghostP.j)> 13)) {dir = LEFT; ghostP.j--;}
        else {dir = RIGHT;  ghostP.j++;}
        if (legal_position(G,P,ghostP,G->ghost[id].status)){G->ghost[id].pos = ghostP;  return ghostP;}
         else if (dir == RIGHT) ghostP.j--;
            else ghostP.j++;
    }
    if (pacP.j < ghostP.j) {
        
        if ((ghostP.i == 14) && ((ghostP.j-pacP.j)> 13)) {dir = RIGHT; ghostP.j++;}
            else {dir = LEFT;   ghostP.j--;}
        if (legal_position(G,P,ghostP,G->ghost[id].status)){G->ghost[id].pos = ghostP;  return ghostP;}
            else if(dir == LEFT) ghostP.j++;
                else ghostP.j--;
    } 
    if (pacP.i > ghostP.i){
        //printf("Ghost va giu");
        dir = DOWN;
        ghostP.i++;
        if (legal_position(G,P,ghostP,G->ghost[id].status)){
            G->ghost[id].pos = ghostP;
            return ghostP;
        } else ghostP.i--;
    } 
    if (pacP.i < ghostP.i){
        //printf("Ghost va su");
        dir = UP;
        ghostP.i--;
        if (legal_position(G,P,ghostP,G->ghost[id].status)) {
            G->ghost[id].pos = ghostP;
            return ghostP;
        } else ghostP.i++;
    }

    dir = UNK_DIRECTION;
    return ghostP;    
}
static struct position escape(struct ghosts *G,struct pacman *P, unsigned int id){
    enum direction dir;
    struct position pacP,ghostP;
    pacP = pacman_get_position(P);
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j= G->ghost[id].pos.j;
    if(pacP.j > ghostP.j){
        dir = LEFT;
        ghostP.j--;
        if (legal_position(G,P,ghostP,G->ghost[id].status)) G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.j < ghostP.j) {
        dir = RIGHT;
        ghostP.j++;
        if (legal_position(G,P,ghostP,G->ghost[id].status)) G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.i > ghostP.i){
        dir = UP;
        ghostP.i--;
        if (legal_position(G,P,ghostP,G->ghost[id].status))  G->ghost[id].pos = ghostP;
        return ghostP;
    } else if (pacP.i < ghostP.i){
        dir = DOWN;
        ghostP.i++;
        if (legal_position(G,P,ghostP,G->ghost[id].status))  G->ghost[id].pos = ghostP;
        return ghostP;
    }
    dir = UNK_DIRECTION;
    return ghostP;    
}
static struct position wayhome(struct ghosts *G,struct pacman *P, unsigned int id){
 struct position;
 unsigned int ghostPi,ghostPj;
 ghostPi = G->ghost[id].pos.i;
 ghostPj = G->ghost[id].pos.j;
 char dir = G->A[ghostPi][ghostPj];
 //printf("Di qui passa il Test ghost case 2) Nella cella >%d:%d< c'è: >%c<",ghostPi,ghostPj,dir);
    switch (dir){
        case 'U':
            G->ghost[id].pos.i--;
        break;
        case 'D':
            G->ghost[id].pos.i++;
        break;
        case 'R':
            G->ghost[id].pos.j++;
        break;
        case 'L':
            G->ghost[id].pos.j--;
        break;
        default: 
            G->ghost[id].pos.i = -1;
            G->ghost[id].pos.j = -1;
        break;
    }
    return G->ghost[id].pos;
}

/* Move the ghost id (according to its status). Returns the new position */
struct position ghosts_move(struct ghosts *G, struct pacman *P, unsigned int id) {
    struct position pos,p = {-1,-1};
    enum direction dir;
    enum ghost_status status;
    pos = G->ghost[id].pos;
    status = ghosts_get_status(G,id);
    printf("Posizione: >>%d:%d<<\n",pos.i,pos.j);
    if(legal_position(G,P,pos,status)) {

        switch (status){
            case NORMAL: 
                printf("STATUS NORMAL\n");
                    p = follow(G,P,id); break;
            case SCARED_NORMAL: 
                printf("STATUS SCARED\n");
                    p = escape(G,P,id); break;    
            case SCARED_BLINKING:
                printf("STATUS SCARED\n");
                    p = escape(G,P,id); break;
            case EYES:
                printf("STATUS EYES\n");
                    p = wayhome(G,P,id);break;
            case UNK_GHOST_STATUS:
                printf("STATUS UNKNOWN\n");
                    break;
        }
    }
    return p;
}

#endif


