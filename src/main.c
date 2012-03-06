/*
 * main.c
 *
 *  Created on: Mar 2, 2012
 *      Author: antoine
 */

#include "../headers/ramdisk.h"


/* TEST 1 pour lancer les tests
 * TEST 0 sinon
 */
#define TESTS 1
#include "../tests/tests.h"

int main(int argc, char* argv[])
{
#if TESTS
	lancer_les_tests();
	printf("Fin des tests\n");

#else

	if (charger_disque() == ERREUR)
	{
		printf("Pas de disque : Creation d'un nouveau disque \n");

		printf("Creation d'un nouveau disque\n");
		creer_disque();

		printf("Disque créé\n");
	}
	else
	{
		printf("Disque chargé\n");

		/* Modifications ... */

	}

	printf("Fin du code : Sauvegarde du disque\n");
	//afficher_disque();
	sauvegarder_disque();

#endif

	return 0;
}

