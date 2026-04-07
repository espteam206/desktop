#pragma once

#include <cstdint>
#include <filesystem>

class Image {
public:
    Image(const std::filesystem::path& path);
    ~Image();

    const inline uint32_t GetID() const { return m_ID; }
    const inline uint32_t GetWidth() const { return m_Width; }
    const inline uint32_t GetHeight() const { return m_Height; }
private:
    uint32_t m_ID;
    int32_t m_Width;
    int32_t m_Height;
};
