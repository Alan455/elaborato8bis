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
    char** A;
    unsigned int nrow;
    unsigned int ncol;
    unsigned int n;
    struct ghost* ghost;
};

//Create the ghosts data structure 
struct ghosts* ghosts_setup(unsigned int num_ghosts) {
    struct ghosts* G = (struct ghosts*)malloc(sizeof(struct ghosts));
    if (G != NULL) {  //se G non punta a NULL
        unsigned int i;
        G->n = num_ghosts;
        G->ghost = (struct ghost*)calloc(num_ghosts, sizeof(struct ghost));
        if (G->ghost != NULL) {
            for (i = 0; i < G->n; i++) {
                G->ghost[i].id = i;
                G->ghost[i].dir = UNK_DIRECTION;
                G->ghost[i].status = UNK_GHOST_STATUS;
                G->ghost[i].pos = UNK_POSITION;
            }
        }
    }
    return G;
}

//Destroy the ghost data structure 
void ghosts_destroy(struct ghosts* G) {
    if (G != NULL) {
        //libero la memoria puntata dal puntatore ghost
        free(G->ghost);
        //libero la memoria puntata dal puntatore G
        free(G);
    }
    return;
}

//Set the arena (A) matrix 
void ghosts_set_arena(struct ghosts* G, char** A, unsigned int nrow, unsigned int ncol) {
    if (G != NULL) {
        //inserisco nel campo A di ghosts il puntatore che punta alla matrice dell'arena
        G->A = A;
        //inizializzo i valori max di riga e colonna della matrice
        G->ncol = ncol;
        G->nrow = nrow;
    }
}


//Set the position of the ghost with that id. 
void ghosts_set_position(struct ghosts* G, unsigned int id, struct position pos) {
    if (G != NULL && id < G->n) G->ghost[id].pos = pos;
    return;
}

//Set the status of the ghost id. 
void ghosts_set_status(struct ghosts* G, unsigned int id, enum ghost_status status) {
    if (G != NULL && id < G->n) G->ghost[id].status = status;
    return;
}

//Return the number of ghosts 
unsigned int ghosts_get_number(struct ghosts* G) {
    if (G != NULL) return G->n;
    return 0;
}


//Return the position of the ghost id. 
struct position ghosts_get_position(struct ghosts* G, unsigned int id) {
    struct position p;
    if (G != NULL && id < G->n)   p = G->ghost[id].pos;
    else p = UNK_POSITION;
    return p;

}

//Return the status of the ghost id. 
enum ghost_status ghosts_get_status(struct ghosts* G, unsigned int id) {
    if (G != NULL && id < G->n) return G->ghost[id].status;
    return UNK_GHOST_STATUS;
}

static int nearby_home(struct ghosts *G, unsigned int nrow, unsigned int ncol, struct position pos) {
    int n = 2, a, b, i = pos.i, j = pos.j;
    char** A;
    A = G->A;
    for (a = -n; a <= n; a++)
        if (i + a >= 0 && i + a < nrow)
            for (b = -n; b <= n; b++)
                if (j + b >= 0 && j + b < ncol)
                    if (A[i + a][j + b] == HOME_SYM)
                        return 1;
    return 0;

}

enum direction legal_dir(struct ghosts* G, struct pacman* P, struct position pos, enum direction dir, enum ghost_status status, unsigned int id) {
    struct position pacP, ghostP;
    struct position ghostP;
    int rows, cols, ghostPi, ghostPj;
    pacP = pacman_get_position(P);
    ghostP = pos;
    ghostPi = ghostP.i;
    ghostPj = ghostP.j;
    rows = G->nrow;
    cols = G->ncol;
    switch (dir) {
    case UP: ghostP.i = (ghostPi - 1 + rows) % rows; break; //se UP decremento il valore Riga
    case DOWN: ghostP.i = (ghostPi + 1 + rows) % rows; break;
    case LEFT: ghostP.j = (ghostPj - 1 + cols) % cols; break; //se LEFT decremento il valore colonna
    case RIGHT: ghostP.j = (ghostPj + 1 + cols) % cols; break;
    case UNK_DIRECTION: break;
    default: printf("errore nella legal movement,non è stato assegnato una direzione");
        return 4;
    }
    if (IS_WALL(G->A, ghostP))    return 0;
    if (IS_GHOST(G->A, ghostP))   return 0;
    if (!(G->ghost[id].status == NORMAL) && IS_PACMAN(G->A, pacP))  return dir;
    if (IS_PACMAN(G->A, pacP)) return 0;
    return dir;
}

enum direction choose_dir(struct ghosts* G, struct pacman* P) {
    int i, k, id;
    struct position pacP, ghostP;
    enum ghost_status status;
    id = G->ghost->id;
    status = G->ghost[id].status;
    pacP = pacman_get_position(P);
    ghostP = G->ghost[id].pos;
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j = G->ghost[id].pos.j;
    static int direzione;
    int tmp[4] = {0,1,2,3};

    if (nearby_home(G->A, G->nrow, G->ncol, ghostP))
       if (legal_dir(G, P, ghostP, UP, status, id))
            direzione = UP;
       
    if (legal_dir(G, P, ghostP, direzione, status, id)) return direzione;

    for (i = 0; i < 4; i++) {
            tmp[i] = legal_dir(G, P, ghostP, i, status, id);
    }

    switch (direzione){
        case 0:
            if (tmp[1]) direzione = tmp[1];
            if (tmp[3]) direzione = tmp[3];
            if (tmp[2]) direzione = tmp[2];
            break;
        case 1:
            if (tmp[0] == 0) direzione = tmp[0];
            if (tmp[2]) direzione = tmp[2];
            if (tmp[3]) direzione = tmp[3];
            break;
        case 2:
            if (tmp[1]) direzione = tmp[1];
            if (tmp[3]) direzione = tmp[3];
            if (tmp[0] == 0) direzione = tmp[0];
            break;
        case 3:
            if (tmp[0] == 0) direzione = tmp[0];
            if (tmp[2]) direzione = tmp[2];
            if (tmp[1]) direzione = tmp[1];
            break;

    }

}

struct position return_pos(struct ghosts* G,enum direction dir,unsigned int id) {
    struct position ghostP;
    ghostP = G->ghost[id].pos;
    int rows, cols, ghostPi, ghostPj;
    ghostPi = ghostP.i;
    ghostPj = ghostP.j;
    rows = G->nrow;
    cols = G->ncol;
    switch (dir) {
    case UP: ghostP.i = (ghostPi - 1 + rows) % rows; break; //se UP decremento il valore Riga
    case DOWN: ghostP.i = (ghostPi + 1 + rows) % rows; break;
    case LEFT: ghostP.j = (ghostPj - 1 + cols) % cols; break; //se LEFT decremento il valore colonna
    case RIGHT: ghostP.j = (ghostPj + 1 + cols) % cols; break;
    }
    return ghostP;
}

//Funzione che segue pacman
static struct position follow(struct ghosts* G, struct pacman* P, unsigned int id) {
    //nearby_home(G);
}
    


//funzione per scappare da pacman, simile alla precedente funzione follow ma inversa
static struct position escape(struct ghosts* G, struct pacman* P, unsigned int id) {
 }

//funzione per ritrovare casa
static struct position wayhome(struct ghosts* G, struct pacman* P, unsigned int id) {
    struct position ghostP;
    enum direction dir;
    enum ghost_status status;
    unsigned int ghostPi, ghostPj;
    ghostP  = G->ghost[id].pos;
    status  = G->ghost[id].status;
    ghostPi = G->ghost[id].pos.i;
    ghostPj = G->ghost[id].pos.j;
    switch (G->A[ghostPi][ghostPj]) {
    case 'U':
        //modifica la nuova posizione in base al valore della cella
        //nel casu 'U' decrementa di uno l'indicatore di riga
        dir = UP;
        //ghostP.i--;
        if ((legal_movement(G, P, ghostP, dir, status, id))) {
            //G->ghost[id].pos.i--;
            return  G->ghost[id].pos;
        }
        break;
    case 'D':
        dir = DOWN;
        //ghostP.i++;
        if ((legal_movement(G, P, ghostP, dir, status, id))) {
            //G->ghost[id].pos.i++;
            return  G->ghost[id].pos;
        }
        break;
    case 'R':
        dir = RIGHT;
        //ghostP.j++;
        if ((legal_movement(G, P, ghostP, dir, status, id))) {
            //G->ghost[id].pos.j++;
            return  G->ghost[id].pos;
        }
        break;
    case 'L':
        dir = LEFT;
        //ghostP.j--;
        if ((legal_movement(G, P, ghostP, dir, status, id))) {
            //G->ghost[id].pos.j--;
            return  G->ghost[id].pos;
        }
        break;
        //se è arrivato a casa
    case HOME_SYM:
        //G->ghost[id].dir = UP;   //direzione sconosciuta
        G->ghost[id].status = NORMAL;   //status normal
        break;
    }

    return G->ghost[id].pos;
}

//Move the ghost id (according to its status). Returns the new position 
struct position ghosts_move(struct ghosts* G, struct pacman* P, unsigned int id) {
    struct position pos;
    enum ghost_status status;
    pos = G->ghost[id].pos;
    status = G->ghost[id].status;
        switch (status) {

        case NORMAL:
            pos = follow(G, P, id); break;
    
        case SCARED_NORMAL:
            pos = escape(G, P, id); break;
        case SCARED_BLINKING:
            pos = escape(G, P, id); break;
        case EYES:
            pos = wayhome(G, P, id); break;
        case UNK_GHOST_STATUS:
            break;
        }
      //ritorno la nuova posizione del ghost che è variata solo se le condizioni sono risultate idonee
    G->ghost[id].pos = pos;
    return pos;
}

#endif


