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

int initialiser_inode(inode_s* ptr)
{
	int ret = ERREUR;

	if (ptr != NULL)
	{
		ptr->taille_fichier = 0;
		ptr->nb_liens = 1;
		ptr->type = repertoire;
		ptr->dernier_acces = time(NULL);
		ret = 1;
	}
	return ret;
}

void afficher_date_inode(inode_s* ptr)
{
	time_t date = time(NULL); // On récupère l'heure courante
	char chaine[26];
	strcpy(chaine, ctime(&date)); // ctime converti date en seconde en une chaine de 26 caracteres
	printf("%s", chaine);
}

int creer_rep_racine()
{

	inode_s racine;
	memset(&racine, 0, sizeof(inode_s));

	return initialiser_inode(&racine);
}

int main(int argc, char* argv[])
{
#if TESTS
	lancer_les_tests();
	printf("Fin des tests\n");
#else

	inode_s* racine;
	initialiser_inode(&racine);

	if (charger_disque() == ERREUR)
	{
		printf("Pas de disque : Creation d'un nouveau disque \n");

		printf("Creation d'un nouveau disque\n");
		creer_disque();

		printf("Disque créé\n");

		printf("Creation répértoire racine : ");
	}
	else
	{
		printf("Disque chargé\n");

		afficher_date_inode(&racine);

		/* Modifications ... */

	}

	printf("Fin du code : Sauvegarde du disque\n");
	//afficher_disque();
	sauvegarder_disque();

#endif

	return 0;
}

