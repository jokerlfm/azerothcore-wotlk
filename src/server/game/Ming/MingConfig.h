#ifndef MING_CONFIG_H
#define MING_CONFIG_H

#define MING_CONFIG_FILE_NAME "ming.conf"

#ifndef MING_MARK
# define MING_MARK "ming"
#endif

#include "Define.h"
#include <stdexcept>
#include <string_view>
#include <vector>

class MingConfig
{
    MingConfig()
    {
        Enable = 0;
        ManagerCheckDelay = 0;
        VenderReplaceDelay_Min = 0;
        VenderReplaceDelay_Max = 0;
    }
    MingConfig(MingConfig const&) = delete;
    MingConfig& operator=(MingConfig const&) = delete;
    ~MingConfig() = default;

public:
    bool LoadAppConfigs(bool isReload = false);
    bool LoadModulesConfigs(bool isReload = false, bool isNeedPrintInfo = true);
    void Configure(std::string const& initFileName, std::vector<std::string> args, std::string_view modulesConfigList = {});

    static MingConfig* instance();

    bool Reload();

public:
    bool StartMing();
    uint32 Enable;
    int ManagerCheckDelay;
    uint32 VenderReplaceDelay_Min;
    uint32 VenderReplaceDelay_Max;

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

class MingConfigException : public std::length_error
{
public:
    explicit MingConfigException(std::string const& message) : std::length_error(message) { }
};

#define sMingConfig MingConfig::instance()

#endif
