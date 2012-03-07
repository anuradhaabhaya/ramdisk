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
#define TESTS 0

#include "../tests/tests.h"

int main(int argc, char* argv[])
{
#if TESTS
	printf("Lancement des tests ...");
	lancer_les_tests();
	printf(" Fin des tests\n\n");
#else

	if (charger_disque() == ERREUR)
	{
		printf("Creation d'un nouveau disque\n");
		creer_disque();
	}
	else
	{
		printf("Disque chargé\n");

		/* Modifications ... */
	}

	printf("Fin du code : Sauvegarde du disque\n");
	sauvegarder_disque();

#endif

	// Fin du code
	printf("Fin du code");
	return 0;
}

