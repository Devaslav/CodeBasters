
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
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
	STUN,
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
	unsigned short x1;
	unsigned short y1;
	unsigned short x2;
	unsigned short y2;
	unsigned short centr_x;
	unsigned short centr_y;

	unsigned short walked;			// Исследованли квадрат - 0  или 1 
	int			   my_hunter;		// Номер моего охотника, идущего туда , иначе -1
	unsigned short ghosts;			// Количество призраков в квадрате
	unsigned short priority;		// Приоритетность исследования квадратов от 1(наивысшая) до 8
}Square;

typedef struct
{
	unsigned short id;				// ИД охотника
	unsigned short x;
	unsigned short y;
	Team team;						// Принадлежность к команде
	unsigned short state;			// Есть ли призрак у охотника
	unsigned short value;			// ИД призрака, иначе -1

	unsigned short squre;			// Квадрат в котором находится охотник
	Action action;
}Hunter;

typedef struct
{
	unsigned short id;				// ИД призрака
	unsigned short x;
	unsigned short y;
	Team team;						// Принадлежность к команде
	unsigned short value;			// 0 если призрак не атакован, иначе количество атакующих охотников
	unsigned short stamina;			// ХП призрака - не регенрят
	int			   my_trg;			// ИД моего охотника, нацеленного на данного призрака, иначе -1
	Action action;
}Ghost;

//
// VARS 
//
Hunter	*HUNTERS;
Ghost	*GHOSTS;
Square *SQRS;
Square *SQRS2;

#define X_PARTS 10
#define Y_PARTS 5


int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
int enTeamId;
int H_Count = 0;
int G_Count = 0;
int Trg_Count = 0;
int *ID_TRG;

unsigned TURN;

int BASE_X, BASE_Y;
/////////////////////////////////////////////////////////////////
//
// FUNCTIONS
//
/////////////////////////////////////////////////////////////////
void Get_SQRS(int Team_ID)
{
	int i, j;

	//if(Team_ID == 0)
		for(i = 0; i< X_PARTS;i++)
		for (j = 0;j < Y_PARTS;j++)
		{
			SQRS[i * Y_PARTS + j].x1 = (16000 / X_PARTS)  * i;
			SQRS[i * Y_PARTS + j].y1 = (9000 / Y_PARTS) * j;
			SQRS[i * Y_PARTS + j].x2 = (16000 / X_PARTS) * (i + 1);
			SQRS[i * Y_PARTS + j].y2 = (9000 / Y_PARTS) * (j + 1);

			SQRS[i * Y_PARTS + j].centr_x = ((16000 / X_PARTS) * i + (16000 / X_PARTS) * (i + 1)) / 2;
			SQRS[i * Y_PARTS + j].centr_y = ((9000 / Y_PARTS) * j + (9000 / Y_PARTS) * (j + 1)) / 2;

			//SQRS[i Y_PARTS + j].priority = round( (GET_DIST(SQRS[i * 5 + j].centr_x, SQRS[i * 5 + j].centr_y, 8000, 4500)) / 1000.0);

			SQRS[i * Y_PARTS + j].walked = 0;
			SQRS[i * Y_PARTS + j].my_hunter = -1;
		}

	/*
	if (Team_ID == 1)
		for (i = 0;i<X_PARTS;i++)
			for (j = 0;j < Y_PARTS;j++)
			{
				SQRS[i * Y_PARTS + j].x1 = 16000 - (16000 / X_PARTS) * (i + 1);
				SQRS[i * Y_PARTS + j].y1 = 9000 - 1800 * (j + 1);
				SQRS[i * Y_PARTS + j].x2 = 16000 - (16000 / X_PARTS) * i;
				SQRS[i * Y_PARTS + j].y2 = 9000 - 1800 * j;

				SQRS[i * Y_PARTS + j].centr_x = (16000 - (16000 / X_PARTS) * (i + 1) + 16000 - (16000 / X_PARTS) * i) / 2;
				SQRS[i * Y_PARTS + j].centr_y = (9000 - 1800 * (j + 1) + 9000 - 1800 * j) / 2;

				//SQRS[i Y_PARTS + j].priority = round((GET_DIST(SQRS[i * 5 + j].centr_x, SQRS[i * 5 + j].centr_y, 8000, 4500)) / 1000.0);

				SQRS[i * Y_PARTS + j].walked = 0;
				SQRS[i * Y_PARTS + j].my_hunter = -1;
			}
	*/

	//for (i = 0;i < X_PARTS*Y_PARTS;i++)
	//{
		//fprintf(stderr, "SQRS[%d].centr_x=%d\n", i, SQRS[i].centr_x);
		//fprintf(stderr, "SQRS[%d].centr_y=%d\n", i, SQRS[i].centr_y);
		//fprintf(stderr, "SQRS[%d].priority=%d\n", i, SQRS[i].priority);
	//}
	
}
/////////////////////////////////////////////////////////////////
void Get_Sort_SQRS(int X, int Y, unsigned TYPE)
{
	int i, j;
	Square Temp_SQR;

	//if(TYPE == 0 || TYPE == 3 || TYPE == 4)
	for (i = 0;i<X_PARTS;i++)
		for (j = 0;j < Y_PARTS;j++)
		{
			SQRS[i * Y_PARTS + j].priority = round((GET_DIST(SQRS[i * Y_PARTS + j].centr_x, SQRS[i * Y_PARTS + j].centr_y, X, Y)) / 1000.0);
		}

	for (i = 0;i<X_PARTS*Y_PARTS;i++)
		for (j = 0;j <X_PARTS*Y_PARTS;j++)
		{
			if (SQRS[i].priority < SQRS[j].priority)
			{
				memcpy(&Temp_SQR, &SQRS[j], sizeof(Square));
				memcpy(&SQRS[j], &SQRS[i], sizeof(Square));
				memcpy(&SQRS[i], &Temp_SQR, sizeof(Square));
			}
		}
}
/////////////////////////////////////////////////////////////////
int In_Square(int x, int y, int S_ID)
{
	if ( x > SQRS[S_ID].x1 && x < SQRS[S_ID].x2 &&
		 y > SQRS[S_ID].y1 && y < SQRS[S_ID].y2	)
	{
		return 1;
	}
	else return 0;
}
/////////////////////////////////////////////////////////////////
void Get_Move(Hunter HTR, unsigned TYPE)
{
	int n_sq;
	unsigned IS_MOVE;

	Get_Sort_SQRS(HTR.x, HTR.y, TYPE);

	IS_MOVE = 0;
	n_sq = 0;
	while (IS_MOVE == 0)
	{
		if (In_Square(HTR.x, HTR.y, n_sq) == 1 && (GET_DIST(HTR.x, HTR.y, SQRS[n_sq].centr_x, SQRS[n_sq].centr_y) < 800.0) )
			SQRS[n_sq].walked = 1;

		if ( (SQRS[n_sq].walked == 0) && ( SQRS[n_sq].my_hunter == -1 || SQRS[n_sq].my_hunter == HTR.id) &&
			!( SQRS[n_sq].x1==0 && SQRS[n_sq].y1==0) && !(SQRS[n_sq].x2 == 16000 && SQRS[n_sq].y2 == 9000)
			)
		{
			printf("MOVE %d %d\n", SQRS[n_sq].centr_x, SQRS[n_sq].centr_y);
			SQRS[n_sq].my_hunter = HTR.id;
			IS_MOVE = 1;
		}
		
		n_sq ++;

		if ( n_sq == (X_PARTS*Y_PARTS-1) )
		{
			IS_MOVE = 2;
			printf("MOVE %d %d\n", SQRS[(X_PARTS*Y_PARTS)/2].centr_x, SQRS[(X_PARTS*Y_PARTS)/2].centr_y);
		}
	}

	if (IS_MOVE == 2)
	{
		for (n_sq = 0;n_sq < (X_PARTS*Y_PARTS - 1);n_sq++)
		{
			SQRS[n_sq].my_hunter = -1;
			SQRS[n_sq].walked = 0;
		}
	}
	//	printf("");
}
/////////////////////////////////////////////////////////////////
void Get_Action(void)
{
	int i, j, k;
	int Ghost_ID,Stun_ID, n_Trg;
	float Min_Dist;
	int Min_Stamina;

	fprintf(stderr, "H_Count=%d\n", H_Count);
	fprintf(stderr, "G_Count=%d\n", G_Count);

	n_Trg = 0;
	for (i = 0;i < H_Count;i++)
	{
		Min_Dist = 1000000.0;
		Min_Stamina = 100;
		Ghost_ID = -1;
		Stun_ID = -1;
		if (HUNTERS[i].team == MY)
		{
			//fprintf(stderr, "HUNTERS[%d].id=%d\n", i, HUNTERS[i].id);
			if (HUNTERS[i].state == 1)
			{
				if (GET_DIST(HUNTERS[i].x, HUNTERS[i].y, BASE_X, BASE_Y) < 1600.0)
				{
					printf("RELEASE\n");
					HUNTERS[i].state = 0;
					//HUNTERS[i].action = PULL;
				}
				else
				{
					printf("MOVE %d %d\n", BASE_X, BASE_Y );
					//HUNTERS[i].action = MOVE;
				}
			}
			else
			{
				
				//
				for (j = 0; j < G_Count; j++)
				{
					//fprintf(stderr, "DIST=%f\n", GET_DIST(HUNTERS[i].x, HUNTERS[i].y, GHOSTS[j].x, GHOSTS[j].y));
					if (GET_DIST(HUNTERS[i].x, HUNTERS[i].y, GHOSTS[j].x, GHOSTS[j].y) < Min_Dist 
						 && GHOSTS[j].stamina <= Min_Stamina
						 //&& !(GHOSTS[j].stamina == 40 && TURN < 10*(10 / Trg_Count) )
						)
					{
						Min_Dist = GET_DIST(HUNTERS[i].x, HUNTERS[i].y, GHOSTS[j].x, GHOSTS[j].y);
						Min_Stamina = GHOSTS[j].stamina;
						//fprintf(stderr, "Min_Dist=%f\n-------\n", Min_Dist);
						Ghost_ID = GHOSTS[j].id;
					}
				}

				//
				for (j = 0; j < H_Count; j++)
				{
					if (HUNTERS[j].team == EN && HUNTERS[j].state != 2 && //  && HUNTERS[j].state == 1 &&
						GET_DIST(HUNTERS[i].x, HUNTERS[i].y, HUNTERS[j].x, HUNTERS[j].y) < 1760.0)
					{
						Stun_ID = HUNTERS[j].id;
					}
				}

				//
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
							if (Min_Dist < 1760.0)
								printf("BUST %d\n", GHOSTS[j].id);
							else
								printf("MOVE %d %d\n", GHOSTS[j].x, GHOSTS[j].y);

							GHOSTS[j].my_trg = HUNTERS[i].id;
						}
					}
				}
				else 
					if(Stun_ID != -1) printf("STUN %d\n", Stun_ID);
					else
					{
						Get_Move(HUNTERS[i], HUNTERS[i].id % 6);
					}
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
	SQRS	= (Square *)calloc(X_PARTS*Y_PARTS, sizeof(Square));
	SQRS2	= (Square *)calloc(X_PARTS*Y_PARTS, sizeof(Square));

	Get_SQRS(myTeamId);

	TURN = 0;

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
				GHOSTS[G_Count].stamina = state;

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
			TURN++;
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