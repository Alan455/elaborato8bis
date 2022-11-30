#define GHOSTS_STUD
#ifdef GHOSTS_STUD

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ghosts.h"
#include "pacman.h"
#include "matrix.h"

static const struct position UNK_POSITION = { -1,-1 };

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
    //riservo la memoria necessaria a contenere la struttura struct ghosts
    struct ghosts *G = (struct ghosts *)malloc(sizeof(struct ghosts));
    if(G != NULL) {  //se G non punta a NULL
        unsigned int i;
        G->n = num_ghosts;
        //riservo la memoria necessaria a contenere num_ghosts*struct ghost dati e la punto a g.ghost
        G->ghost = (struct ghost *)calloc(num_ghosts,sizeof(struct ghost));
        //inizializzo i vari campi dei ghost
        for(i = 0; i < G->n; i++) {
            G->ghost[i].id = i;                                         
            G->ghost[i].dir = UNK_DIRECTION;
            G->ghost[i].status = UNK_GHOST_STATUS;  //status sconosciuto preso da una enum
            G->ghost[i].pos = UNK_POSITION; //se le coordinate sono libere asseggno le coordinate al fantasma
        }
    }
    return G;
}
/* Destroy the ghost data structure */
void ghosts_destroy(struct ghosts *G) {
    if (G != NULL){
        //libero la memoria puntata dal puntatore ghost
        free(G->ghost); 
        //libero la memoria puntata dal puntatore G
        free(G); 
    }
return;
}
/* Set the arena (A) matrix */
void ghosts_set_arena(struct ghosts *G, char **A, unsigned int nrow,unsigned int ncol) {
    if (G != NULL) {
        //inserisco nel campo A di ghosts il puntatore che punta alla matrice dell'arena
        G->A = A;
        //inizializzo i valori max di riga e colonna della matrice
        G->ncol = ncol;
        G->nrow = nrow; 
    }
}
/* Set the position of the ghost with that id. */
void ghosts_set_position(struct ghosts *G, unsigned int id, struct position pos) {
    if(G != NULL && id < G->n ) G->ghost[id].pos = pos;
    return;
}
/* Set the status of the ghost id. */
void ghosts_set_status(struct ghosts *G, unsigned int id, enum ghost_status status) {
    if(G != NULL && id < G->n) G->ghost[id].status = status;
    return;
}
/* Return the number of ghosts */
unsigned int ghosts_get_number(struct ghosts *G) {
    if(G != NULL) return G->n;;
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
    return stat;
}
struct position ghosts_move(struct ghosts *G, struct pacman*P, unsigned int id) {
     switch(ghosts_get_status(G,id)) {
        case NORMAL: 
            return ghost_move_normal(G,P,id); 
        case SCARED_NORMAL: 
            return ghost_move_scared(G,P,id); 
        case SCARED_BLINKING: 
            return ghost_move_scared(G,P,id); 
        case EYES: 
            return ghost_move_eyes(G,P,id); 
        case UNK_GHOST_STATUS:
            return UNK_GHOST_STATUS; 
    } 
}
static int legal_position(struct ghosts *G, struct pacman*P, struct position pos, enum ghost_status status) { 
    if(IS_WALL(G->A,pos)) { 
        return 0; 
    } else { 
        unsigned int i; 
        struct position p= pacman_get_position(P); 
        IS_PACMAN(G->A,pos);    // check pacmanintersect … 
        IS_PACMAN(G->A,pos);  // check ghost intersect … } 
        return 1; 
    } 
}
static struct position ghost_move(struct ghosts *G, struct pacman*P, unsigned int id, enum direction dir[]) {
     struct position pos = G->ghost[id].pos; 
     int d; 
    for(d = 0; d < 4; d++) { 
        struct position new = new_position(pos,dir[d],G->nrow,G->ncol); 
        if(legal_position(G,P,new,G->ghost[id].status)) {
            G->ghost[id].pos = new; 
            G->ghost[id].dir = dir[d]; 
            return new;
        }
    } 
    return pos; 
}
static void setup_remaining_dir(struct ghosts *G, unsigned int id, enum direction dir[]) { 
    enum direction ghostdir = G->ghost[id].dir;
    int tmp[4] = {0}, d, i; 
    dir[1] = dir[2] = dir[3] = UNK_DIRECTION; 
    tmp[dir[0]] = 1; 
    if (dir[0] != ghostdir) { //top priority
            dir[1] = ghostdir;
            tmp[dir[1]] = 1;
    } else { //Lower priority to the direction opposite to the current one … 
        dir[2] = ghostdir;
        tmp[dir[2]] = 1;
    } 
    for(i=0; i<=3 ; i++)
    if(dir[i] == UNK_DIRECTION){
        do{
            d = rand() % 4;
        } while (tmp[d] == 1);
        dir[i]= d;
        tmp[d] = 1;
    }
}
static int nearby_home(char **A, unsigned int nrow, unsigned int ncol, struct position pos) {
    int n = 2, a, b, i = pos.i, j=pos.j; 
    for(a=-n; a<=n; a++) 
        if(i+a >=0 && i+a<nrow) 
            for(b=-n; b<=n; b++) 
                if(j+b >=0 && j+b<ncol) 
                    if(A[i+a][j+b] == HOME_SYM) 
                        return 1;
    return 0;
                    
}

static void select_dir_towards(struct ghosts *G, struct pacman*P, unsigned int id, enum direction dir[]) {
     struct position g = ghosts_get_position(G,id); 
     struct position p = pacman_get_position(P); 
     if(nearby_home(G->A,G->nrow,G->ncol,g)) dir[0] = UP; 
        else if(g.i== p.i) dir[0] = LEFT;
            else if(g.j== p.j) dir[0] = DOWN;
                else dir[0] = RIGHT;
                dir[0] = G->ghost[id].dir;
     setup_remaining_dir(G,id,dir);
}
static void select_dir_away(struct ghosts *G, struct pacman*P, unsigned int id, enum direction dir[]) { 
    struct position g = ghosts_get_position(G,id); 
    struct position p = pacman_get_position(P); 
    if(nearby_home(G->A,G->nrow,G->ncol,g)) dir[0] = DOWN; 
        else if(g.i== p.i) dir[0] = RIGHT; 
            else if(g.j== p.j)dir[0] = UP; 
                else dir[0] = LEFT; 
                dir[0] = G->ghost[id].dir;
    setup_remaining_dir(G,id,dir); 
}
static void select_dir_home(struct ghosts *G, unsigned int id, enum direction dir[]) {
     int i  = G->ghost[id].pos.i; 
     int j =  G->ghost[id].pos.j; 
     char c = G->A[i][j]; 
     dir[0] = c == 'L' ? LEFT: c == 'R' ? RIGHT : c == 'U' ? UP : DOWN; 
     setup_remaining_dir(G,id,dir);
}

static struct position ghost_move_normal(struct ghosts *G, struct pacman *P, unsigned int id) {
    //select_dir_towards(G,P,id,dir);
    return ghost_move(G,P,id,dir); 
}

static struct position ghost_move_scared(struct ghosts *G, struct pacman *P, unsigned int id) { 
    //select_dir_away(G,P,id,dir);
    return ghost_move(G,P,id,dir); 
}

static struct position ghost_move_eyes(struct ghosts *G, struct pacman *P, unsigned int id) {
    //select_dir_home(G,P,id,dir);
    return ghost_move(G,P,id,dir); 
}

#endif


