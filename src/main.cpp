#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <cmath>
#include <vector>
#include <string>

const int WINDOW_WIDTH = 1440;
const int WINDOW_HEIGHT = 880;

struct Node {
    ImVec2 pos;
    bool selected;
    std::string label;
};

// Render input node wave box with high contrast colors
void DrawInputWaveBox(ImDrawList* drawList, ImVec2 pos, bool selected, float timeOffset) {
    ImVec2 boxSize(56.0f, 24.0f);
    ImVec2 minP(pos.x - boxSize.x * 0.5f, pos.y - boxSize.y * 0.5f);
    ImVec2 maxP(pos.x + boxSize.x * 0.5f, pos.y + boxSize.y * 0.5f);

    ImU32 bgCol = selected ? IM_COL32(255, 220, 0, 255) : IM_COL32(10, 25, 45, 240);
    ImU32 borderCol = selected ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 220, 255, 220);
    ImU32 waveCol = selected ? IM_COL32(10, 10, 10, 255) : IM_COL32(0, 255, 180, 255);

    drawList->AddRectFilled(minP, maxP, bgCol, 4.0f);
    drawList->AddRect(minP, maxP, borderCol, 4.0f, 0, 1.8f);

    const int samples = 14;
    ImVec2 pts[samples];
    for (int i = 0; i < samples; ++i) {
        float px = minP.x + 3.0f + ((boxSize.x - 6.0f) / (samples - 1)) * i;
        float py = pos.y + sinf((i * 0.45f) + timeOffset) * 5.0f;
        pts[i] = ImVec2(px, py);
    }
    for (int i = 0; i < samples - 1; ++i) {
        drawList->AddLine(pts[i], pts[i+1], waveCol, 2.0f);
    }
}

// Render circular neuron badge with sharp, high-contrast borders
void DrawNeuronBadge(ImDrawList* drawList, ImVec2 pos, std::string label, float activation) {
    ImU32 outerGlow = IM_COL32(255, (int)(100 + activation * 120), 0, (int)(30 + activation * 40));
    ImU32 innerBg   = IM_COL32(12, 22, 40, 255);
    ImU32 borderCol = IM_COL32(0, 220, 255, 240);

    drawList->AddCircleFilled(pos, 11.0f, outerGlow);
    drawList->AddCircleFilled(pos, 8.5f, innerBg);
    drawList->AddCircle(pos, 8.5f, borderCol, 0, 1.5f);
    drawList->AddText(ImVec2(pos.x - 6.0f, pos.y - 6.0f), IM_COL32(255, 255, 255, 255), label.c_str());
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "NeuralT v2.1 [C++/OpenGL Neural Network Editor]", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    std::vector<Node> inputLayer(17);
    std::vector<Node> hiddenLayer1(26);
    std::vector<Node> hiddenLayer2(38);
    std::vector<Node> outputLayer(14);

    std::vector<std::string> outLabels = {"Num0", "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Speed0", "Speed1", "Speed2", "Speed3"};

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        float time = (float)glfwGetTime() * 0.8f;

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        // High-Contrast Deep Navy Background (#050914)
        drawList->AddRectFilled(ImVec2(0, 0), ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), IM_COL32(5, 9, 20, 255));

        float colX[4] = { 180.0f, 520.0f, 880.0f, 1220.0f };
        float colWidth = 280.0f;

        for (size_t i = 0; i < inputLayer.size(); ++i) {
            inputLayer[i] = { ImVec2(colX[0], 90.0f + (i * 42.0f)), (i == 1 || i == 16), "In" + std::to_string(i) };
        }
        for (size_t i = 0; i < hiddenLayer1.size(); ++i) {
            hiddenLayer1[i] = { ImVec2(colX[1], 75.0f + (i * 28.0f)), false, "H" + std::to_string(i) };
        }
        for (size_t i = 0; i < hiddenLayer2.size(); ++i) {
            hiddenLayer2[i] = { ImVec2(colX[2], 65.0f + (i * 19.5f)), false, "H" + std::to_string(i) };
        }
        for (size_t i = 0; i < outputLayer.size(); ++i) {
            outputLayer[i] = { ImVec2(colX[3], 95.0f + (i * 48.0f)), (i == 1 || i == 11), outLabels[i] };
        }

        // Layer Panel Containers (High-Contrast Glass Panels)
        auto drawContainer = [&](float centerX, float width, const char* title, const char* activation) {
            ImVec2 minP(centerX - width * 0.5f, 40.0f);
            ImVec2 maxP(centerX + width * 0.5f, 810.0f);
            drawList->AddRectFilled(minP, maxP, IM_COL32(10, 18, 32, 220), 8.0f);
            drawList->AddRect(minP, maxP, IM_COL32(0, 180, 255, 120), 8.0f, 0, 1.5f);
            drawList->AddText(ImVec2(minP.x + 14, minP.y + 10), IM_COL32(255, 255, 255, 255), title);
            drawList->AddText(ImVec2(minP.x + 14, minP.y + 26), IM_COL32(0, 230, 255, 240), activation);
        };

        drawContainer(colX[0], colWidth, "Inputs: 17", "Activation: Linear (click)");
        drawContainer(colX[1], colWidth, "HL 1 - Neurons: 26", "Activation: ReLU (click)");
        drawContainer(colX[2], colWidth, "HL 2 - Neurons: 38", "Activation: ReLU (click)");
        drawContainer(colX[3], colWidth, "Outputs - Neurons: 14", "Activation: Sigmoid (click)");

        // High-Breadth, High-Contrast Multi-Strand Strings
        auto drawDenseConnections = [&](const std::vector<Node>& layerA, const std::vector<Node>& layerB) {
            for (size_t i = 0; i < layerA.size(); ++i) {
                for (size_t j = 0; j < layerB.size(); ++j) {
                    const auto& a = layerA[i];
                    const auto& b = layerB[j];
                    
                    for (int strand = 0; strand < 2; ++strand) {
                        float offset = (strand == 0) ? -2.0f : 2.0f;
                        ImVec2 startP(a.pos.x, a.pos.y + offset);
                        ImVec2 endP(b.pos.x, b.pos.y + offset);

                        float signal = (sinf(i * 0.2f + j * 0.15f + strand * 1.5f + time * 1.2f) + 1.0f) * 0.5f;
                        
                        // Vibrant, high-contrast color scheme (Neon Orange vs Electric Cyan / Magenta)
                        int r, g, bCol;
                        if ((i + j + strand) % 3 == 0) {
                            // Vivid Amber-Orange
                            r = 255; g = (int)(120 + signal * 80); bCol = 10;
                        } else if ((i + j + strand) % 3 == 1) {
                            // Electric Cyan
                            r = 0; g = (int)(200 + signal * 55); bCol = 255;
                        } else {
                            // Vivid Magenta/Rose
                            r = 255; g = 30; bCol = (int)(150 + signal * 105);
                        }

                        int alpha = (int)(30 + (signal * 50)); // Controlled opacity to maintain clarity

                        // Increased Breadth (Thickness 2.4px to 3.2px)
                        float thickness = 2.4f + (signal * 0.8f);
                        drawList->AddLine(startP, endP, IM_COL32(r, g, bCol, alpha), thickness);
                    }
                }
            }
        };

        drawDenseConnections(inputLayer, hiddenLayer1);
        drawDenseConnections(hiddenLayer1, hiddenLayer2);
        drawDenseConnections(hiddenLayer2, outputLayer);

        // Render Inputs
        for (const auto& node : inputLayer) {
            DrawInputWaveBox(drawList, node.pos, node.selected, time * 1.5f);
            drawList->AddText(ImVec2(node.pos.x - 85.0f, node.pos.y - 7.0f), IM_COL32(230, 240, 255, 255), node.label.c_str());
        }

        // Render Neurons
        for (size_t i = 0; i < hiddenLayer1.size(); ++i) {
            float act = (sinf(i * 0.5f + time * 1.0f) + 1.0f) * 0.5f;
            DrawNeuronBadge(drawList, hiddenLayer1[i].pos, "H" + std::to_string(i % 10), act);
        }

        for (size_t i = 0; i < hiddenLayer2.size(); ++i) {
            float act = (cosf(i * 0.4f + time * 0.9f) + 1.0f) * 0.5f;
            DrawNeuronBadge(drawList, hiddenLayer2[i].pos, "H" + std::to_string(i % 10), act);
        }

        // Output Pill Badges
        for (const auto& node : outputLayer) {
            ImVec2 minP(node.pos.x - 25, node.pos.y - 11);
            ImVec2 maxP(node.pos.x + 55, node.pos.y + 11);
            ImU32 bg = node.selected ? IM_COL32(255, 230, 0, 255) : IM_COL32(14, 28, 50, 240);
            ImU32 textCol = node.selected ? IM_COL32(0, 0, 0, 255) : IM_COL32(240, 245, 255, 255);

            drawList->AddRectFilled(minP, maxP, bg, 4.0f);
            drawList->AddRect(minP, maxP, IM_COL32(0, 200, 255, 180), 4.0f, 0, 1.2f);
            drawList->AddText(ImVec2(minP.x + 8, minP.y + 3), textCol, node.label.c_str());
        }

        // Header Title
        ImGui::SetNextWindowPos(ImVec2(colX[1] - 80, 8));
        ImGui::Begin("Header", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
        ImGui::TextColored(ImVec4(0.0f, 0.90f, 1.0f, 1.0f), "NeuralT v2.1 (C++/OpenGL - Neural Network Editor MB)");
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.02f, 0.04f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
