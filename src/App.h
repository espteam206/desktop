#pragma once

#include <array>
#include <filesystem>
#include <string>
#include <vector>

class App {
public:
    static App* Init();

    void Update(float dt);
    void Shutdown();
private:
    App();
    ~App();

    static App* s_Instance;
private:
    void WindowDockSpace();
    void WindowInput();
    void WindowGraph();
    void WindowMixtures();
    void WindowDemoGraph();

    void HelpWidget(const char*);
    
    void LoadData(const std::filesystem::path& file);
private:
    enum ContributorType {
        Cement,
        Admixture,
        Aggregate,
        Transport,
        CONTRIBUTOR_TYPE_CNT
    };

    struct MixtureVal {
        ContributorType Type;
        float Value;
        float Accuracy;
        std::string Name;
        std::string Location;
        std::string Source;
    };

    std::vector<MixtureVal> m_MixVals;

    // We need an array of just names for the input dropdown menu
    // (imgui accepts a const char*[] for the dropdown options)
    // std::array<std::vector<const char*>, CONTRIBUTOR_TYPE_CNT> m_ContrNames;

    struct InputVal {
        int32_t Type;
        float Value;
        // TransportType Trans;
        float Dist;

        InputVal() : Type(-1), Value(0.0f), Dist(0.0f) {}
    };

    std::vector<InputVal> m_CemVals;

    // Window toggles
    bool m_ShowInput = true;
    bool m_ShowGraph = true;
    bool m_ShowDemoGraph = false;
    bool m_ShowImGuiDemo = false;
    bool m_ShowMixturesVals   = false;
    bool m_ShowErrorAnalysis  = false;
};
