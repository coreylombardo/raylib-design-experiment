#define CLAY_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include "../build/external/clay/clay.h"
#include "../build/external/clay/renderers/raylib/clay_renderer_raylib.c"
#include "../build/external/raylib-master/src/raylib.h"

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Clay error: %s\n", errorData.errorText.chars);
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 820;

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena) {
        .memory = malloc(clayRequiredMemory),
        .capacity = clayRequiredMemory
    };
    Clay_Initialize(
        clayMemory, 
        (Clay_Dimensions){
            screenWidth, screenHeight
        }, 
        (Clay_ErrorHandler){
            HandleClayErrors
        }
    );
    Clay_Raylib_Initialize(screenWidth, screenHeight, "raylib-design-experiment", FLAG_WINDOW_RESIZABLE);

    Font fonts[1];
    fonts[0] = GetFontDefault();
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
    
    while(!WindowShouldClose()) {
        // clay: create layout of the page
        Clay_SetLayoutDimensions(
            (Clay_Dimensions) {
                (float)GetScreenWidth(),
                (float)GetScreenHeight()
            }
        );

        Clay_BeginLayout();

            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_GROW(0)
                    },
                    .padding = CLAY_PADDING_ALL(16)
                },
                .backgroundColor = { 30, 30, 30, 255 }
            }) {
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(200),
                            .height = CLAY_SIZING_GROW(0)
                        }
                    },
                    .backgroundColor = { 255, 0, 0, 255 }
                }) {}
            }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout(GetFrameTime());

        // raylib: draw all the stuff above
        BeginDrawing();
            ClearBackground(BLACK);
            Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }

    Clay_Raylib_Close();
    return 0;
}