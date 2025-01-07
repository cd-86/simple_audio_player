#ifndef WINDOW_H
#define WINDOW_H
#include <cstdint>
#include <miniaudio.h>

struct GLFWwindow;

class Window
{
public:
    Window();
    ~Window();

    int run();

private:
    void initWindow();
    void cleanup();

    void drawUI();

    void playSound();

    void stopSound();

    static void ErrorCallback (int error_code, const char* description);

    static void AudioDeviceDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

private:
    GLFWwindow *m_window {nullptr};

    ma_decoder *m_decoder {nullptr};

    ma_device  *m_device {nullptr};

    uint64_t m_soundLength {0};
    uint64_t m_soundCursor {0};
    uint32_t m_soundSampleRate {1};
    int m_jumpToFrame {-1};

    bool m_isPlay {false};

};


#endif //WINDOW_H
