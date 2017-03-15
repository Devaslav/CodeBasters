
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
	int			   my_trg;			// ИД моего охотника, нацеленного на данного призрака, иначе -1
	Action action;
}Ghost;

//
// VARS 
//
Hunter	*HUNTERS;
Ghost	*GHOSTS;
Square *SQRS;
int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
int enTeamId;
int H_Count = 0;
int G_Count = 0;
int Trg_Count = 0;
int *ID_TRG;

int BASE_X, BASE_Y;
/////////////////////////////////////////////////////////////////
//
// FUNCTIONS
//
/////////////////////////////////////////////////////////////////
void Get_SQRS(int Team_ID)
{
	int i, j;
	Square Temp_SQR;

	if(Team_ID == 0)
		for(i = 0; i< 8;i++)
		for (j = 0;j < 5;j++)
		{
			SQRS[i * 5 + j].x1 = 2000 * i;
			SQRS[i * 5 + j].y1 = 1800 * j;
			SQRS[i * 5 + j].x2 = 2000 * (i + 1);
			SQRS[i * 5 + j].y2 = 1800 * (j + 1);

			SQRS[i * 5 + j].centr_x = (2000 * i + 2000 * (i + 1)) / 2;
			SQRS[i * 5 + j].centr_y = (1800 * j + 1800 * (j + 1)) / 2;

			SQRS[i * 5 + j].priority = round( (GET_DIST(SQRS[i * 5 + j].centr_x, SQRS[i * 5 + j].centr_y, 8000, 4500)) / 1000.0);
			SQRS[i * 5 + j].walked = 0;
			SQRS[i * 5 + j].my_hunter = -1;
		}

	if (Team_ID == 1)
		for (i = 0;i<8;i++)
			for (j = 0;j < 5;j++)
			{
				SQRS[i * 5 + j].x1 = 16000 - 2000 * (i + 1);
				SQRS[i * 5 + j].y1 = 9000 - 1800 * (j + 1);
				SQRS[i * 5 + j].x2 = 16000 - 2000 * i;
				SQRS[i * 5 + j].y2 = 9000 - 1800 * j;

				SQRS[i * 5 + j].centr_x = (16000 - 2000 * (i + 1) + 16000 - 2000 * i) / 2;
				SQRS[i * 5 + j].centr_y = (9000 - 1800 * (j + 1) + 9000 - 1800 * j) / 2;

				SQRS[i * 5 + j].priority = round((GET_DIST(SQRS[i * 5 + j].centr_x, SQRS[i * 5 + j].centr_y, 8000, 4500)) / 1000.0);
				SQRS[i * 5 + j].walked = 0;
				SQRS[i * 5 + j].my_hunter = -1;
			}

	for (i = 0;i<40;i++)
		for (j = 0;j < 40;j++)
		{
			if (SQRS[i].priority < SQRS[j].priority)
			{
				memcpy(&Temp_SQR, &SQRS[j], sizeof(Square));
				memcpy(&SQRS[j],  &SQRS[i],  sizeof(Square));
				memcpy(&SQRS[i],  &Temp_SQR, sizeof(Square));
			}
		}

	
	for (i = 0;i < 40;i++)
	{
		fprintf(stderr, "SQRS[%d].centr_x=%d\n", i, SQRS[i].centr_x);
		fprintf(stderr, "SQRS[%d].centr_y=%d\n", i, SQRS[i].centr_y);
		fprintf(stderr, "SQRS[%d].priority=%d\n", i, SQRS[i].priority);
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
void Get_Move(Hunter HTR)
{
	int n_sq = 0;
	unsigned IS_MOVE;

	IS_MOVE = 0;
	while (IS_MOVE == 0)
	{
		if (In_Square(HTR.x, HTR.y, n_sq) == 1 && (GET_DIST(HTR.x, HTR.y, SQRS[n_sq].centr_x, SQRS[n_sq].centr_y) < 500.0) )
			SQRS[n_sq].walked = 1;

		if ( (SQRS[n_sq].walked == 0) && ( SQRS[n_sq].my_hunter == -1 || SQRS[n_sq].my_hunter == HTR.id) )
		{
			printf("MOVE %d %d\n", SQRS[n_sq].centr_x, SQRS[n_sq].centr_y);
			SQRS[n_sq].my_hunter = HTR.id;
			IS_MOVE = 1;
		}
		n_sq++;
		if (n_sq == 39)
		{
			IS_MOVE = 2;
			printf("MOVE %d %d\n", SQRS[0].centr_x, SQRS[0].centr_y);
		}
	}

	if (MOVE == 2)
	{
		for (n_sq = 0;n_sq < 40;n_sq++)
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
					HUNTERS[i].action = PULL;
				}
				else
				{
					printf("MOVE %d %d\n", BASE_X, BASE_Y);
					HUNTERS[i].action = MOVE;
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
				else Get_Move(HUNTERS[i]);
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
	SQRS	= (Square *)calloc(8*5, sizeof(Square));

	Get_SQRS(myTeamId);

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