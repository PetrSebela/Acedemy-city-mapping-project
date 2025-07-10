#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>

#include "../vendored/imgui/imgui.h"
#include "../vendored/imgui/backends/imgui_impl_sdl2.h"
#include "../vendored/imgui/backends/imgui_impl_opengl3.h"

#include "utils.hpp"
#include "renderer.hpp"
#include "importer.hpp"

#include "scene.hpp"

void sdl_setup()
{
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot initialize SDL");
        exit(1);
    }         

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void imgui_setup(SDL_Window *window, SDL_GLContext context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
}

SDL_Window * setup_window()
{
    SDL_Window *window;
    SDL_GLContext context;

    sdl_setup();
 
    window = SDL_CreateWindow(
        "Academy city mapping project",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1920,                               
        1080,                               
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE  
    );

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        exit(1);
    }

    context = SDL_GL_CreateContext(window);
    imgui_setup(window, context);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		exit(1);
	}

    SDL_GL_SetSwapInterval(0);  // Disable V-sync

    return window;
} 

int main(int argc, char* argv[]) 
{
    SDL_Window *window = setup_window();

    Renderer renderer;

    Entity dragon;
    dragon.tmp_model = Importer::LoadModel("../data/80k_smooth.obj");

    float rotation = 0;
    auto past_time = std::chrono::high_resolution_clock::now();
    bool running = true;
    bool rotate = true;
    bool use_wireframe = false;

    glm::vec3 camera_velocity(0);
    glm::vec3 camera_angle(0);

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if(event.type == SDL_WINDOWEVENT)
            {
                // printf("win\n");
                if (event.window.type == SDL_WINDOWEVENT_RESIZED)
                {
                    int width, height;
                    SDL_GetWindowSize(window, &width, &height);
                    glViewport(0, 0, width, height);
                    printf("%d %d\n", width, height);
                }
            }

            if(event.type == SDL_KEYDOWN && event.key.repeat == 0)
            {
                SDL_Keycode key = event.key.keysym.sym; 
                if(key == SDLK_w)
                {
                    camera_velocity.z += 1;
                }
                
                if(key == SDLK_s)
                {
                    camera_velocity.z -= 1;
                }
                
                if(key == SDLK_a)
                {
                    camera_velocity.x += 1;
                }
                
                if(key == SDLK_d)
                {
                    camera_velocity.x -= 1;
                }
                
                if(key == SDLK_LSHIFT)
                {
                    camera_velocity.y += 1;
                }
                
                if(key == SDLK_SPACE)
                {
                    camera_velocity.y -= 1;
                }
            }

            if(event.type == SDL_KEYUP && event.key.repeat == 0)
            {
                SDL_Keycode key = event.key.keysym.sym; 
                if(key == SDLK_w)
                {
                    camera_velocity.z -= 1;
                }
                
                if(key == SDLK_s)
                {
                    camera_velocity.z += 1;
                }
                
                if(key == SDLK_a)
                {
                    camera_velocity.x -= 1;
                }
                
                if(key == SDLK_d)
                {
                    camera_velocity.x += 1;
                }
                
                if(key == SDLK_LSHIFT)
                {
                    camera_velocity.y -= 1;
                }

                if(key == SDLK_SPACE)
                {
                    camera_velocity.y += 1;
                }
            }

            if(event.type == SDL_MOUSEMOTION)
            {
                camera_angle.x -= event.motion.yrel * 0.001;
                camera_angle.y -= event.motion.xrel * 0.001;
                // printf("%d %d \n", event.motion.xrel, event.motion.yrel);
            }

            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        
        // Calculate past frame time
        auto current_time = std::chrono::high_resolution_clock::now();
        auto frame_delta = std::chrono::duration<double>(current_time - past_time).count();
        past_time = current_time; 
        
        renderer.UpdateCamera(camera_velocity * (float)frame_delta * 0.5f, camera_angle);
        renderer.Clear();
        renderer.Render(dragon);

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Performance
        ImGui::Begin("Performance", &running, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::Text("Frame time: %lf ms", frame_delta * 1000);
        ImGui::Text("FPS: %lf", 1 / frame_delta);
        ImGui::End();

        ImGui::Begin("Debug", &running, ImGuiWindowFlags_NoTitleBar);

        // Set FOV
        ImGui::SliderFloat("FOV:", &renderer.fov, 10, 150);

        ImGui::SliderFloat("rotation:", &rotation, 0, 360);

        ImGui::Checkbox("Autorotate", &rotate);

        if(ImGui::Checkbox("Wireframe", &use_wireframe))
        {
            renderer.UseWireframe(use_wireframe);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    // Graceful finish
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
