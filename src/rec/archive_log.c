/**
 * @brief  Function for making archive log.
 * @author Dražen Bandić
 * @return No retun value
 */

#include "archive_log.h"

void AK_archive_log(){

    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm=localtime(&tv.tv_sec);
    int i = 0;   

    FILE *fp;

    char log_name[30];

    sprintf(log_name, "%d.%d.%d-%d:%02d:%02d.%lu.log", tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);

    fp = fopen(log_name, "a+");

    for(i = 0; i < MAX_REDO_LOG_ENTRIES; i++){

        fprintf(fp, "%s ", (char *) redo_log->table_name[i]);
        fprintf(fp, "%s ", (char *) redo_log->command[i]);
        fprintf(fp, "%s\n", (char *) redo_log->attributes[i]);
    }
    fclose(fp);
}