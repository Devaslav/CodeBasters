
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define GET_DIST(X1, Y1, X2, Y2) ( sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)) ) 

int				MIN(int a, int b);
int				MAX(int a, int b);
void			CHANGE(int *A, int *B);
//
// STRUCTS 
//
typedef enum 
{
	MOVE,
	BUST,
	PULL,
	STOP
}Action;

typedef enum
{
	MY,
	EN,
	GH
}Team;

typedef struct
{
	unsigned short id;				// ИД охотника
	unsigned short x;
	unsigned short y;
	Team team;						// Принадлежность к команде
	unsigned short state;			// Есть ли призрак у охотника
	unsigned short value;			// ИД призрака, иначе -1
	Action action;
}Hunter;

typedef struct
{
	unsigned short id;				// ИД призрака
	unsigned short x;
	unsigned short y;
	Team team;						// Принадлежность к команде
	unsigned short value;			// 0 если призрак не атакован, иначе количество атакующих охотников
	int			   my_trg;			// ИД моего охотника, нацеленного на данного призрака, иначе -1
	Action action;
}Ghost;

//
// VARS 
//
Hunter	*HUNTERS;
Ghost	*GHOSTS;
int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
int enTeamId;
int H_Count = 0;
int G_Count = 0;
int Trg_Count = 0;
int *ID_TRG;

int BASE_X, BASE_Y;
//
// FUNCTIONS
//
void Get_Action(void)
{
	int i, j, k;
	int Ghost_ID, T_Dist, n_Trg;
	float Min_Dist;

	fprintf(stderr, "H_Count=%d\n", H_Count);
	fprintf(stderr, "G_Count=%d\n", G_Count);

	n_Trg = 0;
	for (i = 0;i < H_Count;i++)
	{
		Min_Dist = 1000000.0;
		Ghost_ID = -1;
		if (HUNTERS[i].team == MY)
		{
			//fprintf(stderr, "HUNTERS[%d].id=%d\n", i, HUNTERS[i].id);
			if (HUNTERS[i].state != 0)
			{
				if (GET_DIST(HUNTERS[i].x, HUNTERS[i].y, BASE_X, BASE_Y) < 1600.0)
				{
					printf("RELEASE\n");
				}
				else
				{
					printf("MOVE %d %d\n", BASE_X, BASE_Y);
				}
			}
			else
			{
				for (j = 0; j < G_Count; j++)
				{
					//fprintf(stderr, "DIST=%f\n", GET_DIST(HUNTERS[i].x, HUNTERS[i].y, GHOSTS[j].x, GHOSTS[j].y));
					if (GET_DIST(HUNTERS[i].x, HUNTERS[i].y, GHOSTS[j].x, GHOSTS[j].y) < Min_Dist
						&& (GHOSTS[j].my_trg == -1))
					{
						Min_Dist = GET_DIST(HUNTERS[i].x, HUNTERS[i].y, GHOSTS[j].x, GHOSTS[j].y);
						fprintf(stderr, "Min_Dist=%d\n-------\n", Min_Dist);
						Ghost_ID = GHOSTS[j].id;
					}
				}

				if (Ghost_ID != -1)
				{
					ID_TRG[n_Trg] = Ghost_ID;
					fprintf(stderr, "ID_TRG[%d]=%d\n", n_Trg, Ghost_ID);
					fprintf(stderr, "Min_Dist=%f\n-------\n", Min_Dist);
					n_Trg++;

					for (j = 0; j < G_Count; j++)
					{
						if (GHOSTS[j].id == Ghost_ID)
						{
							if (Min_Dist > 900 && Min_Dist < 1760)
								printf("BUST %d\n", GHOSTS[j].id);
							else
								printf("MOVE %d %d\n", GHOSTS[j].x, GHOSTS[j].y);

							GHOSTS[j].my_trg = HUNTERS[i].id;
						}
					}
				}
				else printf("MOVE %d %d\n", 16000-BASE_X, 9000-BASE_Y - 1500*i);
			}
		}
	}


}

/**
* Send your busters out into the fog to trap ghosts and bring them home!
**/

////////////////////////////////////////////////////////////////////////////
int main()
{
	int bustersPerPlayer; // the amount of busters you control
	scanf("%d", &bustersPerPlayer);
	int ghostCount; // the amount of ghosts on the map
	scanf("%d", &ghostCount);

	scanf("%d", &myTeamId);
	if (myTeamId == 0) { enTeamId = 1; BASE_X = 0; BASE_Y = 0; }
	else { enTeamId = 0; BASE_X = 16000; BASE_Y = 9000; }

	HUNTERS = (Hunter *)calloc(2*bustersPerPlayer, sizeof(Hunter));
	GHOSTS	= (Ghost *)	calloc(ghostCount, sizeof(Ghost));

	ID_TRG = (int *)calloc(bustersPerPlayer, sizeof(int));
	Trg_Count = bustersPerPlayer;
	// game loop
	while (1) {
		int entities; // the number of busters and ghosts visible to you
		scanf("%d", &entities);

		H_Count = 0;
		G_Count = 0;
		for (int i = 0; i < entities; i++)
		{
			int entityId; // buster id or ghost id
			int x;
			int y; // position of this buster / ghost
			int entityType; // the team id if it is a buster, -1 if it is a ghost.
			int state; // For busters: 0=idle, 1=carrying a ghost.
			int value; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
			scanf("%d%d%d%d%d%d", &entityId, &x, &y, &entityType, &state, &value);

			if (entityType == -1)
			{
				GHOSTS[G_Count].id = entityId;
				GHOSTS[G_Count].team = GH;
				GHOSTS[G_Count].x = x;
				GHOSTS[G_Count].y = y;
				GHOSTS[G_Count].value = value;

				GHOSTS[G_Count].my_trg = -1;

				G_Count++;
			}
			else
			{	
				HUNTERS[H_Count].id = entityId;
				HUNTERS[H_Count].x = x;
				HUNTERS[H_Count].y = y;
				HUNTERS[H_Count].value = value;
				HUNTERS[H_Count].state = state;

				if (entityType == myTeamId) HUNTERS[H_Count].team = MY;
				if (entityType == enTeamId) HUNTERS[H_Count].team = EN;

				H_Count++;

			}
		}

		// Write an action using printf(). DON'T FORGET THE TRAILING \n
		// To debug: fprintf(stderr, "Debug messages...\n");
		//printf("MOVE 8000 4500\n"); // MOVE x y | BUST id | RELEASE
		//for (int i = 0; i < 2*bustersPerPlayer; i++) 
		//{
			Get_Action();
		//}
	}

	return 0;
}


///////////////////////////****///////////////////////////
void CHANGE(int *A, int *B)
{
	*A = *A^*B;
	*B = *A^*B;
	*A = *A^*B;
}
///////////////////////////****///////////////////////////
int MIN(int a, int b)
{
	return a <= b ? a : b;
}
///////////////////////////****///////////////////////////
int MAX(int a, int b)
{
	return a >= b ? a : b;
}
///////////////////////////****///////////////////////////