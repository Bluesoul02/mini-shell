#include "commandes.h"

struct dirent *ptr;

void mycd(char * directory) {
  if (strcmp(directory, "~") == 0) directory = getenv("HOME");
    if (chdir(directory) != 0)
        printf("No such file or directory\n");
}

void myexit() {
  //TO DO exit qui quittera le shell sans tuer les cmd lancé en bg
  exit(0);
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
  char * buffer = NULL;
  int sizeBuffer = 0;
  DIR *dir = opendir(direct);

  if (!dir) {
    perror("opendir");
    exit(1);
  }

  while ((ptr = readdir(dir)) != NULL) {
    printf("maxSize boucle\n");

    if ((!strncmp(ptr->d_name, ".", 1) || !strcmp(ptr->d_name, "..")))
      continue; // skip . and ..

    if (strlen(direct) + strlen(ptr->d_name) + 1 >= sizeBuffer) {
      sizeBuffer = strlen(direct) + strlen(ptr->d_name) + 10;
      buffer = (char *)realloc(buffer, sizeBuffer);
      assert(buffer);
    }

    sprintf(buffer, "%s/%s", direct, ptr->d_name);
    printf("%s\n", buffer);
    // erreur ici pour le -R
    if (fstatat(dirfd(dir), buffer, &fst, AT_SYMLINK_NOFOLLOW) == -1) {
      perror("fstatat maxSizeInRep");
      exit(1);
    }

    printf("post fstatat\n");
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
    printf("myls1\n");
    long int maxSize = maxSizeInRep(directory);
    printf("myls2\n");
    unsigned length = floor(log10(maxSize)) + 1;
    char *buffer = NULL;
    int sizeBuffer = 0;
    int count = 0;
    bool isR = strstr(parameters, "R") != NULL;

    if (isR) {
      if (stat(directory, &fst) == -1) {
        perror("stat");
        exit(1);
      }

      if (!S_ISDIR(fst.st_mode))
        printf("%s\n", directory);
      else
        printf("%s:\n", directory);
    }
    
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
        printf(" %*ld", length, (long)fst.st_size);   // file size
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

    // le option_R du prof marche pas 
    if (isR) {
      printf("\n");

      // recursive call
      rewinddir(dir);

      while ((ptr = readdir(dir)) != NULL) {

        if ((!strncmp(ptr->d_name, ".", 1) || !strcmp(ptr->d_name, "..")))
          continue; // skip . and ..

        // if directory
        if (ptr->d_type & DT_DIR) {

          if (strlen(directory) + strlen(ptr->d_name) + 1 >= sizeBuffer) {
            sizeBuffer = strlen(directory) + strlen(ptr->d_name) + 10;
            buffer = (char *)realloc(buffer, sizeBuffer);
            assert(buffer);
          }

          sprintf(buffer, "%s/%s", directory, ptr->d_name);

          printf("%s\n", buffer);
          myls(buffer, parameters);
        }
      }
    }

    if (closedir(dir) == -1) {
        perror("close dir");
        exit(1);
    }

    if (buffer)
        free(buffer);
}

int is_joker(char c) {
  if (c == '*' || c == '?' || c == '[') return 1;
  return 0;
}

int myglob(glob_t globbuf, char *tab[BUFFER_SIZE], int limit) {
  int count=0, joker=0, index[100], count2=0, jokers[100];
  for(int x=0; x<limit; x++) {
    for(int y=0; y<strlen(tab[x]); y++) if(is_joker(tab[x][y])) {
      joker=1;
      break;
    }
    if(joker) jokers[count2++] = x;
    else index[count++] = x;
    joker=0;
  }
  globbuf.gl_offs = count;
  for(int x=0; x<count2; x++) {
    if(x==0) glob(tab[jokers[x]], GLOB_DOOFFS, NULL, &globbuf);
    else glob(tab[jokers[x]], GLOB_DOOFFS | GLOB_APPEND, NULL, &globbuf);
  }
  if(count2) {
    if(!globbuf.gl_pathc) return 0;
    for(int x=0; x<count; x++) globbuf.gl_pathv[x] = tab[index[x]];
    execvp(globbuf.gl_pathv[0], &globbuf.gl_pathv[0]);
  }
  return 1;
}