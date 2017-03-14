
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	Action action;
}Ghost;

//
// VARS 
//
Hunter	*HUNTERS;
Ghost	*GHOSTS;
int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
int enTeamId;

//
// FUNCTIONS
//
void Get_Action(void)
{
	
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
	if (myTeamId == 0) enTeamId = 1; else enTeamId = 0;

	HUNTERS = (Hunter *)calloc(2*bustersPerPlayer, sizeof(Hunter));
	GHOSTS	= (Ghost *)	calloc(ghostCount, sizeof(Ghost));

	// game loop
	while (1) {
		int entities; // the number of busters and ghosts visible to you
		scanf("%d", &entities);

		int H_Count=0;
		int G_Count=0;
		for (int i = 0; i < entities; i++)
		{
			int entityId; // buster id or ghost id
			int x;
			int y; // position of this buster / ghost
			int entityType; // the team id if it is a buster, -1 if it is a ghost.
			int state; // For busters: 0=idle, 1=carrying a ghost.
			int value; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
			scanf("%d%d%d%d%d%d", &entityId, &x, &y, &entityType, &state, &value);

			switch(entityType)
			{
				case -1:
					GHOSTS[G_Count].id = entityId;
					GHOSTS[G_Count].team = GH;
					GHOSTS[G_Count].x = x;
					GHOSTS[G_Count].y = y;
					GHOSTS[G_Count].value = value;

					G_Count++;
				break;
				///////
				case 0:
				case 1:
					HUNTERS[H_Count].id = entityId;
					HUNTERS[H_Count].x = x;
					HUNTERS[H_Count].y = y;
					HUNTERS[H_Count].value = value;

					if (entityType == myTeamId) HUNTERS[H_Count].team = MY;
					if (entityType == enTeamId) HUNTERS[H_Count].team = EN;
					H_Count++;
				break;
			}
		}
		for (int i = 0; i < bustersPerPlayer; i++) {

			// Write an action using printf(). DON'T FORGET THE TRAILING \n
			// To debug: fprintf(stderr, "Debug messages...\n");

			printf("MOVE 8000 4500\n"); // MOVE x y | BUST id | RELEASE
		}
	}

	return 0;
}