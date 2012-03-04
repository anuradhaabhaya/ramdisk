/*
 * main.c
 *
 *  Created on: Mar 2, 2012
 *      Author: antoine
 */

#include "../headers/ramdisk.h"

int main(int argc, char* argv[])
{
	if (charger_disque() == 1)
	{
		printf("Disque chargé\n");

		affiche_superbloc();
		afficher_map_bloc();
		afficher_map_inode();

		/* Modifications ... */

		sauvegarder_disque();
	}
	else if (charger_disque() == 0)
	{
		printf("Erreur : pas de disque\n");

		printf("Creation d'un nouveau disque\n");
		creer_disque();
		printf("Disque créé\n");

		affiche_superbloc();
		afficher_map_bloc();
		afficher_map_inode();

		sauvegarder_disque();
	}

	printf("Fin du code\n");
	return 0;
}

