/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  render.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:37:31
 * Description :  None
 * Note        : 
 ************************************************************************/
#include <codecvt>
#include <locale>
#include <chrono>
#include "render.h"
#include "shader.h"
#include "infra/include/Logger.h"
#include "infra/include/Timestamp.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

namespace playsdk {

Render::Render(DecodedFrameList& video_decoded_frame_queue) : video_decoded_frame_queue_(video_decoded_frame_queue) {
    window_width_ = 0;
    window_height_ = 0;
    //dashboard_= std::make_shared<Gui>("dashboard");
}

Render::~Render() {
}

bool Render::initial() {
    init_promise_ = std::make_shared<std::promise<bool>>();
    std::future<bool> future = init_promise_->get_future();
    infra::Thread::start();
    auto wait_result = future.wait_for(std::chrono::milliseconds(500));
    if (wait_result == std::future_status::timeout) {
        errorf("init timeout\n");
        return false;
    }
    return future.get();
}

bool Render::initShader() {
    if (shader_) {
        return true;
    }
    shader_ = std::make_shared<Shader>();

    const GLchar* vertex_shader_glsl = R"_(
        #version 330 core
        precision mediump float;
        attribute vec3 position;
        attribute vec2 coordinate;
        uniform float center_sacle_x;
        uniform float center_sacle_y;
        varying vec2 textureOut;
        void main(void)
        {
            gl_Position = vec4(position.x * center_sacle_x, position.y * center_sacle_y, position.z, 1.0);
            textureOut = coordinate;
        }
    )_";

    const GLchar* fragment_shader_glsl = R"_(
        #version 330 core
        varying vec2 textureOut;
        uniform sampler2D tex_y;
        uniform sampler2D tex_u;
        uniform sampler2D tex_v;
        void main(void)
        {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(tex_y, textureOut).r;
            yuv.g = texture2D(tex_u, textureOut).r - 0.5;
            yuv.b = texture2D(tex_v, textureOut).r - 0.5;
            rgb = mat3( 1.0,     1.0,       1.0,
                        0.0,     -0.21482,  2.12798,
                        1.28033, -0.38059,  0.0) * yuv;
            gl_FragColor = vec4(rgb, 1);
        }
    )_";

    if (!shader_->loadShader(vertex_shader_glsl, fragment_shader_glsl)) {
        shader_.reset();
        return false;
    }
    return true;
}

static const GLfloat s_vertices_coord[] = {
    // positions        // textureCoords
    -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,  // left top  
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f,  // right top 
     1.0f, -1.0f, 0.0f,   1.0f, 1.0f,  // right bottom
    -1.0f, -1.0f, 0.0f,   0.0f, 1.0f   // left bottom
};

static const unsigned int s_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

bool Render::initShaders() {
    if (!initShader()) {
        return false;
    }

    shader_->use();
    GLuint position = shader_->getAttribLocation("position");
    GLuint coordinate = shader_->getAttribLocation("coordinate");
    GLuint texturey = shader_->getUniformLocation("tex_y");
    GLuint textureu = shader_->getUniformLocation("tex_u");
    GLuint texturev = shader_->getUniformLocation("tex_v");

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(coordinate);

    glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices_coord), s_vertices_coord, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_indices), s_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(coordinate, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    glUniform1i(texturey, 0);
    glUniform1i(textureu, 1);
    glUniform1i(texturev, 2);

    //texture
    glGenTextures(3, gl_textureid_);
    glBindTexture(GL_TEXTURE_2D, gl_textureid_[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, gl_textureid_[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, gl_textureid_[2]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Render::setClient(std::weak_ptr<IClient> client) {
    client_ = client;
}

WindowEvent s_window_input_event;

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
    glViewport(0, 0, width, height);
}

static void window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

static void toggleFullscreen(GLFWwindow* window) {
    static bool fullscreen = false;
    static int windowedWidth, windowedHeight, windowedX, windowedY;
    fullscreen = !fullscreen;
    if (fullscreen) {
        // 保存当前窗口位置和大小
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        // 获取主显示器的视频模式
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        // 设置窗口的显示模式
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        // 设置全屏模式
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        // 恢复窗口模式
        glfwSetWindowMonitor(window, NULL, windowedX, windowedY, windowedWidth, windowedHeight, 0);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    static std::chrono::high_resolution_clock::time_point lastClickTime;
    static int clickCount = 0;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastClickTime).count();
        if (elapsedTime < 300) { // 300毫秒内认为是双击
            tracef("Double click detected!\n");
            clickCount = 0;
            toggleFullscreen(window);
        } else {
            clickCount = 1;
        }
        lastClickTime = currentTime;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        tracef("right click detected!\n");
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        tracef("middle click detected!\n");
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    //tracef("Mouse position: (%0.3f, %0.3f)\n", xpos, ypos);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    std::lock_guard<std::mutex> guard(s_window_input_event.mutex);
    s_window_input_event.whell_scroll_x = (float)xoffset;
    s_window_input_event.whell_scroll_y = (float)yoffset;
    //tracef("Mouse wheel scroll: xoffset = %0.1f, yoffset = %0.1f\n", xoffset, yoffset);
}

void Render::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

static void glfw_error_callback(int error, const char* description) {
    errorf("GLFW Error %d: %s\n", error, description);
}

GLFWwindow* Render::initWindowEnvironment() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        errorf("glfwInit failed\n");
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "playsdk", NULL, NULL);
    if (window == nullptr) {
        errorf("Failed to create GLFW window\n");
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetKeyCallback(window, window_key_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        errorf("Failed to initialize GLAD\n");
        glfwTerminate();
        return nullptr;
    }
    return window;
}

void Render::setCenterScale(GLFWwindow* window, int32_t video_width, int32_t video_height) {
    int32_t window_width = 0;
    int32_t window_height = 0;
    //glfwGetFramebufferSize(window, &window_width, &window_height);
    glfwGetWindowSize(window, &window_width, &window_height);
    if (window_width == window_width_ && window_height == window_height_) {
        return;
    }
    window_width_ = window_width;
    window_height_ = window_height;

    float video_aspect_ratio = 1.0f * video_width / video_height;
    float window_aspect_ration = 1.0f * window_width / window_height;
    if (window_aspect_ration > video_aspect_ratio) {
        // adjust x
        float window_width_need = 1.0f * video_width / video_height * window_height;
        center_scale_x_ = window_width_need / window_width;
        center_scale_y_ = 1.0f;
    } else {
        // adjust y
        float window_height_need = 1.0f * video_height / video_width * window_width;
        center_scale_y_ = window_height_need / window_height;
        center_scale_x_ = 1.0f;
    }
    shader_->setUniformFloat("center_sacle_y", center_scale_y_);
    shader_->setUniformFloat("center_sacle_x", center_scale_x_);

    polyon_.setCenterScale(center_scale_x_, center_scale_y_);
}

void Render::renderVideoFrame(GLFWwindow* window) {
    if (video_decoded_frame_queue_.size()) {
        DecodedFrame frame = video_decoded_frame_queue_.front();
        int64_t now = infra::getCurrentTimeMs();
        //debugf("redner pts:%lld, now:%lld\n", frame.frame_->pkt_pts, now);
        if (frame.frame_->pts <= now) {
            if (video_decoded_frame_queue_.size() > 1) {
                video_decoded_frame_queue_.pop();
            }
        }
        current_pts_ = frame.frame_->pts;

        shader_->use();
        glBindVertexArray(VAO_);

        int32_t width = frame.frame_->width;
        int32_t height = frame.frame_->height;
        setCenterScale(window, width, height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.frame_->data[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.frame_->data[1]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.frame_->data[2]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Render::setSpeed(float speed) {
    speed_ = speed;
}

void Render::setAudioCurrentPts(int64_t pts) {
    audio_current_pts_ = pts;
}

void Render::setVideoRate(float rate) {
    video_rate_ = rate;
}

bool Render::setTrackingBox(Json::Value data) {
    //tracef("%s\n", data.toStyledString().data());
    auto box = std::make_shared<CurrentDetectResult>();
    box->timestamp = data["timestamp"].asUInt();
    for (uint32_t i = 0; i < data["targets"].size(); i++) {
        Target target;
        target.type = (TargetType)data["targets"][i]["type"].asInt();
        target.id = data["targets"][i]["id"].asUInt();
        target.rect.x = data["targets"][i]["x"].asFloat();
        target.rect.y = data["targets"][i]["y"].asFloat();
        target.rect.w = data["targets"][i]["w"].asFloat();
        target.rect.h = data["targets"][i]["h"].asFloat();
        box->targets.push_back(std::move(target));
    }
    std::lock_guard<std::mutex> guard(tracking_box_list_mutex_);
    tracking_box_list_.push(box);
    return true;
}

void Render::readerTextInfo(GLFWwindow* window) {
    int32_t display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.85f, 0.0f));
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float scale = 0.8f;
    model = glm::scale(model, glm::vec3(display_h * scale / display_w, scale, 0.0f));

    glm::mat4 projection = glm::ortho(0.0f, window_width_*1.0f, 0.0f, window_height_*1.0f);

    std::string now = infra::getCurrentTime();
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring text = converter.from_bytes(now);
    text_.render(model, text.data(), text.length(), glm::vec3(1.0, 1.0, 1.0));

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.3f, 0.85f, 0.0f));
    model = glm::scale(model, glm::vec3(display_h * scale / display_w, scale, 0.0f));

    wchar_t video_pts[512] = {0};
    swprintf(video_pts, L"pts:%lld %lld %0.1fkbps", current_pts_ / 100, audio_current_pts_ / 100, video_rate_);
    text_.render(model, video_pts, wcslen(video_pts), glm::vec3(1.0, 1.0, 0.0));
}

void Render::renderTrackingBox(GLFWwindow* window) {
    static int64_t last_video_pts = 0;
    if (last_video_pts == current_pts_) {
        return;
    }
    last_video_pts = current_pts_;

    std::shared_ptr<CurrentDetectResult> result;
    {
        std::lock_guard<std::mutex> guard(tracking_box_list_mutex_);
        if (tracking_box_list_.size()) {
            result = tracking_box_list_.front();
            tracking_box_list_.pop();
        }
    }

    std::vector<std::vector<Position>> polyons;
    if (result) {
        for (auto &t : result->targets) {
            std::vector<Position> polyon;
            polyon.push_back({ t.rect.x, t.rect.y, 0.0f });
            polyon.push_back({ t.rect.x + t.rect.w, t.rect.y, 0.0f });
            polyon.push_back({ t.rect.x + t.rect.w, t.rect.y + t.rect.h, 0.0f });
            polyon.push_back({ t.rect.x, t.rect.y + t.rect.h, 0.0f });
            polyons.push_back(polyon);
        } 
    }
    //tracef("polyons size:%d\n", polyons.size());
    //adaptiveRender(polyons);
    polyon_.setPointLines(polyons);
}

void Render::adaptiveRender(std::vector<std::vector<Position>>& polyons) {
    for (auto& polygon : polyons) {
        for (auto& position : polygon) {
            position.x = position.x * center_scale_x_;
            position.y = position.y * center_scale_y_;
        }
    }
}

void Render::renderGui(GLFWwindow* window) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (auto client = client_.lock()) {
        client->interaction((void*)window);
    }

#if 0

    ImGuiIO& io = ImGui::GetIO();
    static bool show_another_window = false;
    static bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("你好Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

#endif

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Render::initGui(GLFWwindow* window) {
    const char* glsl_version = "#version 130";
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    if (io.Fonts->AddFontFromFileTTF("./simhei.ttf", 16.0f, NULL,io.Fonts->GetGlyphRangesChineseSimplifiedCommon()) == nullptr) {
        errorf("imgui load ttf error\n");
        return false;
    }

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

#define RENDER_FPS 65

void Render::run() {
    warnf("render thread start\n");
    bool init_reesult = false;
    GLFWwindow* window = nullptr;
    do {
        window = initWindowEnvironment();
        if (window == nullptr) {
            break;
        }
        if (!initShaders()) {
            glfwTerminate();
            break;
        }
        if (!polyon_.initialize()) {
            glfwTerminate();
            break;
        }
        if (!text_.initialize()) {
            glfwTerminate();
            break;
        }
        if (!initGui(window)) {
            glfwTerminate();
            break;
        }
        init_reesult = true;
    } while (0);
    
    init_promise_->set_value(std::move(init_reesult));

    //int window_width, window_height;
    //glfwGetWindowSize(window, &window_width, &window_height);
    //dashboard_->initialize(window_width, window_height);  //set resolution

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window) && running()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();

        // input
        processInput(window);
        glfwPollEvents();

        //render
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.5f, 0.5f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderVideoFrame(window);

        renderTrackingBox(window);
        polyon_.render();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        readerTextInfo(window);

        renderGui(window);

        glfwSwapBuffers(window);

        double elapsedTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - currentTime).count();
        double sleepTime = (1.0f / RENDER_FPS) - elapsedTime;
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
        lastTime = std::chrono::high_resolution_clock::now();
    }
    glfwTerminate();
    warnf("render thread exit\n");
}

}