/*
 * Auteur : Anuradha Abhaya
 * Date : 14/01/2011
 *
 * Fichier : ramdisk.h
 * Titre :
 *
 * OS : Ubuntu 11.04 The Natty Narwhal - 32 bits
 * IDE : Eclipse for C/C++ Developers
 *
 */

// Remarques :  revoir nombre d'adresses contenues dans un bloc indirect
//              le préprocesseur ne comprendrait pas sizeof ? oO (lu sur internet, à vérifier)
#ifndef RAMDISK_H_
#define RAMDISK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/* Pour le debogage */
#define DEBUG printf("DEBUG %s : %d\n", __FILE__, __LINE__);
#define DEBUG_INT(i) printf("DEBUG_INT %s = %d, %s %d ~~\n", #i, i, __FILE__, __LINE__);

/* Macros */
#define NB_BLOCS 1024 /* nombre total de blocs */
#define NB_INODES ((NB_BLOCS)/4) /* nombre total d'inodes */
#define TAILLE_BLOC 256 /* taille d'un bloc, en octets */

// A voir si on garde car peut avoir des problèmes avec sizeof ?
#define BLOCS_LISTE_INODES ((NB_INODES)*(sizeof(inode_s))/(TAILLE_BLOC))+1 /* +1 au cas où impair */
#define BLOCS_RESTANTS ((NB_BLOCS)-(BLOCS_LISTE_INODES)-3) /* nombre de blocs contenant les blocs de donnees */
#define BLOCS_IND ((TAILLE_BLOC)/sizeof(short int)) /* nombre d'adresses contenues dans un bloc indirect */

#define MAX 30 /* taille du buffer */
#define NB_FO 10 /* nombre maximum de fichiers ouverts en même temps */
#define FICHIER "mon_disque" /* Nom du fichier où l'on sauvegarde notre disque*/

#define ERREUR -1 /* Code d'erreur */

/* Verifications */
#define BLOC_VALIDE(id) (((id) < BLOCS_RESTANTS) && ((id) >= 0))
#define INODE_VALIDE(id) (((id) < NB_INODES) && ((id) >= 0))
#define EST_BIT(bit) ((bit) == 0 || (bit) == 1)

/*
 ==========================

 STRUCTURES ET ENUMERATIONS

 ==========================
 */

typedef char octet;

typedef enum
{
	ordinaire, repertoire
} type_e;

typedef enum
{
	blocs, inodes
} map_e;

typedef enum
{
	lecture, ecriture, lecture_ecriture, ecriture_fin
} flag_e;

typedef struct
{
	octet donnee[TAILLE_BLOC]; /* un bloc est consitué de TAILLE_BLOC octet */
} bloc_s;

typedef struct
{
	type_e type; /* type du fichier */
	int taille_fichier; /* taille du fichier en octets */
	time_t dernier_acces; /* date de dernier acces au fichier */
	short int nb_liens; /* compteur indiquant le nombre de liens physiques sur cet inode */
	short int bloc_direct[10]; /* indices des 10 blocs directs */
	short int ind_simple; /* indice du bloc d'indirection simple */
	short int ind_double; /* indice du bloc d'indirection double */
	short int ind_triple; /* indice du bloc d'indirection triple */
} inode_s;

typedef struct
{
	uint32_t taille_sys; /* taille du systeme de fichiers */
	uint16_t nb_blocs; /* nombre de blocs */
	uint16_t taille_bloc; /* taille d'un bloc */
	uint16_t nb_blocs_libres; /* nombre de blocs libres */
	uint16_t nb_inodes; /* nombre d'inodes */
	uint16_t nb_inodes_libres; /* nombre d'inodes libres */
	uint16_t inode_racine; /* l'inode du fichier racine */
} superbloc_s;

typedef struct
{
	bloc_s superbloc; /* le super-bloc : bloc 0 */
	bloc_s blocs_map; /* donne le status de chaque bloc : bloc 1 */
	bloc_s inodes_map; /* donne le status de chaque inode : bloc 2 */
	bloc_s inode[BLOCS_LISTE_INODES]; /* blocs contenant la liste des inodes */
	bloc_s bloc[BLOCS_RESTANTS]; /* blocs contenant les donnees des fichiers */
} ramdisk_s;


/*
 =======================

 PROTOTYPES DE FONCTIONS

 =======================
 */

/* Chargement et sauvegarde */
int charger_disque();
int sauvegarder_disque();

/* Creation du disque */
void creer_disque();
void creer_superbloc();

/* Acces aux zones */
superbloc_s* recup_ptr_superbloc();
inode_s* recup_ptr_inode(int n);

/* Gestion des inodes */
inode_s* recup_ptr_inode(int n);
int initialiser_inode(inode_s* ptr_inode, type_e type);

/* Gestion des maps */
void creer_maps();
void liberer_id(map_e map, int id);
int recuperer_id(map_e map);
octet info_map(map_e map, int pos);
void modifier_map(map_e map, int pos, int val);
octet modifier_octet(octet o, octet masque, int val);

/* Affichage d'informations : debug */

void afficher_date_inode(inode_s* ptr_inode);
void afficher_inode(inode_s* ptr_inode);
void afficher_disque();
void afficher_superbloc();
void afficher_map_bloc();
void afficher_map_inode();

/* Fonction pour les tests */
void effacer_disque();
void recopier_liste_inodes(inode_s* tab, int n);
bloc_s* recup_ptr_donnees(); // pour test

#endif /* RAMDISK_H_ */


