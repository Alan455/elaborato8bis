#define GHOSTS_STUD
#ifdef GHOSTS_STUD


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ghosts.h"
#include "pacman.h"
#include "matrix.h"

static const struct position UNK_POSITION = {-1,-1}; // Set unknown position to (UINT_MAX,UINT_MAX)

struct ghosts {
    int n; //numero di fantasmi
    struct ghost ghost;
};


struct ghost {
    int id; 
   	enum ghost_status status;
	enum direction dir;
	struct position pos;
    //struct ghost 
    //char **A;
	unsigned int nrow;
	unsigned int ncol;
};

/* Create the ghosts data structure */
struct ghosts *ghosts_setup(unsigned int num_ghosts) { 
    struct ghosts *G = (struct ghosts *)malloc(sizeof(struct ghosts));
    //srand(time(NULL));
    if(G != NULL) {                 //se G non punta a NULL
        unsigned int i;
        //G->n = num_ghosts;                                                  // al campo n di G assegniamo num_ghosts
        G->ghost = (struct ghost *)calloc(num_ghosts,sizeof(struct ghost)); //al campo ghost di G assegniamo dinamicamente la memoria
        for(i = 0; i < G->n; i++) {                     //per ogni ghost presente assegniamo una UNK_POSITION e una direzione casuale
            G->ghost[i]->pos   = UNK_POSITION ;
            G->ghost->dir   = UNK_DIRECTION;
            G->ghost->id    = i;
            G->ghost->status = UNK_GHOST_STATUS;*/
        }
    }
    return G;
}

/* Destroy the ghost data structure */
void ghosts_destroy(struct ghosts *G) {
    return;
    
}

/* Set the arena (A) matrix */
void ghosts_set_arena(struct ghosts *G, char **A, unsigned int nrow, 
                                                      unsigned int ncol) {
    return;                                                      
}

/* Set the position of the ghost with that id. */
void ghosts_set_position(struct ghosts *G, unsigned int id, struct position pos) {
    return;
}

/* Set the status of the ghost id. */
void ghosts_set_status(struct ghosts *G, unsigned int id, enum ghost_status status) {
    return;
}

/* Return the number of ghosts */
unsigned int ghosts_get_number(struct ghosts *G) {
    return 0;
}

/* Return the position of the ghost id. */
struct position ghosts_get_position(struct ghosts *G, unsigned int id) {
    struct position p; 
    return p;

}

/* Return the status of the ghost id. */
enum ghost_status ghosts_get_status(struct ghosts *G, unsigned int id) {
    return 0;
}

/* Move the ghost id (according to its status). Returns the new position */
struct position ghosts_move(struct ghosts *G, struct pacman *P, unsigned int id) {
    struct position p; 
    return p;
}

#endif


