/**
@file product.c Provides functions for relational product operation
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 17 */

#include "product.h"

/**
 * @author Dino Laktašić
 * @brief  Function to make product of two tables
 * @param srcTable1 name of the first table
 * @param srcTable2 name of the second table
 * @param dstTable name of the product table
 * @return if success returns EXIT_SUCCESS, else returns EXIT_ERROR
 */
int AK_product(char *srcTable1, char * srcTable2, char * dstTable) {
	AK_PRO;
	table_addresses *src_addr1 = (table_addresses *) AK_get_table_addresses(srcTable1);
	table_addresses *src_addr2 = (table_addresses *) AK_get_table_addresses(srcTable2);

	int startAddress1 = src_addr1->address_from[0];
	int startAddress2 = src_addr2->address_from[0];

	if ((startAddress1 != 0) && (startAddress2 != 0)) {
		//initialize new segment
		int num_att1 = AK_num_attr(srcTable1);
		int num_att2 = AK_num_attr(srcTable2);

		register int i, j, k,   m, n, o,   u;

		int address, type, size;

		// will be needed later as a place to hold cell data
		char celldata[MAX_VARCHAR_LENGTH];

		AK_block *tbl1_temp_block = (AK_block *) AK_read_block(startAddress1);
		AK_block *tbl2_temp_block = (AK_block *) AK_read_block(startAddress2);

		//Currently it works with headers no longer than MAX_ATTRIBUTES. The same header is written in all allocated table blocks.
		//This is wrong and need to be corrected.
		//If header doesn't fit in the first block than system must write the remain attributes from header to the new block.
		//Correction must be handled in all functions that write, read or count header attributes.
		int head = 0;
		AK_header header[MAX_ATTRIBUTES];

		/* Adding header to new aggregation table */
		while (strcmp(tbl1_temp_block->header[head].att_name, "") != 0) {
			memcpy(&header[head], &tbl1_temp_block->header[head], sizeof (tbl1_temp_block->header[head]));
			head++;
		}

		head = 0;
		while (strcmp(tbl2_temp_block->header[head].att_name, "") != 0) {
			memcpy(&header[head + num_att1], &tbl2_temp_block->header[head], sizeof (tbl2_temp_block->header[head]));
			head++;
		}

		/* renaming column names which are same in both tables */
		for ( i = 0; i < num_att1 + num_att2; i++ ) {
			for ( j = i + 1; j < num_att1 + num_att2; j++ ) {
				if ( strcmp( header[ i ].att_name, header[ j ].att_name ) == 0 ) {
					char column_name[ MAX_ATT_NAME ];  // name of column which is same in both tables

					strcpy( column_name, header[ i ].att_name );

					strcpy( header[ i ].att_name, srcTable1 );
					strcat( header[ i ].att_name, "." );
					strcat( header[ i ].att_name, column_name );

					strcpy( header[ j ].att_name, srcTable2 );
					strcat( header[ j ].att_name, "." );
					strcat( header[ j ].att_name, column_name );
					break;
				}
			}
		}

		/* initializing new table with header */
		AK_initialize_new_segment(dstTable, SEGMENT_TYPE_TABLE, header);

		Ak_dbg_messg(LOW, REL_OP, "\nTABLE %s CREATED from %s and %s\n", dstTable, srcTable1, srcTable2);
		Ak_dbg_messg(MIDDLE, REL_OP, "\nAK_product: start copying data\n");

		/*product procedure function */
		AK_product_procedure(srcTable1,srcTable2,dstTable,header);

		Ak_dbg_messg(LOW, REL_OP, "PRODUCT_TEST_SUCCESS\n\n");
		AK_EPI;
		return EXIT_SUCCESS;
	} else {
		Ak_dbg_messg(LOW, REL_OP, "\n AK_product: Table/s doesn't exist!");
		AK_free(src_addr1);
		AK_free(src_addr2);
		AK_EPI;
		return EXIT_ERROR;
	}
}

/**
 * @author Dino Laktašić
 * @brief  Product procedure (iterating trough both tables and concating rows)
 * @param srcTable1 name of the first table
 * @param srcTable2 name of the second table
 * @param dstTable name of the product table
 * @param header header of product table
 */
void AK_product_procedure(char *srcTable1, char * srcTable2, char * dstTable, AK_header header[MAX_ATTRIBUTES]){
	table_addresses *src_addr1 = (table_addresses *) AK_get_table_addresses(srcTable1);
	table_addresses *src_addr2 = (table_addresses *) AK_get_table_addresses(srcTable2);

	AK_block *tbl1_temp_block = (AK_block *) AK_read_block(src_addr1->address_from[0]);
	AK_block *tbl2_temp_block = (AK_block *) AK_read_block(src_addr2->address_from[0]);

	//initialize new segment
		int num_att1 = AK_num_attr(srcTable1);
		int num_att2 = AK_num_attr(srcTable2);

	// will be needed later as a place to hold cell data
		char celldata[MAX_VARCHAR_LENGTH];

	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof(struct list_node));
	Ak_Init_L3(&row_root);

	register  j, k, l, m, n, o,   u;
		/**
		 * Product procedure
		 * Going through one table, and for each row in it, going through another table,
		 * and joining rows that way!
		 */
		register i = 0;
		/* for each extent in first table */
		while ( src_addr1->address_from[i] != 0 ) {
			/* for each block in extent (first table) */
			for ( j = src_addr1->address_from[ i ]; j < src_addr1->address_to[ i ]; j++ ) {
				tbl1_temp_block = (AK_block *) AK_read_block(j);
				if ( tbl1_temp_block->last_tuple_dict_id == 0 ) { break; }

				/* for each row in block (first table)... */
				for ( k = 0; k < tbl1_temp_block->last_tuple_dict_id; k += num_att1 ) {
					// for each row in block, we have to read every other row from the second table
					m = 0;
					/* for each extent in other table */
					while ( src_addr2->address_from[m] != 0 ) {
						/* for each block in extent (other table) */
						for ( n = src_addr2->address_from[ m ]; n < src_addr2->address_to[ m ]; n++ ) {
							tbl2_temp_block = (AK_block *) AK_read_block(n);
							if ( tbl2_temp_block->last_tuple_dict_id == 0 ) { break; }

							/* for each row in block (other table) ... */
							for ( o = 0; o < tbl2_temp_block->last_tuple_dict_id; o += num_att2 ) {
								/**
								 * Sorry about indentations, but it is necessary to do it this way,
								 * until someone creates function to iterate through table rows
								 * (which would be pretty neat, btw.)
								 * At this level of code, we have access to rows from the first
								 * and from the second table.
								 * Then we do it this way: for each row in first table,
								 * read row from second table. And concatenate them!
								 * Since we have loop hierarchy here, each row from first table
								 * will be concatenated with each row from second table.
								 * End of story! Let's get back to work...
								 * BTW. Please comment your code in the future.
								 * It is a lot easier when someone explains to you what
								 * he or she was thinking that moment.
								 * Write comments in english. Write 'em in croatian.
								 * It does not matter! Just explain yourself!
								 * And share the idea about comments among others, please. Thank you!
								 */

								/* now we just have to copy cell data from one and from another row */
								int cellid, celltype, celladdress, cellsize;
								/* first table first */
							 	for ( u = 0; u < num_att1; u++ ) {
							 		cellid = k + u;
							 		celltype = tbl1_temp_block->tuple_dict[ cellid ].type;
							 		celladdress = tbl1_temp_block->tuple_dict[ cellid ].address;
							 		cellsize = tbl1_temp_block->tuple_dict[ cellid ].size;
							 		memcpy( celldata, &(tbl1_temp_block->data[ celladdress ]), cellsize );
							 		if ( celltype == TYPE_VARCHAR ) {
							 			celldata[ cellsize ] = '\0';
							 		}

							 		Ak_Insert_New_Element( celltype, celldata, dstTable,
							 			header[ u ].att_name, row_root);
							 	}

							 	/* and then rows from the second table rows come into play */
							 	for ( u = 0; u < num_att2; u++ ) {
							 		cellid = o + u;
							 		celltype = tbl2_temp_block->tuple_dict[ cellid ].type;
							 		celladdress = tbl2_temp_block->tuple_dict[ cellid ].address;
							 		cellsize = tbl2_temp_block->tuple_dict[ cellid ].size;
							 		memcpy( celldata, &(tbl2_temp_block->data[ celladdress ]), cellsize );
							 		if ( celltype == TYPE_VARCHAR ) {
							 			celldata[ cellsize ] = '\0';
							 		}

							 		Ak_Insert_New_Element( celltype, celldata, dstTable,
							 			header[ num_att1 + u ].att_name, row_root);
							 	}

							 	/* finally, inserting new row in table and emptying linked list */
							 	Ak_insert_row(row_root);
							 	Ak_DeleteAll_L3(&row_root);
							}
						}
						m++;
					}
				}
			}
			i++;
		}
	AK_free(row_root);
}

/**
 * @author Dino Laktašić
 * @brief  Function for product operator testing
 *
 */
TestResult AK_op_product_test() {
	AK_PRO;
	char *sys_table = "AK_relation";
    	char *destTable = "product_test";
    	char *tblName1 = "employee";
    	char *tblName2 = "department";

	printf("\n********** PRODUCT TEST **********\n\n");


    	if (AK_if_exist(destTable, sys_table) == 0) {
    		printf("Table %s does not exist!\n", destTable);
		AK_product(tblName1, tblName2, destTable);
    	}
    	else {
		printf("Table %s already exists!\n", destTable);
   	}

   	AK_print_table(destTable);

	/**
	 * How does this test work?
	 * First, it reads all cells from both of the tables, employee and department.
	 * After that, it reads all cells from product_test table and compares
	 * the data.
	 */

	int i,j,k, current_row, num_errors = 0;
	int employee_numrows = AK_get_num_records("employee"),
		department_numrows = AK_get_num_records("department"),
		employee_numattr = AK_num_attr("employee"),
		department_numattr = AK_num_attr("department");
	table_addresses *employee_addr = (table_addresses *) AK_get_table_addresses("employee"),
					*department_addr = (table_addresses *) AK_get_table_addresses("department"),
					*product_addr = (table_addresses *) AK_get_table_addresses("product_test");
	AK_block *tmp_block;
	// defining placeholders for columns that will be read
	int *t1_idprof = AK_malloc( employee_numrows * sizeof(int) ),
		*t1_iddept = AK_malloc( employee_numrows * sizeof(int) );

	int *t2_iddept = AK_malloc( department_numrows * sizeof(int) );
	char **t2_deptname = AK_malloc( department_numrows * sizeof(char *) ),
		 **t2_manager = AK_malloc( department_numrows * sizeof(char *) );
	for ( i = 0; i < department_numrows; i++ ) {
		t2_deptname[i] = AK_malloc( MAX_VARCHAR_LENGTH );
		t2_deptname[i][0] = '\0';
		t2_manager[i] = AK_malloc( MAX_VARCHAR_LENGTH );
		t2_manager[i][0] = '\0';
	}

	/**
	 * Reading data from first two tables (employee and department)
	 */
	i = current_row = 0;
	/* for each extent in first table */
	while ( employee_addr->address_from[i] != 0 ) {
		/* for each block in extent (first table) */
		for ( j = employee_addr->address_from[ i ]; j < employee_addr->address_to[ i ]; j++ ) {
			tmp_block = (AK_block *) AK_read_block(j);
			if ( tmp_block->last_tuple_dict_id == 0 ) { break; }

			/* for each row in block (first table)... */
			for ( k = 0; k < tmp_block->last_tuple_dict_id; k += employee_numattr ) {
				/* let's find out cell data right now */
				int celladdress;
				// copying first cell in row
				celladdress = tmp_block->tuple_dict[ k ].address;
				memcpy(&(t1_idprof[current_row]), &(tmp_block->data[celladdress]), sizeof(int));
				// copying second cell in row
				celladdress = tmp_block->tuple_dict[ k + 1 ].address;
				memcpy(&(t1_iddept[current_row]), &(tmp_block->data[celladdress]), sizeof(int));

				current_row++;
			}
		}
		i++;
	}

	// second table
	i = current_row = 0;
	/* for each extent in second table */
	while ( department_addr->address_from[i] != 0 ) {
		/* for each block in extent (second table) */
		for ( j = department_addr->address_from[ i ]; j < department_addr->address_to[ i ]; j++ ) {
			tmp_block = (AK_block *) AK_read_block(j);
			if ( tmp_block->last_tuple_dict_id == 0 ) { break; }

			/* for each row in block (first table)... */
			for ( k = 0; k < tmp_block->last_tuple_dict_id; k += department_numattr ) {
				/* let's find out cell data right now */
				int celladdress, cellsize;
				// copying first cell in row
				celladdress = tmp_block->tuple_dict[ k ].address;
				memcpy(&(t2_iddept[current_row]), &(tmp_block->data[celladdress]), sizeof(int));
				// copying second cell in row
				celladdress = tmp_block->tuple_dict[ k + 1 ].address;
				cellsize = tmp_block->tuple_dict[ k + 1 ].size;
				memcpy(t2_deptname[current_row], &(tmp_block->data[celladdress]), cellsize);
				t2_deptname[current_row][cellsize] = '\0';
				// copying third cell in row
				celladdress = tmp_block->tuple_dict[ k + 2 ].address;
				cellsize = tmp_block->tuple_dict[ k + 2 ].size;
				memcpy(t2_manager[current_row], &(tmp_block->data[celladdress]), cellsize);
				t2_manager[current_row][cellsize] = '\0';

				current_row++;
			}
		}
		i++;
	}


	/**
	 * Now reading data from product table
	 * and comparing it to the data in first two tables
	 */
	// first declaring variables which will hold temporary cell data, to be compared with original data
	int prod_idprof, prod_iddept_t1, prod_iddept_t2;
	char *prod_deptname = AK_malloc( MAX_VARCHAR_LENGTH * sizeof(char) ),
		 *prod_manager = AK_malloc( MAX_VARCHAR_LENGTH * sizeof(char) );

	// then reading data from the rows
	i = current_row = 0;
	while ( product_addr->address_from[i] != 0 ) {
		/* for each block in extent (product table) */
		for ( j = product_addr->address_from[ i ]; j < product_addr->address_to[ i ]; j++ ) {
			tmp_block = (AK_block *) AK_read_block(j);
			if ( tmp_block->last_tuple_dict_id == 0 ) { break; }

			/* for each row in block (product table)... */
			for ( k = 0; k < tmp_block->last_tuple_dict_id; k += employee_numattr + department_numattr ) {
				/* let's find out cell data right now */
				int celladdress, cellsize;
				// copying first cell in row (id_prof)
				celladdress = tmp_block->tuple_dict[ k ].address;
				memcpy( &prod_idprof, &(tmp_block->data[celladdress]), sizeof(int) );
				// copying second cell in row (employee.id_department)
				celladdress = tmp_block->tuple_dict[ k + 1 ].address;
				memcpy( &prod_iddept_t1, &(tmp_block->data[celladdress]), sizeof(int) );
				// copying third cell in row (department.id_department)
				celladdress = tmp_block->tuple_dict[ k + 2 ].address;
				memcpy( &prod_iddept_t2, &(tmp_block->data[celladdress]), sizeof(int) );
				// copying fourth cell in row (dep_name)
				celladdress = tmp_block->tuple_dict[ k + 3 ].address;
				cellsize = tmp_block->tuple_dict[ k + 3 ].size;
				memcpy( prod_deptname, &(tmp_block->data[celladdress]), cellsize );
				prod_deptname[cellsize] = '\0';
				// copying fifth cell in row (manager)
				celladdress = tmp_block->tuple_dict[ k + 4 ].address;
				cellsize = tmp_block->tuple_dict[ k + 4 ].size;
				memcpy( prod_manager, &(tmp_block->data[celladdress]), cellsize );
				prod_manager[cellsize] = '\0';

				/* now comparing data to the original two tables */
				int first_table_current_row = current_row / department_numrows,
					second_table_current_row = current_row % department_numrows;

				if ( prod_idprof != t1_idprof[ first_table_current_row ] ) {
					num_errors++;
					printf("Error in product table, row: %d! Wrong id_prof!\n", current_row+1);
	            	printf("Table showed value: '%d', but it should show: '%d'\n\n",
	            		prod_idprof, t1_idprof[ first_table_current_row ] );
				}
				if ( prod_iddept_t1 != t1_iddept[ first_table_current_row ] ) {
					num_errors++;
					printf("Error in product table, row: %d! Wrong id_department (2nd column)!\n", current_row+1);
	            	printf("Table showed value: '%d', but it should show: '%d'\n\n",
	            		prod_iddept_t1, t1_iddept[ first_table_current_row ] );
				}
				if ( prod_iddept_t2 != t2_iddept[ second_table_current_row ] ) {
					num_errors++;
					printf("Error in product table, row: %d! Wrong id_department (3rd column)!\n", current_row+1);
	            	printf("Table showed value: '%d', but it should show: '%d'\n\n",
	            		prod_iddept_t2, t2_iddept[ second_table_current_row ] );
				}
				if ( strcmp( prod_deptname, t2_deptname[ second_table_current_row ] ) != 0 ) {
					num_errors++;
					printf("Error in product table, row: %d! Wrong dep_name!\n", current_row+1);
	            	printf("Table showed value: '%s', but it should show: '%s'\n\n",
	            		prod_deptname, t2_deptname[ second_table_current_row ] );
				}
				if ( strcmp( prod_manager, t2_manager[ second_table_current_row ] ) != 0 ) {
					num_errors++;
					printf("Error in product table, row: %d! Wrong manager!\n", current_row+1);
	            	printf("Table showed value: '%s', but it should show: '%s'\n\n",
	            		prod_manager, t2_manager[ second_table_current_row ] );
				}

				current_row++;
			}
		}
		i++;
	}

	if ( num_errors == 0) {
		printf("\nTEST PASSED!\n");
    } else {
    	printf("\nTEST FAILED! Number of errors: %d\n", num_errors);
    }

	AK_free(t1_idprof);
	AK_free(t1_iddept);
	AK_free(t2_iddept);

	for ( i = 0; i < department_numrows; i++ ) {
		AK_free(t2_deptname[i]);
		AK_free(t2_manager[i]);
	}
	AK_free(t2_deptname);
	AK_free(t2_manager);
	AK_EPI;
	return TEST_result(0,0);
}
