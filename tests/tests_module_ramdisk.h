/*
 * tests_module_ramdisk.h
 *
 *  Created on: Mar 5, 2012
 *      Author: antoine
 */

#ifndef TESTS_MODULE_RAMDISK_H_
#define TESTS_MODULE_RAMDISK_H_

#include "tests.h"

// tests structure
void tests_ramdisk();
void test_creation();
void verif_superbloc();
void test_creation_map();
void verif_maps_crees();
void modifier_maps();

// tests inodes
void tests_inodes();
void test_init_inodes();
void test_inodes_dans_blocs();

#endif /* TESTS_MODULE_RAMDISK_H_ */
