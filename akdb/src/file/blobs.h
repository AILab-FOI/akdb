/**
    @file blobs.h Provides data structures for manipulating blobs
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
 */

#ifndef BLOBS
#define BLOBS

#include "../auxi/test.h"
#include "table.h"
#include "fileio.h"

#include "id.h"

struct _file_metadata {
  char *new_path;
  char *new_name;
  char *old_path;
  char *old_name;
  char *checksum;
};

typedef struct _file_metadata AK_Metadata;
typedef struct _file_metadata *AK_File_Metadata;

AK_File_Metadata AK_File_Metadata_malloc();

/* Helper functions */
int AK_mkdir(const char *path);
int AK_mkdir(const char *path);
int AK_copy(const char *from, const char *to);
char *AK_concat(char *s1, char *s2);
char *AK_clear_all_newline(char *str);
void AK_split_path_file(char **p, char **f, char *pf);


char *AK_GUID();

int AK_folder_exists(char *foldername);
int AK_check_folder_blobs();

int AK_write_metadata(char *oid, AK_File_Metadata meta);
AK_File_Metadata AK_read_metadata(char *oid);


char *AK_lo_import(char *filepath);
int AK_lo_export(char *oid, char *filepath);
int AK_lo_unlink(char *oid);

TestResult AK_lo_test();

#endif
