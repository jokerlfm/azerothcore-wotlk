#ifndef LINGER_CONFIG_H
#define LINGER_CONFIG_H

#define LINGER_CONFIG_FILE_NAME "linger.conf"

#ifndef LINGER_MARK
# define LINGER_MARK "linger"
#endif

#ifndef LINGER_UPDATE_DELAY
# define LINGER_UPDATE_DELAY 3600000
#endif

#include "Define.h"
#include <stdexcept>
#include <string_view>
#include <vector>

class LingerConfig
{
    LingerConfig()
    {
        Enable = 0;
    }
    LingerConfig(LingerConfig const&) = delete;
    LingerConfig& operator=(LingerConfig const&) = delete;
    ~LingerConfig() = default;

public:
    bool LoadAppConfigs(bool isReload = false);
    bool LoadModulesConfigs(bool isReload = false, bool isNeedPrintInfo = true);
    void Configure(std::string const& initFileName, std::vector<std::string> args, std::string_view modulesConfigList = {});

    static LingerConfig* instance();

    bool Reload();

public:
    bool StartLinger();
    uint32 Enable;

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

class LingerConfigException : public std::length_error
{
public:
    explicit LingerConfigException(std::string const& message) : std::length_error(message) { }
};

#define sLingerConfig LingerConfig::instance()

#endif
