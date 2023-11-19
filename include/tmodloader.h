// Author.name = MyGuy

#ifndef TMODLOADER_PROX_CHAT
#define TMODLOADER_PROX_CHAT

// C# moment (It's the syntax I'm familiar with, ok)
// I later learned that uint8_t exists :|
typedef unsigned char byte;

// Comments next to fields indicate byte / range
typedef struct data_container { // 128 bytes
    float posX; // 0 - 3
    float posY; // 4 - 7
    byte team; // 8
    byte dead; // 9
    byte radioChannel; // 10
    byte inWorld; // 11
    byte nameLen; // 12
    char* name; // 13 - 34
    byte worldNameLen; // 35
    char* worldName; // 36 - 63
    // int pid; 64 - 66
    byte teamRestrict; // 67
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

#endif