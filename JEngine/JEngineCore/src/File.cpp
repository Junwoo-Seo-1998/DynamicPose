#include "File.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

bool File::CheckExists(const std::filesystem::path& path)
{
    if (!std::filesystem::exists(path))
    {
        //Log::Error("There is no file : {}", path);
        return false;
    }
    return true;
}

std::vector<std::filesystem::path> File::GetFileLists(const std::filesystem::path& dir_path)
{
    if (!CheckExists(dir_path))
        return {};

    std::vector<std::filesystem::path> file_lists;
    for (const auto& entry : std::filesystem::directory_iterator(dir_path))
    {
        file_lists.push_back(entry.path().string());
    }

    return file_lists;
}

std::vector<std::filesystem::path> File::GetFileListsRecv(const std::filesystem::path& dir_path)
{
    if (!CheckExists(dir_path))
        return {};

    std::vector<std::filesystem::path> file_lists;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path))
    {
        file_lists.push_back(entry.path().string());
    }

    return file_lists;
}

std::string File::ReadFileToString(const std::string& file_name)
{
    if (!CheckExists(file_name)) return {};

    std::ifstream file(file_name);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::tuple<std::shared_ptr<char[]>, int> File::ReadFileToBytes(const std::filesystem::path& file_name)
{
    if (!CheckExists(file_name)) return {};

    std::ifstream stream(file_name, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return { nullptr, 0 };
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    int size = static_cast<int>(end - stream.tellg());

    if (size == 0)
    {
        // File is empty
        return { nullptr, 0 };
    }

    std::shared_ptr<char[]> buffer(new char[size]);
    stream.read(buffer.get(), size);
    stream.close();

    return { buffer, size };
}