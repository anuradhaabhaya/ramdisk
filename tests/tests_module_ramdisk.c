/*
 * tests_module_ramdisk.c
 *
 *  Created on: Mar 5, 2012
 *      Author: antoine
 */

#include "tests_module_ramdisk.h"

/* Ensemble des tests de la structure */
void tests_ramdisk()
{
	creer_disque(); /* Creation d'un disque */
	verif_superbloc(); /* Tests pour voir si on recupere les bonnes donnees */

	sauvegarder_disque(); /* Sauvegarde */

	memset(&disque, 0, sizeof(ramdisk_s)); /* On ecrase tout */

	charger_disque(); /* On recupere le disque*/
	verif_superbloc(); /* On vérifie si on a les bonnes données */
	verif_maps_crees(); /* Test des maps apres creation (tout vide pour les blocs et juste les 3 premiers inodes occupes*/
	modifier_maps(); /* Tests apres modification des maps*/
}

void verif_superbloc()
{
	superbloc_s *ptr = (superbloc_s *) &disque.superbloc;

	assert(ptr->inode_racine == 2);
	assert(ptr->nb_blocs == NB_BLOCS);
	assert(ptr->nb_blocs_libres == BLOCS_RESTANTS);
	assert(ptr->nb_inodes == NB_INODES);
	assert(ptr->nb_inodes_libres ==NB_INODES - 3);
	assert(ptr->taille_bloc == TAILLE_BLOC);
	assert(ptr->taille_sys == TAILLE_BLOC * NB_BLOCS);
}

void verif_maps_crees()
{
	int i;

	/* On verifie que les 3 premiers inodes sont a 1 et tous les autres à 0 */
	for (i = 0; i < NB_INODES; i = i + 1)
	{
		if (i < 3)
			assert(info_map(inodes, i) == 1);
		else
			assert(info_map(inodes, i) == 0);
	}

	/* On verifie que tous les blocs sont à 0 */
	for (i = 0; i < BLOCS_RESTANTS; i = i + 1)
	{
		assert(info_map(blocs,i) == 0);
	}

	/* Cas limites : demande d'états incorrects */
	assert(info_map(inodes, -1) == -1);
	assert(info_map(blocs, -1) == -1);
	assert(info_map(inodes, NB_INODES) == -1);
	assert(info_map(blocs, BLOCS_RESTANTS) == -1);
}

void modifier_maps()
{
	int i;
	superbloc_s *ptr = (superbloc_s *) &disque.superbloc;

	/* Modification des blocs : tout a 1*/
	for (i = 0; i < BLOCS_RESTANTS; i++)
	{
		//modifier_map(blocs, i, 1); // modifier map ne décremente pas !!
		recuperer_id(blocs);
	}

	assert(ptr->nb_blocs_libres == 0);

	/* Modification des inodes */
	for (i = 0; i < NB_INODES; i++)
	{
		//modifier_map(inodes, i, 1);
		recuperer_id(inodes);
	}

	assert(ptr->nb_inodes_libres == 0);

	/* Verification que tout est à 1*/
	for (i = 0; i < BLOCS_RESTANTS; i++)
		assert(info_map(blocs, i) == 1);

	for (i = 0; i < NB_INODES; i++)
		assert(info_map(inodes, i) == 1);

	/* Plus de place disponible */
	assert(recuperer_id(inodes) == -1);
	assert(recuperer_id(blocs) == -1);
	assert(ptr->nb_blocs_libres == 0);
	assert(ptr->nb_inodes_libres == 0);


	/* Quelques modif */

	/* Liberation */
	liberer_id(inodes, 214);
	liberer_id(inodes, 18);
	assert(ptr->nb_inodes_libres == 2);

	liberer_id(blocs, 5);
	liberer_id(blocs, 850);
	assert(ptr->nb_blocs_libres == 2);

	/* Verification que les bits ont bien ete mis à 0 */
	assert(info_map(inodes, 18) == 0);
	assert(info_map(inodes, 214) == 0);
	assert(info_map(blocs, 5) == 0);
	assert(info_map(blocs, 850) == 0);

	/* Recuperation des id : on recupere bien ce qu'on a libéré */
	assert(recuperer_id(inodes) == 18);
	assert(recuperer_id(inodes) == 214);
	assert(recuperer_id(blocs) == 5);
	assert(recuperer_id(blocs) == 850);

	/* Verification que les bits ont bien ete REMIS à 1 */
	assert(info_map(inodes, 18) == 1);
	assert(info_map(inodes, 214) == 1);
	assert(info_map(blocs, 5) == 1);
	assert(info_map(blocs, 850) == 1);
}
