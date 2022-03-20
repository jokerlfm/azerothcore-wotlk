#ifndef NINGER_CONFIG_H
#define NINGER_CONFIG_H

#define NINGER_CONFIG_FILE_NAME "ninger.conf"

#ifndef NINGER_MARK
# define NINGER_MARK "ninger"
#endif

#ifndef AOE_TARGETS_RANGE
# define AOE_TARGETS_RANGE 4.0f
#endif

#ifndef MID_RANGE
# define MID_RANGE 8.0f
#endif

#ifndef MIN_DISTANCE_GAP
# define MIN_DISTANCE_GAP 0.5f
#endif

#ifndef FOLLOW_MIN_DISTANCE
# define FOLLOW_MIN_DISTANCE 0.5f
#endif

#ifndef FOLLOW_NEAR_DISTANCE
# define FOLLOW_NEAR_DISTANCE 5.0f
#endif

#ifndef FOLLOW_NORMAL_DISTANCE
# define FOLLOW_NORMAL_DISTANCE 12.0f
#endif

#ifndef FOLLOW_FAR_DISTANCE
# define FOLLOW_FAR_DISTANCE 25.0f
#endif

#ifndef FOLLOW_MAX_DISTANCE
# define FOLLOW_MAX_DISTANCE 38.0f
#endif

#ifndef MELEE_MIN_DISTANCE
# define MELEE_MIN_DISTANCE 0.1f
#endif

#ifndef MELEE_MAX_DISTANCE
# define MELEE_MAX_DISTANCE 1.0f
#endif

#ifndef RANGE_MIN_DISTANCE
# define RANGE_MIN_DISTANCE 7.0f
#endif

#ifndef RANGE_NORMAL_DISTANCE
# define RANGE_NORMAL_DISTANCE 25.0f
#endif

#ifndef RANGE_MAX_DISTANCE
# define RANGE_MAX_DISTANCE 38.0f
#endif

#include "Define.h"
#include <stdexcept>
#include <string_view>
#include <vector>

class NingerConfig
{
    NingerConfig()
    {
        Enable = 0;
        DPSDelay = 0;
        UpdateDelay = 0;
        ManagerCheckDelay = 0;
        NingerCountEachLevel = 0;
        TeleportDelay = 0;
        CorpseRunDelay = 0;
    }
    NingerConfig(NingerConfig const&) = delete;
    NingerConfig& operator=(NingerConfig const&) = delete;
    ~NingerConfig() = default;

public:
    bool LoadAppConfigs(bool isReload = false);
    bool LoadModulesConfigs(bool isReload = false, bool isNeedPrintInfo = true);
    void Configure(std::string const& initFileName, std::vector<std::string> args, std::string_view modulesConfigList = {});

    static NingerConfig* instance();

    bool Reload();

public:
    bool StartNinger();
    uint32 Enable;
    uint32 Reset;
    uint32 UpdateDelay;
    uint32 DPSDelay;
    uint32 TeleportDelay;
    uint32 CorpseRunDelay;
    uint32 ManagerCheckDelay;
    uint32 NingerCountEachLevel;

    std::string const GetFilename();
    std::string const GetConfigPath();
    [[nodiscard]] std::vector<std::string> const& GetArguments() const;
    std::vector<std::string> GetKeysByString(std::string const& name);

    template<class T>
    T GetOption(std::string const& name, T const& def, bool showLogs = true) const;

    /*
     * Deprecated geters. This geters will be deleted
     */

    [[deprecated("Use GetOption<std::string> instead")]]
    std::string GetStringDefault(std::string const& name, const std::string& def, bool showLogs = true);

    [[deprecated("Use GetOption<bool> instead")]]
    bool GetBoolDefault(std::string const& name, bool def, bool showLogs = true);

    [[deprecated("Use GetOption<int32> instead")]]
    int GetIntDefault(std::string const& name, int def, bool showLogs = true);

    [[deprecated("Use GetOption<float> instead")]]
    float GetFloatDefault(std::string const& name, float def, bool showLogs = true);

    /*
     * End deprecated geters
     */

    bool isDryRun() { return dryRun; }
    void setDryRun(bool mode) { dryRun = mode; }

private:
    /// Method used only for loading main configuration files (authserver.conf and worldserver.conf)
    bool LoadInitial(std::string const& file, bool isReload = false);
    bool LoadAdditionalFile(std::string file, bool isOptional = false, bool isReload = false);

    template<class T>
    T GetValueDefault(std::string const& name, T const& def, bool showLogs = true) const;

    bool dryRun = false;

    std::vector<std::string /*config variant*/> _moduleConfigFiles;
};

class NingerConfigException : public std::length_error
{
public:
    explicit NingerConfigException(std::string const& message) : std::length_error(message) { }
};

#define sNingerConfig NingerConfig::instance()

#endif
