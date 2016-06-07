#include "archive_log.h"

/**
 * Function creates a binary file that stores all commands that failed to
 * execute with a number that shows the size of how many commands failed.
 * @todo this function takes static filename to store the failed commands,
 *       create certain logic that would make the function to use dynamic filename
 *       (this is partly implemented inside AK_get_timestamp, but there is no logic
 *       that uses the last file when recovering - recovery.c)                                    {link} recovery.c function test
 * @brief  Function for making archive log.
 * @author Dražen Bandić, update by Tomislav Turek
 * @return No retun value
 */
void AK_archive_log() {
    AK_PRO;
    FILE *fp;
    
    // open rec.bin in binary mode
    fp = fopen("../src/rec/rec.bin", "wb");
    // write redo_log structure to binary file
    fwrite(&redo_log, sizeof(redo_log), 1, fp);
    
    fclose(fp);
    AK_EPI;
}

/**
 * This function returns the current timestamp that could be concatenated to a 
 * log file in future usages.
 * @author Dražen Bandić main logic, replaced by Tomislav Turek
 * @brief Function that returns the current timestamp 
 * @todo Think about this in the future when creating multiple binary recovery
 *       files. Implementation gives the timestamp, but is not used anywhere for now.
 * @return char array in format day.month.year-hour:min:sec.usecu.log
 */
char* AK_get_timestamp() {
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    AK_PRO;
    
    // get time
    gettimeofday(&tv, &tz);
    tm=localtime(&tv.tv_sec);
    
    // save timestamp
    char log_name[30];
    sprintf(log_name, "%d.%d.%d-%d:%02d:%02d.%lu.log", tm->tm_mday, tm->tm_mon+1,
            tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);
    
    AK_EPI;
    return &log_name;
}
