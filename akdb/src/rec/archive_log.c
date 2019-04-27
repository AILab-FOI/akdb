
#include "archive_log.h"

/**
 * Function that creates a binary file that stores all commands that failed to
 * execute with a number that shows the size of how many commands failed.
 * @todo this function takes static filename to store the failed commands,
 *       create certain logic that would make the function to use dynamic filename
 *       (this is partly implemented inside AK_get_timestamp, but there is no logic
 *       that uses the last file when recovering - recovery.c)                                    
 * {link} recovery.c function test
 * @brief  Function for making archive log.
 * @author Dražen Bandić, update by Tomislav Turek
 * @return No retun value
 */
void AK_archive_log(int sig) {
    //AK_PRO;

    FILE *fp;

    AK_check_folder_archivelog();

    printf("AK_archive_log: Archiving redo log commands to separate file...\n");

    // open new file named with current timestamp in binary mode
    char* timestamp = AK_get_timestamp();

    char *destination = malloc(strlen(ARCHIVELOG_PATH)+strlen(timestamp)+2);
    strcpy(destination, ARCHIVELOG_PATH);
    strcat(destination, "/");
    strcat(destination, timestamp);
    fp = fopen(destination, "wb");

    AK_free(timestamp);
	 
    AK_redo_log log;

    memcpy(log.command_recovery, redo_log->command_recovery, sizeof(log.command_recovery));

    log.number = redo_log->number;
    // write redo_log structure to binary file

    fwrite(&log, sizeof(log), 1, fp);
 
    fclose(fp);
    AK_free(destination);	
    //AK_EPI;
    if(sig == SIGINT) {
	
        exit(EXIT_SUCCESS);
    }
}

/**
 * Function that empties the archive log - with dynamic archive log name, this function is no longer needed
 */
void AK_empty_archive_log() {
    //AK_PRO;
    FILE *fp;
    printf("AK_archive_log: emptying file...\n");

    // open rec.bin in binary mode
    fp = fopen("../src/rec/rec.bin", "wb");
    // immediately close to delete contents
    fclose(fp);
}

/**
 * This function returns the current timestamp that could be concatenated to a 
 * log file in future usages.
 * 
 * @author Dražen Bandić main logic, replaced by Tomislav Turek
 * @brief Function that returns the current timestamp 
 * @todo Think about this in the future when creating multiple binary recovery
 *       files. Implementation gives the timestamp, but is not used anywhere for now.
 * @return char array in format day.month.year-hour:min:sec.usecu.bin
 */
char* AK_get_timestamp() {
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    FILE *fp;

    AK_PRO;
    
    // get time
    gettimeofday(&tv, &tz);
    tm=localtime(&tv.tv_sec);

    // save timestamp
    char* log_name = malloc(30);
    sprintf(log_name, "%d.%d.%d-%d:%02d:%02d.%lu.bin", tm->tm_mday, tm->tm_mon+1,
            tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);

    char *destination = malloc(strlen(ARCHIVELOG_PATH)+strlen("/latest.txt")+1);
    strcpy(destination, ARCHIVELOG_PATH);
    strcat(destination, "/latest.txt");
	
    // save file name into separate txt file
    fp = fopen(destination, "w+");
    fprintf(fp, log_name);	
    fclose(fp);
    
    AK_free(destination);

    AK_EPI;
    return log_name;
}

int AK_check_folder_archivelog() {
  struct stat s;
  int err = stat(ARCHIVELOG_PATH, &s);
  if(-1 == err) {
      if(ENOENT == errno) {
          printf("[INFO] There is no folder for archive log. Creating folder at: %s\n", ARCHIVELOG_PATH);
          	#ifdef _WIN32 // not tested
        		return _mkdir(ARCHIVELOG_PATH);
    		#else
    		#if _POSIX_C_SOURCE
        		return mkdir(ARCHIVELOG_PATH, 0755);
    		#else
        		return mkdir(ARCHIVELOG_PATH, 0755); // not sure if this works on mac
    		#endif
    		#endif
      } else {
          perror("stat");
          exit(1);
      }
  } else {
      if(S_ISDIR(s.st_mode)) {
        printf("[INFO] Folder archivelog already exists\n");
      } else {
          /* exists but is no folder */
      }
  }
  return 1;
}
