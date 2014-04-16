/**
@file dbman.c Defines functions for the disk manager
*/
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * dbman.c
 * Copyright (C) Markus Schatten 2009 <markus.schatten@foi.hr>
 *
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "dbman.h"
pthread_mutex_t fileLockMutex = PTHREAD_MUTEX_INITIALIZER;
int k;


/**
 * @author Markus Schatten
 * @brief  Function that initializes a new database file named DB_FILE. It opens database file. New block is allocated. In this
 block type of header is set to FREE_INT, attribute names are set to FREE_CHAR, integrities are set to FREE_INT,
 constraint names are set to FREE_CHAR, constraint names and codes are set to FREE_CHAR. Type, address and size of tuples
 * are set to FREE_INT. Data in block is set to FREE_CHAR. Type of block is BLOCK_TYPE_FREE, it is not chained and id of
 * last tuple is 0.
 * @param size size of new file in in blocks
 * @return EXIT_SUCCESS if the file has been written to disk, EXIT_ERROR otherwise
 */
int AK_init_db_file(int size) {
	Ak_dbg_messg(HIGH, DB_MAN, "AK_block: %i, AK_header: %i, AK_tuple_dict: %i , char: %i, int: %i\n",
		sizeof(AK_block), sizeof(AK_header), sizeof(AK_tuple_dict), sizeof(char), sizeof(int));

	int sz;

	db_file_size = size;


	if ((db = fopen(DB_FILE, "rb+")) == NULL) {
		printf("AK_init_db_file: ERROR. Cannot open db file %s.\n", DB_FILE);
		exit(EXIT_ERROR);
	}


	sz = fsize(db);
	printf("AK_init_db_file: size db file %d. --- %d ---- %d\n", sz, AK_ALLOCATION_TABLE_SIZE, AK_allocationbit->last_initialized);


	if (sz > AK_ALLOCATION_TABLE_SIZE){
		printf("AK_init_db_file: Already initialized.\n");
		fclose(db);
		return (EXIT_SUCCESS);
	}

	printf("AK_init_db_file: Initializing DB file..."
		"\nPlease be patient, this can take several minutes depending "
		"on disk performance.\n");



	if (AK_allocate_blocks(db, AK_init_block(), 0, MAX_BLOCK_INIT_NUM) != EXIT_SUCCESS){
		printf("AK_init_db_file: ERROR. Problem with blocks allocation %s.\n", DB_FILE);
		exit(EXIT_ERROR);
	}

	AK_allocationbit->allocationtable[0] = 0;
	AK_allocationbit->last_allocated = 1;

	AK_blocktable_flush();

	printf("AK_init_db_file: Donex!\n");

	return (EXIT_SUCCESS);
}




/**
 * @author dv
 * @param bitset int pointer, cointainer for bit set
 * @param fromWhere has meaning just in SEQUENCE case. It describes from which address searching have to start.
 * @param gaplength tells how many used blocks could be tolerated in bitset
 * @param num Tells how many AK_free blocks has been needed
 * @param mode Defines how to obtain set of indexes to AK_free addresses
 * @param target has meaning just in case mode=AROUND: set must be as much as possible close to target
 * from both sides
 * @brief  Function prepare demanded sets from allocation table
 * @return pointer to integer indexes field with prepared set. If it , for any reason, is not possible
 * set has FREE_INT fullfilment.
 */
int * AK_get_allocation_set(int* bitsetbs, int fromWhere, int gaplength, int num, AK_allocation_set_mode mode, int target){
	register int i = 0, j = 0, k = 0;
	int numAK_free = 0, tmp = 0, tmpb = 0;
	int uu = AK_allocationbit->last_initialized;
	int AK_freebe[uu];

	if (gaplength < 1)gaplength = 1;
	if (fromWhere != 0)fromWhere = AK_allocationbit->last_allocated;

	for (i = 0; i < num; i++)bitsetbs[i] = FREE_INT;
	for (i = 0; i < uu; i++)AK_freebe[i] = FREE_INT;

	for (i = 0; i < uu; i++)
	if (!BITTEST(AK_allocationbit->bittable, i))AK_freebe[k++] = i;

	numAK_free = k;

	if (numAK_free < num){
		//no more room to place new allocations
		return bitsetbs;
	}

	switch (mode){

	case allocationSEQUENCE:
		//no gaps allowed
		//sequentialy from last allocated or from fromWhere
		if (fromWhere){
			if (num > (uu - AK_allocationbit->last_allocated))return bitsetbs;
			for (i = 0; i < numAK_free; i++)if (AK_freebe[i] == AK_allocationbit->last_allocated)break;
		}
		else i = 0;

		k = 1;
		for (; i < numAK_free - num + 1; i++){
			for (j = i; j<i + num; j++)
			if (((AK_freebe[j + 1] - AK_freebe[j]) == 1) || ((j>0) && ((AK_freebe[j - 1] + 1) == AK_freebe[j]) && ((j - i + 1) == num))){

				continue;
			}
			else k = 0;
			if (k){
				k = 0;
				for (j = i; j < i + num; j++)bitsetbs[k++] = AK_freebe[j];

				return bitsetbs;
				break;
			}
			k = 1;
		}
		return bitsetbs;
		break;


	case allocationUPPER:
		k = 0; j = 0; i = 0;
		tmp = AK_freebe[0];

		while (i < numAK_free){


			if (AK_freebe[i] != FREE_INT && (gaplength >= abs(AK_freebe[i] - tmp))){

				tmp = AK_freebe[i];
				bitsetbs[k++] = AK_freebe[i++];
			}
			else i++;

			if (i == (numAK_free - 1) || k == num)break;

			if (k && (gaplength < abs(AK_freebe[i] - bitsetbs[k - 1]))){

				k = 0;
				for (j = 0; j < num; j++)bitsetbs[j] = FREE_INT;
				tmp = AK_freebe[i];
			}
		}
		if (num != k){

			for (i = 0; i < num; i++)bitsetbs[i] = FREE_INT;
			return bitsetbs;
			break;
		}
		return bitsetbs;
		break;




	case allocationLOWER:
		k = 0; j = 0;
		i = numAK_free - 1;
		if (numAK_free<num){
			return bitsetbs;
			break;
		}

		tmp = AK_freebe[i];

		while (i>-1){

			if (AK_freebe[i] != FREE_INT && (gaplength >= abs(AK_freebe[i] - tmp))){
				tmp = AK_freebe[i];
				bitsetbs[k++] = AK_freebe[i--];
			}
			else i--;

			if (i == -1 || k == num)break;

			if (k && (gaplength < abs(AK_freebe[i] - bitsetbs[k - 1]))){

				k = 0;
				for (j = 0; j < num; j++)bitsetbs[j] = FREE_INT;
				tmp = AK_freebe[i];
			}
		}
		if (num != k){

			for (i = 0; i < num; i++)bitsetbs[i] = FREE_INT;
			return bitsetbs;
			break;
		}
		return bitsetbs;
		break;


	case allocationAROUND:

		for (i = 0; i < numAK_free; i++)if (AK_freebe[i] == target)break;
		if ((i + 1) == numAK_free){
			//no target
			return bitsetbs;
		}
		k = 0; j = i - 1;
		tmp = AK_freebe[i];
		tmpb = AK_freebe[j];

		while (num){
			if ((i < numAK_free) && (AK_freebe[i] != FREE_INT) && (gaplength >= abs(AK_freebe[i] - tmp))){
				bitsetbs[k++] = AK_freebe[i];
				num--;
				tmp = AK_freebe[i];
				i++;
			}
			else i = numAK_free;


			if ((j >= 0) && (AK_freebe[j] != FREE_INT) && (gaplength >= abs(AK_freebe[j] - tmpb))){
				bitsetbs[k++] = AK_freebe[j];
				num--;
				tmpb = AK_freebe[j];
				j--;
			}
			else j = -1;

			if ((i >= numAK_free) && j<0 && num>0){
				for (i = 0; i < num; i++)bitsetbs[i] = FREE_INT;
				return bitsetbs;
				break;
			}

		}

		if (num != 0){
			for (i = 0; i < num; i++)bitsetbs[i] = FREE_INT;
			return bitsetbs;
			break;
		}

		return bitsetbs;
		break;


	case allocationNOMODE:
		return bitsetbs;
		break;

	}


	return bitsetbs;

}


/**
 * @author dv
 * @brief  Function dumpes allocation table
 * @return nothing
 */
void AK_allocationtable_dump(int zz){
	int i;
	printf("Dump of allocation table\n");
	if (zz){
		for (i = 0; i < DB_FILE_BLOCKS_NUM; i++){
			if (AK_allocationbit->allocationtable[i] != 0xFFFFFFFF){
				printf("(%u) ", AK_allocationbit->allocationtable[i]);
				if ((i % (CHAR_IN_LINE / 8) == 0))printf(" [%d]\n", i);
				continue;
			}
			else printf("(%d) ", 0);
			if ((i % (CHAR_IN_LINE / 8) == 0))printf(" [%d]\n", i);

		}
	}


	printf("\n - - - - - -\n\n");
}


/**
 * @author dv
 * @brief  Function dumpes allocation table
 * @return nothing
 */
void AK_blocktable_dump(int zz){
	int i;
	printf("Dump of allocation table\n");
	if (zz){
		for (i = 0; i < DB_FILE_BLOCKS_NUM; i++){
			if (AK_allocationbit->last_initialized < i){
				printf("(%d-U) ", i);
				if ((i % (CHAR_IN_LINE / 8) == 0))printf("\n");
				continue;
			}
			if (BITTEST(AK_allocationbit->bittable, i))
				printf("(%d-X) ", i);
			else printf("(%d-O) ", i);
			if ((i % (CHAR_IN_LINE / 8) == 0))printf("\n");

		}
	}
	else
	{
		for (i = 0; i < DB_FILE_BLOCKS_NUM; i++){
			if (AK_allocationbit->last_initialized < i){
				printf("U ");
				if ((i % (CHAR_IN_LINE / 2) == 0))printf("\n");
				continue;
			}
			if (BITTEST(AK_allocationbit->bittable, i))
				printf("X ");
			else printf("O ");
			if ((i % (CHAR_IN_LINE / 2) == 0))printf("\n");

		}
	}

	printf("\n - - - - - -\n\n");
}

/**
 * @author dv
 * @brief  Function flushes bitmask table to disk
 * @return EXIT_SUCCESS if the file has been written to disk, EXIT_ERROR otherwise
 */
int AK_blocktable_flush(){

	if ((db = fopen(DB_FILE, "rb+")) == NULL) {
		printf("AK_allocationbit: ERROR. Cannot open db file %s.\n", DB_FILE);
		exit(EXIT_ERROR);
	}

	pthread_mutex_lock(&fileLockMutex);
	if (AK_fwrite(AK_allocationbit, AK_ALLOCATION_TABLE_SIZE, 1, db) != 1) {
		printf("AK_allocationbit: ERROR. Cannot write bit vector \n");
		exit(EXIT_ERROR);
	}
	pthread_mutex_unlock(&fileLockMutex);

	rewind(db);
	fclose(db);

	return (EXIT_SUCCESS);
}


/**
 * @author dv
 * @brief  Function gets allocation table from disk
 * @return EXIT_SUCCESS if the file has been taken from disk, EXIT_ERROR otherwise
 */
int AK_blocktable_get(){

	if ((db = fopen(DB_FILE, "rb+")) == NULL) {
		printf("AK_allocationbit: ERROR. Cannot open db file %s.\n", DB_FILE);
		exit(EXIT_ERROR);
	}

	pthread_mutex_lock(&fileLockMutex);
	if (AK_fread(AK_allocationbit, AK_ALLOCATION_TABLE_SIZE, 1, db) == 0) {
		printf("AK_allocationbit:  Cannot read bit-vector %d.\n", AK_ALLOCATION_TABLE_SIZE);
		exit(EXIT_ERROR);
	}
	pthread_mutex_unlock(&fileLockMutex);

	rewind(db);
	fclose(db);
	return (EXIT_SUCCESS);
}



/**
 * @brief  Helper function to determine file size
 * @return file size
 */
int fsize(FILE *fp){
	int sz = 1;
	struct stat st;
	stat(DB_FILE, &st);
	sz = st.st_size;
	if (sz != 1)return sz;

	int prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	fseek(fp, prev, SEEK_SET);
	return sz;
}



/**
 * @author dv
 * @brief  Function that initializes allocation table, write it to disk and cache in memory
 * @return EXIT_SUCCESS if the file has been written to disk, EXIT_ERROR otherwise
 */
int AK_init_allocation_table(){
	int i, sz;

	if ((AK_allocationbit = (AK_blocktable *)AK_malloc(sizeof(AK_blocktable))) == NULL) {
		printf("AK_allocationbit: ERROR. Cannot allocate  bit vector \n");
		exit(EXIT_ERROR);
	}

	if ((db = fopen(DB_FILE, "rb+")) == NULL) {
		if ((db = fopen(DB_FILE, "wb+")) == NULL) {
			printf("AK_allocationbit: ERROR. Cannot open db file %s.\n", DB_FILE);
			exit(EXIT_ERROR);
		}
	}

	sz = fsize(db);


	pthread_mutex_lock(&fileLockMutex);
	//if this is very first time
	if (sz == 0){
		for (i = 0; i < DB_FILE_BLOCKS_NUM; i++){
			BITCLEAR(AK_allocationbit->bittable, i);
			AK_allocationbit->allocationtable[i] = 0xFFFFFFFF;

		}
		AK_allocationbit->last_allocated = (int)0;
		AK_allocationbit->last_initialized = (int)0;
		AK_allocationbit->prepared = (int)0;
		AK_allocationbit->ltime = time(NULL);
		// memset(AK_allocationbit->allocationtable, 0xFFFFFFFF, sizeof(AK_allocationbit->allocationtable));

		if (AK_fwrite(AK_allocationbit, AK_ALLOCATION_TABLE_SIZE, 1, db) != 1) {
			printf("AK_allocationbit: ERROR. Cannot write bit vector \n");
			exit(EXIT_ERROR);
		}
		pthread_mutex_unlock(&fileLockMutex);


	}

	else if (AK_fread(AK_allocationbit, AK_ALLOCATION_TABLE_SIZE, 1, db) == 0) {
		printf("AK_allocationbit:  Cannot read bit-vector %d.\n", AK_ALLOCATION_TABLE_SIZE);
		exit(EXIT_ERROR);
	}

	fclose(db);
	pthread_mutex_unlock(&fileLockMutex);

	return (EXIT_SUCCESS);


}


/**
 * @author Markus Schatten , rearranged by dv
 * @brief  Function that initializes new block
 * @return pointer to block allocated in memory
 */
AK_block *  AK_init_block(){
	register int i = 0, j, k;
	AK_block * block = NULL;

	if ((block = (AK_block *)AK_malloc(sizeof (AK_block))) == NULL) {
		printf("AK_init_db_file: ERROR. Cannot allocate block %d\n", i);
		exit(EXIT_ERROR);
	}

	for (i = 0; i < MAX_ATTRIBUTES; i++) {
		block->header[i].type = FREE_INT;
		for (j = 0; j < MAX_ATT_NAME; j++) {
			block->header[i].att_name[j] = FREE_CHAR;
		}
		for (j = 0; j < MAX_CONSTRAINTS; j++) {
			block->header[i].integrity[j] = FREE_INT;
			for (k = 0; k < MAX_CONSTR_NAME; k++) {
				block->header[i].constr_name[j][k] = FREE_CHAR;
			}
			for (k = 0; k < MAX_CONSTR_CODE; k++) {
				block->header[i].constr_code[j][k] = FREE_CHAR;
			}
		}
	}

	for (i = 0; i < DATA_BLOCK_SIZE; i++) {
		block->tuple_dict[i].type = FREE_INT;
		block->tuple_dict[i].address = FREE_INT;
		block->tuple_dict[i].size = FREE_INT;
	}

	for (i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++) {
		block->data[i] = FREE_CHAR;
	}

	block->type = BLOCK_TYPE_FREE;
	block->chained_with = NOT_CHAINED;
	block->AK_free_space = DATA_BLOCK_SIZE * DATA_ENTRY_SIZE * sizeof (int);
	block->last_tuple_dict_id = 0;

	return block;
}


/**
 * @author  dv
 * @brief  Function that dumps block
 * @return nothing
 */
int  AK_print_block(AK_block * block, int num, char* gg, FILE *fpp){
	register int i = 0, j, k;
	int tmp = 0, tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, kk = 0;
	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	FILE *fp;

	if (fpp == NULL){


		char log_name[60];

		sprintf(log_name, "log/%d%d%d%d%02d%02d%lu_%s.txt", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec, gg);

		fp = fopen(log_name, "a+");

		if (fp == NULL){
			printf("Logging cannot open file %s\n", log_name);
			exit(EXIT_ERROR);
		}
	}
	else fp = fpp;

	if (block == NULL){

		if ((block = (AK_block *)AK_malloc(sizeof (AK_block))) == NULL) {
			printf("AK_init_db_file: ERROR. Cannot allocate block %d\n", i);
			exit(EXIT_ERROR);
		}

		block = AK_read_block(num);
		kk = 1;

	}

	fprintf(fp, "\n %d.%d.%d-%d:%02d:%02d.%lu \tcalling from:%s \n log address:%d \t memory address:%d \n\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec, gg, num, &block);


	if (block->type == BLOCK_TYPE_FREE)
		fprintf(fp, "block->type = BLOCK_TYPE_FREE\n");
	else fprintf(fp, "block->type = %d\n", block->type);

	if (block->chained_with == NOT_CHAINED)
		fprintf(fp, "block->chained_with = NOT_CHAINED\n");
	else fprintf(fp, "block->chained_with = %d\n", block->chained_with);

	fprintf(fp, "block->AK_free_space =%d\n", DATA_BLOCK_SIZE * DATA_ENTRY_SIZE * sizeof (int));
	fprintf(fp, "block->last_tuple_dict_id = %d\n", block->last_tuple_dict_id);


	fprintf(fp, "\n\n\t\tHEADER\n\n");
	for (i = 0; i < MAX_ATTRIBUTES; i++) {
		if (block->header[i].type == FREE_INT){
			if (tmp < 5)
				fprintf(fp, "block->header[ %d ].type = FREE_INT;\n", i);
			tmp++;
		}
		else{
			if (tmp >= 5)fprintf(fp, "block->header[ ].type = Repeated %d times\n", tmp);
			tmp = 0;
			fprintf(fp, "block->header[ %d ].type = %d\n", i, block->header[i].type);
		}

		for (j = 0; j < MAX_ATT_NAME; j++) {
			if (block->header[i].att_name[j] == FREE_CHAR){
				if (tmp1 < 5)
					fprintf(fp, "block->header[ %d ].att_name[ %d ] = FREE_CHAR\n", i, j);
				tmp1++;
			}
			else{
				if (tmp1 >= 5)fprintf(fp, "header[].att_name[] = Repeated %d times\n", tmp1);
				tmp1 = 0;
				fprintf(fp, "block->header[ %d ].att_name[ %d ] = %c\n", i, j, block->header[i].att_name[j]);
			}
		}


		for (j = 0; j < MAX_CONSTRAINTS; j++) {

			if (block->header[i].integrity[j] == FREE_INT){
				if (tmp2 < 5)
					fprintf(fp, "block->header[ %d ].integrity[ %d ] = FREE_INT\n", i, j);
				tmp2++;
			}
			else{
				if (tmp2 >= 5)fprintf(fp, "block->header[].integrity[] = Repeated %d times\n", tmp2);
				tmp2 = 0;
				fprintf(fp, "block->header[ %d ].integrity[ %d ] = %d\n", i, j, block->header[i].integrity[j]);
			}


			for (k = 0; k < MAX_CONSTR_NAME; k++) {
				if (block->header[i].constr_name[j][k] == FREE_CHAR){
					if (tmp3 < 5)
						fprintf(fp, "block->header[ %d ].constr_name[%d][%d] = FREE_CHAR\n", i, j, k);
					tmp3++;
				}
				else{
					if (tmp3 >= 5)fprintf(fp, "block->header[].constr_name[][]  = Repeated %d times\n", tmp3);
					tmp3 = 0;
					fprintf(fp, "block->header[%d].constr_name[%d][%d] = %c\n", i, j, k, block->header[i].constr_name[j][k]);
				}
			}

			for (k = 0; k < MAX_CONSTR_CODE; k++) {
				if (block->header[i].constr_code[j][k] == FREE_CHAR){
					if (tmp4 < 5)
						fprintf(fp, "block->header[ %d ].constr_code[%d][%d] = FREE_CHAR\n", i, j, k);
					tmp4++;
				}
				else{
					if (tmp4 >= 5)fprintf(fp, "block->header[].constr_code[][]  = Repeated %d times\n", tmp4);
					tmp4 = 0;
					fprintf(fp, "block->header[%d].constr_code[%d][%d] = %c\n", i, j, k, block->header[i].constr_code[j][k]);
				}
			}
		}
	}
	if (tmp >= 4)fprintf(fp, "block->header[ ].type = Repeated %d times\n", tmp);
	if (tmp1 >= 4)fprintf(fp, "header[].att_name[] = Repeated %d times\n", tmp1);
	if (tmp2 >= 4)fprintf(fp, "block->header[].integrity[] = Repeated %d times\n", tmp2);
	if (tmp3 >= 4)fprintf(fp, "block->header[].constr_name[][]  = Repeated %d times\n", tmp3);
	if (tmp4 >= 4)fprintf(fp, "block->header[].constr_code[][]  = Repeated %d times\n", tmp4);

	tmp = 0;
	tmp1 = 0;
	tmp2 = 0;
	tmp3 = 0;
	tmp4 = 0;

	fprintf(fp, "\n\n\n\t\t tuple_dict\n\n");

	for (i = 0; i < DATA_BLOCK_SIZE; i++) {
		if (block->tuple_dict[i].type == FREE_INT){
			if (tmp < 5)
				fprintf(fp, "block->tuple_dict[ %d ].type = FREE_INT\n", i);
			tmp++;
		}
		else{
			if (tmp >= 5)fprintf(fp, "block->tuple_dict[].type = Repeated %d times\n", tmp);
			tmp = 0;
			fprintf(fp, "block->tuple_dict[ %d ].type = %d\n", i, block->tuple_dict[i].type);
		}

		if (block->tuple_dict[i].address == FREE_INT){
			if (tmp1 < 5)
				fprintf(fp, "block->tuple_dict[ %d ].address = FREE_INT\n", i);
			tmp1++;
		}
		else{
			if (tmp1 >= 5)fprintf(fp, "block->tuple_dict[].address = Repeated %d times\n", tmp1);
			tmp1 = 0;
			fprintf(fp, "block->tuple_dict[ %d ].address = %d\n", i, block->tuple_dict[i].address);
		}

		if (block->tuple_dict[i].size == FREE_INT){
			if (tmp2 < 5)
				fprintf(fp, "block->tuple_dict[ %d ].size  = FREE_INT\n", i);
			tmp2++;
		}
		else{
			if (tmp2 >= 5)fprintf(fp, "block->tuple_dict[].size = Repeated %d times\n", tmp2);
			tmp2 = 0;
			fprintf(fp, "block->tuple_dict[ %d ].size  = %d\n", i, block->tuple_dict[i].size);
		}

	}

	if (tmp >= 4)fprintf(fp, "block->tuple_dict[].type = Repeated %d times\n", tmp);
	if (tmp1 >= 4)fprintf(fp, "block->tuple_dict[].address = Repeated %d times\n", tmp1);
	if (tmp2 >= 4)fprintf(fp, "block->tuple_dict[].size = Repeated %d times\n", tmp2);

	fprintf(fp, "\n\n\n\t\t data\n\n");

	for (i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++) {
		if (block->data[i] == FREE_CHAR){
			if (tmp3 < 5)
				fprintf(fp, "block->data[ %d ]=FREE_CHAR\n", i);
			tmp3++;
		}
		else{
			if (tmp3 >= 5)fprintf(fp, "block->data[] = Repeated %d times\n", tmp3);
			tmp3 = 0;
			fprintf(fp, "block->data[ %d ]=%c\n", i, block->data[i]);
		}
	}
	if (tmp3 >= 5)fprintf(fp, "block->data[] = Repeated %d times\n", tmp3);

	if (kk)AK_free(block);

	if (fpp == NULL)fclose(fp);

	return (EXIT_SUCCESS);
}




/**
 * @author Markus Schatten , rearranged by dv
 * @brief  Function that allocates new blocks by placing them to appropriate place
 * and then update last initialized index
 * @return EXIT_SUCCESS if the file has been written to disk, EXIT_ERROR otherwise
 */
int  AK_allocate_blocks(FILE* db, AK_block * block, int FromWhere, int HowMany){
	register int i = 0;

	if (db == NULL){
		if ((db = fopen(DB_FILE, "rb+")) == NULL) {
			printf("AK_init_db_file: ERROR. Cannot open db file %s.\n", DB_FILE);
			return EXIT_ERROR;
		}


	}

	if (fseek(db, AK_ALLOCATION_TABLE_SIZE + FromWhere * sizeof(AK_block), SEEK_SET) != 0) {
		printf("AK_allocationbit: ERROR. Cannot set position to move for AK_blocktable \n");
		return EXIT_ERROR;

	}


	for (i = FromWhere; i < FromWhere + HowMany; i++) {
		block->address = i;

		pthread_mutex_lock(&fileLockMutex);
		if (AK_fwrite(block, sizeof (*block), 1, db) != 1) {
			printf("AK_init_db_file: ERROR. Cannot write block %d\n", i);
			return EXIT_ERROR;
		}
		pthread_mutex_unlock(&fileLockMutex);

	}


	fclose(db);

	AK_allocationbit->last_initialized = i;
	AK_blocktable_flush();
	printf("AK_allocationbit->last_initialized %d\n", AK_allocationbit->last_initialized);

	return (EXIT_SUCCESS);

}





/**
 * @author Markus Schatten, updated dv
 * @brief  Function that reads a block at a given address (block number less than db_file_size).
 * New block is allocated. Database file is opened. Position is set to provided address block.
 * At the end function reads file from that position.
 * @param address block number (address)
 * @return pointer to block allocated in memory
 */
AK_block * AK_read_block(int address) {
	if (DB_FILE_BLOCKS_NUM<address || 0>address){
		printf("AK_read_block: ERROR. Out of range %s  address:%d  DB_FILE_BLOCKS_NUM:%d\n", DB_FILE, address, DB_FILE_BLOCKS_NUM);
		exit(EXIT_ERROR);
	}

	pthread_mutex_lock(&fileLockMutex);

	AK_block * blockr = AK_malloc(sizeof(AK_block));

	if ((db = fopen(DB_FILE, "rb")) == NULL) {
		printf("AK_read_block: ERROR. Cannot open db file %s.\n", DB_FILE);
		pthread_mutex_unlock(&fileLockMutex);
		exit(EXIT_ERROR);
	}


	if (fseek(db, address * sizeof(AK_block)+AK_ALLOCATION_TABLE_SIZE, SEEK_SET) != 0) {
		printf("AK_read_block: ERROR. Cannot set position to provided address block %d.\n", address);
		pthread_mutex_unlock(&fileLockMutex);
		exit(EXIT_ERROR);

	}

	if (AK_fread(blockr, sizeof(AK_block), 1, db) == 0) {
		printf("AK_read_block: ERROR. Cannot read block %d.\n", address);
		pthread_mutex_unlock(&fileLockMutex);
		exit(EXIT_ERROR);
	}

	fclose(db);
	pthread_mutex_unlock(&fileLockMutex);
	return (blockr);
}

/**
 * @author Markus Schatten
 * @brief  Function writes a block to DB file. Database file is opened. Position is set to provided address block. Block is
 written to provided address.
 * @param block poiner to block allocated in memory to write
 * @return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
 */
int AK_write_block(AK_block * block) {
	if ((db = fopen(DB_FILE, "rb+")) == NULL) {
		printf("AK_write_block: ERROR. Cannot open db file %s.\n", DB_FILE);
		exit(EXIT_ERROR);
	}
	if (fseek(db, block->address * sizeof(AK_block)+AK_ALLOCATION_TABLE_SIZE, SEEK_SET) != 0) {
		printf("AK_write_block: ERROR. Cannot set position to provided address block %d.\n", block->address);
		exit(EXIT_ERROR);
	}
	pthread_mutex_lock(&fileLockMutex);
	if (AK_fwrite(block, sizeof (*block), 1, db) != 1) {
		printf("AK_write_block: ERROR. Cannot write block at provided address %d.\n", block->address);
		pthread_mutex_unlock(&fileLockMutex);
		exit(EXIT_ERROR);
	}


	fclose(db);
	Ak_dbg_messg(HIGH, DB_MAN, "AK_write_block: Written block at address %d\n", block->address*sizeof(AK_block)+AK_ALLOCATION_TABLE_SIZE);
	pthread_mutex_unlock(&fileLockMutex);
	return (EXIT_SUCCESS);
}




/**
 * @author Nikola Bakoš, updated by Dino Laktašiæ (fixed header BUG), refurbished by dv
 * @brief  Function copy header to blocks.
 * @param header pointer to header provided for copy
 * @param blocknum pointer to addresses of blocks that header needs to be copied
 * @param num number of blocks waiting for its header
 * @return number of performed header copy
 */
int AK_copy_header(AK_header *header, int * blocknum, int num)
{
	int j = 0;
	int num_blocks = 0; /// var to check the number of written blocks
	int header_att_id = 0;
	int h_id = 0;
	AK_block *block;
	// for (j = 0; j < num; j++)
	//	printf("AK_copy_header %d. -- %d.\n", num, blocknum[j]);

	//Addresses DO NOT NEED to be sequential !!
	for (j = 0; j < num; j++) {
		block = AK_read_block(blocknum[j]);
		//	for (header_att_id = 0; header_att_id < MAX_ATTRIBUTES; header_att_id++) {
		//memcpy(&block->header[ header_att_id], &header[ header_att_id ], sizeof( *header));
		//memcpy(& block->header[header_att_id], header, sizeof ( * header));
		//}
		header_att_id = 0;
		//@TODO the check fails second time around if the table has MAX_ATTRIBUTES
		while (((h_id = header_att_id) < MAX_ATTRIBUTES) && (header[header_att_id].type != 0)) {
			if (h_id >= 0) {
				memcpy(&block->header[h_id], &header[header_att_id++], sizeof(*header));
				//printf("Block count: %d, Header ID: %d, %s\n", num_blocks, header_att_id - 1, block->header[ h_id ].att_name);
			}
		}
		/**/

		//}
		block->type = BLOCK_TYPE_NORMAL;
		block->AK_free_space = 0;
		block->last_tuple_dict_id = 0;

		/// if write of block succeded increase var num_blocks, else nothing
		if (AK_write_block(block) == EXIT_SUCCESS) {
			num_blocks++;
		}

		AK_free(block);
	}


	return num_blocks;
}


/**
 * @author dv
 * @brief  Function alocates new extent of blocks. Number of blocks is not ordered as well as a way of search for them.
 * @param start_address address (block number) to start searching for sufficient space
 * @param desired_size number of desired blocks
 * @param AK_allocation_set_mode a way of trying to fing AK_free space. Can be one of:
 allocationSEQUENCE,
 allocationUPPER,
 allocationLOWER,
 allocationAROUND
 * @param border number of allocated blocks gap
 * @param target block address around which other blocks have to be searched
 * @param header pointer to header that should be written to the new extent (all blocks)
 * @param int gl gap size
 * @return pointer to set of alocated block addresses
 */
int* AK_get_extent(int start_address, int desired_size, AK_allocation_set_mode* mode, int border, int target, AK_header *header, int gl) {
	register int i; /// vars for loop [for]
	int num_blocks = 0;



	int * blocknum = (int*)AK_malloc(sizeof(int)*(desired_size + 1));

	for (i = 0; i < 4; i++){

		if (allocationNOMODE == mode[i]){
			blocknum[0] = FREE_INT;
			return blocknum;
		}
		//get set - addresses of allocated AK_free blocks
		blocknum = AK_get_allocation_set(blocknum, border, gl, desired_size, mode[i], target);

		if (blocknum[0] == FREE_INT && mode[i] == allocationSEQUENCE){
			//there is no space at current boundaries - try to get more
			if (AK_allocate_blocks(NULL, AK_init_block(), AK_allocationbit->last_initialized, desired_size) != EXIT_SUCCESS){
				printf("AK_new_extent E1: ERROR. Problem with blocks allocation %s.\n", DB_FILE);
				return blocknum;
			}

			blocknum = AK_get_allocation_set(blocknum, start_address, gl, desired_size, allocationSEQUENCE, 6);
			if (blocknum[0] == FREE_INT){
				printf("AK_new_extent E2: ERROR. Problem with blocks allocation %s.\n", DB_FILE);
				return blocknum;
			}

		}
		if (blocknum[0] != FREE_INT)break;
	}

	if (blocknum[0] == FREE_INT)
		return blocknum;

	num_blocks = AK_copy_header(header, blocknum, desired_size);


	Ak_dbg_messg(HIGH, DB_MAN, "AK_get_extent: first_address_of_extent= %i , num_alocated_blocks= %i , end_address= %i, num_blocks= %i\n", blocknum[0], desired_size, blocknum[desired_size - 1], num_blocks);

	/// if some blocks are not succesfully allocated, which means that the extend allocation has FAILED
	if (num_blocks != desired_size) {
		Ak_dbg_messg(LOW, DB_MAN, "AK_get_extent: ERROR. Cannot allocate extent %d\n", blocknum[0]);
		blocknum[0] = FREE_INT;
		return blocknum;
	}

	//still hawen't saved what happened to allocation table
	for (i = 0; i < desired_size; i++){
		BITSET(AK_allocationbit->bittable, blocknum[i]);
		if (i < (desired_size - 1))AK_allocationbit->allocationtable[blocknum[i]] = blocknum[i + 1];
	}
	AK_allocationbit->allocationtable[blocknum[i - 1]] = blocknum[0];
	AK_allocationbit->last_allocated += i;

	AK_blocktable_flush();
	//now we have


	return blocknum;



}


/**
 * @author dv
 * @brief  Function alocates new blocks for increasing extent size.
 * @param start_address first address of extent that is subject of increasing
 * @param add_size number how many new blocks is to be added to existing extent
 * @param AK_allocation_set_mode a way of trying to fing AK_free space. Can be one of:
 allocationSEQUENCE,
 allocationUPPER,
 allocationLOWER,
 allocationAROUND
 * @param border number of allocated blocks gap
 * @param target block address around which other blocks have to be searched
 * @param header pointer to header that should be written to the new extent (all blocks)
 * @param int gl gap size
 * @return pointer to set of alocated block addresses
 */
int* AK_increase_extent(int start_address, int add_size, AK_allocation_set_mode* mode, int border, int target, AK_header *header, int gl){
	register int i; /// vars for loop [for]
	unsigned int last_address = 0;
	int * blocknum = (int*)AK_malloc(sizeof(int)*(add_size + 1));
	blocknum[0] = FREE_INT;

	for (i = 0; i < DB_FILE_BLOCKS_NUM; i++){
		if (AK_allocationbit->allocationtable[i] == start_address){
			last_address = AK_allocationbit->allocationtable[i];
			break;
		}

	}

	//there is no start_address in extent's blocks allocation table
	if (!last_address)return blocknum;


	//TODO maybe some situations will ask for start address. Zero covers all situationa.
	blocknum = AK_get_extent(0, add_size, mode, border, target, header, gl);

	//problem with obtaining new blocks
	if (blocknum[0] == FREE_INT)return blocknum;

	AK_allocationbit->allocationtable[last_address] = blocknum[0];
	for (i = 1; i < add_size; i++){
		AK_allocationbit->allocationtable[blocknum[i - 1]] = blocknum[i];
	}
	AK_allocationbit->allocationtable[blocknum[add_size - 1]] = start_address;

	return blocknum;
}



/**
 * @author Nikola Bakoš, updated by Dino Laktašiæ (fixed header BUG), refurbished by dv
 * @brief  Function alocates new extent of blocks. If argument "old_size" is 0 than size of extent is INITIAL_EXTENT_SIZE.
 * Otherwise, resize factor is set according to type of extent. If writing of block is successful, number of blocks is
 *         incremented.
 * @param start_address address (block number) to start searching for sufficient space
 * @param old_size size of previous extent in same segment (in blocks)
 * @param extent_type type of extent (can be one of:
 SEGMENT_TYPE_SYSTEM_TABLE,
 SEGMENT_TYPE_TABLE,
 SEGMENT_TYPE_INDEX,
 SEGMENT_TYPE_TRANSACTION,
 SEGMENT_TYPE_TEMP
 * @param header pointer to header that should be written to the new extent (all blocks)
 * @return address (block number) of new extent if successful, EXIT_ERROR otherwise
 */
int AK_new_extent(int start_address, int old_size, int extent_type, AK_header *header) {
	int req_AK_free_space; /// var - How much of space is required for extent
	register int i; /// vars for loop [for]
	int num_blocks = 0;
	int firstAddress = 0;


	/// if the old_size is 0 then the size of new extent is INITIAL_EXTENT_SIZE
	if (old_size == 0) {
		req_AK_free_space = INITIAL_EXTENT_SIZE;
	}
	else {
		float RESIZE_FACTOR = 0;

		switch (extent_type) {
		case SEGMENT_TYPE_TABLE:
			RESIZE_FACTOR = EXTENT_GROWTH_TABLE;
			break;
		case SEGMENT_TYPE_INDEX:
			RESIZE_FACTOR = EXTENT_GROWTH_INDEX;
			break;
		case SEGMENT_TYPE_TRANSACTION:
			RESIZE_FACTOR = EXTENT_GROWTH_TRANSACTION;
			break;
		case SEGMENT_TYPE_TEMP:
			RESIZE_FACTOR = EXTENT_GROWTH_TEMP;
			break;
		}

		req_AK_free_space = old_size + old_size * RESIZE_FACTOR;
	}

	int * blocknum = (int*)AK_malloc(sizeof(int)*(req_AK_free_space + 1));

	//get set - addresses of allocated AK_free blocks
	blocknum = AK_get_allocation_set(blocknum, 1, 0, req_AK_free_space, allocationSEQUENCE, 6);
	firstAddress = blocknum[0];


	if (blocknum[0] == FREE_INT){
		//there is no space at current boundaries - try to get more
		if (AK_allocate_blocks(NULL, AK_init_block(), AK_allocationbit->last_initialized, req_AK_free_space) != EXIT_SUCCESS){
			printf("AK_new_extent: ERROR. Problem with blocks allocation %s.\n", DB_FILE);
			return(EXIT_ERROR);
		}

		blocknum = AK_get_allocation_set(blocknum, 1, 0, req_AK_free_space, allocationSEQUENCE, 6);
		if (blocknum[0] == FREE_INT){
			printf("AK_new_extent: ERROR. Problem with blocks allocation %s.\n", DB_FILE);
			return(EXIT_ERROR);
		}

	}

	num_blocks = AK_copy_header(header, blocknum, req_AK_free_space);

	printf("AK_new_extent: first_address_of_extent= %i , num_alocated_blocks= %i , end_address= %i, num_blocks= %i\n", blocknum[0], req_AK_free_space, blocknum[req_AK_free_space - 1], num_blocks);

	firstAddress = blocknum[0];

	Ak_dbg_messg(HIGH, DB_MAN, "AK_new_extent: first_address_of_extent= %i , num_alocated_blocks= %i , end_address= %i, num_blocks= %i\n", blocknum[0], req_AK_free_space, blocknum[req_AK_free_space - 1], num_blocks);

	/// if some blocks are not succesfully allocated, which means that the extend allocation has FAILED
	if (num_blocks != req_AK_free_space) {
		Ak_dbg_messg(LOW, DB_MAN, "AK_new_extent: ERROR. Cannot allocate extent %d\n", blocknum[0]);
		return (EXIT_ERROR);
	}

	//still hawen't saved what happened to allocation table
	for (i = 0; i < req_AK_free_space; i++){
		BITSET(AK_allocationbit->bittable, blocknum[i]);
		if (i < (req_AK_free_space - 1))AK_allocationbit->allocationtable[blocknum[i]] = blocknum[i + 1];
	}
	AK_allocationbit->allocationtable[blocknum[i - 1]] = blocknum[0];
	AK_allocationbit->last_allocated += i;

	AK_blocktable_flush();
	//now we have

	AK_free(blocknum);

	return (firstAddress);
}


/**
 * @author Tomislav Fotak, refurbished by dv
 * @brief  Function that allocates new segment of extents. In this phase of implementation, only extents
 containing INITIAL_EXTENT_SIZE blocks can be allocated. If extent is successfully allocated,
 number of allocated extents is incremented and function goes to next block after allocated extent.
 Otherwise, function moves to INITIAL_EXTENT_SIZE blocks. In that way function gets either first block of
 new extent or some block in that extent which will not be AK_free.

 * @param name (character pointer) name of segment
 * @param type segment type (possible values:
 SEGMENT_TYPE_SYSTEM_TABLE,
 SEGMENT_TYPE_TABLE,
 SEGMENT_TYPE_INDEX,
 SEGMENT_TYPE_TRANSACTION,
 SEGMENT_TYPE_TEMP)
 * @param header (header pointer) pointer to header that should be written to the new extent (all blocks)
 * @return EXIT_SUCCESS for success or EXIT_ERROR if some error occurs
 */
int AK_new_segment(char * name, int type, AK_header *header) {
	int segment_start_addr = 1; /// start address for segment because we can not allocate segment in block 0
	int first_allocated_block = -1;

	Ak_dbg_messg(HIGH, DB_MAN, "AK_new_segment: %d, %s.\n", type, name);


	first_allocated_block = AK_new_extent(segment_start_addr, 0, type, header);

	if (first_allocated_block == -1) {
		return (EXIT_ERROR);
	}
	return first_allocated_block;
}

/**
 * @author Matija Novak
 * @brief  Function for creating header and initalize integrity, constraint name and constraint
 code with parameter values of function.

 * @param name name of the atribute
 * @param type type of the atribute
 * @param integrity standard integrity costraint
 * @param constr_name extra integrity constraint name
 * @param contr_code extra integrity costraint code
 * @return AK_header
 */
AK_header * AK_create_header(char * name, int type, int integrity, char * constr_name, char * contr_code) {
	AK_header * catalog_header = (AK_header *)AK_malloc(sizeof (AK_header));
	memset(catalog_header, 0, sizeof (AK_header));

	//AK_archive_log("AK_create_header", name, type, integrity, constr_name, contr_code); //ARCHIVE_LOG

	Ak_dbg_messg(HIGH, DB_MAN, "AK_create_header: Header: %s, %d\n", name, strlen(name));
	catalog_header->type = type;
	memcpy(catalog_header->att_name, name, strlen(name));

	register int i, j, k;

	/// initialize catalog_header->integrity and catalog_header->constr_name[][] and catalog_header->constr_code[][] with data given as functions parameters
	for (i = 0; i < MAX_CONSTRAINTS; i++) {
		catalog_header->integrity[i] = integrity;
		for (j = 0; j < MAX_CONSTR_NAME; j++) {
			catalog_header->constr_name[i][j] = constr_name;
		}
		for (k = 0; k < MAX_CONSTR_CODE; k++) {
			catalog_header->constr_code[i][k] = contr_code;
		}
	}

	return catalog_header;
}

/**
 * @author Matija Novak
 * @brief  Function for inserting entry in tuple_dict and data of a block. Address, type and size of
 catalog_tuple_dict are set.  Free space of block is also set.
 * @param block_adress adress of a block in which we want insert data
 * @param type type of entry_data
 * @param entry_data (char) data which is inserted, can be int but must first be converted to char
 * @param i (int) adress in tuple_dict array (example block_address->tuple_dict[i])
 * @return No return value because it gets the address of an block like a function parameter
 and works directly with the orginal block
 */
void AK_insert_entry(AK_block * block_address, int type, void * entry_data, int i) {
	AK_tuple_dict * catalog_tuple_dict = (AK_tuple_dict *)AK_malloc(sizeof (AK_tuple_dict));

	Ak_dbg_messg(HIGH, DB_MAN, "AK_insert_entry: Insert data: %d  Size of data:\n", *((int *)entry_data));

	/// using strlen becuse sizeof(entry_data) is always 4
	/// copy data into bloc->data on start position bloc->AK_free_space
	if (type == TYPE_INT) {
		memcpy(block_address->data + block_address->AK_free_space, entry_data, AK_type_size(type, entry_data));
		Ak_dbg_messg(HIGH, DB_MAN, "AK_insert_entry: Insert data: %d  Size of data:\n", (int)entry_data);
	}
	else {
		memcpy(block_address->data + block_address->AK_free_space, entry_data, AK_type_size(type, entry_data));
		Ak_dbg_messg(HIGH, DB_MAN, "AK_insert_entry: Insert data: %s  Size of data:\n", (char *)entry_data);
	}
	/// address of entry data in block->data
	catalog_tuple_dict->address = block_address->AK_free_space;

	/// calculate next AK_free space for the next entry data
	block_address->AK_free_space += AK_type_size(type, entry_data); ///sizeof(entry_data)+1);///(sizeof(int));
	/// no need for "+strlen(entry_data)" while "+1" is like "new line"

	/// type of entry data
	catalog_tuple_dict->type = type;
	/// size of entry data
	catalog_tuple_dict->size = AK_type_size(type, entry_data);

	/// copy tuple_dict to block->tuple_dict[i]
	/// must use & becouse tuple_dict[i] is value and catalog_tuple_dict adress
	memcpy(&block_address->tuple_dict[i], catalog_tuple_dict, sizeof (*catalog_tuple_dict));
}

/**
 * @author Matija Novak
 * @brief  Function initialises the sytem table catalog and writes the result in first (0) block in db_file. Catalog block,
 catalog header name, catalog header address are allocated. Address, type, chained_with and AK_free_space attributes are
 initialized. Names of various database elements are written in block.
 * @param relation address of system table of relation in db_file
 * @param attribute address of system table of attribute in db_file
 * @param index address of system table of index in db_file
 * @param view address of system table of view in db_file
 * @param sequence address of system table of sequence in db_file
 * @param function address of system table of function in db_file
 * @param function_arguments address of system table of function_arguments in db_file
 * @param trigger address of system table of trigger in db_file
 * @param trigger_conditions address of system table of trigger_conditions in db_file
 * @param db address of system table of db in db_file
 * @param db_obj address of system table of db_obj in db_file
 * @param user address of system table of user in db_file
 * @param group address of system table of group in db_file
 * @param user_group address of system table of users associated with groups in db_file
 * @param user_right address of system table of user right in db_file
 * @param group_right address of system table of group right in db_file
 * @param constraint address of system table of constraint in db_file
 * @param constraintNull address of system table of constraintNull in db_file
 * @param reference address of system table of reference in db_file
 * @return EXIT_SUCCESS if initialization was succesful if not returns EXIT_ERROR
 */
int AK_init_system_tables_catalog(int relation, int attribute, int index, int view, int sequence, int function, int function_arguments,
	int trigger, int trigger_conditions, int db, int db_obj, int user, int group, int user_group, int user_right, int group_right, int constraint, int constraintNull, int constraintUnique, int reference) {
	Ak_dbg_messg(HIGH, DB_MAN, "AK_init_system_tables_catalog: Initializing system tables catalog\n");

	AK_block * catalog_block = (AK_block *)AK_malloc(sizeof (AK_block));
	/// first header attribute of catalog_block
	AK_header * catalog_header_name = (AK_header *)AK_malloc(sizeof (AK_header));
	catalog_header_name = AK_create_header("Name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
	/// second attribute of catalog_block
	AK_header * catalog_header_address = (AK_header *)AK_malloc(sizeof (AK_header));
	catalog_header_address = AK_create_header("Address", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);

	/// initialize other elements of block (adress, type, chained_with, AK_free_space)
	catalog_block->address = 0;
	catalog_block->type = BLOCK_TYPE_NORMAL;
	catalog_block->chained_with = NOT_CHAINED;
	catalog_block->AK_free_space = 0; ///using as an address for the first AK_free space in block->data


	/// merge catalog_heder with heders created before
	memcpy(&catalog_block->header[0], catalog_header_name, sizeof (*catalog_header_name));
	memcpy(&catalog_block->header[1], catalog_header_address, sizeof (*catalog_header_address));

	// AK_tuple_dict tuple_dict[DATA_BLOCK_SIZE];
	// memcpy(catalog_block->tuple_dict, tuple_dict, sizeof (tuple_dict));

	int i = 0;
	/// insert data and tuple_dict in block
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_relation", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &relation, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_attribute", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &attribute, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_index", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &index, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_view", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &view, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_sequence", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &sequence, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_function", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &function, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_function_arguments", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &function_arguments, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_trigger", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &trigger, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_trigger_conditions", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &trigger_conditions, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_db", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &db, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_db_obj", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &db_obj, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_user", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &user, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_group", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &group, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_user_group", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &user_group, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_user_right", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &user_right, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_group_right", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &group_right, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_constraints_between", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &constraint, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_constraints_not_null", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &constraintNull, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_constraints_unique", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &constraintUnique, i);
	i++;
	AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_reference", i);
	i++;
	AK_insert_entry(catalog_block, TYPE_INT, &reference, i);


	catalog_block->last_tuple_dict_id = i;

	/// call function for writing the block on the first place in the file (ie. first block is on position zero)
	if (AK_write_block(catalog_block) == EXIT_SUCCESS) {
		return EXIT_SUCCESS;
	}
	else {
		return EXIT_ERROR;
	}
}

/**
 * @author Miroslav Policki
 * @brief  Function that sets the first num ints of a block of memory to the specified value
 * @param block pointer to the block of memory to fill
 * @param value int value to be set
 * @param num number of ints in the block of memory to be set
 * @return No return value
 */
void AK_memset_int(void *block, int value, size_t num) {
	size_t i;

	for (i = 0; i < num; i++)
		*((int *)block + i) = value;
}
/**
  * @author Unknown
  * @brief Function that registers system tables. Block at the given address is read. Various data from function arguments are 		   written in block about different database elements.
  * @param relation relation in database
  * @param attribute attribute in databse
  * @param index index in database
  * @param view view in database
  * @param sequence sequence in database
  * @param function function in database
  * @param function_arguments functional_arguments in databse
  * @param trigger trigger in database
  * @param trigger_conditions trigger conditions in databse
  * @param db database
  * @param db_obj database object
  * @param user user in database
  * @param group group in database
  * @param user_group user associated with group in database
  * @param user_right user right in database
  * @param group_right group right in database
  * @param constraint constraint in database
  * @param constraintNull Null constraint in database
  * @param reference reference database
  * @return EXIT_SUCCESS
  */
int AK_register_system_tables(int relation, int attribute, int index, int view, int sequence, int function, int function_arguments,
	int trigger, int trigger_conditions, int db, int db_obj, int user, int group, int user_group, int user_right, int group_right, int constraint, int constraintNull, int constraintUnique, int reference) {
	AK_block *relationTable = AK_read_block(relation);
	int i = 1, j = 0;
	int end;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_relation", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &relation, j);
	j++;
	end = relation + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_attribute", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &attribute, j);
	j++;
	end = attribute + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_index", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &index, j);
	j++;
	end = index + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_view", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &view, j);
	j++;
	end = view + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_sequence", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &sequence, j);
	j++;
	end = sequence + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_function", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &function, j);
	j++;
	end = function + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_function_arguments", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &function_arguments, j);
	j++;
	end = function_arguments + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_trigger", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &trigger, j);
	j++;
	end = trigger + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_trigger_conditions", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &trigger_conditions, j);
	j++;
	end = trigger_conditions + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_db", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &db, j);
	j++;
	end = db + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_db_obj", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &db_obj, j);
	j++;
	end = db_obj + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_user", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &user, j);
	j++;
	end = user + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_group", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &group, j);
	j++;
	end = group + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_user_group", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &user_group, j);
	j++;
	end = user_group + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_user_right", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &user_right, j);
	j++;
	end = user_right + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_group_right", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &group_right, j);
	j++;
	end = group_right + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_constraints_between", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &constraint, j);
	j++;
	end = constraint + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_constraints_not_null", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &constraintNull, j);
	j++;
	end = constraintNull + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j); /*OVDJE SAM DODAO relationUNIQUE*/
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_constraints_unique", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &constraintUnique, j);
	j++;
	end = constraintUnique + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	AK_insert_entry(relationTable, TYPE_INT, &i, j);
	j++;
	AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_reference", j);
	j++;
	AK_insert_entry(relationTable, TYPE_INT, &reference, j);
	j++;
	end = reference + INITIAL_EXTENT_SIZE;
	AK_insert_entry(relationTable, TYPE_INT, &end, j);
	j++;
	i++;

	relationTable->last_tuple_dict_id = j;

	AK_write_block(relationTable);
	return EXIT_SUCCESS;
}

/**
 * @author Miroslav Policki
 * @brief  Function initializes the system catalog. Headers for system tables are defined. Segments for those system tables are
 allocated. Above function AK_register_system_tables() to register system tables.
 * @return EXIT_SUCCESS if the system catalog has been successfully initialized, EXIT_ERROR otherwise
 */
int AK_init_system_catalog() {
	int relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, user_group, user_right, group_right, constraint, constraintNull, constraintUnique, reference;
	int i;

	Ak_dbg_messg(HIGH, DB_MAN, "AK_init_system_catalog: System catalog initialization started...\n");

	AK_header hConstraintNotNull[5] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "tableName", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "constraintName", 0, '\0', '\0' },
		{ TYPE_VARCHAR, "attributeName", 0, '\0', '\0' },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hConstraintUnique[5] = { /*DODAO U VEZI UNIQUE*/
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "tableName", 0, '\0', '\0' },
		{ TYPE_VARCHAR, "constraintName", 0, '\0', '\0' },
		{ TYPE_VARCHAR, "attributeName", 0, '\0', '\0' },
		{ 0, '\0', 0, '\0', '\0' }
	};


	AK_header hConstraintBetween[7] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "tableName", 0, '\0', '\0' },
		{ TYPE_VARCHAR, "constraintName", 0, '\0', '\0' },
		{ TYPE_VARCHAR, "attributeName", 0, '\0', '\0' },
		{ TYPE_VARCHAR, "start_value", 0, '\0', '\0' },
		{ TYPE_VARCHAR, "end_value", 0, '\0', '\0' },
		{ 0, '\0', 0, '\0', '\0' }
	};


	AK_header hRelation[5] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0' },
		{ TYPE_INT, "start_address", 0, '\0', '\0' },
		{ TYPE_INT, "end_address", 0, '\0', '\0' },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hAttribute[5] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ TYPE_INT, "type", 0, '\0', '\0', },
		{ TYPE_INT, "table_id", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hIndex[7] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ TYPE_INT, "start_address", 0, '\0', '\0', },
		{ TYPE_INT, "end_address", 0, '\0', '\0' },
		{ TYPE_INT, "table_id", 0, '\0', '\0', },
		{ TYPE_INT, "attribute_id", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hView[4] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "query", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hSequence[8] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ TYPE_INT, "current_value", 0, '\0', '\0', },
		{ TYPE_INT, "increment", 0, '\0', '\0', },
		{ TYPE_INT, "max", 0, '\0', '\0', },
		{ TYPE_INT, "min", 0, '\0', '\0', },
		{ TYPE_BOOL, "cycle", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hFunction[5] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ TYPE_INT, "arg_num", 0, '\0', '\0', },
		{ TYPE_INT, "return_type", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hFunction_arguments[5] = {
		{ TYPE_INT, "func_id", 0, '\0', '\0', },
		{ TYPE_INT, "att_num", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "att_type", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "att_name", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hTrigger[7] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "event", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "condition", 0, '\0', '\0', },
		{ TYPE_INT, "action", 0, '\0', '\0', },
		{ TYPE_INT, "on", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hTrigger_conditions[5] = {
		{ TYPE_INT, "trigger", 0, '\0', '\0', }, //pk
		{ TYPE_INT, "id", 0, '\0', '\0', }, //pk
		{ TYPE_VARCHAR, "data", 0, '\0', '\0', },
		{ TYPE_INT, "type", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};


	AK_header hDb[3] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hDb_obj[3] = {
		{ TYPE_INT, "db_id", 0, '\0', '\0', },
		{ TYPE_INT, "artifact_id", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};


	AK_header hUser[4] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "username", 0, '\0', '\0', },
		{ TYPE_INT, "password", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hGroup[3] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hUserGroup[3] = {
		{ TYPE_INT, "user_id", 0, '\0', '\0', },
		{ TYPE_INT, "group_id", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hUserRight[5] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_VARCHAR, "name", 0, '\0', '\0', },
		{ TYPE_INT, "artifact_id", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "right_type", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hGroupRight[5] = {
		{ TYPE_INT, "obj_id", { 0 }, { { '\0' } }, { { '\0' } } },
		{ TYPE_INT, "group_id", 0, '\0', '\0', },
		{ TYPE_INT, "artifact_id", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "right_type", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	AK_header hReference[7] = {
		{ TYPE_VARCHAR, "table", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "constraint", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "attribute", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "parent", 0, '\0', '\0', },
		{ TYPE_VARCHAR, "parent_attribute", 0, '\0', '\0', },
		{ TYPE_INT, "type", 0, '\0', '\0', },
		{ 0, '\0', 0, '\0', '\0' }
	};

	for (i = 0; i < 4; i++) {
		AK_memset_int(hConstraintNotNull[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hConstraintNotNull[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hConstraintNotNull[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) { /*DODAO UNIQUE*/
		AK_memset_int(hConstraintUnique[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hConstraintUnique[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hConstraintUnique[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 6; i++) {
		AK_memset_int(hConstraintBetween[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hConstraintBetween[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hConstraintBetween[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) {
		AK_memset_int(hRelation[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hRelation[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hRelation[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) {
		AK_memset_int(hAttribute[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hAttribute[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hAttribute[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 6; i++) {
		AK_memset_int(hIndex[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hIndex[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hIndex[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 3; i++) {
		AK_memset_int(hView[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hView[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hView[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 7; i++) {
		AK_memset_int(hSequence[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hSequence[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hSequence[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) {
		AK_memset_int(hFunction[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hFunction[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hFunction[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) {
		AK_memset_int(hFunction_arguments[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hFunction_arguments[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hFunction_arguments[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 6; i++) {
		AK_memset_int(hTrigger[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hTrigger[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hTrigger[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) {
		AK_memset_int(hTrigger_conditions[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hTrigger_conditions[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hTrigger_conditions[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 2; i++) {
		AK_memset_int(hDb[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hDb[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hDb[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 2; i++) {
		AK_memset_int(hDb_obj[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hDb_obj[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hDb_obj[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 3; i++) {
		AK_memset_int(hUser[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hUser[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hUser[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 2; i++) {
		AK_memset_int(hGroup[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hGroup[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hGroup[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 2; i++) {
		AK_memset_int(hUserGroup[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hUserGroup[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hUserGroup[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) {
		AK_memset_int(hUserRight[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hUserRight[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hUserRight[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 4; i++) {
		AK_memset_int(hGroupRight[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hGroupRight[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hGroupRight[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for (i = 0; i < 6; i++) {
		AK_memset_int(hReference[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hReference[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hReference[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}
	Ak_dbg_messg(HIGH, DB_MAN, "AK_init_system_catalog: Creating new segments...\n");



	relation = AK_new_segment("AK_relation", SEGMENT_TYPE_SYSTEM_TABLE, hRelation);
	attribute = AK_new_segment("AK_attribute", SEGMENT_TYPE_SYSTEM_TABLE, hAttribute);
	index = AK_new_segment("AK_index", SEGMENT_TYPE_SYSTEM_TABLE, hIndex);
	view = AK_new_segment("AK_view", SEGMENT_TYPE_SYSTEM_TABLE, hView);
	sequence = AK_new_segment("AK_sequence", SEGMENT_TYPE_SYSTEM_TABLE, hSequence);
	function = AK_new_segment("AK_function", SEGMENT_TYPE_SYSTEM_TABLE, hFunction);
	function_arguments = AK_new_segment("AK_function_arguments", SEGMENT_TYPE_SYSTEM_TABLE, hFunction_arguments);
	trigger = AK_new_segment("AK_trigger", SEGMENT_TYPE_SYSTEM_TABLE, hTrigger);
	trigger_conditions = AK_new_segment("AK_trigger_conditions", SEGMENT_TYPE_SYSTEM_TABLE, hTrigger_conditions);
	db = AK_new_segment("AK_db", SEGMENT_TYPE_SYSTEM_TABLE, hDb);
	db_obj = AK_new_segment("AK_db_obj", SEGMENT_TYPE_SYSTEM_TABLE, hDb_obj);
	user = AK_new_segment("AK_user", SEGMENT_TYPE_SYSTEM_TABLE, hUser);
	group = AK_new_segment("AK_group", SEGMENT_TYPE_SYSTEM_TABLE, hGroup);
	user_group = AK_new_segment("AK_user_group", SEGMENT_TYPE_SYSTEM_TABLE, hUserGroup);
	user_right = AK_new_segment("AK_user_right", SEGMENT_TYPE_SYSTEM_TABLE, hUserRight);
	group_right = AK_new_segment("AK_group_right", SEGMENT_TYPE_SYSTEM_TABLE, hGroupRight);
	constraint = AK_new_segment("AK_constraints_between", SEGMENT_TYPE_SYSTEM_TABLE, hConstraintBetween);
	constraintNull = AK_new_segment("AK_constraints_not_null", SEGMENT_TYPE_SYSTEM_TABLE, hConstraintNotNull);
	constraintUnique = AK_new_segment("AK_constraints_unique", SEGMENT_TYPE_SYSTEM_TABLE, hConstraintUnique);
	reference = AK_new_segment("AK_reference", SEGMENT_TYPE_SYSTEM_TABLE, hReference);

	Ak_dbg_messg(LOW, DB_MAN, "AK_init_system_catalog: Segments created!\n");

	if (EXIT_SUCCESS == AK_init_system_tables_catalog(relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, user_group, user_right, group_right, constraint, constraintNull, constraintUnique, reference)) {
		AK_register_system_tables(relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, user_group, user_right, group_right, constraint, constraintNull, constraintUnique, reference);
		printf("AK_init_system_catalog: System catalog initialized!\n");
		return EXIT_SUCCESS;
	}
	else {
		printf("AK_init_system_catalog: ERROR. AK_init_system_tables_catalog(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) failed.\n", relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, user_group, user_right, group_right);
		return EXIT_ERROR;
	}
}

/**
 * @author Markus Schatten
 * @brief  Function deletes a block by a given block address (resets the header and data). Types, integrities, constraint names,
 constraint codes are set to "AK_free" values. In tuple dictionary type, address and size are set to FREE_INT values. Data 		   of block is set to FREE_CHAR.
 * @param address address of the block to be deleted
 * @return returns EXIT_SUCCESS if deletion successful, else EXIT_ERROR
 */
int AK_delete_block(int address) {
	register int i, j, k;

	AK_header head[MAX_ATTRIBUTES];
	AK_tuple_dict tuple_dict[DATA_BLOCK_SIZE];
	unsigned char data[DATA_BLOCK_SIZE * DATA_ENTRY_SIZE];

	for (i = 0; i < MAX_ATTRIBUTES; i++) {
		head[i].type = FREE_INT;
		for (j = 0; j < MAX_ATT_NAME; j++) {
			head[i].att_name[j] = FREE_CHAR;
		}
		for (j = 0; j < MAX_CONSTRAINTS; j++) {
			head[i].integrity[j] = FREE_INT;
			for (k = 0; k < MAX_CONSTR_NAME; k++) {
				head[i].constr_name[j][k] = FREE_CHAR;
			}
			for (k = 0; k < MAX_CONSTR_CODE; k++) {
				head[i].constr_code[j][k] = FREE_CHAR;
			}
		}
	}

	for (i = 0; i < DATA_BLOCK_SIZE; i++) {
		tuple_dict[i].type = FREE_INT;
		tuple_dict[i].address = FREE_INT;
		tuple_dict[i].size = FREE_INT;
	}
	for (i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++) {
		data[i] = FREE_CHAR;
	}

	AK_block * block = AK_read_block(address);
	block->type = BLOCK_TYPE_FREE;
	block->chained_with = NOT_CHAINED;
	block->AK_free_space = DATA_BLOCK_SIZE * DATA_ENTRY_SIZE * sizeof (int);
	memcpy(block->header, head, sizeof (*head));
	memcpy(block->tuple_dict, tuple_dict, sizeof (*tuple_dict));
	memcpy(block->data, data, sizeof (*data));

	//remove its status from allocation table
	BITCLEAR(AK_allocationbit->bittable, address);
	if (address == AK_allocationbit->last_allocated)AK_allocationbit->last_allocated = address - 1;
	AK_blocktable_flush();
	AK_blocktable_get();

	if (AK_write_block(block) == EXIT_SUCCESS) {
		return EXIT_SUCCESS;
	}
	return EXIT_ERROR;
}

/**
 * @author Dejan Samboliæ
 * @brief  Function deletes an extent between begin and end blocks
 * @param begin address of extent's first block
 * @param end address of extent's last block
 * @return EXIT_SUCCESS if extent has been successfully deleted, EXIT_ERROR otherwise
 */
int AK_delete_extent(int begin, int end) {
	int address;
	for (address = begin; address < end + 1; address++) {
		if (AK_delete_block(address) == EXIT_ERROR) {
			return EXIT_ERROR;
		}
		AK_allocationbit->allocationtable[address] = 0xFFFFFFFF;
	}
	return (EXIT_SUCCESS);
}

/**
 * @author Mislav Èakariæ
 * @param name name of the segment
 * @param type type of the segment
 * @return EXIT_SUCCESS if extent has been successfully deleted, EXIT_ERROR otherwise
 */
int AK_delete_segment(char * name, int type) {
	int i = 0;
	table_addresses *addresses = (table_addresses*)AK_get_segment_addresses(name, type);
	while (addresses->address_from[i] != 0) {
		if (AK_delete_extent(addresses->address_from[i], addresses->address_to[i]) == EXIT_ERROR)
			return EXIT_ERROR;
		i++;
	}
	AK_list *row_root = (AK_list *)AK_malloc(sizeof (AK_list));
	Ak_Init_L(row_root);
	char *system_table;
	switch (type) {
	case SEGMENT_TYPE_TABLE:
		system_table = "AK_relation";
		break;
	case SEGMENT_TYPE_INDEX:
		system_table = "AK_index";
		break;
	case SEGMENT_TYPE_TRANSACTION:
		//TO-DO
		printf("Not implemented yet!\n");
		return EXIT_ERROR;
		break;
	case SEGMENT_TYPE_TEMP:
		//TO-DO
		printf("Not implemented yet!\n");
		return EXIT_ERROR;
		break;
	default:
		return EXIT_ERROR;
	}
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, system_table, "name", row_root, 1);
	Ak_delete_row(row_root);
	AK_free(row_root);
	return EXIT_SUCCESS;
}

/**
 * @author Markus Schatten
 * @return Function that calls functions AK_init_db_file() and AK_init_system_catalog() to initialize disk manager
 */
int AK_init_disk_manager() {
	//int size_in_mb = DB_FILE_SIZE;
	float size = DB_FILE_BLOCKS_NUM; //1024 * 1024 * size_in_mb / sizeof ( AK_block);

	if (AK_init_allocation_table() == EXIT_ERROR)
		exit(EXIT_ERROR);

	if (AK_allocationbit->prepared == 31){

		printf("\n\tDisk manager has been initialized at %s\n\n", asctime(localtime(&AK_allocationbit->ltime)));
		Ak_dbg_messg(LOW, DB_MAN, "Block size is: %d\n", sizeof (AK_block));
		Ak_dbg_messg(LOW, DB_MAN, "%d blocks for %d MiB\n", (int)size, DB_FILE_SIZE);


		return EXIT_SUCCESS;

	}

	printf("AK_init_disk_manager: Initializing disk manager...\n\n");
	Ak_dbg_messg(LOW, DB_MAN, "AK_init_disk_manager: Block size is: %d\n", sizeof (AK_block));
	Ak_dbg_messg(LOW, DB_MAN, "AK_init_disk_manager: We need %d blocks for %d MiB\n", (int)size, DB_FILE_SIZE);

	if (AK_init_db_file((int)size) == EXIT_SUCCESS) {
		if (AK_init_system_catalog() == EXIT_SUCCESS) {
			printf("AK_init_disk_manager: Disk manager initialized!\n\n");
			AK_allocationbit->prepared = 31;
			AK_allocationbit->ltime = time(NULL);
			AK_blocktable_flush();
			int vari;
			scanf("%d", &vari);
			return EXIT_SUCCESS;
		}
		printf("AK_init_disk_manager: ERROR. System catalog initialization failed!\n");
		return EXIT_ERROR;
	}
	printf("AK_init_disk_manager: ERROR. DB file initialization failed!");
	return EXIT_ERROR;
}

void AK_allocationbit_test() {

	AK_blocktable_dump(0);

}

void AK_allocationtable_test() {

	AK_allocationtable_dump(1);


}
