#pragma once

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <memory>
#include <optional>

namespace devilution {

bool FileExists(const char *path);
bool FileExistsAndIsWriteable(const char *path);
bool GetFileSize(const char *path, std::uintmax_t *size);
bool ResizeFile(const char *path, std::uintmax_t size);
void RemoveFile(const char *lpFileName);
std::optional<std::fstream> CreateFileStream(const char *path, std::ios::openmode mode);
FILE *FOpen(const char *path, const char *mode);

std::unique_ptr<wchar_t[]> ToWideChar(std::string_view path);

} // namespace devilution
