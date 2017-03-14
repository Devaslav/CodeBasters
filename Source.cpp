
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
* Send your busters out into the fog to trap ghosts and bring them home!
**/
int main()
{
	int bustersPerPlayer; // the amount of busters you control
	scanf("%d", &bustersPerPlayer);
	int ghostCount; // the amount of ghosts on the map
	scanf("%d", &ghostCount);
	int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
	scanf("%d", &myTeamId);

	// game loop
	while (1) {
		int entities; // the number of busters and ghosts visible to you
		scanf("%d", &entities);
		for (int i = 0; i < entities; i++) {
			int entityId; // buster id or ghost id
			int x;
			int y; // position of this buster / ghost
			int entityType; // the team id if it is a buster, -1 if it is a ghost.
			int state; // For busters: 0=idle, 1=carrying a ghost.
			int value; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
			scanf("%d%d%d%d%d%d", &entityId, &x, &y, &entityType, &state, &value);
		}
		for (int i = 0; i < bustersPerPlayer; i++) {

			// Write an action using printf(). DON'T FORGET THE TRAILING \n
			// To debug: fprintf(stderr, "Debug messages...\n");

			printf("MOVE 8000 4500\n"); // MOVE x y | BUST id | RELEASE
		}
	}

	return 0;
}