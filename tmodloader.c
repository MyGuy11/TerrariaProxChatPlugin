// Author.name = MyGuy

#ifndef _WIN32

#include "include/mumble/MumbleAPI_v_1_0_x.h"
#include "include/mumble/MumblePlugin_v_1_0_x.h"
#include "include/tmodloader.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

struct MumbleAPI_v_1_0_x mumbleAPI;
mumble_plugin_id_t ownID;

const char* fileName = "tModLoaderProxChat.tmp";
char* filePath;
char* buf;
char* logPath;
FILE* logFp;

int fd;
char* mappedFile;
char* newContext;
data_container_t* data;

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
    data->radioChannel = mmfPtr[10];
    data->nameLen = mmfPtr[12];
    data->name = &mmfPtr[13];
    data->worldNameLen = mmfPtr[33];
    data->worldName = &mmfPtr[34];

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
    printf("RadioChannel: %d\n", data->radioChannel);
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

    printf("WorldNameLen: %d\n", data->worldNameLen);

    for (int i = 0; i < data->worldNameLen; i++) {
        switch (i) {
            case 0:
                printf("WorldName: %c", data->worldName[i]);
                break;
                
            default:
                printf("%c", data->worldName[i]);
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
    fprintf(logFp, "RadioChannel: %d\n", data->radioChannel);
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

    fprintf(logFp, "WorldNameLen: %d\n", data->worldNameLen);

    for (int i = 0; i < data->worldNameLen; i++) {
        switch (i) {
            case 0:
                fprintf(logFp, "WorldName: %c", data->worldName[i]);
                break;
                
            default:
                fprintf(logFp, "%c", data->worldName[i]);
        }
    }
    fprintf(logFp, "\n");
}

void mmfPeek() {
    system("clear");
    for (int i = 0; i < 64; i++) {
        printf("mappedFile[%d]: %c\n", i, mappedFile[i]);
        if (i > 59 && i < 63) {
            printf("mappedFile[%d]: %d\n", i, mappedFile[i]);
        }
    }
    
}

ProxError_t unix_main() {
    char* env = getenv("TMPDIR");
    size_t len = strlen(env) + strlen(fileName) + 1;
    strlcpy(filePath, getenv("TMPDIR"), len);
    strlcat(filePath, fileName, len);
    

    if ((fd = open(filePath, O_RDONLY)) == -1) {
        perror("open failed");
        printf("run terraria first!\n"); // Convert to mumble error in the init function
        return FileError;
    }

    mappedFile = (char*)mmap(NULL, 64, PROT_READ, MAP_SHARED, fd, 0);

    ProxError_t err = NoError;
    while (err == NoError && mappedFile[63] != 1) {
        system("clear");
        err = readData(mappedFile);
        print_data();
    }
    printf("err: %d\n", err);

    return err;
}

ProxError_t unix_init() {
    buf = (char*)malloc(sizeof(float));
    data = (data_container_t*)malloc(sizeof(data));
    newContext = (char*)malloc(sizeof(char) * 28);

    if ((fd = open(filePath, O_RDONLY)) == -1) {
        mumbleAPI.log(ownID, "Failed to open file!\nRun Terraria First!");
        return FileError;
    }

    mappedFile = (char*)mmap(NULL, 64, PROT_READ, MAP_SHARED, fd, 0);

    if (mappedFile == (char*)-1) {
        return MemoryMappedFileError;
    }

    return NoError;
}

mumble_error_t mumble_init(mumble_plugin_id_t pluginID) {
    ownID = pluginID;

    char* env = getenv("HOME");
    char* logAppend = "/.local/share/tModLoaderProxChat.log";
    size_t len = strlen(env) + strlen(logAppend) + 1;
    logPath = (char*)malloc(sizeof(char) * len);

    strlcpy(logPath, env, len);
    strlcat(logPath, env, len);

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
    version.minor = 0;
    version.patch = 1;

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

    char* env = getenv("TMPDIR");
    size_t len = strlen(env) + strlen(fileName) + 1;
    filePath = (char*)malloc(sizeof(char) * len);
    strlcpy(filePath, env, len);
    strlcat(filePath, fileName, len);
    
    fprintf(logFp, "filePath: %s\n", filePath);

    bool gameIsRunning = false;
    ProxError_t perr;

    fputs("unix_init(): mumble_initPositionalData()\n", logFp);
    perr = unix_init();
    
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
        buf[i] = mappedFile[60 + i];
    }
    buf[3] = 0;
    int tmlPid = *(int*)buf;

    for (int i = 0; i < programCount; i++) {
        if (programPIDs[i] == tmlPid) {
            fputs("Game is running: mumble_initPositionalData()\n", logFp);
            gameIsRunning = true;
        }
    }

    if (!gameIsRunning) {
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

    // Mumble | Game
	// X      | X
	// Y      | Y
	// Z      | -
    fputs("Reading mmf: mumble_fetchPositionalData()\n", logFp);
    readData(mappedFile);

    if (data->inWorld == 0) {
        fputs("Not in world\n", logFp);
        fclose(logFp);
        return true;
    }

    log_data();
    fclose(logFp);

    for (int i = 0; i < 3; i++) {
        avatarPos[i] = cameraPos[i] = 0.0f;
        avatarDir[i] = cameraDir[i] = 0.0f;
        avatarAxis[i] = cameraAxis[i] = 0.0f;
    }

    avatarPos[0] = cameraPos[0] = data->posX;
    avatarPos[1] = cameraPos[1] = data->posY;

    strcpy(newContext, data->worldName);

    newContext[data->worldNameLen] = data->dead == 1 ? 't' : data->team;
    *context = newContext;

    *identity = data->name;

    // If positional data could be fetched successfully

    return true;
	// otherwise return false
}

void mumble_shutdownPositionalData() {
	// Unlink the connection to the supported game
    // Perform potential clean-up code
    logFp = fopen(logPath, "a");
    fputs("Positional Shutdown: mumble_shutdownPositionalData()\n", logFp);
    mumbleAPI.log(ownID, "Positional Shutdown");
    
    fclose(logFp);
    munmap(mappedFile, 64);

    free(buf);
    free(data);
    free(filePath);
    free(newContext);
}

int main() {
    /*
    data = (data_container_t*)malloc(sizeof(data));
    buf = (char*)malloc(sizeof(float));
    filePath = (char*)malloc(sizeof(char) * 1024);
    //pid = (int)malloc(sizeof(int));

    printf("error code: %d\n", unix_main());
    munmap(mappedFile, 64);

    free(buf);              // Automatically gets freed by OS on Process Exit
    free(data);             // But best practice is to manually unmap and free
    */
}

#endif