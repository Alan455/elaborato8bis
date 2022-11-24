#include <stdio.h>
#include <stdlib.h>

//#define DEBUG
//#define FILE_OUTPUT

#define MAIN8
#ifdef MAIN8
#include "ghosts.h"
#include "global.h"
#include "matrix.h"
#include "pacman.h"

#define NROWS 31
#define NCOLS 28

char m1[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx            xx            xx xxxx xxxxx xx xxxxx xxxx xx xxxx xxxxx xx xxxxx xxxx xx xxxx xxxxx xx xxxxx xxxx xx                          xx xxxx xx xxxxxxxx xx xxxx xx xxxx xx xxxxxxxx xx xxxx xx      xx    xx    xx      xxxxxxx xxxxx xx xxxxx xxxxxxxxxxxx xxxxx xx xxxxx xxxxxxxxxxxx xx    G     xx xxxxxxxxxxxx xx xx    xx xx xxxxxxxxxxxx xx x      x xx xxxxxx          x  XX  x          xxxxxx xx xG  G Gx xx xxxxxxxxxxxx xx xxxxxxxx xx xxxxxxxxxxxx xx          xx xxxxxxxxxxxx xx xxxxxxxx xx xxxxxxxxxxxx xx xxxxxxxx xx xxxxxxx            xx            xx xxxx xxxxx xx xxxxx xxxx xx xxxx xxxxx xx xxxxx xxxx xx   xx        <       xx   xxxx xx xx xxxxxxxx xx xx xxxxxx xx xx xxxxxxxx xx xx xxxx      xx    xx    xx      xx xxxxxxxxxx xx xxxxxxxxxx xx xxxxxxxxxx xx xxxxxxxxxx xx                          xxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char p1[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxRRRRRDLLLRRDxxDLLLRRDLLLLLxxDxxxxDxxxxxDxxDxxxxxDxxxxDxxDxxxxDxxxxxDxxDxxxxxDxxxxDxxDxxxxDxxxxxDxxDxxxxxDxxxxDxxRRRRRRRRDLLLLRRRRDLLLLLLLLxxUxxxxUxxDxxxxxxxxDxxUxxxxUxxUxxxxUxxDxxxxxxxxDxxUxxxxUxxRRRRRUxxRRRDxxDLLLxxULLLLLxxxxxxxDxxxxxDxxDxxxxxDxxxxxxxxxxxxDxxxxxDxxDxxxxxDxxxxxxxxxxxxDxxRRRRDDLLLLxxDxxxxxxxxxxxxDxxUxxRDDLxxUxxDxxxxxxxxxxxxDxxUxRRDDLLxUxxDxxxxxxRRRRRRRRRUxRRXXLLxULLLLLLLLLxxxxxxUxxUxRRUULLxUxxUxxxxxxxxxxxxUxxUxxxxxxxxUxxUxxxxxxxxxxxxUxxULLLLRRRRUxxUxxxxxxxxxxxxUxxUxxxxxxxxUxxUxxxxxxxxxxxxUxxUxxxxxxxxUxxUxxxxxxxRRRRRRRRULLLxxRRRULLLLLLLLxxUxxxxUxxxxxUxxUxxxxxUxxxxUxxUxxxxUxxxxxUxxUxxxxxUxxxxUxxULLxxULLLRRULRULLLRRUxxRRUxxxxDxxUxxUxxxxxxxxUxxUxxDxxxxxxDxxUxxUxxxxxxxxUxxUxxDxxxxRRRRRUxxULLLxxRRRUxxULLLLLxxUxxxxxxxxxxUxxUxxxxxxxxxxUxxUxxxxxxxxxxUxxUxxxxxxxxxxUxxULLLLLLRRRRULRULLLLLRRRRRUxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";


#define NROWS2 29
#define NCOLS2 25

char m2[] = "xxxxxx xxx xxx xxx xxxxxxxxxxxx xxx xxx xxx xxxxxxxx                     xxxx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xx                         xx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xx        G       G        xx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xx           XXX           xx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xx        G       G        xx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xx            <            xx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xxxx xxx xxx xxx xxx xxx xxxx                     xxxxxxxx xxx xxx xxx xxxxxxxxxxxx xxx xxx xxx xxxxxx";
char p2[] = "xxxxxxDxxxDxxxDxxxDxxxxxxxxxxxxDxxxDxxxDxxxDxxxxxxxxRRRRRRRRDLLRDLLLLLLLLxxxxDxxxDxxxDxxxDxxxDxxxDxxxxDxxxDxxxDxxxDxxxDxxxDxxxxDxxxDxxxDxxxDxxxDxxxDxxRRRRRRRRRRDLLRDLLLLLLLLLLxxDxxxDxxxDxxxDxxxDxxxDxxxxDxxxDxxxDxxxDxxxDxxxDxxxxDxxxDxxxDxxxDxxxDxxxDxxRRRRRRRRRRDLLRDLLLLLLLLLLxxDxxxDxxxDxxxDxxxDxxxDxxxxDxxxDxxxDxxxDxxxDxxxDxxxxDxxxDxxxDxxxDxxxDxxxDxxRRRRRRRRRRRXXXLLLLLLLLLLLxxUxxxUxxxUxxxUxxxUxxxUxxxxUxxxUxxxUxxxUxxxUxxxUxxxxUxxxUxxxUxxxUxxxUxxxUxxRRRRRRRRRRULLRULLLLLLLLLLxxUxxxUxxxUxxxUxxxUxxxUxxxxUxxxUxxxUxxxUxxxUxxxUxxxxUxxxUxxxUxxxUxxxUxxxUxxRRRRRRRRRRULLRULLLLLLLLLLxxUxxxUxxxUxxxUxxxUxxxUxxxxUxxxUxxxUxxxUxxxUxxxUxxxxUxxxUxxxUxxxUxxxUxxxUxxxxRRRRRRRRULLRULLLLLLLLxxxxxxxxUxxxUxxxUxxxUxxxxxxxxxxxxUxxxUxxxUxxxUxxxxxx";

static const struct position UNK_POSITION = { -1,-1 }; // Set unknown position to (UINT_MAX,UINT_MAX)


struct arena {
	char** arena;
	char** path;
	unsigned int nrow;
	unsigned int ncol;
	struct pacman* pacman;
	struct ghosts* ghosts;
} *A = NULL;

#define EQ_POS(p,p1) ((p.i == p1.i) && (p.j == p1.j))
int ghost_legal_position(struct arena *A,int id);

/**************************************************************************************************************/
static void arena_destroy(void) {
	if (A != NULL) {
		pacman_destroy(A->pacman);
		ghosts_destroy(A->ghosts);
		matrix_free(A->arena);
		matrix_free(A->path);
		free(A);
		A = NULL;
	}
}

static int arena_setup(char* positions, char* path, unsigned int nrow, unsigned int n) {
	A = (struct arena*)calloc(1, sizeof(struct arena));
	if (A == NULL) {    
		free(A);
		return 1;
	}
	else {
		A->arena = matrix_vcopy(positions, nrow, n);
		A->path = matrix_vcopy(path, nrow, n);
		A->nrow = nrow;
		A->ncol = n;
		return 0;
	}
}

static struct pacman* setup_pacman(char** N, unsigned int nrow, unsigned int ncol, struct position pos) {
	struct pacman* P = pacman_setup();
	enum direction dir = UNK_DIRECTION;

	pacman_set_position(P, pos);
	pacman_set_direction(P, LEFT);
	pacman_set_status(P, WEAK);
	pacman_set_arena(P, N, nrow, ncol);
	return P;
}

static struct ghosts* setup_ghosts(char** N, unsigned int nrow, unsigned int ncol, int num_ghosts, struct position *gPos) {
	int i;
	struct ghosts* G = ghosts_setup(num_ghosts);
	ghosts_set_arena(G, N, nrow, ncol);
	if (gPos != NULL) {
		for (i = 0; i < num_ghosts; i++) {
			ghosts_set_position(G, i, gPos[i]);
			ghosts_set_status(G, i, NORMAL);

		}
	}
	return G;
}



/*TEST*************************************************************************************************/


int test_setup() {
	struct position pPos = { 23,14 };
	struct position gPos[4] = { {15,11},{15,14},{15,16},{11,13}};
	//test 1 setup
	int iTest, nTest = 20, errors = 0, actual = 0;
	printf("1) Test SETUP 1\n");

	arena_setup(m1, p1, NROWS, NCOLS);
	A->pacman=setup_pacman(A->path, NROWS, NCOLS, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS, NCOLS, 4, gPos);
	actual=ghosts_get_number(A->ghosts);
	if (actual != 4) {
		printf("Setup: Errore ghosts_get_number. Ris %d - Ris atteso: 4 \n", actual);
		errors++;
	}
	for (iTest = 2; iTest <= nTest; iTest += 4) {
		ghosts_destroy(A->ghosts);
		A->ghosts = ghosts_setup(iTest);
		actual=ghosts_get_number(A->ghosts);
		if (actual != iTest) {
			printf("Setup: Errore ghosts_get_number. Ris %d - Ris atteso: %d\n", actual,iTest);
			errors++;
		}
	}
		
	arena_destroy();
	return errors;
}


int test_setup2() {
	struct position pPos = { 22,12 };
	struct position gPos[4] = { {10,8},{10,16},{18,8},{18,16} };
	//test 1 setup
	int iTest, nTest = 20, errors = 0, actual;
	printf("1) Test SETUP 2\n");

	arena_setup(m2, p2, NROWS2, NCOLS2);
	A->pacman = setup_pacman(A->path, NROWS2, NCOLS2, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS2, NCOLS2, 4, gPos);
	actual=ghosts_get_number(A->ghosts);
	if (actual != 4) {
		printf("Setup: Errore ghosts_get_number. Ris %d - Ris atteso: 4\n", actual);
		errors++;
	}
	for (iTest = 2; iTest <= nTest; iTest += 4) {
		ghosts_destroy(A->ghosts);
		A->ghosts = ghosts_setup(iTest);
		actual=ghosts_get_number(A->ghosts);
		if (actual != iTest) {
			printf("Setup: Errore ghosts_get_number. Ris %d - Ris atteso: %d\n", actual,iTest);
			errors++;
		}
	}

	arena_destroy();
	return errors;
}


int test_ghost() {
	struct position pPos = { 23,14 };
	struct position gPos[10] = { {15,11},{15,13},{15,14},{15,16},{11,13},{11,17},{23,13},{23,12},{23,15},{14,27} };
	int i, iTest, iS, errors = 0;
	//test 1 setup
	printf("2) Test GHOST 1\n");
	arena_setup(m1, p1, NROWS, NCOLS);
	A->pacman = setup_pacman(A->path, NROWS, NCOLS, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS, NCOLS, 10, gPos);

	for (iS = 0; iS <= 3; iS++) {
		for (iTest = 0; iTest < 10; iTest++) {
			ghosts_set_position(A->ghosts, iTest, gPos[iTest]);
			ghosts_set_status(A->ghosts, iTest, iS);

		}
		for (i = 0; i < 30; i++) {
			for (iTest = 0; iTest < 10; iTest++) {
				ghosts_move(A->ghosts, A->pacman, iTest);
				//check se corretta	
				if (!ghost_legal_position(A,iTest)) {
					printf("Move: Errore ghosts_move\n");
					errors++;
				}
			}
		}
	}

	arena_destroy();
	return errors;
}


int test_ghost2() {
	struct position pPos = { 22,12 };
	struct position gPos[10] = { {10,8},{10,16},{18,8},{18,16} ,{6,8},{6,16},{22,8},{22,16} ,{6,1},{6,24} };
	int i, iTest, iS, errors = 0;
	//test 1 setup
	printf("2) Test GHOST 2\n");
	arena_setup(m2, p2, NROWS2, NCOLS2);
	A->pacman = setup_pacman(A->path, NROWS2, NCOLS2, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS2, NCOLS2, 10, gPos);

	for (iS = 0; iS <= 3; iS++) {
		for (iTest = 0; iTest < 10; iTest++) {
			ghosts_set_position(A->ghosts, iTest, gPos[iTest]);
			ghosts_set_status(A->ghosts, iTest, iS);

		}
		for (i = 0; i < 30; i++) {
			for (iTest = 0; iTest < 10; iTest++) {
				ghosts_move(A->ghosts, A->pacman, iTest);
				//check se corretta	
				if (!ghost_legal_position(A,iTest)) {
					printf("Move: Errore ghosts_move\n");
					errors++;
				}
			}
		}
	}
	
	arena_destroy();
	return errors;
}


//1) Casi particolari
//g[0] 14, 0->deve andare in 14, 27
//g[1] 14, 1

int test_ghost_case1() {
	struct position pPos = { 23,14 };
	struct position gPos[2] = { {14,0},{14,1} };
	struct position gAPos, gEPos = { 14,27 };
	int errors = 0;
	printf("3) Test GHOST case 1\n");
	arena_setup(m1, p1, NROWS, NCOLS);
	A->pacman = setup_pacman(A->path, NROWS, NCOLS, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS, NCOLS, 2, gPos);

	ghosts_move(A->ghosts, A->pacman, 0);
	gAPos = ghosts_get_position(A->ghosts, 0);

	if (!EQ_POS(gAPos, gEPos)) {
		printf("Errore test case 1\n pos: [%d,%d] - pos attesa: [%d,%d]\n", gAPos.i, gAPos.j, gEPos.i, gEPos.j);
		errors++;
	}

	arena_destroy();
	return errors;
}




//2) Casi particolari
//g_eye[0] 14, 0->deve andare in 14, 27
//pacman 14, 1

int test_ghost_case2() {
	struct position pPos = { 14,1 };
	struct position gPos[1] = { {14,0}};
	struct position gAPos, gEPos = { 14,27 };
	int errors = 0;
	printf("3) Test GHOST case 2\n");
	arena_setup(m1, p1, NROWS, NCOLS);
	A->pacman = setup_pacman(A->path, NROWS, NCOLS, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS, NCOLS, 1, gPos);

	ghosts_set_status(A->ghosts, 0, EYES);
	ghosts_move(A->ghosts, A->pacman, 0);
	gAPos = ghosts_get_position(A->ghosts, 0);

	if (!EQ_POS(gAPos, gEPos)) {
		printf("Errore test case 2\n pos: [%d,%d] - pos attesa: [%d,%d]\n", gAPos.i, gAPos.j, gEPos.i, gEPos.j);
		errors++;
	}

	arena_destroy();
	return errors;
}

//
//3) Casi particolari
//g_eye[0] 29, 7->deve andare in 29, 8
//pacman 29, 6
//

int test_ghost_case3() {
	struct position pPos = { 29,6 };
	struct position gPos[1] = { {29,7} };
	struct position gAPos, gEPos = { 29,8 };
	int errors = 0;
	printf("3) Test GHOST case 3\n");
	arena_setup(m1, p1, NROWS, NCOLS);
	A->pacman = setup_pacman(A->path, NROWS, NCOLS, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS, NCOLS, 1, gPos);

	ghosts_set_status(A->ghosts, 0, EYES);
	ghosts_move(A->ghosts, A->pacman, 0);
	gAPos = ghosts_get_position(A->ghosts, 0);

	if (!EQ_POS(gAPos, gEPos)) {
		printf("Errore test case 3\n pos: [%d,%d] - pos attesa: [%d,%d]\n", gAPos.i, gAPos.j, gEPos.i, gEPos.j);
		errors++;
	}

	arena_destroy();
	return errors;
}

//4) Casi particolari
//g_eye[0] 29, 1->deve stare fermo
//g_eye[1] 28, 1->deve andare in 27, 1
//pacman 29, 2

int test_ghost_case4() {
	struct position pPos = { 29,2 };
	struct position gPos[2] = { {29,1}, {28,1} };
	struct position gAPos, gEPos[2] = { { 29,1},{27,1}};
	int errors = 0;
	printf("3) Test GHOST case 4\n");
	arena_setup(m1, p1, NROWS, NCOLS);
	A->pacman = setup_pacman(A->path, NROWS, NCOLS, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS, NCOLS, 2, gPos);
	ghosts_set_status(A->ghosts, 0, EYES);
	ghosts_set_status(A->ghosts, 1, EYES);
	ghosts_move(A->ghosts, A->pacman, 0);
	gAPos = ghosts_get_position(A->ghosts, 0);
	if (!EQ_POS(gAPos, gEPos[0])) {
		printf("Errore test case 4\n pos: [%d,%d] - pos attesa: [%d,%d]\n", gAPos.i, gAPos.j, gEPos[0].i, gEPos[0].j);
		errors++;
	}
	ghosts_move(A->ghosts, A->pacman, 1);
	gAPos = ghosts_get_position(A->ghosts, 1);
	if (!EQ_POS(gAPos, gEPos[1])) {
		printf("Errore test case 4\n pos: [%d,%d] - pos attesa: [%d,%d]\n", gAPos.i, gAPos.j, gEPos[1].i, gEPos[1].j);
		errors++;
	}

	arena_destroy();
	return errors;
}



#define PrintStatus(a) (a== NORMAL ? printf("NORMAL") : ((a == SCARED_NORMAL || a ==  SCARED_BLINKING) ? printf("SCARED"): printf("EYES"))) 
#define NMOVE 20
#define NG 6

//posizione laterale


float test_fitness_ghost(int fitMat[NROWS][NCOLS], struct position pPos, struct position gPos[NG], enum ghost_status status) {

	struct position oldP, newP;
	int i, iTest, move=0,fitness = 0;
	printf("4) Test Fitness GHOST case "); PrintStatus(status); printf("\n");
	arena_setup(m1, p1, NROWS, NCOLS);
	A->pacman = setup_pacman(A->path, NROWS, NCOLS, pPos);
	A->ghosts = setup_ghosts(A->path, NROWS, NCOLS, NG, gPos);
	for (i = 0; i < NG; i++)
		ghosts_set_status(A->ghosts, i, status);
	
	for (i = 0; i < NG; i++)
		for (iTest = 0; iTest < NMOVE; iTest++) {
			oldP = ghosts_get_position(A->ghosts, i);
			ghosts_move(A->ghosts, A->pacman, i);
			newP = ghosts_get_position(A->ghosts, i);
			if (ghost_legal_position(A, i)) {
				if (fitMat[oldP.i][oldP.j] > 0) {
					fitness += (fitMat[oldP.i][oldP.j] - fitMat[newP.i][newP.j]);	//check se corretta	
					move++;
				}
			}
			else {
				printf("Move: Errore ghosts_move\n");
				if (status == SCARED_NORMAL || status == SCARED_BLINKING) 
					fitness += 2;
				else 
					fitness -= 2;
				move++;
			}
					
		}

	arena_destroy();
	if (status == SCARED_NORMAL || status == SCARED_BLINKING) fitness = -fitness;
	return (float)fitness/move;
}


//test NORMAL
float test_fitness_ghost1() {

	struct position pPos = { 14,26 };
	struct position gPos[NG] = { {14,5},{1,3},{26,21},{29,1},{5,24},{11,11} };

	int fitMat[NROWS][NCOLS] = {
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,24,23,22,21,20,19,20,21,22,23,24,25,-1,-1,24,23,22,21,20,19,18,19,20,21,22,23,-1},
	{-1,23,-1,-1,-1,-1,18,-1,-1,-1,-1,-1,24,-1,-1,23,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,22,-1},
	{-1,22,-1,-1,-1,-1,17,-1,-1,-1,-1,-1,23,-1,-1,22,-1,-1,-1,-1,-1,16,-1,-1,-1,-1,21,-1},
	{-1,21,-1,-1,-1,-1,16,-1,-1,-1,-1,-1,22,-1,-1,21,-1,-1,-1,-1,-1,15,-1,-1,-1,-1,20,-1},
	{-1,20,19,18,17,16,15,16,17,18,19,20,21,22,21,20,19,18,17,16,15,14,15,16,17,18,19,-1},
	{-1,19,-1,-1,-1,-1,14,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,13,-1,-1,-1,-1,18,-1},
	{-1,18,-1,-1,-1,-1,13,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1,12,-1,-1,-1,-1,17,-1},
	{-1,17,16,15,14,13,12,-1,-1,21,20,19,18,-1,-1,17,18,19,20,-1,-1,11,12,13,14,15,16,-1},
	{-1,-1,-1,-1,-1,-1,11,-1,-1,-1,-1,-1,17,-1,-1,16,-1,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,16,-1,-1,15,-1,-1,-1,-1,-1, 9,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 9,-1,-1,12,13,14,15,16,15,14,13,12,11,-1,-1, 8,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 8,-1,-1,11,-1,-1,16,17,16,15,-1,-1,10,-1,-1, 7,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 7,-1,-1,10,-1,18,17,18,17,16,17,-1, 9,-1,-1, 6,-1,-1,-1,-1,-1,-1},
	{ 2, 3, 4, 3, 4, 5, 6, 7, 8, 9,-1,19,18,19,18,17,18,-1, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1},
	{-1,-1,-1,-1,-1,-1, 7,-1,-1,10,-1,20,19,20,19,18,19,-1, 9,-1,-1, 6,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 8,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,10,-1,-1, 7,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 9,-1,-1,12,13,14,15,16,15,14,13,12,11,-1,-1, 8,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,10,-1,-1,13,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1, 9,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,11,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,13,-1,-1,10,-1,-1,-1,-1,-1,-1},
	{-1,17,16,15,14,13,12,13,14,15,16,17,18,-1,-1,17,16,15,14,13,12,11,12,13,14,15,16,-1},
	{-1,18,-1,-1,-1,-1,13,-1,-1,-1,-1,-1,19,-1,-1,18,-1,-1,-1,-1,-1,12,-1,-1,-1,-1,17,-1},
	{-1,19,-1,-1,-1,-1,14,-1,-1,-1,-1,-1,20,-1,-1,19,-1,-1,-1,-1,-1,13,-1,-1,-1,-1,18,-1},
	{-1,20,21,22,-1,-1,15,16,17,18,19,20,21,22,21,20,19,18,17,16,15,14,-1,-1,21,20,19,-1},
	{-1,-1,-1,23,-1,-1,16,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,15,-1,-1,22,-1,-1,-1},
	{-1,-1,-1,22,-1,-1,17,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1,16,-1,-1,21,-1,-1,-1},
	{-1,23,22,21,20,19,18,-1,-1,21,22,23,24,-1,-1,23,22,21,20,-1,-1,17,18,19,20,21,22,-1},
	{-1,24,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,24,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,23,-1},
	{-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,24,-1},
	{-1,26,27,28,29,30,31,32,31,30,29,28,27,28,27,26,27,28,29,30,31,30,29,28,27,26,25,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
	};
	return test_fitness_ghost(fitMat, pPos, gPos, NORMAL);
}


//test NORMAL
float test_fitness_ghost2() {


	struct position pPos = { 26,12 };
	struct position gPos[NG] = { {14,5},{1,3},{26,21},{29,1},{5,24},{11,11} };

	int fitMat[NROWS][NCOLS] = {
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{ -1,36,35,34,33,32,31,32,33,34,35,36,37,-1,-1,40,41,42,43,42,41,40,41,42,43,44,45,-1 },
		{ -1,35,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,36,-1,-1,39,-1,-1,-1,-1,-1,39,-1,-1,-1,-1,44,-1 },
		{ -1,34,-1,-1,-1,-1,29,-1,-1,-1,-1,-1,35,-1,-1,38,-1,-1,-1,-1,-1,38,-1,-1,-1,-1,43,-1 },
		{ -1,33,-1,-1,-1,-1,28,-1,-1,-1,-1,-1,34,-1,-1,37,-1,-1,-1,-1,-1,37,-1,-1,-1,-1,42,-1 },
		{ -1,32,31,30,29,28,27,28,29,30,31,32,33,34,35,36,37,38,39,38,37,36,37,38,39,40,41,-1 },
		{ -1,31,-1,-1,-1,-1,26,-1,-1,31,-1,-1,-1,-1,-1,-1,-1,-1,38,-1,-1,35,-1,-1,-1,-1,40,-1 },
		{ -1,30,-1,-1,-1,-1,25,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,37,-1,-1,34,-1,-1,-1,-1,39,-1 },
		{ -1,29,28,27,26,25,24,-1,-1,33,32,31,30,-1,-1,33,34,35,36,-1,-1,33,34,35,36,37,38,-1 },
		{ -1,-1,-1,-1,-1,-1,23,-1,-1,-1,-1,-1,29,-1,-1,32,-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,28,-1,-1,31,-1,-1,-1,-1,-1,31,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,21,-1,-1,24,25,26,27,28,29,30,29,28,27,-1,-1,30,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,20,-1,-1,23,-1,-1,28,29,30,31,-1,-1,26,-1,-1,29,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,19,-1,-1,22,-1,30,29,30,31,32,33,-1,25,-1,-1,28,-1,-1,-1,-1,-1,-1 },
		{ 24,23,22,21,20,19,18,19,20,21,-1,31,30,31,32,33,34,-1,24,25,26,27,28,29,30,31,32,33 },
		{ -1,-1,-1,-1,-1,-1,17,-1,-1,20,-1,32,31,32,33,34,35,-1,23,-1,-1,26,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,16,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,25,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,15,-1,-1,18,19,20,21,22,23,24,23,22,21,-1,-1,24,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,14,-1,-1,17,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,23,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,13,-1,-1,16,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1,22,-1,-1,-1,-1,-1,-1 },
		{ -1,17,16,15,14,13,12,13,14,15,14,13,12,-1,-1,15,16,17,18,19,20,21,22,23,24,25,26,-1 },
		{ -1,18,-1,-1,-1,-1,11,-1,-1,-1,-1,-1,11,-1,-1,14,-1,-1,-1,-1,-1,20,-1,-1,-1,-1,27,-1 },
		{ -1,19,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,10,-1,-1,13,-1,-1,-1,-1,-1,19,-1,-1,-1,-1,28,-1 },
		{ -1,20,19,18,-1,-1, 9, 8, 7, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,-1,-1,25,26,27,-1 },
		{ -1,-1,-1,17,-1,-1,10,-1,-1, 5,-1,-1,-1,-1,-1,-1,-1,-1,14,-1,-1,19,-1,-1,24,-1,-1,-1 },
		{ -1,-1,-1,16,-1,-1,11,-1,-1, 4,-1,-1,-1,-1,-1,-1,-1,-1,13,-1,-1,20,-1,-1,23,-1,-1,-1 },
		{ -1,17,16,15,14,13,12,-1,-1, 3, 2, 1, 0,-1,-1, 9,10,11,12,-1,-1,21,22,23,22,21,20,-1 },
		{ -1,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,19,-1 },
		{ -1,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 2,-1,-1, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1 },
		{ -1,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }
	};
	return test_fitness_ghost(fitMat, pPos, gPos, NORMAL);
}

//test SCARED
float test_fitness_ghost3() {

	struct position pPos = { 14,26 };
	struct position gPos[NG] = { {14,5},{14,27},{14,24},{12,21},{12,14},{20,15} };

	int fitMat[NROWS][NCOLS] = {
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,24,23,22,21,20,19,20,21,22,23,24,25,-1,-1,24,23,22,21,20,19,18,19,20,21,22,23,-1},
	{-1,23,-1,-1,-1,-1,18,-1,-1,-1,-1,-1,24,-1,-1,23,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,22,-1},
	{-1,22,-1,-1,-1,-1,17,-1,-1,-1,-1,-1,23,-1,-1,22,-1,-1,-1,-1,-1,16,-1,-1,-1,-1,21,-1},
	{-1,21,-1,-1,-1,-1,16,-1,-1,-1,-1,-1,22,-1,-1,21,-1,-1,-1,-1,-1,15,-1,-1,-1,-1,20,-1},
	{-1,20,19,18,17,16,15,16,17,18,19,20,21,22,21,20,19,18,17,16,15,14,15,16,17,18,19,-1},
	{-1,19,-1,-1,-1,-1,14,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,13,-1,-1,-1,-1,18,-1},
	{-1,18,-1,-1,-1,-1,13,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1,12,-1,-1,-1,-1,17,-1},
	{-1,17,16,15,14,13,12,-1,-1,21,20,19,18,-1,-1,17,18,19,20,-1,-1,11,12,13,14,15,16,-1},
	{-1,-1,-1,-1,-1,-1,11,-1,-1,-1,-1,-1,17,-1,-1,16,-1,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,16,-1,-1,15,-1,-1,-1,-1,-1, 9,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 9,-1,-1,12,13,14,15,16,15,14,13,12,11,-1,-1, 8,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 8,-1,-1,11,-1,-1,16,17,16,15,-1,-1,10,-1,-1, 7,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 7,-1,-1,10,-1,18,17,18,17,16,17,-1, 9,-1,-1, 6,-1,-1,-1,-1,-1,-1},
	{ 2, 3, 4, 3, 4, 5, 6, 7, 8, 9,-1,19,18,19,18,17,18,-1, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1},
	{-1,-1,-1,-1,-1,-1, 7,-1,-1,10,-1,20,19,20,19,18,19,-1, 9,-1,-1, 6,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 8,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,10,-1,-1, 7,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1, 9,-1,-1,12,13,14,15,16,15,14,13,12,11,-1,-1, 8,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,10,-1,-1,13,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1, 9,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,11,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,13,-1,-1,10,-1,-1,-1,-1,-1,-1},
	{-1,17,16,15,14,13,12,13,14,15,16,17,18,-1,-1,17,16,15,14,13,12,11,12,13,14,15,16,-1},
	{-1,18,-1,-1,-1,-1,13,-1,-1,-1,-1,-1,19,-1,-1,18,-1,-1,-1,-1,-1,12,-1,-1,-1,-1,17,-1},
	{-1,19,-1,-1,-1,-1,14,-1,-1,-1,-1,-1,20,-1,-1,19,-1,-1,-1,-1,-1,13,-1,-1,-1,-1,18,-1},
	{-1,20,21,22,-1,-1,15,16,17,18,19,20,21,22,21,20,19,18,17,16,15,14,-1,-1,21,20,19,-1},
	{-1,-1,-1,23,-1,-1,16,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,15,-1,-1,22,-1,-1,-1},
	{-1,-1,-1,22,-1,-1,17,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1,16,-1,-1,21,-1,-1,-1},
	{-1,23,22,21,20,19,18,-1,-1,21,22,23,24,-1,-1,23,22,21,20,-1,-1,17,18,19,20,21,22,-1},
	{-1,24,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,24,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,23,-1},
	{-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,24,-1},
	{-1,26,27,28,29,30,31,32,31,30,29,28,27,28,27,26,27,28,29,30,31,30,29,28,27,26,25,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
	};
	return test_fitness_ghost(fitMat, pPos, gPos, SCARED_NORMAL);
}

//test SCARED
float test_fitness_ghost4() {

	struct position pPos = { 26,12 };
	struct position gPos[NG] = { {14,5},{1,3},{26,21},{29,1},{5,24},{11,11} };

	int fitMat[NROWS][NCOLS] = {
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{ -1,36,35,34,33,32,31,32,33,34,35,36,37,-1,-1,40,41,42,43,42,41,40,41,42,43,44,45,-1 },
		{ -1,35,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,36,-1,-1,39,-1,-1,-1,-1,-1,39,-1,-1,-1,-1,44,-1 },
		{ -1,34,-1,-1,-1,-1,29,-1,-1,-1,-1,-1,35,-1,-1,38,-1,-1,-1,-1,-1,38,-1,-1,-1,-1,43,-1 },
		{ -1,33,-1,-1,-1,-1,28,-1,-1,-1,-1,-1,34,-1,-1,37,-1,-1,-1,-1,-1,37,-1,-1,-1,-1,42,-1 },
		{ -1,32,31,30,29,28,27,28,29,30,31,32,33,34,35,36,37,38,39,38,37,36,37,38,39,40,41,-1 },
		{ -1,31,-1,-1,-1,-1,26,-1,-1,31,-1,-1,-1,-1,-1,-1,-1,-1,38,-1,-1,35,-1,-1,-1,-1,40,-1 },
		{ -1,30,-1,-1,-1,-1,25,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,37,-1,-1,34,-1,-1,-1,-1,39,-1 },
		{ -1,29,28,27,26,25,24,-1,-1,33,32,31,30,-1,-1,33,34,35,36,-1,-1,33,34,35,36,37,38,-1 },
		{ -1,-1,-1,-1,-1,-1,23,-1,-1,-1,-1,-1,29,-1,-1,32,-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,28,-1,-1,31,-1,-1,-1,-1,-1,31,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,21,-1,-1,24,25,26,27,28,29,30,29,28,27,-1,-1,30,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,20,-1,-1,23,-1,-1,28,29,30,31,-1,-1,26,-1,-1,29,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,19,-1,-1,22,-1,30,29,30,31,32,33,-1,25,-1,-1,28,-1,-1,-1,-1,-1,-1 },
		{ 24,23,22,21,20,19,18,19,20,21,-1,31,30,31,32,33,34,-1,24,25,26,27,28,29,30,31,32,33 },
		{ -1,-1,-1,-1,-1,-1,17,-1,-1,20,-1,32,31,32,33,34,35,-1,23,-1,-1,26,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,16,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,25,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,15,-1,-1,18,19,20,21,22,23,24,23,22,21,-1,-1,24,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,14,-1,-1,17,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,23,-1,-1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1,-1,-1,13,-1,-1,16,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1,22,-1,-1,-1,-1,-1,-1 },
		{ -1,17,16,15,14,13,12,13,14,15,14,13,12,-1,-1,15,16,17,18,19,20,21,22,23,24,25,26,-1 },
		{ -1,18,-1,-1,-1,-1,11,-1,-1,-1,-1,-1,11,-1,-1,14,-1,-1,-1,-1,-1,20,-1,-1,-1,-1,27,-1 },
		{ -1,19,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,10,-1,-1,13,-1,-1,-1,-1,-1,19,-1,-1,-1,-1,28,-1 },
		{ -1,20,19,18,-1,-1, 9, 8, 7, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,-1,-1,25,26,27,-1 },
		{ -1,-1,-1,17,-1,-1,10,-1,-1, 5,-1,-1,-1,-1,-1,-1,-1,-1,14,-1,-1,19,-1,-1,24,-1,-1,-1 },
		{ -1,-1,-1,16,-1,-1,11,-1,-1, 4,-1,-1,-1,-1,-1,-1,-1,-1,13,-1,-1,20,-1,-1,23,-1,-1,-1 },
		{ -1,17,16,15,14,13,12,-1,-1, 3, 2, 1, 0,-1,-1, 9,10,11,12,-1,-1,21,22,23,22,21,20,-1 },
		{ -1,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,19,-1 },
		{ -1,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 2,-1,-1, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1 },
		{ -1,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,-1 },
		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }
	};

	return test_fitness_ghost(fitMat, pPos, gPos, SCARED_NORMAL);
}





//test EYES
float test_fitness_ghost5() {

	struct position pPos = { 14,26 };
	struct position gPos[NG] = { {14,5},{1,3},{26,21},{29,1},{5,24},{11,11} };

	int fitMat[NROWS][NCOLS] = {
{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
{-1,25,24,23,22,21,20,21,22,23,22,21,20,-1,-1,20,21,22,23,22,21,20,21,22,23,24,25,-1},
{-1,24,-1,-1,-1,-1,19,-1,-1,-1,-1,-1,19,-1,-1,19,-1,-1,-1,-1,-1,19,-1,-1,-1,-1,24,-1},
{-1,23,-1,-1,-1,-1,18,-1,-1,-1,-1,-1,18,-1,-1,18,-1,-1,-1,-1,-1,18,-1,-1,-1,-1,23,-1},
{-1,22,-1,-1,-1,-1,17,-1,-1,-1,-1,-1,17,-1,-1,17,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,22,-1},
{-1,21,20,19,18,17,16,15,14,13,14,15,16,17,17,16,15,14,13,14,15,16,17,18,19,20,21,-1},
{-1,22,-1,-1,-1,-1,17,-1,-1,12,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1,17,-1,-1,-1,-1,22,-1},
{-1,23,-1,-1,-1,-1,18,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,11,-1,-1,18,-1,-1,-1,-1,23,-1},
{-1,24,23,22,21,20,19,-1,-1,10, 9, 8, 7,-1,-1, 7, 8, 9,10,-1,-1,19,20,21,22,23,24,-1},
{-1,-1,-1,-1,-1,-1,18,-1,-1,-1,-1,-1, 6,-1,-1, 6,-1,-1,-1,-1,-1,18,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,-1, 5,-1,-1, 5,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,16,-1,-1, 7, 6, 5, 4, 3, 3, 4, 5, 6, 7,-1,-1,16,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,15,-1,-1, 8,-1,-1, 3, 2, 2, 3,-1,-1, 8,-1,-1,15,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,14,-1,-1, 9,-1, 3, 2, 1, 1, 2, 3,-1, 9,-1,-1,14,-1,-1,-1,-1,-1,-1},
{19,18,17,16,15,14,13,12,11,10,-1, 2, 1, 0, 0, 1, 2,-1,10,11,12,13,14,15,16,17,18,19},
{-1,-1,-1,-1,-1,-1,14,-1,-1,11,-1, 3, 2, 1, 1, 2, 3,-1,11,-1,-1,14,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,15,-1,-1,12,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1,15,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,16,-1,-1,13,14,15,16,17,17,16,15,14,13,-1,-1,16,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,17,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,14,-1,-1,17,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,18,-1,-1,15,-1,-1,-1,-1,-1,-1,-1,-1,15,-1,-1,18,-1,-1,-1,-1,-1,-1},
{-1,24,23,22,21,20,19,18,17,16,17,18,19,-1,-1,19,18,17,16,17,18,19,20,21,22,23,24,-1},
{-1,25,-1,-1,-1,-1,20,-1,-1,-1,-1,-1,20,-1,-1,20,-1,-1,-1,-1,-1,20,-1,-1,-1,-1,25,-1},
{-1,26,-1,-1,-1,-1,21,-1,-1,-1,-1,-1,21,-1,-1,21,-1,-1,-1,-1,-1,21,-1,-1,-1,-1,26,-1},
{-1,27,28,29,-1,-1,22,23,24,25,24,23,22,23,23,22,23,24,25,24,23,22,-1,-1,29,28,27,-1},
{-1,-1,-1,30,-1,-1,23,-1,-1,26,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,23,-1,-1,30,-1,-1,-1},
{-1,-1,-1,29,-1,-1,24,-1,-1,27,-1,-1,-1,-1,-1,-1,-1,-1,27,-1,-1,24,-1,-1,29,-1,-1,-1},
{-1,30,29,28,27,26,25,-1,-1,28,29,30,31,-1,-1,31,30,29,28,-1,-1,25,26,27,28,29,30,-1},
{-1,31,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,32,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,-1},
{-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,33,-1,-1,33,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,32,-1},
{-1,33,34,35,36,37,38,39,38,37,36,35,34,35,35,34,35,36,37,38,39,38,37,36,35,34,33,-1},
{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	};

	return test_fitness_ghost(fitMat, pPos, gPos, EYES);
}

//test EYES
float test_fitness_ghost6() {

	struct position pPos = { 14,26 };
	struct position gPos[NG] = { {14,5},{14,27},{14,24},{12,21},{12,14},{20,15} };

	int fitMat[NROWS][NCOLS] = {
{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
{-1,25,24,23,22,21,20,21,22,23,22,21,20,-1,-1,20,21,22,23,22,21,20,21,22,23,24,25,-1},
{-1,24,-1,-1,-1,-1,19,-1,-1,-1,-1,-1,19,-1,-1,19,-1,-1,-1,-1,-1,19,-1,-1,-1,-1,24,-1},
{-1,23,-1,-1,-1,-1,18,-1,-1,-1,-1,-1,18,-1,-1,18,-1,-1,-1,-1,-1,18,-1,-1,-1,-1,23,-1},
{-1,22,-1,-1,-1,-1,17,-1,-1,-1,-1,-1,17,-1,-1,17,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,22,-1},
{-1,21,20,19,18,17,16,15,14,13,14,15,16,17,17,16,15,14,13,14,15,16,17,18,19,20,21,-1},
{-1,22,-1,-1,-1,-1,17,-1,-1,12,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1,17,-1,-1,-1,-1,22,-1},
{-1,23,-1,-1,-1,-1,18,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,11,-1,-1,18,-1,-1,-1,-1,23,-1},
{-1,24,23,22,21,20,19,-1,-1,10, 9, 8, 7,-1,-1, 7, 8, 9,10,-1,-1,19,20,21,22,23,24,-1},
{-1,-1,-1,-1,-1,-1,18,-1,-1,-1,-1,-1, 6,-1,-1, 6,-1,-1,-1,-1,-1,18,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,-1, 5,-1,-1, 5,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,16,-1,-1, 7, 6, 5, 4, 3, 3, 4, 5, 6, 7,-1,-1,16,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,15,-1,-1, 8,-1,-1, 3, 2, 2, 3,-1,-1, 8,-1,-1,15,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,14,-1,-1, 9,-1, 3, 2, 1, 1, 2, 3,-1, 9,-1,-1,14,-1,-1,-1,-1,-1,-1},
{19,18,17,16,15,14,13,12,11,10,-1, 2, 1, 0, 0, 1, 2,-1,10,11,12,13,14,15,16,17,18,19},
{-1,-1,-1,-1,-1,-1,14,-1,-1,11,-1, 3, 2, 1, 1, 2, 3,-1,11,-1,-1,14,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,15,-1,-1,12,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1,15,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,16,-1,-1,13,14,15,16,17,17,16,15,14,13,-1,-1,16,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,17,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,14,-1,-1,17,-1,-1,-1,-1,-1,-1},
{-1,-1,-1,-1,-1,-1,18,-1,-1,15,-1,-1,-1,-1,-1,-1,-1,-1,15,-1,-1,18,-1,-1,-1,-1,-1,-1},
{-1,24,23,22,21,20,19,18,17,16,17,18,19,-1,-1,19,18,17,16,17,18,19,20,21,22,23,24,-1},
{-1,25,-1,-1,-1,-1,20,-1,-1,-1,-1,-1,20,-1,-1,20,-1,-1,-1,-1,-1,20,-1,-1,-1,-1,25,-1},
{-1,26,-1,-1,-1,-1,21,-1,-1,-1,-1,-1,21,-1,-1,21,-1,-1,-1,-1,-1,21,-1,-1,-1,-1,26,-1},
{-1,27,28,29,-1,-1,22,23,24,25,24,23,22,23,23,22,23,24,25,24,23,22,-1,-1,29,28,27,-1},
{-1,-1,-1,30,-1,-1,23,-1,-1,26,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,23,-1,-1,30,-1,-1,-1},
{-1,-1,-1,29,-1,-1,24,-1,-1,27,-1,-1,-1,-1,-1,-1,-1,-1,27,-1,-1,24,-1,-1,29,-1,-1,-1},
{-1,30,29,28,27,26,25,-1,-1,28,29,30,31,-1,-1,31,30,29,28,-1,-1,25,26,27,28,29,30,-1},
{-1,31,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,32,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,-1},
{-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,33,-1,-1,33,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,32,-1},
{-1,33,34,35,36,37,38,39,38,37,36,35,34,35,35,34,35,36,37,38,39,38,37,36,35,34,33,-1},
{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	};

	return test_fitness_ghost(fitMat, pPos, gPos, EYES);
}




/*Main*****************************************************************************************************/


int main(int argc, char** argv) {

	int errors_ghost = 0;
	int errors_cases = 0;
	int errors_setup = 0;
	float f1=0, f2=0, f3=0;


#ifdef FILE_OUTPUT	
	char  fileName[50];
	FILE* fp;

	sprintf(fileName, "%s_elab8_test1.txt", argv[1]);

	//sprintf(fileName, "output.txt");
	fp = freopen(fileName, "w", stdout);

#endif





#ifdef TEST_SETUP
	errors_setup += test_setup();
	errors_setup += test_setup2();
	printf("ERROR SETUP=%d\n", errors_setup);
#endif	
#ifdef TEST_MOVE
	errors_ghost += test_ghost();
	errors_ghost += test_ghost2();
	printf("ERROR RANDOM MOVE=%d\n", errors_ghost);
#endif	
#ifdef TEST_MOVE2	
	
	errors_cases += test_ghost_case1();
	errors_cases += test_ghost_case2();
	errors_cases += test_ghost_case3();
	errors_cases += test_ghost_case4();
	printf("ERROR MOVE CASES=%d\n", errors_cases);
#endif	
#ifdef TEST_FITNESS	
//	printf("\n4) Test FITNESS\n");
	f1 = (test_fitness_ghost1() + test_fitness_ghost2())/2;
	f2 = (test_fitness_ghost3() + test_fitness_ghost4())/2;
	f3 = (test_fitness_ghost5() + test_fitness_ghost6())/2;
	printf("FITNESS: NORMAL=%.2f SCARED=%.2f EYES=%.2f \n", f1, f2, f3);
#endif	



#ifdef FILE_OUTPUT	
	fclose(fp);

	fp = fopen(argv[2], "a");
	fprintf(fp, "%s ", argv[1]);

	fprintf(fp, "\nERRORS: SETUP=%d GHOSTS=%d CASES=%d \n", errors_setup, errors_ghost,errors_cases);
	fprintf(fp, "FITNESS: NORMAL=%.2f SCARED=%.2f EYES=%.2f \n", f1, f2, f3);


	fclose(fp);


#endif

	return 0;

}


#endif

