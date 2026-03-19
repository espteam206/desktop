#include <filesystem>
#include <fstream>
#include <string_view>
#include <optional>

class CSV {
public:
    CSV(const std::filesystem::path& csvFile)
        : m_Data(ReadFile(csvFile)), m_Pos(m_Data.data()),
        m_Lines(0), m_Row(1), m_Column(1)
    {
        for (size_t i = 0; i < m_Data.size(); ++i)
            m_Lines += m_Data[i] == '\n';
    }

    inline uint32_t GetLineCount() const { return m_Lines; }
    inline uint32_t GetRow()       const { return m_Row; }
    inline uint32_t GetColumn()    const { return m_Column; }

    bool NextLine() {
        if (m_Pos == m_Data.data() + m_Data.size())
            return false;

        size_t idx = m_Data.find('\n', m_Pos - m_Data.data());
        if (idx == std::string::npos)
            return false;

        size_t temp = (m_Pos - m_Data.data());

        m_Pos = m_Data.data() + idx + 1;

        if (m_Pos == m_Data.data() + m_Data.size())
            return false;

        m_Row++;
        m_Column = 1;
        return true;
    }

    std::optional<std::string_view> ReadLine() {
        if (m_Pos == m_Data.data() + m_Data.size())
            return std::nullopt;

        size_t idx = m_Data.find('\n', m_Pos - m_Data.data());
        if (idx == std::string::npos)
            return std::nullopt;

        char* end = m_Data.data() + idx;

        if (end + 1 == m_Data.data() + m_Data.size()) {
            m_Pos = end + 1;
            return std::nullopt;
        }

        std::string_view str(m_Pos, end - m_Pos);
        m_Pos = end + 1;
        m_Row++;
        m_Column = 1;
        return { str };
    }

    template<typename T>
    std::optional<T> Next();
private:
    std::string m_Data;
    char* m_Pos;

    uint32_t m_Lines;
    uint32_t m_Row;
    uint32_t m_Column;
private:
    static std::string ReadFile(const std::filesystem::path& file) {
        // TODO: error handling of file reading
        size_t size = std::filesystem::file_size(file);
        std::string contents(size, '\0');
        std::fstream ifs = std::fstream(file, std::ios::in);
        ifs.read(contents.data(), size);
        return contents;
    }
};

template<>
inline std::optional<std::string> CSV::Next<std::string>() {
    if (m_Pos == m_Data.data() + m_Data.size())
        return std::nullopt;

    if (*m_Pos == '\n')
        return std::nullopt;

    // TODO: construct new string for escaping " case

    bool skip_quote = false, end_of_line = false;

    size_t idx;
    if (*m_Pos == '"') {
        idx = m_Pos - m_Data.data();

        while (idx = m_Data.find('"', idx + 1) + 1) { // Find the matching quote
            // Error cases
            if (idx == m_Data.size()) {
                m_Pos = m_Data.data() + m_Data.size();
                return std::nullopt;
            } else if (idx == std::string::npos) {
                m_Pos++;
                break;
            }
            
            if (m_Data[idx] == '"') { // "" escapes the quote, so continue
                continue;
            } else if (m_Data[idx] == ',') {
                m_Pos++; // Skip the first '"' character
                idx--;   // Remove the last '"' character
                skip_quote = true;
                break;
            } else {
                return std::nullopt;
            }
        }
    } else {
        idx = m_Data.find_first_of(",\n", m_Pos - m_Data.data());
        end_of_line = m_Data[idx] == '\n';
    }

    if (idx == std::string::npos)
        idx = m_Data.size();

    char* end = m_Data.data() + idx;
    std::string str(m_Pos, end - m_Pos);
    m_Pos = end + 1 + skip_quote - end_of_line;
    m_Column++;

    return { str };
}

template<>
inline std::optional<float> CSV::Next<float>() {
    if (m_Pos == m_Data.data() + m_Data.size())
        return std::nullopt;

    if (*m_Pos == '\n')
        return std::nullopt;

    size_t idx = m_Data.find_first_of(",\n", m_Pos - m_Data.data());

    if (idx == std::string::npos)
        idx = m_Data.size();

    char* end = m_Data.data() + idx;

    char* numEnd;
    float value = std::strtof(m_Pos, &numEnd);

    // Check for error
    if (numEnd == m_Pos) {
        m_Pos = m_Data.data() + idx + 1;
        return std::nullopt;
    }

    if (*numEnd == '%')
        value *= 0.01f;

    m_Pos = m_Data.data() + idx + 1;
    m_Column++;

    return value;
}

#if 0
int main() {
    std::filesystem::path file = "Book(Data).csv";
    CSV s = CSV(file);

    // Skip the header
    s.NextLine();

    do {
        auto contributor = s.Next<std::string>();
        auto category    = s.Next<std::string>();
        auto name        = s.Next<std::string>();
        auto location    = s.Next<float>();
        auto value       = s.Next<float>();
        auto accuracy    = s.Next<float>();
        auto source      = s.Next<std::string>();

        if (name && value)
            std::cout << "Name: " << name.value() << " value: " << value.value() << " accuracy: " << accuracy.value() << "\n";
    } while (s.NextLine());

    return 0;
}
#endif
