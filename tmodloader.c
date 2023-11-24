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

const int MMF_MAP_SIZE = 128;
const char* file_name = "tModLoaderProxChat.tmp";
char* mmf_path;
char* buf;
char* log_path;
FILE* logfp;

int fd;
char* mmf;
char* new_context;
data_container_t* data;

ProxError_t read_data(char* mmf_ptr) {
    for (int i = 0; i < 4; i++) {
        buf[i] = (byte)mmf_ptr[i];
        #ifdef DEBUG
        printf("mmf[%d]: %u\n", i, (byte)mmf_ptr[i]);
        #endif
    }
    data->posx = *(float*)buf;

    for (int i = 0; i < 4; i++) {
        buf[i] = mmf_ptr[4 + i];
        #ifdef DEBUG
        printf("mmf[%d]: %u\n", 4 + i, (byte)mmf_ptr[4 + i]);
        #endif
    }
    data->posy = *(float*)buf;

    data->team = mmf_ptr[8];
    data->dead = mmf_ptr[9];
    data->radio_channel = mmf_ptr[10];
    data->inworld = mmf_ptr[11];
    data->namelen = mmf_ptr[12];
    data->name = &mmf_ptr[13];
    data->world_name_len = mmf_ptr[35];
    data->world_name = &mmf_ptr[36];
    data->team_restrict = mmf_ptr[67];

    #ifdef DEBUG
    printf("mmf[%d]: %u\n", 8, mmf_ptr[8]);
    printf("mmf[%d]: %u\n", 9, mmf_ptr[9]);
    printf("mmf[%d]: %u\n", 10, mmf_ptr[10]);
    printf("mmf[%d]: %u\n", 11, mmf_ptr[11]);
    printf("mmf[%d]: %u\n", 11, mmf_ptr[12]);

    printf("namelen: %d\n", data->namelen);
    
    for (int i = 0; i < data->namelen; i++) {
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
    printf("PosX: %.6f\n", data->posx);
    printf("PosY: %.6f\n", data->posy);
    printf("Team: %d\n", data->team);
    printf("Dead: %d\n", data->dead);
    printf("RadioChannel: %d\n", data->radio_channel);
    printf("InWorld: %d\n", data->inworld);
    printf("NameLen: %d\n", data->namelen);

    for (int i = 0; i < data->namelen; i++) {
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

#ifdef DEBUG
void log_data() {
    //printf("%s\n", filePath);
    fprintf(logfp, "PosX: %.6f\n", data->posx);
    fprintf(logfp, "PosY: %.6f\n", data->posy);
    fprintf(logfp, "Team: %d\n", data->team);
    fprintf(logfp, "Dead: %d\n", data->dead);
    fprintf(logfp, "RadioChannel: %d\n", data->radio_channel);
    fprintf(logfp, "InWorld: %d\n", data->inworld);
    fprintf(logfp, "NameLen: %d\n", data->namelen);
    
    for (int i = 0; i < data->namelen; i++) {
        switch (i) {
            case 0:
                fprintf(logfp, "Name: %c", data->name[i]);
                break;
                
            default:
                fprintf(logfp, "%c", data->name[i]);
        }
    }
    fprintf(logfp, "\n");

    fprintf(logfp, "WorldNameLen: %d\n", data->world_name_len);

    for (int i = 0; i < data->world_name_len; i++) {
        switch (i) {
            case 0:
                fprintf(logfp, "WorldName: %c", data->world_name[i]);
                break;
                
            default:
                fprintf(logfp, "%c", data->world_name[i]);
        }
    }
    fprintf(logfp, "\n");
}
#endif

void mmfPeek() {
    system("clear");
    for (int i = 0; i < MMF_MAP_SIZE; i++) {
        printf("mmf[%d]: %c\n", i, mmf[i]);
        if (i > 59 && i < 63) {
            printf("mmf[%d]: %d\n", i, mmf[i]);
        }
    }
    
}

ProxError_t unix_init() {
    buf = (char*)malloc(sizeof(float));
    data = (data_container_t*)malloc(sizeof(data_container_t));
    new_context = (char*)malloc(sizeof(char) * 28);

    if (buf == NULL || data == NULL || new_context == NULL) {
        mumbleAPI.log(ownID, "malloc failed for init() vars, fatal error");
        return MallocError;
    }

    if ((fd = open(mmf_path, O_RDONLY)) == -1) {
        mumbleAPI.log(ownID, "Failed to open file!\nRun Terraria First!");
        return FileError;
    }

    mmf = (char*)mmap(NULL, MMF_MAP_SIZE, PROT_READ, MAP_SHARED, fd, 0);

    if (mmf == (char*)-1) {
        return MemoryMappedFileError;
    }

    return NoError;
}

mumble_error_t mumble_init(mumble_plugin_id_t pluginID) {
    ownID = pluginID;

    char* env = getenv("HOME");
    char* append = "/.local/share/tModLoaderProxChat.log";
    size_t len = strlen(env) + strlen(append) + 1;
    log_path = (char*)malloc(sizeof(char) * len);

    if (log_path == NULL) {
        mumbleAPI.log(ownID, "malloc failed for logPath, fatal error");
        return MUMBLE_EC_GENERIC_ERROR;
    }

    strlcpy(log_path, env, len);
    strlcat(log_path, append, len);

    logfp = fopen(log_path, "w");
    fputs("Plugin init: mumble_init()\n", logfp);
    fputs(log_path, logfp);

    if (mumbleAPI.log(ownID, "Terraria ProxChat Initializtion") != MUMBLE_STATUS_OK) {
		// Logging failed -> usually you'd probably want to log things like this in your plugin's
		// logging system (if there is any)

        fputs("Mumble didn't log: mumble_init()\n", logfp);
    }
    
    // MMF setup
    env = getenv("TMPDIR");
    len = strlen(env) + strlen(file_name) + 1;

    mmf_path = (char*)malloc(sizeof(char) * len);
if (mmf_path == NULL) {
        mumbleAPI.log(ownID, "malloc failed for filePath, fatal error");
        return MUMBLE_EC_GENERIC_ERROR;
    }
    strlcpy(mmf_path, env, len);
    strlcat(mmf_path, file_name, len);
    fprintf(logfp, "filePath: %s\n", mmf_path);

    ProxError_t perr;

    fputs("unix_init(): mumble_init()\n", logfp);
    perr = unix_init();
    
    if (perr != NoError) {
        char* ec_buffer = (char*)malloc(1024);
        strcat(ec_buffer, "Initialization failed!\nCheck Error Log!\nError: ");
        strcat(ec_buffer, proxerror_tostring(perr));

        fprintf(logfp, "Prox Init failed: mumble_init()\nErr code: %d\n", perr);
        fputs(ec_buffer, logfp);
        fclose(logfp);

        mumbleAPI.log(ownID, ec_buffer);
        mumbleAPI.log(ownID, log_path);

        return MUMBLE_EC_GENERIC_ERROR;
    }

    fclose(logfp);
    return MUMBLE_STATUS_OK;
}

void mumble_shutdown() {
    logfp = fopen(log_path, "a");
    fputs("shutting down plugin: mumble_shutdown()\n", logfp);
    
    if (munmap(mmf, MMF_MAP_SIZE) != 0) {
        mumbleAPI.log(ownID, "Munmap failure!");
        fputs("Munmap failure!\n", logfp);
    }

    free(buf);
    free(data);
    free(mmf_path);
    free(new_context);
    fputs("Freed vars\n", logfp);

	if (mumbleAPI.log(ownID, "Plugin Shutdown") != MUMBLE_STATUS_OK) {
		// Logging failed -> usually you'd probably want to log things like this in your plugin's
		// logging system (if there is any)
        fputs("failed to shutdown plugin!: mumble_shutdown()\n", logfp);
    }

    fclose(logfp);
    free(log_path);
}

uint8_t mumble_initPositionalData(const char *const *programNames, const uint64_t *programPIDs, size_t programCount) {
    // Check if the supported game is in the list of programs and if yes
    // check if the position can be obtained from the program
    logfp = fopen(log_path, "a");
    fputs("Terraria ProxChat Positional Data Intitialization: mumble_initPositionalData()\n", logfp);

    bool gameIsRunning = false;

    if (data == NULL) {
        fclose(logfp);
        return MUMBLE_PDEC_ERROR_TEMP;
    }

    if (mmf == NULL) {
        fclose(logfp);
        return MUMBLE_PDEC_ERROR_TEMP;
    }
    
    fputs("pid read", logfp);
    for (int i = 0; i < 3; i++) {
        buf[i] = mmf[64 + i];
    }
    buf[3] = 0; // Only using 3 out of 4 bytes
    data->pid = *(int*)buf;
    fprintf(logfp, "pid: %d\n", data->pid);

    for (int i = 0; i < programCount; i++) {
        if (programPIDs[i] == data->pid) {
            fputs("Game is running: mumble_initPositionalData()\n", logfp);
            gameIsRunning = true;
        }
    }

    if (!gameIsRunning) {
        fputs("Game isn't running: mumble_initPositionalData()\n", logfp);
        fclose(logfp);

        return MUMBLE_PDEC_ERROR_TEMP;
    }
    fputs("Reading mmf for world data: mumble_initPositionalData()\n", logfp);
    ProxError_t err = read_data(mmf);

    if (err != NoError) {
        mumbleAPI.log(ownID, "Error: " + err);
        mumbleAPI.log(ownID, proxerror_tostring(err));
        fputs("mmf read error, err code: " + err + '\n', logfp);
        fclose(logfp);

        return MUMBLE_PDEC_ERROR_TEMP; 
    }
 
    if (data->inworld == 0) {
        fputs("Not in world\n", logfp);
        fclose(logfp);

        return MUMBLE_PDEC_ERROR_TEMP;
    }

    
    fclose(logfp);

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

    logfp = fopen(log_path, "a");
    if (data == (data_container_t*)0) {
        mumbleAPI.log(ownID, "data is null");
        fputs("data is null\n", logfp);
        fclose(logfp);
        return false;
    }

    // Mumble | Game
	// X      | X
	// Y      | Y
	// Z      | -

    fputs("Reading mmf: mumble_fetchPositionalData()\n", logfp);
    ProxError_t err = read_data(mmf);

    if (err != NoError) {
        mumbleAPI.log(ownID, "Error: " + err);
        mumbleAPI.log(ownID, proxerror_tostring(err));
        fputs("mmf read error, err code: " + err + '\n', logfp);
        fputs(proxerror_tostring(err), logfp);
        fclose(logfp);

        return false; 
    }

    if (data->inworld == 0) {
        fputs("Not in world\n", logfp);
        fclose(logfp);

        return false;
    }

    log_data();
    //fclose(logfp);

    for (int i = 0; i < 3; i++) {
        avatarPos[i] = cameraPos[i] = 0.0f;
        avatarDir[i] = cameraDir[i] = 0.0f;
        avatarAxis[i] = cameraAxis[i] = 0.0f;
    }

    avatarPos[0] = cameraPos[0] = data->posx;
    avatarPos[1] = cameraPos[1] = data->posy;

    strlcpy(new_context, data->world_name, sizeof(char) * 28);

    if (data->team_restrict) {
        new_context[data->world_name_len] = data->dead == 1 ? 'd' : 'r';
    }
    else {
        new_context[data->world_name_len] = data->dead == 1 ? 'd' : data->team;
    }

    new_context[data->world_name_len + 1] = '\0';
    *context = new_context;
    *identity = data->name;
    fprintf(logfp, "context: %s\n", *context);
    fprintf(logfp, "identity: %s\n", *identity);
    fclose(logfp);

    // If positional data could be fetched successfully
    return true;
	// otherwise return false
}

void mumble_shutdownPositionalData() {
	// Unlink the connection to the supported game
    // Perform potential clean-up code
    logfp = fopen(log_path, "a");
    fputs("Positional Shutdown: mumble_shutdownPositionalData()\n", logfp);
    mumbleAPI.log(ownID, "Positional Shutdown");
    fclose(logfp);
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
    fopen(log_path, "a");
    fprintf(logfp, "Called mumble_releaseResource but expected that this never gets called -> Aborting");
    fclose(logfp);
    abort();
}

mumble_version_t mumble_getVersion() {
    mumble_version_t version;

    version.major = 1;
    version.minor = 2;
    version.patch = 5;

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

/*
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

    mmf = (char*)mmap(NULL, 128, PROT_READ, MAP_SHARED, fd, 0);

    ProxError_t err = NoError;
    while (err == NoError && mmf[127] != 1) {
        system("clear");
        err = readData(mmf);
        print_data();
    }
    printf("err: %d\n", err);

    return err;
}

int main() {

    char* env = getenv("HOME");
    char* logAppend = "/.local/share/tModLoaderProxChat.log";
    size_t len = strlen(env) + strlen(logAppend) + 1;
    logPath = (char*)malloc(sizeof(char) * len);

    strlcpy(logPath, env, len);
    strlcat(logPath, logAppend, len);

    printf("%s\n", logPath);

    data = (data_container_t*)malloc(sizeof(data));
    buf = (char*)malloc(sizeof(float));
    filePath = (char*)malloc(sizeof(char) * 1024);
    //pid = (int)malloc(sizeof(int));

    printf("error code: %d\n", unix_main());
    munmap(mmf, 64);

    free(buf);              // Automatically gets freed by OS on Process Exit
    free(data);             // But best practice is to manually unmap and free
}
*/

#endif