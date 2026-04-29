#include <stdio.h>
#include <dirent.h>
#include "islands.h"
#include "game/map.h"

#ifdef _WIN32
#include <direct.h> //WINDOWS
#elif __APPLE__
#include <sys/stat.h> //MACOS
#endif

FILE *GetFile(const char* path){

    printf("loading filesystem\n");

    #ifdef _WIN32
        _mkdir("editor");

    #elif __APPLE__
    mkdir("editor", 0777);
    #endif

    FILE *fptr;

    // Create a file
    fptr = fopen(path, "rb");
    if(!fptr){
        fptr = fopen(path,"w");
        // Close the file
        fclose(fptr);
        return 0;
    }
    fclose(fptr);

    return fptr;
}

Map LoadMapFile(const char* path){
    Map loadMap;
    FILE * fptr = GetFile(path);
    if(fptr){
        fptr = fopen(path, "rb");
        fread(&loadMap, sizeof(Map), 1, fptr);
        fclose(fptr);
        return loadMap;
    }
  return (Map){0};
}

int GetMapCount(){
    #ifdef _WIN32
    _mkdir("editor");
    #elif __APPLE__
    DIR *directory;
    struct dirent *entry;
    // Open current directory "."
    directory = opendir("editor");
    if (directory == NULL) {
        perror("Unable to open directory");
        return 1;
    }

    for(int i = 0; i < 99; i++){
        entry = readdir(directory);
        if(entry){
            printf("%s\n", entry->d_name);
        }else{
            printf("%d\n", i);
            return i;
        }
    }
    return 0;
    #endif 
}