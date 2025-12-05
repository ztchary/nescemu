#include <SDL2/SDL.h>
#include <stdio.h>

// Define screen dimensions
const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 240;

int main(int argc, char* argv[]) {
    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Create the Window
    SDL_Window* window = SDL_CreateWindow(
        "nesc",                  // Window Title
        SDL_WINDOWPOS_CENTERED,  // X Position
        SDL_WINDOWPOS_CENTERED,  // Y Position
        SCREEN_WIDTH,            // Width
        SCREEN_HEIGHT,           // Height
        SDL_WINDOW_SHOWN         // Flags
    );

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. Get the window surface (so we can see something, usually black by default)
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    
    // Fill the surface white (optional, just to show the window is working)
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));
    SDL_UpdateWindowSurface(window);

    // 4. Main Event Loop
    SDL_Event e;
    int quit = 0;

    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit (clicks X on window)
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    // 5. Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
