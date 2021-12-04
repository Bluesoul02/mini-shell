#include "commandes.h"
#include <grp.h>
#include <pwd.h>
#include <stdint.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>

struct dirent *ptr;

void mycd(char * directory) {
    printf("%s", directory);
    if (chdir(directory) != 0)
        printf("No such file or directory");
}

/**
 * @brief
 *
 * @param mode
 * @param str
 */
void modeToLetter(int mode, char *str) {
  str[0] = '-';

  // file type
  if (S_ISDIR(mode))
    str[0] = 'd';

  if (S_ISCHR(mode))
    str[0] = 'c';

  if (S_ISBLK(mode))
    str[0] = 'b';

  // permission for owner
  if (mode & S_IRUSR)
    str[1] = 'r';
  else
    str[1] = '-';

  if (mode & S_IWUSR)
    str[2] = 'w';
  else
    str[2] = '-';

  if (mode & S_IXUSR)
    str[3] = 'x';
  else
    str[3] = '-';

  // permission for owner group
  if (mode & S_IRGRP)
    str[4] = 'r';
  else
    str[4] = '-';

  if (mode & S_IWGRP)
    str[5] = 'w';
  else
    str[5] = '-';

  if (mode & S_IXGRP)
    str[6] = 'x';
  else
    str[6] = '-';

  // permission for others
  if (mode & S_IROTH)
    str[7] = 'r';
  else
    str[7] = '-';

  if (mode & S_IWOTH)
    str[8] = 'w';
  else
    str[8] = '-';

  if (mode & S_IXOTH)
    str[9] = 'x';
  else
    str[9] = '-';

  str[10] = '\0';
} // modeToLetter

/**
 * @brief
 *
 * @param direct
 * @return int
 */
int maxSizeInRep(char *direct) {
  long int max = 0;
  struct stat fst;
  char *buffer = NULL;
  int sizeBuffer = 0;
  DIR *dir = opendir(direct);

  if (!dir) {
    perror("opendir");
    exit(1);
  }

  while ((ptr = readdir(dir)) != NULL) {
    if (strlen(direct) + strlen(ptr->d_name) + 1 >= sizeBuffer) {
      sizeBuffer = strlen(direct) + strlen(ptr->d_name) + 10;
      buffer = (char *)realloc(buffer, sizeBuffer);
      assert(buffer);
    }

    sprintf(buffer, "%s/%s", direct, ptr->d_name);
    if (fstatat(dirfd(dir), buffer, &fst, AT_SYMLINK_NOFOLLOW) == -1) {
      perror("stat");
      exit(1);
    }

    if ((long)fst.st_size > max)
      max = (long)fst.st_size;
  }

  if (closedir(dir) == -1) {
    perror("close dir");
    exit(1);
  }

  if (buffer)
    free(buffer);
  return max;
} // maxSizeInRep


void myls(char * directory, char * parameters) {
    if (strcmp(directory, "") == 0) directory = ".";
    struct stat fst;
    struct tm *mytime;
    char str[12];
    long int maxSize = maxSizeInRep(directory);
    unsigned lenght = floor(log10(maxSize)) + 1;
    char *buffer = NULL;
    int sizeBuffer = 0;
    DIR *dir = opendir(directory);

    if (!dir) {
        perror("opendir");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL) {
        if ((!strncmp(ptr->d_name, ".", 1) || !strcmp(ptr->d_name, "..")) && strstr(parameters, "a") == NULL)
        continue; // skip anything that start with . and .. except if a is in the parameters

        if (strlen(directory) + strlen(ptr->d_name) + 1 >= sizeBuffer) {
        sizeBuffer = strlen(directory) + strlen(ptr->d_name) + 10;
        buffer = (char *)realloc(buffer, sizeBuffer);
        assert(buffer);
        }

        sprintf(buffer, "%s/%s", directory, ptr->d_name);
        if (fstatat(dirfd(dir), buffer, &fst, AT_SYMLINK_NOFOLLOW) == -1) {
        perror("fstatat");
        exit(1);
        }

        modeToLetter(fst.st_mode, str);               // permission information
        printf("%s", str);                            // file type and permission
        printf(" %ld", fst.st_nlink);                 // file hard links
        printf(" %s", getpwuid(fst.st_uid)->pw_name); // file's owner
        printf(" %s", getgrgid(fst.st_gid)->gr_name); // file's owner group
        printf(" %*ld", lenght, (long)fst.st_size);   // file size
        mytime = localtime(&fst.st_mtime);            // file time
        printf(" %d-%02d-%02d %02d:%02d", mytime->tm_year + 1900,
            mytime->tm_mon + 1, mytime->tm_mday, mytime->tm_hour,
            mytime->tm_min);
        if (S_ISDIR(fst.st_mode)) printf(BLUE(" %s"), ptr->d_name); // if is directory write the name in blue
        else if (S_ISLNK(fst.st_mode)) printf(CYAN(" %s"), ptr->d_name); // if is symbolic link write the name in cyan
        else if (S_ISBLK(fst.st_mode) || S_ISCHR(fst.st_mode)) printf(YELLOW(" %s"), ptr->d_name); // if is block device or character device write the name in yellow
        // else is regular file
        else printf(" %s", ptr->d_name); // file name
        printf("\n");
    }

    if (closedir(dir) == -1) {
        perror("close dir");
        exit(1);
    }

    if (buffer)
        free(buffer);
}