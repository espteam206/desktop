#include "App.h"
#include "Types.h"

#include <imgui.h>
#include <implot.h>

App* App::s_Instance;

App* App::Init() {
    return new App;
}

App::App() {
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Load font
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto/roboto_regular.ttf", 24.0f, &fontConfig);

    // Styling
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.GrabRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.WindowBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.WindowMinSize = { 200.0f, 200.0f };

    m_CemVals.reserve(3);
    m_CemVals.emplace_back(InputVal{ 0 });
}

App::~App() {
    // stuff
}

void App::WindowInput() {
    if (!m_ShowInput)
        return;

    ImGui::Begin("Mixture parameters", &m_ShowInput);

    ImGui::Text("Cementitious materials");
    ImGui::SameLine();
    HelpWidget(R"(Input cementitious materials here
*more stuff about cementitious materials*)");

    ImGui::Separator();

    // float minusWidth = ImGui::CalcTextSize("-").x;
    float minusHeight = ImGui::GetTextLineHeightWithSpacing();

    ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit;

    for (int i = 0; i < m_CemVals.size(); ++i) {
        ImGui::PushID(i);

        ImGui::BeginTable("inputs", 3, tableFlags);
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        // The delete button
        // TODO: Make it square
        ImGui::PushItemWidth(100);
        if (ImGui::Button("-")) {
            m_CemVals.erase(std::next(m_CemVals.begin(), i));
            ImGui::EndTable();
            ImGui::PopID();
            continue;
        }
        ImGui::PopItemWidth();

        ImGui::TableNextColumn();
        // ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 30);
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("Type", &m_CemVals[i].Type, CEMENT_STRS, CEMENT_TYPES_CNT);
        ImGui::TableNextColumn();
        ImGui::InputFloat("%", &m_CemVals[i].Value);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); // Skip the column with the "minus" button

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("Transport Type", (int*)&m_CemVals[i].Trans, TRANSPORT_STRS, TRANSPORT_TYPES_CNT);
        ImGui::TableNextColumn();
        ImGui::InputFloat("km", &m_CemVals[i].Dist);

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::EndTable();

        ImGui::PopID();
    }

    if (ImGui::Button("Add##Cementitious")) {
        m_CemVals.push_back({ 0 });
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    ImGui::PushItemWidth(ImGui::GetWindowSize().x);
    ImGui::Button("Calculate!");

    ImGui::End();
}

void App::WindowGraph() {
    if (!m_ShowGraph)
        return;

    ImGui::Begin("Carbon Footprint", &m_ShowGraph);

    const char* labels[] = {
        "One",
        "Two",
        "Three",
    };

    float values[] = {
        1.0f,
        2.0f,
        3.0f,
    };

    ImPlotPieChartFlags flags = 0;
    flags |= ImPlotPieChartFlags_Exploding;
    flags |= ImPlotPieChartFlags_Normalize;
    flags |= ImPlotPieChartFlags_IgnoreHidden;

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    float width = std::min(viewportSize.x, viewportSize.y);

    // Centre the graph
    float off = (viewportSize.x - width) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    if (ImPlot::BeginPlot("##Footprint composition", { width, width })) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::PlotPieChart(labels, values, 3, 0.5, 0.5, 0.4, "%.2f", 90, flags);
        ImPlot::EndPlot();
    }

    ImGui::End();
}

void App::WindowDockSpace() {
    // Fullscreen stuff
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiID dockspaceID = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New");

            ImGui::Separator();

            if (ImGui::MenuItem("Quit")) {}

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Graph"))         { m_ShowGraph = true; }
            if (ImGui::MenuItem("Mixture Input")) { m_ShowInput = true; }

            ImGui::Separator();

            if (ImGui::MenuItem("Demo Graph"))      { m_ShowDemoGraph = true; }
            if (ImGui::MenuItem("Show ImGui Demo")) { m_ShowImGuiDemo = true; }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("About"))
            ImGui::OpenPopup("About##Popup");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("About##Popup")) {
            ImGui::Text("Concrete carbon calculator\n");
            ImGui::Text(R"(Project by:
Jeffrey Chen
Ruth Cheng
Zoey Fong
Raphael Larroquette
Justin Ma
Julian Poon)");
            ImGui::TextLinkOpenURL("Learn more", "https://github.com/espteam206/desktop");
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void App::HelpWidget(const char* text) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::TextLinkOpenURL("Learn more", "https://github.com/espteam206/desktop");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void App::Update(float dt) {
    if (m_ShowImGuiDemo)
        ImGui::ShowDemoWindow(&m_ShowImGuiDemo);

    WindowDockSpace();
    WindowInput();
    WindowGraph();
    WindowDemoGraph();
}

void App::Shutdown() {
    delete s_Instance;
    s_Instance = nullptr;
}

// Since this is just for development, leave it last
void App::WindowDemoGraph() {
    if (!m_ShowDemoGraph)
        return;

    ImGui::Begin("Demo graph", &m_ShowDemoGraph);

    static const char* labels1[]    = {"Frogs","Hogs","Dogs","Logs"};
    static float data1[]            = {0.15f,  0.30f,  0.2f, 0.05f};
    static ImPlotPieChartFlags flags = 0;
    ImGui::SetNextItemWidth(250);
    ImGui::DragFloat4("Values", data1, 0.01f, 0, 1);

    if (ImPlot::BeginPlot("##Pie1", ImVec2(ImGui::GetTextLineHeight()*16,ImGui::GetTextLineHeight()*16), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::PlotPieChart(labels1, data1, 4, 0.5, 0.5, 0.4, "%.2f", 90, flags);
        ImPlot::EndPlot();
    }

    ImGui::SameLine();

    static const char* labels2[]   = {"A","B","C","D","E"};
    static int data2[]             = {1,1,2,3,5};

    ImPlot::PushColormap(ImPlotColormap_Pastel);
    if (ImPlot::BeginPlot("##Pie2", ImVec2(ImGui::GetTextLineHeight()*16,ImGui::GetTextLineHeight()*16), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::PlotPieChart(labels2, data2, 5, 0.5, 0.5, 0.4, "%.0f", 180, flags);
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();

    ImGui::End();
}

