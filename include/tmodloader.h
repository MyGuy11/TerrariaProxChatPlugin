// Author.name = MyGuy

#ifndef TMODLOADER_PROX_CHAT
#define TMODLOADER_PROX_CHAT

// C# moment (It's the syntax I'm familiar with, ok)
// I later learned that uint8_t exists :|
typedef unsigned char byte;

// Comments next to fields indicate byte / range
typedef struct data_container { // 128 bytes
    float posx; // 0 - 3
    float posy; // 4 - 7
    byte team; // 8
    byte dead; // 9
    byte radio_channel; // 10
    byte inworld; // 11
    byte namelen; // 12
    char* name; // 13 - 34
    byte world_name_len; // 35
    char* world_name; // 36 - 63
    int pid; // 64 - 66
    byte team_restrict; // 67
    // bool data_dumper_toggle // 127

} data_container_t;

typedef enum ProxError {
    NoError               = 0x0,
    FileError             = 0x1,
    MemoryMappedFileError = 0x2,
    TerrariaExit          = 0x3,
    RecievedExitMessage   = 0x4,
    MallocError           = 0x5,
} ProxError_t;

const char* proxerror_tostring(ProxError_t perr) {
    switch(perr) {
        case NoError:
            return "There is no error.";
        case FileError:
            return "Mmf file not found. Please run tModloader and ensure that the ProxChat mod is installed.";
        case MemoryMappedFileError:
            return "Could not open the mmf file. Please run tModLoader with the ProxChat mod first.";
        case TerrariaExit:
            return "unused rn";
        case RecievedExitMessage:
            return "not used in mumble";
        case MallocError:
            return "Malloc failed, please free up some memory on your system.";
        default:
            return "Unknown error code.";
    }
}

#endif