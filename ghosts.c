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

static int nearby_home(char** A, unsigned int nrow, unsigned int ncol, struct position pos) {
    int n = 2, a, b, i = pos.i, j = pos.j;
    for (a = -n; a <= n; a++)
        if (i + a >= 0 && i + a < nrow)
            for (b = -n; b <= n; b++)
                if (j + b >= 0 && j + b < ncol)
                    if (A[i + a][j + b] == HOME_SYM)
                        return 1;
    return 0;

}

//Controlla che i movimenti del ghost avvengano in posizioni idonee in base al suo stato
enum direction legal_movement(struct ghosts* G, struct pacman* P, struct position pos, enum direction dir, enum ghost_status status, unsigned int id) {
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
    if (IS_WALL(G->A, ghostP))    return 4;
    if (IS_GHOST(G->A, ghostP))   return 4;
    if (!(G->ghost[id].status == NORMAL) && IS_PACMAN(G->A, pacP))  return dir;
    return dir;
}

struct position ghostPosition(struct ghosts* G,enum direction dir,unsigned int id) {
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
    int i, k;
    struct position pacP, ghostP;
    enum ghost_status status;
    status = G->ghost[id].status;
    pacP = pacman_get_position(P);
    ghostP = G->ghost[id].pos;
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j = G->ghost[id].pos.j;
    static int direzione[4] = { -1,-1,-1,-1 };
    int tmp[4] = { 0,1,2,3 };

    if (nearby_home(G->A, G->nrow, G->ncol, ghostP)) { direzione[0] = UP; direzione[1] = RIGHT; direzione[2] = LEFT; direzione[3] = RIGHT; }

    for (i = 0; i < 4; i++) {
        tmp[i] = legal_movement(G, P, ghostP, i, status, id);
    }
    if ((pacP.j >= ghostP.j) && (pacP.i >= ghostP.i)) { direzione[0] = LEFT; direzione[1] = UP; direzione[2] = RIGHT; direzione[3] = DOWN; }
    else if ((pacP.j <= ghostP.j) && (pacP.i <= ghostP.i)) { direzione[0] = RIGHT; direzione[1] = DOWN;  direzione[2] = LEFT; direzione[3] = UP; }
    else if ((pacP.i >= ghostP.i) && (pacP.j <= ghostP.j)) { direzione[0] = UP; direzione[1] = LEFT;  direzione[2] = RIGHT; direzione[3] = DOWN; }
    else if ((pacP.i <= ghostP.i) && (pacP.j >= ghostP.j)) { direzione[0] = DOWN; direzione[1] = RIGHT; direzione[2] = LEFT; direzione[3] = UP; }

    for (i = 0; i < 4; i++)
        for (k = 0; k < 4; k++)
            if (direzione[i] == tmp[k]) ghostPosition(G, direzione[i], id);
}
    


//funzione per scappare da pacman, simile alla precedente funzione follow ma inversa
static struct position escape(struct ghosts* G, struct pacman* P, unsigned int id) {
    int i;
    enum direction dir;
    struct position pacP, ghostP;
    pacP = pacman_get_position(P);
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j = G->ghost[id].pos.j;
    int direzione[4] = { -1,-1,-1,-1 };
    if ((pacP.j >= ghostP.j) && (pacP.i >= ghostP.i)) { direzione[0] = LEFT; direzione[1] = UP; direzione[2] = RIGHT; direzione[3] = DOWN; }
             else if ((pacP.j <= ghostP.j) && (pacP.i <= ghostP.i)) { direzione[0] = RIGHT; direzione[1] = DOWN;  direzione[2] = LEFT; direzione[3] = UP; }
                    else if ((pacP.i >= ghostP.i) && (pacP.j <= ghostP.j)) { direzione[0] = UP; direzione[1] = LEFT;  direzione[2] = RIGHT; direzione[3] = DOWN; }
                            else if ((pacP.i <= ghostP.i) && (pacP.j >= ghostP.j)) { direzione[0] = DOWN; direzione[1] = RIGHT; direzione[2] = LEFT; direzione[3] = UP; }
    for (i = 1; i < 4; i++)
        direzione[i] = rand() % 4;

    for (i = 0; i < 4; i++) {
        dir = direzione[i];
        if (legal_movement(G, P, ghostP, dir, G->ghost[id].status, id))return G->ghost[id].pos;
    }
    return G->ghost[id].pos;
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


