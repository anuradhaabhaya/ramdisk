/*
 * ramdisk.c
 *
 *  Created on: Mar 3, 2012
 *      Author: antoine
 */

#include "../headers/ramdisk.h"

extern ramdisk_s disque;

/* Chargement et sauvegarde */

/* int chargement_disque()
 *     Action           Charger le ramdisk contenu dans un fichier vers la variable globale disque
 *
 *     Retourne         ERREUR	en cas d'erreur (erreur lors fopen ou erreur lors fread)
 *     					(Tester != erreur)
 */
int charger_disque()
{
	int ret = 0; /* Code de retour */

	FILE* fichier = fopen(FICHIER, "rb"); /* Acces en lecture : mode rb */

	if (fichier != NULL)
	{
		memset(&disque, 0, sizeof(ramdisk_s)); /* Tous les octets du disque à 0 */

		ret = fread(&disque, sizeof(ramdisk_s), 1, fichier); /* Lecture depuis le fichier vers le disque */

		fclose(fichier);
	}

	if (ret != 1) /* Si probleme lors lecture */
		ret = ERREUR;

	return ret;
}

/* int sauvegarder_disque()
 *     Action           Stocker la variable globale disque dans un fichier
 *
 *     Retourne         ERREUR en cas d'erreurs
 *     					(tester != ERREUR)
 */
int sauvegarder_disque()
{
	int ret = 0;

	FILE* fichier = fopen(FICHIER, "wb+"); /* On ecrase l'ancien disque : mode wb+ */

	if (fichier != NULL)
	{
		ret = fwrite(&disque, sizeof(ramdisk_s), 1, fichier); /* Recopier l'intégralité du ramdisk dans un fichier */

		fclose(fichier);
	}

	if (ret != 1) /* Si erreur lors ecriture */
		ret = ERREUR;

	return ret;
}

/* Creation du disque */

/* void creer_disque();
 *      Action
 *              Initialisation de la variable globale disque
 *                              - Initialisation du super bloc
 *                              - Initialisation des maps
 */
void creer_disque()
{
	// Mise a 0 de tous les octets du disque
	memset(&disque, 0, sizeof(ramdisk_s));

	creer_superbloc();

	creer_maps();
}

/* void creer_superbloc();
 *      Action
 *              Initialisation des champs du superbloc
 *                              - NB_BLOCS, NB_INODES etc ...
 */
void creer_superbloc()
{
	superbloc_s sb; /* Creation nouveau super bloc */
	memset(&sb, 0, sizeof(superbloc_s));

	sb.taille_sys = NB_BLOCS * TAILLE_BLOC;
	sb.nb_blocs = NB_BLOCS; // pourquoi laisser le nb_blocs, on l'a en define ?
	sb.nb_blocs_libres = BLOCS_RESTANTS;
	sb.taille_bloc = TAILLE_BLOC; // on l'a en define, meme remarque
	sb.nb_inodes = NB_INODES; // Pareil
	sb.nb_inodes_libres = NB_INODES - 3; // On reserve l'inode racine et inodes reservés
	sb.inode_racine = 2;

	memcpy(&disque.superbloc, &sb, sizeof(superbloc_s));
}

/* Gestion des maps */

/* void creer_map()
 *      Action
 *              Se charge d'initialiser les maps (inodes 0,1 et 2 reserves)
 */
void creer_maps()
{
	int i = 0;

	/* Les 3 premiers inodes sont reserves seulement */
	for (i = 0; i < 3; i++)
		modifier_map(inodes, i, 1);

	/* Pas besoin de parcourir la map des blocs : tout est deja a 0 */
}

/* void liberer_id(map_e map, uint16_t id)
 * 		Arguments
 * 			map_e map
 * 			uint16_t id
 *
 * 		Action
 * 			Met l'état à 0 de l'id "id" dans la map "map"
 * 			ET INCREMENTE LE NOMBRE D'INODES/BLOCS disponibles dans le superbloc
 */
void liberer_id(map_e map, uint16_t id)
{
	superbloc_s* ptr = (superbloc_s *) &disque.superbloc;

	if (ptr != NULL)
	{
		if (map == blocs)
		{
			modifier_map(blocs, id, 0);
			ptr->nb_blocs_libres++; // un bloc de +
		}
		else if (map == inodes)
		{
			modifier_map(inodes, id, 0);
			ptr->nb_inodes_libres++; // un bloc de -
		}
	}
}

/* int recuperer_id(map_e map)
 * 		Arguments
 * 			map_e map
 *
 * 		Action
 * 			Récupère le premier bloc/inode disponible et passe son état à utilisé (1)
 *	 	 	ET DECREMENTE LE NOMBRE D'INODES/BLOCS disponibles dans le superbloc
 *
 * 		Retourne
 * 			Id du premier/inode disponible
 *			ERREUR en cas d'erreur
 */
int recuperer_id(map_e map)
{
	int i = 0;
	int max = 0;
	superbloc_s* p_superbloc = (superbloc_s *) &disque.superbloc;

	if (map == blocs)
		max = BLOCS_RESTANTS;
	else if (map == inodes)
		max = NB_INODES;

	octet recup = info_map(map, i); /* On recupere le premier element : i = 0*/

	while (recup == 1 && i < max) /* Tant qu'on parcourt des id occupes */
	{
		i = i + 1;
		recup = info_map(map, i); /* On recupere l'état de l'octet à la position i */
	}

	if (recup != ERREUR)
	{
		/* On quitte la boucle, on regarde si l'état est valide ( != ERREUR ) */
		modifier_map(map, i, 1); /* Et on met à 1 dans la map*/
		if (map == blocs)
			p_superbloc->nb_blocs_libres--;
		else if (map == inodes)
			p_superbloc->nb_inodes_libres--;
	}
	else
		i = ERREUR;

	return i;
}

/* octet info_map(map_e, int pos)
 *      Arguments
 *              map_e map       Map dans laquelle on veut une information (bloc ou inodes)
 *              int pos         position dans la map
 *
 *      Action
 *              Savoir si (bloc/inode) occupe
 *
 *      Retourne
 *              1 si occupe
 *              0 sinon
 */
octet info_map(map_e map, int pos)
{
	octet recup = ERREUR;
	char masque = 7 - (pos % 8);
	int num_octet = (pos % (8 * TAILLE_BLOC)) / 8;
	int args_valides = 0; /* 1 si les arguments sont corrects */

	if (map == blocs && BLOC_VALIDE(pos))
	{
		args_valides = 1;
		recup = disque.blocs_map.donnee[num_octet];
	}

	else if (map == inodes && INODE_VALIDE(pos))
	{
		args_valides = 1;
		recup = disque.inodes_map.donnee[num_octet];
	}

	if (args_valides)
	{
		recup = recup >> masque;
		recup = recup & 1;
	}

	return recup;
}

/* void modifier_map(map_e, int pos, int val)
 *      Arguments
 *              map_e map       Map que l'on veut modifier (blocs ou inodes)
 *              int pos         Position dans la map du bit à modifier
 *              int val         Valeur que l'on veut mettre à la position pos (0 ou 1)
 *
 *      Action
 *              Modifier le bit à la position pos par la valeur val
 *
 */
void modifier_map(map_e map, int pos, uint8_t val)
{
	bloc_s *ptr = NULL; /* Pointe sur la map à modifier (bloc 1 ou 2)*/
	octet masque = 1 << (7 - (pos % 8)); /* Sert à modifier le bit à la position pos */
	octet recup = ERREUR; /* octet de la map qui sera modifie */
	int num_octet = (pos % (8 * TAILLE_BLOC)) / 8; /* numero de l'octet dans le bloc a modifier */
	int args_valides = 0;

	// Initialisation des variables
	if (map == blocs && BLOC_VALIDE(pos) && EST_BIT(val))
	{
		args_valides = 1;
		recup = disque.blocs_map.donnee[num_octet];
		ptr = &disque.blocs_map;
	}
	else if (map == inodes && INODE_VALIDE(pos))
	{
		args_valides = 1;
		recup = disque.inodes_map.donnee[num_octet];
		ptr = &disque.inodes_map;
	}

	if (args_valides)
	{
		recup = modifier_octet(recup, masque, val); /* Calcul du nouvel octet */
		ptr->donnee[num_octet] = recup; /* Remplacement de l'ancien octet par l'octet o qui vient d'etre calculé */
	}
}

/* octet modifier_octet(octet o, octet masque, uint8_t val);
 *      Arguments
 *              octet o         octet qui sera modifie par le masque
 *              octet masque    8 bits qui vont modifier l'octet o
 *              uint8_t val     valeur que l'on veut mettre dans l'octet o
 */
octet modifier_octet(octet o, octet masque, uint8_t val)
{
	if (val == 1)
	{
		o = o | masque;
	}
	else if (val == 0)
	{
		o = o & ~masque;
	}

	return o;
}

/* Affichage d'informations : debug */

void afficher_disque()
{
	afficher_superbloc();
	afficher_map_bloc();
	afficher_map_inode();
}

void afficher_superbloc()
{
	superbloc_s* ptr = (superbloc_s *) &disque.superbloc;

	printf("Donnees du super bloc :\n");
	printf("--------------------------\n");
	printf("Nombre blocs : %d\n", ptr->nb_blocs);
	printf("Taille des blocs : %d octets\n", ptr->taille_bloc);
	printf("Nombre blocs libres : %d\n", ptr->nb_blocs_libres);
	printf("Nombre d'inodes : %d\n", ptr->nb_inodes);
	printf("Nombre d'inodes libres : %d\n", ptr->nb_inodes_libres);
	printf("Inode racine : %d\n", ptr->inode_racine);
}

void afficher_map_bloc()
{
	int i;

	printf("\nAffichage de la map des blocs :\n");

	for (i = 0; i < BLOCS_RESTANTS; i = i + 1)
		printf("%d", info_map(blocs, i));

	printf("\n");
}

void afficher_map_inode()
{
	int i;
	superbloc_s* ptr = (superbloc_s *) &disque.superbloc;

// tester ptr ?
	printf("\nAffichage de la map des inodes :\n");

	for (i = 0; i < ptr->nb_inodes; i = i + 1)
		printf("%d", info_map(inodes, i));

	printf("\n");

}
