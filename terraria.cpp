// Mumble plugin for Terraria/tModLoader
// 

// COMMENT OUT BEFORE BUILD

//#define MUMBLE_PLUGIN_WRAPPER_USE_SERVER_EVENT_CALLBACKS // for no errors in vscode
//#define MUMBLE_PLUGIN_WRAPPER_USE_POSITIONAL_AUDIO
//COMMENT OUT BEFORE BUILD


#include "mumble/plugin/MumblePlugin.h"
#include "methods.h"

class TerrariaPlugin : public MumblePlugin {
public:
    TerrariaPlugin()
        : MumblePlugin("Terraria Proximity Chat", "MyGuy",
                       "A proximity chat plugin for Terraria. Requires tModLoader 1.4 and the ProxChat mod. You can get the mod through the mod browser in game, or on the github for this plugin. [link here]") {}
    
    virtual void onServerSynchronized(mumble_connection_t connection) noexcept override {
        try {
			std::cout << "Server " << m_api.getServerHash(connection) << " finished synchronizing" << std::endl;
			for (auto currentUser : m_api.getAllUsers(connection)) {
				std::cout << "Found user " << m_api.getUserName(connection, currentUser) << " ("
						  << m_api.getUserHash(connection, currentUser) << ")" << std::endl;
			}
		} catch (const MumbleAPIException &e) {
			std::cerr << "onServerSynchronized: " << e.what() << " (ErrorCode " << e.errorCode() << ")" << std::endl;
		}
    }

    virtual void releaseResource(const void* ptr) noexcept override {
        std::terminate();
    }
    
    virtual uint32_t getFeatures() const noexcept override {
        return MUMBLE_FEATURE_POSITIONAL;
    }

    virtual mumble_version_t getVersion() const noexcept override {
        const MumbleVersion b = { 4, 2, 0 };
        return mumble_version_t(b);
    }

    virtual uint8_t initPositionalData(std::vector< ProgramInformation > &programs) noexcept override {
        bool isGameRunning = false;
        // Not certain if this works or not yet, will have to test later
        for (size_t i = 0; i < programs.size(); i++) {
            if (strncmp(programs[i].name, "tModLoader", 11) != 0) {
                isGameRunning = true;
            }
        }

        // explicitly stating bool result for clarity
        if (isGameRunning == true) {
            return MUMBLE_PDEC_OK;
        }
        else {
            return MUMBLE_PDEC_ERROR_TEMP;
        }
    }

    virtual bool fetchPositionalData(float *avatarPos, float *avatarDir, float *avatarAxis, float *cameraPos, float *cameraDir,
                                    float* cameraAxis, const char** context, const char** identity) noexcept override {

        // Mumble -> Game
        // X -> X
        // Y -> Y
        // Z -> Y potentially, will have to test
        
        #ifdef __APPLE__
            std::string tempDataPath = getenv("HOME");
            tempDataPath += "/.local/share/tModLoader/ProxChat.dat";
            const std::string dataPath = tempDataPath;
        #else
            #ifdef _WIN32
                std::string tempDataPath = getenv("LOCALAPPDATA");
                tempDataPath += "\\tModLoader\\ProxChat.dat";
                const std::string dataPath = tempDataPath;
            #else
                std::string tempDataPath = getenv("HOME");
                tempDataPath += "/.local/tModLoader/ProxChat.dat";
                const std::string dataPath = tempDataPath;
            #endif // _WIN32
        #endif // __APPLE__
            
        for (uint8_t i = 0; i < 4; i++) {
            avatarPos[i] = cameraPos[i] = 0.0f;
            avatarDir[i] = cameraDir[i] = 0.0f;
            avatarAxis[i] = cameraAxis[i] = 0.0f;
        }

        std::vector<std::string> text = readPos(dataPath);

        std::string name = assignValue_s(text, "Name");
        float posX = assignValue_f(text, "PositionX") / 16; // position in blocks
        float posY = assignValue_f(text, "PositionY") / 16; // position in blocks
        int team = assignValue_i(text, "Team");
        bool dead = assignValue_b(text, "Dead");
        bool inWorld = assignValue_b(text, "InWorld");

        const char* blank = "";
        context = &blank;
        identity = &blank;

        // explicitly stating bool result for clarity
        if (inWorld == false) {
            return true;
        }

        if (dead == true) {
            context[0] = "dead";
        }
        else {
            context[0] = "alive";
        }
        
        identity[0] = name.c_str();
        context[1] = (char*)team;

        // Terraria calculates posisiton in pixels
        // Every block is 16 pixels (conversion done earlier)
        // A block is (2ft)^2
        // 1ft is 0.3048m
        
        float posX_m = (posX * 2) / 0.3048;
        float posY_m = (posY * 2) / 0.3048;

        avatarPos[0] = cameraPos[0] = posX_m;
        avatarPos[1] = cameraPos[1] = posY_m;

        return true;
    }

    virtual void shutdownPositionalData() noexcept override{
        return;
    }

};

MumblePlugin& MumblePlugin::getPlugin() noexcept {
    static TerrariaPlugin plugin;

    return plugin;
}





/*
MumbleStringWrapper mubmle_getName() {
    static const char* name = "Terraria Proximity Chat";

    MumbleStringWrapper wrapper;
    wrapper.data = name;
    wrapper.size = strlen(name);
    wrapper.needsReleasing = false;

    return wrapper;
}

MumbleStringWrapper mumble_getAuthor() {
    static const char* author = "MyGuyButAgain";

    MumbleStringWrapper wrapper;
    wrapper.data = author;
    wrapper.size = strlen(author);
    wrapper.needsReleasing = false;

    return wrapper;
}

MumbleStringWrapper mumble_getDescription() {
    static const char* description = "A proximity chat plugin for Terraria. Requires tModLoader 1.4 and the ProxChat mod. You can get the mod through the mod browser in game, or on the github for this plugin. [link here]";

    MumbleStringWrapper wrapper;
    wrapper.data = description;
    wrapper.size = strlen(description);
    wrapper.needsReleasing = false;

    return wrapper;
}
*/





