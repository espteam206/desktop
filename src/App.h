#pragma once

#include "Types.h"

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
    void WindowDemoGraph();

    void HelpWidget(const char*);
private:
    // TODO: define a struct for mixture values
    // std::string Name?
    // std::string (enum?) Type
    // float Value
    // float Distance
    // std::string (enum?) Method

    struct InputVal {
        int32_t Type;
        float Value;
        TransportType Trans;
        float Dist;
    };

    std::vector<InputVal> m_CemVals;
    std::vector<InputVal> m_AdmVals;
    std::vector<InputVal> m_AggVals;

    // Window toggles
    bool m_ShowInput = true;
    bool m_ShowGraph = true;
    bool m_ShowDemoGraph = false;
    bool m_ShowImGuiDemo = false;
};
