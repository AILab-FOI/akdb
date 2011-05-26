/**
@file test.c Provides functions for testing purposes
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

#include "test.h"

/**
 * @brief Function for creating test tables
 * @author Dino Laktašić
 * @return void
 */
void create_test_tables() {
    int mbr, year, id_prof, id_department;
    float weight;

    //---------------------------------------> CREATE TABLE 'STUDENT' <---------------------------------------
    //create header
    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header* temp;

    temp = (AK_header*) AK_create_header("mbr", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("weight", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 4, temp, sizeof ( AK_header));
    memset(t_header + 5, 0, MAX_ATTRIBUTES - 5);

    //create table
    char *tblName = "student";
    int startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    element row_root = (element) malloc(sizeof (list));
    InitializeList(row_root);

    mbr = 35890;
    year = 1999;
    weight = 80.00;

    //insert rows in table student
    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Dino", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Laktasic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Netko", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Netkic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Mislav", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Cakaric", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivan", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Horvat", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivo", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Marko", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Markovic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivan", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivanovic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Josip", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Josipovic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivan", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ankovic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Marina", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Marovic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Mario", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Maric", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Matija", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Matkovic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivana", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Ivic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "John", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Smith", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "William", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Brown", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "David", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Jones", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Robert", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "White", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "James", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Jones", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Jack", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Moore", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Joseph", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Harris", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Richard", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Thomas", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Daniel", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Jackson", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Martin", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Clark", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Joe", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Davis", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Paul", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Lee", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &mbr, tblName, "mbr", row_root);
    InsertNewElement(TYPE_VARCHAR, "Steve", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Parker", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, &year, tblName, "year", row_root);
    InsertNewElement(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //--------------------------------------> CREATE TABLE 'PROFESSOR' <-------------------------------------
    //create header
    AK_header t_header2[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("tel", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("email", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 4, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("web_page", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 5, temp, sizeof ( AK_header));
    memset(t_header2 + 6, 0, MAX_ATTRIBUTES - 6);

    //create table
    tblName = "professor";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header2);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    //row_root = (element) malloc(sizeof (list));
    InitializeList(row_root);

    id_prof = 35890;
    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Baca", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042390873", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "miroslav.baca@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/baca.miroslav/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Balaban", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "000000000", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "igor.balaban@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/balaban.igor/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Antun", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Brumnic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "antun.brumnic@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/brumnic.antun/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Mirko", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Cubrilo", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "mirko.cubrilo@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/cubrilo.mirko/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Dragutin", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Kermek", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "dragutin.kermek@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/kermek.dragutin/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Tonimir", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Kisasondi", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "tonimir.kisasondi@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/kisasondi.tonimir/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Alen", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Lovrencic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042390866", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "alovrenc@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/lovrencic.alen/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Markus", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Schatten", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042390892", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "markus.schatten@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/schatten.markus/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Neven", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Vrcek", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042390892", tblName, "tel", row_root);
	InsertNewElement(TYPE_VARCHAR, "neven.vrcek@foi.hr", tblName, "email", row_root);
	InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/vrcek.neven/", tblName, "web_page", row_root);
	insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


	//--------------------------------------> CREATE TABLE 'PROFESSOR2' <-------------------------------------
	//create header
	AK_header t_header7[ MAX_ATTRIBUTES ];

	temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2, temp, sizeof ( AK_header));
	temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2 + 1, temp, sizeof ( AK_header));
	temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2 + 2, temp, sizeof ( AK_header));
	temp = (AK_header*) AK_create_header("tel", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2 + 3, temp, sizeof ( AK_header));
	memset(t_header2 + 4, 0, MAX_ATTRIBUTES - 4);

	//create table
	tblName = "professor2";
	startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header2);

	if (startAddress != EXIT_ERROR)
		printf("\nTABLE %s CREATED!\n", tblName);

	//row_root = (element) malloc(sizeof (list));
	InitializeList(row_root);

	id_prof = 35890;
	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Baca", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042390873", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Balaban", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "000000000", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Antun", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Brumnic", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Mirko", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Cubrilo", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Dragutin", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Kermek", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Tonimir", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Kisasondi", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Alen", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Lovrencic", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042390866", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Markus", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Schatten", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042390892", tblName, "tel", row_root);
	insert_row(row_root);

	id_prof++;
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	InsertNewElement(TYPE_VARCHAR, "Neven", tblName, "firstname", row_root);
	InsertNewElement(TYPE_VARCHAR, "Vrcek", tblName, "lastname", row_root);
	InsertNewElement(TYPE_INT, "042390892", tblName, "tel", row_root);
	insert_row(row_root);

	AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //--------------------------------------> CREATE TABLE 'ASSISTANT' <-------------------------------------
    //create table, same header as professor for intersect test
    AK_header t_header3[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("tel", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("email", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 4, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("web_page", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 5, temp, sizeof ( AK_header));
    memset(t_header3 + 6, 0, MAX_ATTRIBUTES - 6);

    tblName = "assistant";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header3);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    //row_root = (element) malloc(sizeof (list));
    InitializeList(row_root);

    id_prof = 35892;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Balaban", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "000000000", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "igor.balaban@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/balaban.igor/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof = 35896;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Tonimir", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Kisasondi", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042213777", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "tonimir.kisasondi@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/kisasondi.tonimir/", tblName, "web_page", row_root);
    insert_row(row_root);

    id_prof = 35898;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Markus", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Schatten", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042390892", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "markus.schatten@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/schatten.markus/", tblName, "web_page", row_root);
    insert_row(row_root);
	
	id_prof = 35899;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_VARCHAR, "Miran", tblName, "firstname", row_root);
    InsertNewElement(TYPE_VARCHAR, "Zlatovic", tblName, "lastname", row_root);
    InsertNewElement(TYPE_INT, "042390858", tblName, "tel", row_root);
    InsertNewElement(TYPE_VARCHAR, "miran.zlatovic@foi.hr", tblName, "email", row_root);
    InsertNewElement(TYPE_VARCHAR, "www.foi.hr/nastavnici/zlatovic.miran/index.html", tblName, "web_page", row_root);
    insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //--------------------------------------> CREATE TABLE 'EMPLOYEE' <--------------------------------------
    //create header
    AK_header t_header4[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header4, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("id_department", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header4 + 1, temp, sizeof ( AK_header));
    memset(t_header4 + 2, 0, MAX_ATTRIBUTES - 2);

    //create table
    tblName = "employee";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header4);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    //row_root = (element) malloc(sizeof (list));
    InitializeList(row_root);

    id_prof = 35890;
    id_department = 1;
    id_prof++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    id_prof++;
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    id_prof++;
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    id_prof++;
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    id_prof++;
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    id_prof++;
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    id_prof++;
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    //don't need id_prof++ here
    id_prof++;
    //id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //-------------------------------------> CREATE TABLE 'DEPARTMENT' <-------------------------------------
    //create header
    AK_header t_header5[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_department", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header5, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("dep_name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header5 + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("manager", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header5 + 2, temp, sizeof ( AK_header));
    memset(t_header5 + 3, 0, MAX_ATTRIBUTES - 3);

    //create table
    tblName = "department";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header5);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    row_root = (element) malloc(sizeof (list));
    InitializeList(row_root);

    id_department = 1;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of Economics", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "Redep", tblName, "manager", row_root);
    insert_row(row_root);

    //id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of Organization", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "Zugaj", tblName, "manager", row_root);
    insert_row(row_root);

    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of Quantitative Methods", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "Kero", tblName, "manager", row_root);
    insert_row(row_root);

    /* too long
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of theoretical and applied foundations of Information Science", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "", tblName, "manager", row_root);
    insert_row(row_root);*/

    //id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of Information Technology and Computing", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "Hutinski", tblName, "manager", row_root);
    insert_row(row_root);

    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of Information Systems Development", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "Vrcek", tblName, "manager", row_root);
    insert_row(row_root);

    /* too long
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of foreign languages and general education discipline", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "", tblName, "manager", row_root);
    insert_row(row_root);*/

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //---------------------------------------> CREATE TABLE 'COURSE' <---------------------------------------
    //create header
    AK_header t_header6[MAX_ATTRIBUTES];

    temp = (AK_header*) AK_create_header("id_course", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 1, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("category", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 2, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("lecturer", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 3, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("active", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 4, temp, sizeof (AK_header));
    memset(t_header6 + 5, '\0', MAX_ATTRIBUTES - 5);

    //create table
    tblName = "course";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header6);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    DeleteAllElements(row_root);
    free(row_root);
    //------------------------------------------------------------------------------------------------------

}
