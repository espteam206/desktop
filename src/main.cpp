#include <print>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <implot.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void RenderDockSpace();
void RenderInput();
void RenderGraph();
void RenderDemoGraph();

static bool show_input = true;
static bool show_graph = true;
static bool show_demo_graph = true;

int main() {
    if (!glfwInit()) {
        std::println("Failed to initialize GLFW.");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "concrete", NULL, NULL);

    if (!window) {
        glfwTerminate();
        std::println("Could not create window.");
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Enable dockspace
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // ImGui::ShowDemoWindow();

        RenderDockSpace();
        RenderInput();
        RenderGraph();
        RenderDemoGraph();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}

static constexpr size_t input_cnt = 3;
float data[input_cnt] = { 0 };
const char* data_names[input_cnt] = {
    "Input 1",
    "Input 2",
    "Input 3",
};

void RenderInput() {
    if (!show_input)
        return;

    ImGui::Begin("Mixture parameters", &show_input);

    for (int i = 0; i < input_cnt; ++i) {
        ImGui::InputFloat(data_names[i], &data[i]);
    }

    ImGui::Button("Calculate!");

    ImGui::End();
}

void RenderDemoGraph() {
    if (!show_demo_graph)
        return;

    ImGui::Begin("Demo graph", &show_demo_graph);

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

void RenderGraph() {
    if (!show_graph)
        return;

    ImGui::Begin("Carbon Footprint", &show_graph);

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

void RenderDockSpace() {
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
    ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New");

            ImGui::Separator();

            if (ImGui::MenuItem("Quit")) {}

            ImGui::EndMenu();
        } else if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Graph"))         { show_graph = true; }
            if (ImGui::MenuItem("Mixture Input")) { show_input = true; }

            ImGui::Separator();

            if (ImGui::MenuItem("Demo Graph"))    { show_demo_graph = true; }

            ImGui::EndMenu();
        } else if (ImGui::BeginMenu("About")) {
            ImGui::Text("Concrete carbon calculator");

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

