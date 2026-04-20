#pragma once
#include <filesystem>

namespace eng
{
    class FileSystem
    {
    public:
        std::filesystem::path GetExecutableFolder() const;
        std::filesystem::path GetAssetsFolder() const;
    };
}