#ifndef NINGER_CONFIG_H
#define NINGER_CONFIG_H

#define NIER_CONFIG_FILE_NAME "nier.conf"

#ifndef NIER_MARK
# define NIER_MARK "NIER"
#endif

#ifndef NIER_NORMAL_DISTANCE
# define NIER_NORMAL_DISTANCE 12.0f
#endif

#ifndef NIER_FAR_DISTANCE
# define NIER_FAR_DISTANCE 28.0f
#endif

#ifndef NIER_MAX_DISTANCE
# define NIER_MAX_DISTANCE 38.0f
#endif

#include "Define.h"
#include <stdexcept>
#include <string_view>
#include <vector>

class NierConfig
{
    NierConfig()
    {
        Enable = 0;
        Reset = 0;
    }
    NierConfig(NierConfig const&) = delete;
    NierConfig& operator=(NierConfig const&) = delete;
    ~NierConfig() = default;

public:
    bool LoadAppConfigs(bool isReload = false);
    bool LoadModulesConfigs(bool isReload = false, bool isNeedPrintInfo = true);
    void Configure(std::string const& initFileName, std::vector<std::string> args, std::string_view modulesConfigList = {});

    static NierConfig* instance();

    bool Reload();

public:
    bool StartNier();
    uint32 Enable;
    uint32 Reset;

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

class NierConfigException : public std::length_error
{
public:
    explicit NierConfigException(std::string const& message) : std::length_error(message) { }
};

#define sNierConfig NierConfig::instance()

#endif
