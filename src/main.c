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

            CLAY(CLAY_ID("Main Container"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childAlignment = { CLAY_ALIGN_X_CENTER },
                    .sizing = {
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_GROW(0)
                    },
                    .padding = CLAY_PADDING_ALL(16)
                },
                .backgroundColor = { 7, 7, 12, 255 }
            }) {
                CLAY(CLAY_ID("Headline"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childAlignment = { CLAY_ALIGN_X_CENTER },
                        .sizing = {
                            .width = (float)GetScreenWidth() / 2,
                            .height = (float)GetScreenHeight() / 2
                        }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Think it. Write it."), CLAY_TEXT_CONFIG({
                        .fontId = 0,
                        .fontSize = 48,
                        .letterSpacing = 2,
                        .textColor = { 255, 255, 255, 255 }
                    }));
                    CLAY_TEXT(CLAY_STRING("Ship it."), CLAY_TEXT_CONFIG({
                        .fontId = 0,
                        .fontSize = 48,
                        .letterSpacing = 2,
                        .textColor = { 255, 255, 255, 255 }
                    }));
                }
                
                CLAY(CLAY_ID("Workspace Prompt"), {
                    .layout = {
                        .sizing = {
                            .width = 250,
                            .height = 250
                        }
                    },
                    .backgroundColor = { 255, 255, 255, 5 }
                });
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