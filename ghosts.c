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
            if (G->ghost != NULL){
                for (i = 0; i < G->n; i++) {
                    G->ghost[i].id = i;
                    G->ghost[i].dir = UNK_DIRECTION;
                    G->ghost[i].status = UNK_GHOST_STATUS;
                    G->ghost[i].pos = UNK_POSITION;
                }
            }
        return G;
        }
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
    if (G != NULL) return G->n;;
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
    enum ghost_status stat;
    if (G != NULL && id < G->n)   stat = G->ghost[id].status;
    return stat;
}

//in base allo STATUS del ghost controllo se la posizione attuale in cui andiamo a mettere il ghost è legale, senza controllare eventuali direzioni di movimento
static int legal_position(struct ghosts* G, struct pacman* P, struct position pos, enum ghost_status status, unsigned int id) {
    struct position pacP, ghostP;
    int ghostPi, ghostPj;
    ghostP = pos;
    ghostPi = pos.i;
    ghostPj = pos.j;
    int i, r, c;
    pacP = pacman_get_position(P);
    if (IS_WALL(G->A, ghostP))    return 0;
    if (IS_GHOST(G->A, ghostP))   return 0;
        if (!(G->ghost[id].status == NORMAL) && IS_PACMAN(G->A, ghostP))  return 0;
    G->ghost[id].pos = ghostP;
    return 1;
}

//Controlla che i movimenti del ghost avvengano in posizioni idonee in base al suo stato
static int legal_movement(struct ghosts* G, struct pacman* P, struct position pos, enum direction dir, enum ghost_status status, unsigned int id) {
    struct position ghostP;
    int rows, cols, ghostPi, ghostPj;
    ghostP = pos;
    ghostPi = ghostP.i;
    ghostPj = ghostP.j;
    rows = G->nrow;
    cols = G->ncol;
    //questo switch modifica il valore del campo pos in base alla direzione indicata da dir
    switch (dir) {
    case UP: ghostPi = (ghostPi - 1 + rows) % rows; break; //se UP decremento il valore Riga
    case DOWN: ghostPi = (ghostPi + 1 + rows) % rows; break;
    case LEFT: ghostPj = (ghostPj - 1 + cols) % cols; break; //se LEFT decremento il valore colonna
    case RIGHT: ghostPj = (ghostPj + 1 + cols) % cols; break;
    case UNK_DIRECTION: break;
    default: printf("errore nella legal movement,non è stato assegnato una direzione");
        return 0;
    }

    return legal_position(G, P, ghostP, status, id);

}


//Funzione che segue pacman
static struct position follow(struct ghosts* G, struct pacman* P, unsigned int id) {
    enum direction dir;
    struct position pacP, ghostP;
    pacP = pacman_get_position(P);
    ghostP = G->ghost[id].pos;
    enum ghost_status status = G->ghost[id].status;
    //printf("STATUS NORMAL\n");
    //quattro if consecutivi, uno per ogni direzione assumibile dal ghost
    //controlla se pacman è a destra del ghost
    if (pacP.j > ghostP.j) {
        //se è vero viene settata la direzione RIGHT
        dir = RIGHT;
        //printf("dir right\n");
        //controlla se nella cella in cui si vuole muovere sono verificate le condizioni necessarie
        //in caso positivo ritorna la nuova posizione del ghost
        if (legal_movement(G, P, ghostP, dir, status, id)) return G->ghost[id].pos;
    }
    if (pacP.j < ghostP.j) {
        dir = LEFT;
        //printf("dir left\n");
        if (legal_movement(G, P, ghostP, dir, status, id))return G->ghost[id].pos;
    }
    if (pacP.i > ghostP.i) {
        dir = DOWN;
        //printf("dir down\n");
        if (legal_movement(G, P, ghostP, dir, status, id))return G->ghost[id].pos;
    }
    if (pacP.i < ghostP.i) {
        dir = UP;
        //printf("dir up\n");
        if (legal_movement(G, P, ghostP, dir, status, id))return G->ghost[id].pos;
    }
    //printf("USCITO DA NORMAL\n");
    //se non è disponibile nessuna direzione valida ritorna la posizione attuale
    return G->ghost[id].pos;
}


//funzione per scappare da pacman, simile alla precedente funzione follow ma inversa
static struct position escape(struct ghosts* G, struct pacman* P, unsigned int id) {
    enum direction dir;
    struct position pacP, ghostP;
    pacP = pacman_get_position(P);
    ghostP.i = G->ghost[id].pos.i;
    ghostP.j = G->ghost[id].pos.j;
    //printf("STATUS ESCAPE\n");
    if (pacP.j > ghostP.j) {
        dir = LEFT;
        //printf("LEFT\n");
        //printf("Eg: >>%d:%d<<\n",ghostP.i,ghostP.j);
        if (legal_movement(G, P, ghostP, dir, G->ghost[id].status, id))return G->ghost[id].pos;
    }
    else if (pacP.j < ghostP.j) {
        dir = RIGHT;
        //printf("RIGHT\n");
        if (legal_movement(G, P, ghostP, dir, G->ghost[id].status, id))return G->ghost[id].pos;
    }
    else if (pacP.i > ghostP.i) {
        dir = UP;
        //printf("UP\n");
        if (legal_movement(G, P, ghostP, dir, G->ghost[id].status, id))return G->ghost[id].pos;
    }
    else if (pacP.i < ghostP.i) {
        dir = DOWN;
        //printf("DOWN\n");
        if (legal_movement(G, P, ghostP, dir, G->ghost[id].status, id))return G->ghost[id].pos;
    }
    //se non è disponibile nessuna direzione valida ritorna la posizione attuale
    return G->ghost[id].pos;
}

//funzione per ritrovare casa
static struct position wayhome(struct ghosts* G, struct pacman* P, unsigned int id) {
    struct position ghostP;
    enum direction dir;
    enum ghost_status status;
    unsigned int ghostPi, ghostPj;
    status = G->ghost[id].status;
    ghostPi = G->ghost[id].pos.i;
    ghostPj = G->ghost[id].pos.j;
    //printf("STATUS WAYHOME\n");
    //printf("%c >>>%d:%d<<<\n",G->A[ghostPi][ghostPj],ghostPi,ghostPj);
    //lo switch prende come parametro il valore della cella di A alle coordinate attuali del ghost
    switch (G->A[ghostPi][ghostPj]) {
    case 'U':
        //modifica la nuova posizione in base al valore della cella
        //nel casu 'U' decrementa di uno l'indicatore di riga
        dir = UP;
        //ghostP.i--;
        //printf("Direzione UP, pos:  >>>%d:%d<<<\n ",ghostPi,ghostPj);
        if ((legal_movement(G, P, ghostP, dir, status, id))) {
            //G->ghost[id].pos.i--;
            return  G->ghost[id].pos;
        }
        break;
    case 'D':
        dir = DOWN;
        //ghostP.i++;
        //printf("Direzione DOWN, pos:  >>>%d:%d<<<\n ",ghostPi,ghostPj);
        if ((legal_movement(G, P, ghostP, dir, status, id))) {
            //G->ghost[id].pos.i++;
            return  G->ghost[id].pos;
        }
        break;
    case 'R':
        dir = RIGHT;
        //ghostP.j++;
        //printf("Direzione RIGHT, pos:  >>>%d:%d<<<\n ",ghostPi,ghostPj);
        if ((legal_movement(G, P, ghostP, dir, status, id))) {
            //G->ghost[id].pos.j++;
            return  G->ghost[id].pos;
        }
        break;
    case 'L':
        dir = LEFT;
        //ghostP.j--;
        //printf("Direzione LEFT, pos:  >>>%d:%d<<<\n ",ghostPi,ghostPj);
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
    //ritorna la nuova posizione di ghost
    return G->ghost[id].pos;
}

//Move the ghost id (according to its status). Returns the new position 
struct position ghosts_move(struct ghosts* G, struct pacman* P, unsigned int id) {
    struct position pos;
    enum ghost_status status;
    //variabile pos assume le coordinate del ghost
    pos = G->ghost[id].pos;
    //variabile status assume lo status del ghost
    status = G->ghost[id].status;
    //se la funzione legal position ritorna 1 come valore, vuol dire che la posizione attuale del ghost è accettabile e posso entrare nello switch
    //printf("GHOSTMOVE\n");
    //printf("G: >>%d:%d<<\n",pos.i,pos.j);
    if ((legal_position(G, P, pos, status, id))) {
        switch (status) {
            //se lo status è normal richiamo la follow che dovrebbe seguire pacman
        case NORMAL:
            pos = follow(G, P, id); break;
            //se lo stato è scared normal o blinking richiamo la escape che dovrebbe scappare da pacman
        case SCARED_NORMAL:
            pos = escape(G, P, id); break;
        case SCARED_BLINKING:
            pos = escape(G, P, id); break;
            //se lo stato è eyes richiamo la way home che dovrebbe seguire il path per riportare il ghost a casa
        case EYES:
            pos = wayhome(G, P, id); break;
        case UNK_GHOST_STATUS:
            break;
        }
    }
    else pos = UNK_POSITION;
    //ritorno la nuova posizione del ghost che è variata solo se le condizioni sono risultate idonee
    G->ghost[id].pos = pos;
    return pos;
}

#endif


