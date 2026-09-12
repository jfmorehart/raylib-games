#include <stdio.h>
#include <dirent.h>
#include "islands.h"
#include "game/map.h"
#include <string.h>
#include "filesystem.h"

#ifdef _WIN32
#include <direct.h> //WINDOWS
#elif __APPLE__
#include <sys/stat.h> //MACOS
#endif

bool FileCheck(const char* path){

    printf("loading filesystem\n");

    #ifdef _WIN32
        _mkdir("editor");

    #elif __APPLE__
    mkdir("editor", 0777);
    mkdir("livemaps", 0777);
    #endif

    FILE *fptr;

    // Create a file
    fptr = fopen(path, "rb");
    if(!fptr){
        // fptr = fopen(path,"w");
        // // Close the file
        // fclose(fptr);
        return false;
    }
    fclose(fptr);
    return true;
}
void AssignName(char filename[STRINGARRAY_STRLEN], const char * name){

    bool goodData = true;
    for(int i = 0; i < STRINGARRAY_STRLEN; i++){
        if(goodData && name[i]){
            filename[i] = name[i];
        }else{
            goodData = false;
            filename[i] = 0;
        }
    }
    filename[STRINGARRAY_STRLEN -1] = 0; //FORCE A NULL TERMINATOR

    printf("fn = %s\n", filename);
    printf("path = %s\n", name);
}

void ReAppendSuffix(char* dest, char * toread, const char * toappend){
    int count;
    const char ** split = TextSplit(toread, '.', &count);
    snprintf(dest, 30, "%s%s", split[0], toappend);
}

void RehydrateMap(Map * toreh, MapRecord * record){
    *toreh = (Map){0}; 
    snprintf(toreh->filename, 20, "%s", record->filename);
    toreh->objective_count = record->objective_count;
    memcpy(toreh->map_objectives, record->map_objectives, sizeof(Objective) * record->objective_count);
    toreh->islandLength = record->islandLength;
    memcpy(toreh->islands, record->islands, sizeof(Island) * record->islandLength);
}
MapRecord DehydrateMap(Map * dehydrate){
    MapRecord dry = (MapRecord){0};
    snprintf(dry.filename, 20, "%s", dehydrate->filename);

    dry.objective_count = dehydrate->objective_count;
    memcpy(dry.map_objectives, dehydrate->map_objectives, sizeof(Objective) * dry.objective_count);
    dry.islandLength = dehydrate->islandLength;
    memcpy(dry.islands, dehydrate->islands, sizeof(Island) * dry.islandLength);
    return dry;
}

Map LoadMapFile(const char* path){ //rehydrates from MapRecord
    Map loadMap;
    MapRecord fromDisk;

    char fullpath[30] = "editor/";
    strcat(fullpath, path);
    bool filexists = FileCheck (fullpath);
    
    if(filexists){
        printf("found file: %s \n", fullpath);
        FILE * fptr = fopen(fullpath, "rb");
        fread(&fromDisk, sizeof(MapRecord), 1, fptr);
        fclose(fptr);
        AssignName(fromDisk.filename, path);
        RehydrateMap(&loadMap, &fromDisk);
        return loadMap;
    }
    printf("error: %s, no such file found\n", fullpath);
  return (Map){0};
}

Fleet LoadFleetFile(const char* path){
    Fleet pfile;
    char fullpath[30] = "editor/";
    strcat(fullpath, path);
    bool filexists = FileCheck (fullpath);

    if(filexists){
        printf("found file: %s \n", fullpath);
        FILE * fptr = fopen(fullpath, "rb");
        fread(&pfile, sizeof(Fleet), 1, fptr);
        fclose(fptr);
        return pfile;
    }
    printf("error: %s, no such file found\n", fullpath);
  return (Fleet){0}; 
}

PolyPoly LoadPolyFile(const char* path){
    PolyPoly pfile;
    char fullpath[30] = "editor/";
    strcat(fullpath, path);
    bool filexists = FileCheck (fullpath);

    if(filexists){
        printf("found file: %s \n", fullpath);
        FILE * fptr = fopen(fullpath, "rb");
        fread(&pfile, sizeof(PolyPoly), 1, fptr);
        fclose(fptr);
        AssignName(pfile.filename, path);
        return pfile;
    }
    printf("error: %s, no such file found\n", fullpath);
  return (PolyPoly){0}; 
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

int AppendStringToStrArr(const char* str, StringArray * strArr){
    int length = strlen(str);
   
    //todo cut string to size max
    int usedSpace = (strArr->numStrings * STRINGARRAY_STRLEN);

    if(STRINGARRAY_STRLEN * (strArr->numStrings + 1) >= STRINGGARY_MAXCHARS) return 0;
    if(strArr->numStrings + 1 >= STRINGARRAY_MAXSTRINGS) return 0;

    strArr->stringIndices[strArr->numStrings] = usedSpace;

    for(int i = 0; i < length; i++){
        strArr->chars[i + usedSpace] = str[i];
    }
    strArr->numStrings++;
    return 1;
}
bool fileIsType(char * file, char * type){

    char *dotptr = strrchr(file, (int){'.'});
    char ending[20];
    bool flipped;
    for(int i = 1; i < 20; i++){
        if(dotptr[i] && flipped){
            ending[i - 1] = dotptr[i];
        }
        else {
            flipped = true;
            ending[i -1] = 0;
        }
    }
    if(!strcmp(ending, type)) return true;
    return false;
}

bool isRecognizedType(char * str){
    char *dotptr = strrchr(str, (int){'.'});
    char ending[20];
    bool flipped;
    for(int i = 1; i < 20; i++){
        if(dotptr[i] && flipped){
            ending[i - 1] = dotptr[i];
        }
        else {
            flipped = true;
            ending[i -1] = 0;
        }
    }
    // if(!strcmp(ending, "situ")) return true;
    if(!strcmp(ending, "map")) return true;
    if(!strcmp(ending, "poly")) return true;
    return false;
}
char *StringAt(StringArray *strArr, int index){
    return &strArr->chars[strArr->stringIndices[index]];
}

void PrintAllStringsInStrArr(StringArray * strArr){
    for(int i = 0; i < strArr->numStrings; i++){
        printf("string %d & strindex: %d is: %s \n", i,  strArr->stringIndices[i], StringAt(strArr, i));
    }
}

StringArray GetMapNames(){
    #ifdef _WIN32
    _mkdir("editor");
    #elif __APPLE__
    DIR *directory;
    struct dirent *entry;
    directory = opendir("editor");
    if (directory == NULL) {
        perror("Unable to open directory");
        return (StringArray){0};
    }

    printf("getting map names\n");

    StringArray strArr = (StringArray){0};
    for(int i = 0; i < 99; i++){
        entry = readdir(directory);
        if(entry){
            if(!fileIsType(entry->d_name, "map")) continue;
            AppendStringToStrArr(entry->d_name, &strArr);
            printf("%s\n", entry->d_name);
        }else{
            PrintAllStringsInStrArr(&strArr);
            return strArr;
        }
    }
    #endif 
}

StringArray GetPolyNames(){
    #ifdef _WIN32
    _mkdir("editor");

    #elif __APPLE__
    DIR *directory;
    struct dirent *entry;
    directory = opendir("editor");
    if (directory == NULL) {
        perror("Unable to open directory");
        return (StringArray){0};
    }

    printf("getting map names\n");

    StringArray strArr = (StringArray){0};
    for(int i = 0; i < 99; i++){
        entry = readdir(directory);
        if(entry){
            if(!fileIsType(entry->d_name, "poly"))continue;
            AppendStringToStrArr(entry->d_name, &strArr);
            printf("%s\n", entry->d_name);
        }else{
            PrintAllStringsInStrArr(&strArr);
            return strArr;
        }
    }
    #endif 
}