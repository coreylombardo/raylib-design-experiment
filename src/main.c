#include <stdio.h>
#include <stdlib.h>
#include "resource_dir.h"

#include "../build/external/raylib-master/src/raylib.h"
#include "../build/external/raylib-master/src/raymath.h"

#define CLAY_IMPLEMENTATION
#include "../build/external/clay/clay.h"
#include "../build/external/clay/renderers/raylib/clay_renderer_raylib.c"

// custom macros
#define SPACER_HORIZONTAL CLAY_AUTO_ID({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
#define VW(n) ((float)GetScreenWidth() * (n) / 100)
#define INTER_REGULAR   0
#define INTER_MEDIUM    1
#define INTER_SEMIBOLD  2
#define INTER_BOLD      3
#define JB_MONO         4

typedef struct {
    Clay_Color color;
} SparkleData;

static SparkleData auroraStar = { .color = { 196, 181, 253, 255 } };

static const float SPARKLE_PTS[8][2] = {
    {12, 2}, {9.8f, 8.8f}, {3, 11}, {9.8f, 13.2f},
    {12, 20}, {14.2f, 13.2f}, {21, 11}, {14.2f, 8.8f}
};

static void DrawGrid2D(int w, int h, int spacing, Color color) {
    for(int x = 0; x <= w; x += spacing) DrawLine(x, 0, x, h, color);
    for(int y = 0; y <= h; y += spacing) DrawLine(0, y, w, y, color);
}

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Clay error: %s\n", errorData.errorText.chars);
}

static void RenderCustomCommands(Clay_RenderCommandArray commands) {
    for (int32_t i = 0; i < commands.length; i++) {
        Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(&commands, i);
        if (cmd->commandType != CLAY_RENDER_COMMAND_TYPE_CUSTOM) continue;

        SparkleData *star = (SparkleData *)cmd->renderData.custom.customData;
        Clay_BoundingBox bb = cmd->boundingBox;
        float sx = bb.width / 24.0f;
        float sy = bb.height / 24.0f;

        Vector2 pts[10];
        pts[0] = (Vector2){ bb.x + 12 * sx, bb.y + 11 * sy };
        for (int j = 0; j < 8; j++) {
            pts[j + 1] = (Vector2){ bb.x + SPARKLE_PTS[j][0] * sx, bb.y + SPARKLE_PTS[j][1] * sy };
        }
        pts[9] = pts[1];

        Color c = { star->color.r, star->color.g, star->color.b, star->color.a };
        DrawTriangleFan(pts, 10, c);
    }
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
    SearchAndSetResourceDir("resources");

    Font fonts[5];
    fonts[INTER_REGULAR]    = LoadFontEx("Inter-Regular.ttf", 144, NULL, 0);
    fonts[INTER_MEDIUM]     = LoadFontEx("Inter-Medium.ttf", 144, NULL, 0);
    fonts[INTER_SEMIBOLD]   = LoadFontEx("Inter-SemiBold.ttf", 144, NULL, 0);
    fonts[INTER_BOLD]       = LoadFontEx("Inter-Bold.ttf", 144, NULL, 0);
    fonts[JB_MONO]          = LoadFontEx("JetBrainsMono-Medium.ttf", 144, NULL, 0);
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
                    .padding = CLAY_PADDING_ALL(20)
                },
            }) {

                CLAY(CLAY_ID("Top Bar"), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                        .sizing = {
                            .width = CLAY_SIZING_GROW(0, 1280),
                            .height = CLAY_SIZING_FIXED(30)
                        },
                        .padding = CLAY_PADDING_ALL(5)
                    },
                    .backgroundColor = { 50, 50, 50, 255 }
                }) {
                    // logo goes here
                    CLAY_TEXT(CLAY_STRING("Aurora"), CLAY_TEXT_CONFIG({
                        .fontId = 0, .fontSize = 12, .letterSpacing = 2,
                        .textColor = { 255, 255, 255, 255 }
                    }));

                    SPACER_HORIZONTAL;

                    // right side: page indicator
                    CLAY_TEXT(CLAY_STRING("test shit"), CLAY_TEXT_CONFIG({
                        .fontId = 0, .fontSize = 14, .letterSpacing = 2,
                        .textColor = { 255, 255, 255, 255 }
                    }));
                }

                CLAY(CLAY_ID("Hero Block"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childAlignment = { CLAY_ALIGN_X_CENTER },
                        .childGap = 20,
                        .sizing = {
                            .width = CLAY_SIZING_FIXED((float)GetScreenWidth() / 2),
                            .height = CLAY_SIZING_FIXED((float)GetScreenHeight() / 2)
                        }
                    }
                }) {
                    CLAY(CLAY_ID("Aurora AI Badge"), {
                        .layout = { 
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }, 
                            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIXED(30) },
                            .padding = { 16, 16, 0, 0 },
                            .childGap = 8
                        },
                        .backgroundColor = { 255, 255, 255, 15.3f },
                        .cornerRadius = CLAY_CORNER_RADIUS(999)
                    }) {
                        CLAY(CLAY_ID("Aurora Sparkle"), {
                            .layout = { .sizing = { CLAY_SIZING_FIXED(16), CLAY_SIZING_FIXED(16) } },
                            .custom = { .customData = &auroraStar }
                        }) {}

                        CLAY_TEXT(CLAY_STRING("Aurora AI is now in your workspace"), CLAY_TEXT_CONFIG({
                            .fontId = INTER_REGULAR, .fontSize = 13, .letterSpacing = 2,
                            .textColor = { 214, 214, 230, 255 }
                        }));
                    }

                    CLAY(CLAY_ID("Headline"), {
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childAlignment = { CLAY_ALIGN_X_CENTER },
                            .childGap = 0
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Think it. Write it."), CLAY_TEXT_CONFIG({
                            .fontId = INTER_BOLD, 
                            .fontSize = (uint16_t)Clamp(VW(7), 42, 76),
                            .letterSpacing = 0,
                            .textColor = { 255, 255, 255, 255 }
                        }));

                        CLAY_TEXT(CLAY_STRING("Ship it."), CLAY_TEXT_CONFIG({
                            .fontId = INTER_BOLD, 
                            .fontSize = (uint16_t)Clamp(VW(7), 42, 76), 
                            .letterSpacing = 1,
                            .textColor = { 255, 255, 255, 255 }
                        }));
                    }

                    CLAY(CLAY_ID("Subtitle"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0, 520) }
                        }
                    }){
                        CLAY_TEXT(CLAY_STRING(
                            "Docs, tasks and wikis in one blazing-fast canvas. Let's set up your workspace in under a minute."
                        ), CLAY_TEXT_CONFIG({
                            .fontId = INTER_REGULAR, .fontSize = 18,
                            .textColor = { 168, 168, 189, 255 },
                            .textAlignment = CLAY_TEXT_ALIGN_CENTER
                        }));
                    } {}
                }
                
                CLAY(CLAY_ID("Workspace Prompt"), {
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(560),
                            .height = CLAY_SIZING_FIXED(300)
                        }
                    },
                    .backgroundColor = { 255, 255, 255, 12.75f },
                    .cornerRadius = CLAY_CORNER_RADIUS(25)
                }) {}
            }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout(GetFrameTime());

        // 
        //      raylib: draw all the stuff above
        //

        BeginDrawing();
            ClearBackground((Color){ 7, 7, 12, 255 });
            DrawGrid2D(GetScreenWidth(), GetScreenHeight(), 56, ColorAlpha((Color){ 255, 255, 255, 255 }, 0.05f));
            Clay_Raylib_Render(renderCommands, fonts);
            RenderCustomCommands(renderCommands);
        EndDrawing();
    }

    for(int i = 0; i < (int)sizeof(fonts) / sizeof(fonts[0]); i++) {
        UnloadFont(fonts[i]);
    }
    Clay_Raylib_Close();
    return 0;
}