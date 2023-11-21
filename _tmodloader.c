// Author.name = MyGuy

// Why does every std function have an underscore in front on windows
// Found out those are special versions, the std versions are normal
// But still, why

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define DEBUG

#include "include/mumble/MumbleAPI_v_1_0_x.h"
#include "include/mumble/MumblePlugin_v_1_0_x.h"
#include "include/tmodloader.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>

struct MumbleAPI_v_1_0_x mumbleAPI;
mumble_plugin_id_t ownID;

const char* fileName = "\\tModLoaderProxChat.tmp";
char* filePath;
char* buf;
char* logPath;
FILE* logFp;

char* mmf;
char* newContext;
data_container_t* data;

size_t strlcpy(char* dst, const char* src, size_t size) {
    // Thx Apple
    // https://opensource.apple.com/source/xnu/xnu-4570.1.46/osfmk/arm/strlcpy.c.auto.html
    const size_t srclen = strlen(src);
    if (srclen + 1 < size) {
        memcpy(dst, src, srclen + 1);
    }
    else if (size != 0) {
        memcpy(dst, src, size - 1);
        dst[size - 1] = '\0';
    }
    return srclen;
}

ProxError_t readData(char* mmfPtr) {
    data->inWorld = mmfPtr[11];

    for (int i = 0; i < 4; i++) {
        buf[i] = (byte)mmfPtr[i];
        #ifdef DEBUG
        printf("mmf[%d]: %u\n", i, (byte)mmfPtr[i]);
        #endif
    }
    data->posX = *(float*)buf;

    for (int i = 0; i < 4; i++) {
        buf[i] = mmfPtr[4 + i];
        #ifdef DEBUG
        printf("mmf[%d]: %u\n", 4 + i, (byte)mmfPtr[4 + i]);
        #endif
    }
    data->posY = *(float*)buf;

    data->team = mmfPtr[8];
    data->dead = mmfPtr[9];
    data->radio_channel = mmfPtr[10];
    data->nameLen = mmfPtr[12];
    data->name = &mmfPtr[13];
    data->world_name_len = mmfPtr[35];
    data->world_name = &mmfPtr[36];

    #ifdef DEBUG
    printf("mmf[%d]: %u\n", 8, mmfPtr[8]);
    printf("mmf[%d]: %u\n", 9, mmfPtr[9]);
    printf("mmf[%d]: %u\n", 10, mmfPtr[10]);
    printf("mmf[%d]: %u\n", 11, mmfPtr[11]);
    printf("mmf[%d]: %u\n", 11, mmfPtr[12]);

    printf("nameLen: %d\n", data->nameLen);
    
    for (int i = 0; i < data->nameLen; i++) {
        switch (i) {
            case 0:
                printf("name: %c", data->name[i]);
                break;
                
            default:
                printf("%c", data->name[i]);
        }
    }
    
    printf("\n");
    #endif

    printf("finish\n");

    return NoError;
}

void print_data() {
    //printf("%s\n", filePath);
    printf("PosX: %.6f\n", data->posX);
    printf("PosY: %.6f\n", data->posY);
    printf("Team: %d\n", data->team);
    printf("Dead: %d\n", data->dead);
    printf("RadioChannel: %d\n", data->radio_channel);
    printf("InWorld: %d\n", data->inWorld);
    printf("NameLen: %d\n", data->nameLen);

    for (int i = 0; i < data->nameLen; i++) {
        switch (i) {
            case 0:
                printf("Name: %c", data->name[i]);
                break;
                
            default:
                printf("%c", data->name[i]);
        }
    }
    printf("\n");

    printf("WorldNameLen: %d\n", data->world_name_len);

    for (int i = 0; i < data->world_name_len; i++) {
        switch (i) {
            case 0:
                printf("WorldName: %c", data->world_name[i]);
                break;
                
            default:
                printf("%c", data->world_name[i]);
        }
    }
    printf("\n");
}

void log_data() {
    //printf("%s\n", filePath);
    #ifdef DEBUG
    fprintf(logFp, "PosX: %.6f\n", data->posX);
    fprintf(logFp, "PosY: %.6f\n", data->posY);
    fprintf(logFp, "Team: %d\n", data->team);
    fprintf(logFp, "Dead: %d\n", data->dead);
    fprintf(logFp, "RadioChannel: %d\n", data->radio_channel);
    fprintf(logFp, "InWorld: %d\n", data->inWorld);
    fprintf(logFp, "NameLen: %d\n", data->nameLen);
    #endif

    for (int i = 0; i < data->nameLen; i++) {
        switch (i) {
            case 0:
                fprintf(logFp, "Name: %c", data->name[i]);
                break;
                
            default:
                fprintf(logFp, "%c", data->name[i]);
        }
    }
    fprintf(logFp, "\n");

    fprintf(logFp, "WorldNameLen: %d\n", data->world_name_len);

    for (int i = 0; i < data->world_name_len; i++) {
        switch (i) {
            case 0:
                fprintf(logFp, "WorldName: %c", data->world_name[i]);
                break;
                
            default:
                fprintf(logFp, "%c", data->world_name[i]);
        }
    }
    fprintf(logFp, "\n");
}

void mmfPeek() {
    system("cls");
    for (int i = 0; i < 64; i++) {
        printf("mmf[%d]: %c\n", i, mmf[i]);
        if (i > 59 && i < 63) {
            printf("mmf[%d]: %d\n", i, mmf[i]);
        }
    }
    
}

void* mmap(char* filePath, int mapSize) {
    
    HANDLE handle = CreateFileA(
        filePath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        printf("invalid handle: err code: %lu\n", GetLastError());
        return (void*)FileError;
    }

    HANDLE mappedHandle = CreateFileMapping(
        handle,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL
    );

    if (mappedHandle == NULL || mappedHandle == (HANDLE)ERROR_ALREADY_EXISTS) {
        printf("invalid mapped handle\nerr code: %lu", GetLastError());
        return (void*)MemoryMappedFileError;
    }

    char* fp = (char*)MapViewOfFile(
        mappedHandle,
        FILE_MAP_READ,
        0,
        0,
        mapSize
    );

    if (fp == NULL) {
        return (void*)MemoryMappedFileError;
    }

    return fp;
}

int munmap(void* mapPtr) {
    if (UnmapViewOfFile(mapPtr) == FALSE) {
        return -1;
    }
    else {
        return 0;
    }
}

ProxError_t windows_main() {
    char* temp = getenv("TMP");
    strcpy(filePath, temp);
    strcat(filePath, fileName);

    mmf = mmap(filePath, 64);

    if ((ProxError_t)mmf == FileError) {
        return FileError;
    }

    if ((ProxError_t)mmf == MemoryMappedFileError) {
        return MemoryMappedFileError;
    }

    ProxError_t err = NoError;

    mmfPeek();
    while (err == NoError && mmf[63] != 1) {
        //system("cls");
        //err = readData(mmf);
        //print_data();
    }
    printf("err: %d\n", err);

    return err;
}

ProxError_t windows_init() {
    buf = (char*)malloc(sizeof(float));
    data = (data_container_t*)malloc(sizeof(data_container_t));
    newContext = (char*)malloc(sizeof(char) * 28);

    if (buf == NULL  || 
        data == NULL || 
        newContext == NULL
    ) {
        mumbleAPI.log(ownID, "malloc failed for init() vars, fatal error");
        return MallocError;
    }

    //Map file to memory here
    mmf = mmap(filePath, 64);
    if ((ProxError_t)mmf == FileError) {
        return FileError;
    }
    if ((ProxError_t)mmf == MemoryMappedFileError) {
        return MemoryMappedFileError;
    }

    return NoError;
}

mumble_error_t mumble_init(mumble_plugin_id_t pluginID) {
    ownID = pluginID;
    

    char* temp = getenv("APPDATA");
    char* append = "\\tModLoaderProxChat.log";
    size_t len = strlen(temp) + strlen(append) + 1;
    logPath = (char*)malloc(sizeof(char) * len);
    if (logPath == NULL) {
        mumbleAPI.log(ownID, "malloc failed for logPath, fatal error");
        return MUMBLE_EC_GENERIC_ERROR;
    }
    strcpy(logPath, temp); // Change to windows version
    strcat(logPath, append);

    logFp = fopen(logPath, "w");
    fputs("Plugin init: mumble_init()\n", logFp);

    if (mumbleAPI.log(ownID, "Terraria ProxChat Initializtion") != MUMBLE_STATUS_OK) {
		// Logging failed -> usually you'd probably want to log things like this in your plugin's
		// logging system (if there is any)

        fputs("Mumble didn't log: mumble_init()\n", logFp);
    }
    fclose(logFp);

    return MUMBLE_STATUS_OK;
}

void mumble_shutdown() {
    logFp = fopen(logPath, "a");
    fputs("shutting down plugin: mumble_shutdown()\n", logFp);
    fclose(logFp);

    free(logPath);

	if (mumbleAPI.log(ownID, "Plugin Shutdown") != MUMBLE_STATUS_OK) {
		// Logging failed -> usually you'd probably want to log things like this in your plugin's
		// logging system (if there is any)
    }
}

struct MumbleStringWrapper mumble_getName() {
	static const char* name = "TerrariaProxChat";

	struct MumbleStringWrapper wrapper;
	wrapper.data = name;
	wrapper.size = strlen(name);
	wrapper.needsReleasing = false;

	return wrapper;
}

mumble_version_t mumble_getAPIVersion() {
	// This constant will always hold the API version that fits the included header files
	return MUMBLE_PLUGIN_API_VERSION;
}

void mumble_registerAPIFunctions(void *apiStruct) {
	// Provided mumble_getAPIVersion returns MUMBLE_PLUGIN_API_VERSION, this cast will make sure
	// that the passed pointer will be cast to the proper type
	mumbleAPI = MUMBLE_API_CAST(apiStruct);
}

void mumble_releaseResource(const void *pointer) {
	// As we never pass a resource to Mumble that needs releasing, this function should never
	// get called
    printf("Called mumble_releaseResource but expected that this never gets called -> Aborting");
    abort();
}

mumble_version_t mumble_getVersion() {
    mumble_version_t version;

    version.major = 1;
    version.minor = 1;
    version.patch = 10;

    return version;
}

struct MumbleStringWrapper mumble_getAuthor() {
    static const char* author = "MyGuy";
    struct MumbleStringWrapper wrapper;

    wrapper.data = author;
    wrapper.size = strlen(author);
    wrapper.needsReleasing = false;

    return wrapper;
}

struct MumbleStringWrapper mumble_getDescription() {
    static const char* description = "A plugin for proximity chat on tModLoader. Requires the Terraria Proxmity Chat mod to function.";
    struct MumbleStringWrapper wrapper;

    wrapper.data = description;
    wrapper.size = strlen(description);
    wrapper.needsReleasing = false;

    return wrapper;
}

uint32_t mumble_getFeatures() {
    return MUMBLE_FEATURE_POSITIONAL;
}

uint8_t mumble_initPositionalData(const char *const *programNames, const uint64_t *programPIDs, size_t programCount) {
    // Check if the supported game is in the list of programs and if yes
    // check if the position can be obtained from the program
    logFp = fopen(logPath, "a");
    mumbleAPI.log(ownID, "Positional Data Intitialization");
    fputs("Terraria ProxChat Positional Data Intitialization: mumble_initPositionalData()\n", logFp);

    char* temp = getenv("TMP");
    size_t len = strlen(temp) + strlen(fileName) + 1;

    filePath = (char*)malloc(sizeof(char) * len);
    if (filePath == NULL) {
        mumbleAPI.log(ownID, "malloc failed for filePath, fatal error");
        return MUMBLE_PDEC_ERROR_TEMP;
    }
    strcpy(filePath, temp);
    strcat(filePath, fileName);
    fprintf(logFp, "filePath: %s\n", filePath);
    
    bool gameIsRunning = false;
    ProxError_t perr = -1;
    fputs("windows_init(): mumble_initPositionalData()\n", logFp);
    perr = windows_init();
    
    if (perr != NoError) {
        fprintf(logFp, "Prox Init failed: mumble_initPositionalData()\nErr code: %d\n", perr);
        fclose(logFp);

        mumbleAPI.log(ownID, "Initialization failed! Check Error Log!");
        mumbleAPI.log(ownID, logPath);
        mumbleAPI.log(ownID, "Ignore the permanent error below, I had to stop mumble from spamming mumble_initPositionalData on an error");
        mumbleAPI.log(ownID, "Make sure to re-enable positional audio in the plugin settings.");

        return MUMBLE_PDEC_ERROR_PERM;
    }

    for (int i = 0; i < 3; i++) {
        buf[i] = mmf[60 + i];
        #ifdef DEBUG
        fprintf(logFp, "mmf[%d]: %d\n", 60 + i, mmf[60 + i]);
        #endif
    }
    buf[3] = 0;
    int tmlPid = *(int*)buf;
    fprintf(logFp, "tmlPID: %d\n", tmlPid);

    for (int i = 0; i < programCount; i++) {
        if (programPIDs[i] == tmlPid) {
            fputs("Game is running: mumble_initPositionalData()\n", logFp);
            gameIsRunning = true;
        }
    }

    if (!gameIsRunning) {
        mumbleAPI.log(ownID, "tModLoader isn't running!");
        fputs("Game isn't running: mumble_initPositionalData()\n", logFp);
        fclose(logFp);
        return MUMBLE_PDEC_ERROR_TEMP;
    }
    
    fclose(logFp);

    // If everything went well
    return MUMBLE_PDEC_OK;
	// Other potential return values are:
	// MUMBLE_PDEC_ERROR_TEMP -> The plugin can temporarily not deliver positional data
	// MUMBLE_PDEC_PERM -> Permanenet error. The plugin will never be able to deliver positional data
}

bool mumble_fetchPositionalData(float *avatarPos, float *avatarDir, float *avatarAxis, float *cameraPos, float *cameraDir,
                                float *cameraAxis, const char **context, const char **identity) {
    // fetch positional data and store it in the respective variables. All fields that can't be filled properly
	// have to be set to 0 or the empty String ""

    logFp = fopen(logPath, "a");
    if (data == (data_container_t*)0) {
        mumbleAPI.log(ownID, "data is null");
        fputs("data is null\n", logFp);
        fclose(logFp);
        return false;
    }

    // Mumble | Game
	// X      | X
	// Y      | Y
	// Z      | -

    fputs("Reading mmf: mumble_fetchPositionalData()\n", logFp);
    ProxError_t err = readData(mmf);

    if (err != NoError) {
        mumbleAPI.log(ownID, "Error " + err);
        fputs("mmf read error, err code: " + err + '\n', logFp);
        return false; 
    }

    if (data->inWorld == 0) {
        mumbleAPI.log(ownID, "Not in world.");
        fputs("Not in world\n", logFp);
        return false;
    }

    log_data();

    for (int i = 0; i < 3; i++) {
        avatarPos[i] = cameraPos[i] = 0.0f;
        avatarDir[i] = cameraDir[i] = 0.0f;
        avatarAxis[i] = cameraAxis[i] = 0.0f;
    }

    avatarPos[0] = cameraPos[0] = data->posX;
    avatarPos[1] = cameraPos[1] = data->posY;

    strlcpy(newContext, data->world_name, sizeof(char) * 28);

    newContext[data->world_name_len] = data->dead == 1 ? '-1' : data->team;
    newContext[data->world_name_len + 1] = '\0';
    *context = newContext;
    fprintf(logFp, "context: %s\n", *context);

    *identity = data->name;
    fprintf(logFp, "identity: %s\n", *identity);

    fclose(logFp);

    // If positional data could be fetched successfully
    return true;
	// otherwise return false
}

void mumble_shutdownPositionalData() {
	// Unlink the connection to the supported game
    // Perform potential clean-up code
    logFp = fopen(logPath, "a");
    data = (data_container_t*)0;
    fputs("Positional Shutdown: mumble_shutdownPositionalData()\n", logFp);
    mumbleAPI.log(ownID, "Positional Shutdown");
    
    

    // WindowsUnmap(mmf);
    if (munmap(mmf) != 0) {
        mumbleAPI.log(ownID, "Munmap failure!");
        fputs("Munmap failure!\n", logFp);
    }

    free(buf);
    free(data);
    free(filePath);
    free(newContext);
    fputs("Freed vars\n", logFp);
    fclose(logFp);
}

int main() {
    /*
    data = (data_container_t*)malloc(64);
    buf = (char*)malloc(sizeof(float));
    //filePath = (char*)malloc(sizeof(char) * 1024);
    logPath = (char*)malloc(sizeof(char) * 1024);
    //pid = (int)malloc(sizeof(int));

    char* temp = getenv("TMP");
    size_t len = strlen(temp) + strlen(fileName) + 1;
    filePath = (char*)malloc(sizeof(char) * len);
    strcpy(filePath, temp);
    strcat(filePath, fileName);
    printf("%s\n", filePath);

    //printf("error code: %d\n", windows_main());

    free(buf);              // Automatically gets freed by OS on Process Exit
    free(data);             // But best practice is to manually unmap and free
    munmap((void*)mmf);
    free(filePath);
    free(logPath);
    */
}
#endif