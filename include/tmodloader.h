// Author.name = MyGuy

#ifndef TMODLOADER_PROX_CHAT
#define TMODLOADER_PROX_CHAT

// I later learned that uint8_t exists :|
// C# moment (It's the syntax I'm familiar with, ok)
typedef unsigned char byte;

// Comments next to fields indicate starting byte
typedef struct data_container {
    float posX; // 0
    float posY; // 4
    byte team; // 8
    byte dead; // 9
    byte radioChannel; // 10
    byte inWorld; // 11
    byte nameLen; // 12
    char* name; // 13 - 32
    byte worldNameLen; // 33
    char* worldName; // 34 - 60

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