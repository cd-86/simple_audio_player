#include "Window.h"

#include <codecvt>
#include <iostream>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <filesystem>

Window::Window()
{
    glfwSetErrorCallback(ErrorCallback);
    initWindow();
}

Window::~Window()
{
}

int Window::run()
{
    if (!m_window)
        return 1;
    while (!glfwWindowShouldClose(m_window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        drawUI();
        glfwSwapBuffers(m_window);
    }
    cleanup();
    return 0;
}

void Window::initWindow()
{
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(800, 100, "Audio player", NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // 禁用保存界面布局
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    // ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();

    io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\msyh.ttc)", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
}

void Window::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();

    stopSound();
}

void Window::drawUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    ImGui::SetNextWindowSize(ImVec2(w, h));
    ImGui::Begin("Controls", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoTitleBar);
    if (ImGui::Button(m_isPlay ? "Pause" : "Play"))
    {
        playSound();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        stopSound();
    }

    ImGui::Separator();

    int i = m_jumpToFrame == -1 ? m_soundCursor : m_jumpToFrame;

    char curStr[16];
    char lenStr[16];
    sprintf(curStr, "%d:%02d", i / m_soundSampleRate / 60, i / m_soundSampleRate % 60);
    sprintf(lenStr, "%d:%02d", m_soundLength / m_soundSampleRate / 60, m_soundLength / m_soundSampleRate % 60);

    if (ImGui::SliderInt(lenStr, &i, 0, m_soundLength, curStr))
    {
        m_jumpToFrame = i;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::playSound()
{
    if (!m_decoder)
    {
        m_decoder = new ma_decoder;
        m_device = new ma_device;
        {
            ma_result result = ma_decoder_init_file("Music/WalkingAndDancing.mp3", nullptr, m_decoder);
            if (result != MA_SUCCESS)
            {
                std::cerr << "Failed to open decoder" << std::endl;
                delete m_decoder;
                delete m_device;
                m_decoder = nullptr;
                m_device = nullptr;
                return;
            }

            ma_decoder_get_length_in_pcm_frames(m_decoder, &m_soundLength);
            m_soundSampleRate = m_decoder->outputSampleRate;

            ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
            deviceConfig.playback.format = m_decoder->outputFormat;
            deviceConfig.playback.channels = m_decoder->outputChannels;
            deviceConfig.sampleRate = m_decoder->outputSampleRate;
            deviceConfig.dataCallback = AudioDeviceDataCallback;
            deviceConfig.pUserData = this;

            if (ma_device_init(NULL, &deviceConfig, m_device) != MA_SUCCESS)
            {
                printf("Failed to open playback device.\n");
                ma_decoder_uninit(m_decoder);
                delete m_decoder;
                delete m_device;
                m_decoder = nullptr;
                m_device = nullptr;
                return;
            }


            if (ma_device_start(m_device) != MA_SUCCESS)
            {
                printf("Failed to start playback device.\n");
                ma_device_uninit(m_device);
                ma_decoder_uninit(m_decoder);
            }
        }
    }
    m_isPlay = !m_isPlay;
}

void Window::stopSound()
{
    if (m_decoder)
    {
        ma_device_uninit(m_device);
        ma_decoder_uninit(m_decoder);
        delete m_decoder;
        delete m_device;
        m_device = nullptr;
        m_decoder = nullptr;
    }
    m_isPlay = false;
    m_soundCursor = 0;
}

void Window::ErrorCallback(int error_code, const char* description)
{
    std::cerr << "Error[" << error_code << "]: " << description << std::endl;
}

void Window::AudioDeviceDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    Window* w = (Window*)pDevice->pUserData;

    if (w->m_decoder == nullptr)
    {
        return;
    }

    if (w->m_jumpToFrame != -1)
    {
        ma_decoder_seek_to_pcm_frame(w->m_decoder, w->m_jumpToFrame);
        // ma_decoder_get_cursor_in_pcm_frames(w->m_decoder, &w->m_soundCursor);
        w->m_soundCursor = w->m_jumpToFrame;
        w->m_jumpToFrame = -1;
        return;
    }

    if (w->m_isPlay)
    {
        ma_decoder_read_pcm_frames(w->m_decoder, pOutput, frameCount, nullptr);
        ma_decoder_get_cursor_in_pcm_frames(w->m_decoder, &w->m_soundCursor);
        return;
    }

    (void)pInput;
}
