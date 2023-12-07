#include "NierConfig.h"
#include "Log.h"
#include "StringConvert.h"
#include "StringFormat.h"
#include "Tokenize.h"
#include "Util.h"
#include <fstream>
#include <mutex>
#include <unordered_map>

namespace
{
    std::string _filename;
    std::vector<std::string> _additonalFiles;
    std::vector<std::string> _args;
    std::unordered_map<std::string /*name*/, std::string /*value*/> _configOptions;
    std::mutex _configLock;
    bool _usingDistConfig = false;

    template<typename Format, typename... Args>
    inline void PrintError(std::string_view filename, Format&& fmt, Args&& ... args)
    {
        std::string message = Acore::StringFormatFmt(std::forward<Format>(fmt), std::forward<Args>(args)...);

        fmt::print("{}\n", message);
    }

    void AddKey(std::string const& optionName, std::string const& optionKey, std::string_view fileName, bool isOptional, [[maybe_unused]] bool isReload)
    {
        auto const& itr = _configOptions.find(optionName);

        // Check exit option
        if (itr != _configOptions.end())
        {
            _configOptions.erase(optionName);
        }

        _configOptions.emplace(optionName, optionKey);
    }

    bool ParseFile(std::string const& file, bool isOptional, bool isReload)
    {
        std::ifstream in(file);

        if (in.fail())
        {
            if (isOptional)
            {
                // No display erorr if file optional
                return false;
            }

            throw NierConfigException(Acore::StringFormatFmt("Config::LoadFile: Failed open {}file '{}'", isOptional ? "optional " : "", file));
        }

        uint32 count = 0;
        uint32 lineNumber = 0;
        std::unordered_map<std::string /*name*/, std::string /*value*/> fileConfigs;

        auto IsDuplicateOption = [&](std::string const& confOption)
        {
            auto const& itr = fileConfigs.find(confOption);
            if (itr != fileConfigs.end())
            {
                PrintError(file, "> Config::LoadFile: Dublicate key name '{}' in config file '{}'", confOption, file);
                return true;
            }

            return false;
        };

        while (in.good())
        {
            lineNumber++;
            std::string line;
            std::getline(in, line);

            // read line error
            if (!in.good() && !in.eof())
            {
                throw NierConfigException(Acore::StringFormatFmt("> Config::LoadFile: Failure to read line number {} in file '{}'", lineNumber, file));
            }

            // remove whitespace in line
            line = Acore::String::Trim(line, in.getloc());

            if (line.empty())
            {
                continue;
            }

            // comments
            if (line[0] == '#' || line[0] == '[')
            {
                continue;
            }

            size_t found = line.find_first_of('#');
            if (found != std::string::npos)
            {
                line = line.substr(0, found);
            }

            auto const equal_pos = line.find('=');

            if (equal_pos == std::string::npos || equal_pos == line.length())
            {
                PrintError(file, "> Config::LoadFile: Failure to read line number {} in file '{}'. Skip this line", lineNumber, file);
                continue;
            }

            auto entry = Acore::String::Trim(line.substr(0, equal_pos), in.getloc());
            auto value = Acore::String::Trim(line.substr(equal_pos + 1, std::string::npos), in.getloc());

            value.erase(std::remove(value.begin(), value.end(), '"'), value.end());

            // Skip if 2+ same options in one config file
            if (IsDuplicateOption(entry))
            {
                continue;
            }

            // Add to temp container
            fileConfigs.emplace(entry, value);
            count++;
        }

        // No lines read
        if (!count)
        {
            if (isOptional)
            {
                // No display erorr if file optional
                return false;
            }

            throw NierConfigException(Acore::StringFormatFmt("Config::LoadFile: Empty file '{}'", file));
        }

        // Add correct keys if file load without errors
        for (auto const& [entry, key] : fileConfigs)
        {
            AddKey(entry, key, file, isOptional, isReload);
        }

        return true;
    }

    bool LoadFile(std::string const& file, bool isOptional, bool isReload)
    {
        try
        {
            return ParseFile(file, isOptional, isReload);
        }
        catch (const std::exception& e)
        {
            PrintError(file, "> {}", e.what());
        }

        return false;
    }
}

bool NierConfig::LoadInitial(std::string const& file, bool isReload /*= false*/)
{
    std::lock_guard<std::mutex> lock(_configLock);
    _configOptions.clear();
    return LoadFile(file, false, isReload);
}

bool NierConfig::LoadAdditionalFile(std::string file, bool isOptional /*= false*/, bool isReload /*= false*/)
{
    std::lock_guard<std::mutex> lock(_configLock);
    return LoadFile(file, isOptional, isReload);
}

NierConfig* NierConfig::instance()
{
    static NierConfig instance;
    return &instance;
}

bool NierConfig::Reload()
{
    if (!LoadAppConfigs(true))
    {
        return false;
    }

    return LoadModulesConfigs(true, false);
}

template<class T>
T NierConfig::GetValueDefault(std::string const& name, T const& def, bool showLogs /*= true*/) const
{
    auto const& itr = _configOptions.find(name);
    if (itr == _configOptions.end())
    {
        if (showLogs)
        {
            LOG_ERROR("server.loading", "> Config: Missing property {} in all config files, at least the .dist file must contain: \"{} = {}\"",
                name, name, Acore::ToString(def));
        }

        return def;
    }

    auto value = Acore::StringTo<T>(itr->second);
    if (!value)
    {
        if (showLogs)
        {
            LOG_ERROR("server.loading", "> Config: Bad value defined for name '{}', going to use '{}' instead",
                name, Acore::ToString(def));
        }

        return def;
    }

    return *value;
}

template<>
std::string NierConfig::GetValueDefault<std::string>(std::string const& name, std::string const& def, bool showLogs /*= true*/) const
{
    auto const& itr = _configOptions.find(name);
    if (itr == _configOptions.end())
    {
        if (showLogs)
        {
            LOG_ERROR("server.loading", "> Config: Missing option {}, add \"{} = {}\"",
                name, name, def);
        }

        return def;
    }

    return itr->second;
}

template<class T>
T NierConfig::GetOption(std::string const& name, T const& def, bool showLogs /*= true*/) const
{
    return GetValueDefault<T>(name, def, showLogs);
}

template<>
bool NierConfig::GetOption<bool>(std::string const& name, bool const& def, bool showLogs /*= true*/) const
{
    std::string val = GetValueDefault(name, std::string(def ? "1" : "0"), showLogs);

    auto boolVal = Acore::StringTo<bool>(val);
    if (!boolVal)
    {
        if (showLogs)
        {
            LOG_ERROR("server.loading", "> Config: Bad value defined for name '{}', going to use '{}' instead",
                name, def ? "true" : "false");
        }

        return def;
    }

    return *boolVal;
}

std::vector<std::string> NierConfig::GetKeysByString(std::string const& name)
{
    std::lock_guard<std::mutex> lock(_configLock);

    std::vector<std::string> keys;

    for (auto const& [optionName, key] : _configOptions)
    {
        if (!optionName.compare(0, name.length(), name))
        {
            keys.emplace_back(optionName);
        }
    }

    return keys;
}

std::string const NierConfig::GetFilename()
{
    std::lock_guard<std::mutex> lock(_configLock);
    return _usingDistConfig ? _filename + ".dist" : _filename;
}

std::vector<std::string> const& NierConfig::GetArguments() const
{
    return _args;
}

std::string const NierConfig::GetConfigPath()
{
    std::lock_guard<std::mutex> lock(_configLock);

#if AC_PLATFORM == AC_PLATFORM_WINDOWS
    return "configs/";
#else
    return std::string(_CONF_DIR) + "/";
#endif
}

void NierConfig::Configure(std::string const& initFileName, std::vector<std::string> args, std::string_view modulesConfigList /*= {}*/)
{
    _filename = initFileName;
    _args = std::move(args);

    // Add modules config if exist
    if (!modulesConfigList.empty())
    {
        for (auto const& itr : Acore::Tokenize(modulesConfigList, ',', false))
        {
            _additonalFiles.emplace_back(itr);
        }
    }
}

bool NierConfig::LoadAppConfigs(bool isReload /*= false*/)
{
    // #1 - Load init config file .conf
    if (!LoadInitial(_filename, isReload))
    {
        return false;
    }

    return true;
}

bool NierConfig::LoadModulesConfigs(bool isReload /*= false*/, bool isNeedPrintInfo /*= true*/)
{
    if (_additonalFiles.empty())
    {
        // Send successful load if no found files
        return true;
    }

    if (isNeedPrintInfo)
    {
        LOG_INFO("server.loading", " ");
        LOG_INFO("server.loading", "Loading modules configuration...");
    }

    // Start loading module configs
    std::string const& moduleConfigPath = GetConfigPath() + "modules/";
    bool isExistDefaultConfig = true;
    bool isExistDistConfig = true;

    for (auto const& distFileName : _additonalFiles)
    {
        std::string defaultFileName = distFileName;

        if (!defaultFileName.empty())
        {
            defaultFileName.erase(defaultFileName.end() - 5, defaultFileName.end());
        }

        // Load .conf.dist config
        isExistDistConfig = LoadAdditionalFile(moduleConfigPath + distFileName, false, isReload);

        if (!isReload && !isExistDistConfig)
        {
            LOG_FATAL("server.loading", "> NierConfig::LoadModulesConfigs: Not found original config '{}'. Stop loading", distFileName);
            ABORT();
        }

        // Load .conf config
        isExistDefaultConfig = LoadAdditionalFile(moduleConfigPath + defaultFileName, true, isReload);

        if (isExistDefaultConfig && isExistDistConfig)
        {
            _moduleConfigFiles.emplace_back(defaultFileName);
        }
        else if (!isExistDefaultConfig && isExistDistConfig)
        {
            _moduleConfigFiles.emplace_back(distFileName);
        }
    }

    if (isNeedPrintInfo)
    {
        if (!_moduleConfigFiles.empty())
        {
            // Print modules configurations
            LOG_INFO("server.loading", " ");
            LOG_INFO("server.loading", "Using modules configuration:");

            for (auto const& itr : _moduleConfigFiles)
            {
                LOG_INFO("server.loading", "> {}", itr);
            }
        }
        else
        {
            LOG_INFO("server.loading", "> Not found modules config files");
        }
    }

    if (isNeedPrintInfo)
    {
        LOG_INFO("server.loading", " ");
    }

    return true;
}

// @deprecated DO NOT USE - use GetOption<std::string> instead.
std::string NierConfig::GetStringDefault(std::string const& name, const std::string& def, bool showLogs /*= true*/)
{
    return GetOption<std::string>(name, def, showLogs);
}

// @deprecated DO NOT USE - use GetOption<bool> instead.
bool NierConfig::GetBoolDefault(std::string const& name, bool def, bool showLogs /*= true*/)
{
    return GetOption<bool>(name, def, showLogs);
}

// @deprecated DO NOT USE - use GetOption<int32> instead.
int NierConfig::GetIntDefault(std::string const& name, int def, bool showLogs /*= true*/)
{
    return GetOption<int32>(name, def, showLogs);
}

// @deprecated DO NOT USE - use GetOption<float> instead.
float NierConfig::GetFloatDefault(std::string const& name, float def, bool showLogs /*= true*/)
{
    return GetOption<float>(name, def, showLogs);
}

bool NierConfig::StartNier()
{
    std::string configFile = GetConfigPath() + std::string(NIER_CONFIG_FILE_NAME);
    std::vector<std::string> args;
    std::string_view moduleList;
    Configure(configFile, args, moduleList);
    if (!LoadAppConfigs())
    {
        sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_ERROR, "Error in nier config file : {}.", NIER_CONFIG_FILE_NAME);
        return false;
    }

    Enable = GetIntDefault("Enable", 0);
    Reset = GetIntDefault("Reset", 0);

    if (Enable == 0)
    {
        sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "nier is disabled.");
        return false;
    }
    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "nier started.");
    return true;
}

#define TEMPLATE_CONFIG_OPTION(__typename) \
    template __typename NierConfig::GetOption<__typename>(std::string const& name, __typename const& def, bool showLogs /*= true*/) const;

TEMPLATE_CONFIG_OPTION(std::string)
TEMPLATE_CONFIG_OPTION(uint8)
TEMPLATE_CONFIG_OPTION(int8)
TEMPLATE_CONFIG_OPTION(uint16)
TEMPLATE_CONFIG_OPTION(int16)
TEMPLATE_CONFIG_OPTION(uint32)
TEMPLATE_CONFIG_OPTION(int32)
TEMPLATE_CONFIG_OPTION(uint64)
TEMPLATE_CONFIG_OPTION(int64)
TEMPLATE_CONFIG_OPTION(float)

#undef TEMPLATE_CONFIG_OPTION
