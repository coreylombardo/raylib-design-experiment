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

static void DrawGrid2D(int w, int h, int spacing, Color color) {
    for(int x = 0; x <= w; x += spacing) DrawLine(x, 0, x, h, color);
    for(int y = 0; y <= h; y += spacing) DrawLine(0, y, w, y, color);
}

typedef enum {
    SHAPE_LOGO,
    SHAPE_SPARKLE
} ShapeType;
typedef struct { ShapeType type; Clay_Color fill, stroke; } LogoData;
typedef struct { ShapeType type; Clay_Color color; } SparkleData;

static LogoData svgLogo = { 
    .type = SHAPE_LOGO,
    .fill = { 124, 92, 255, 255 }, 
    .stroke = { 255, 255, 255, 255 } 
};
static SparkleData svgSparkle = { 
    .type = SHAPE_SPARKLE,
    .color = { 196, 181, 253, 255 } 
};

static const Vector2 LOGO_PTS[4] = {
    {8, 16},    {8, 8},     {16, 16},   {16, 8}
};
static const Vector2 SPARKLE_PTS[8] = {
    {12, 2},    {14.2f, 8.8f},  {21, 11},   {14.2f, 13.2f},
    {12, 20},   {9.8f, 13.2f},  {3, 11},    {9.8f, 8.8f}
}; static const Vector2 SPARKLE_CENTER = {12, 11};

static Vector2 ToScreen(Rectangle bb, Vector2 pt, float sx, float sy) {
    return (Vector2){ bb.x + pt.x * sx, bb.y + pt.y * sy };
}

static Color ToRaylib(Clay_Color c) {
    return (Color){ (unsigned char)c.r, (unsigned char)c.g, 
                    (unsigned char)c.b, (unsigned char)c.a, };
}

void DrawLogo(Vector2 pos, float size, Color fill, Color stroke) {
    float s = size / 24;
    Rectangle bb = { pos.x, pos.y, size, size };

    Vector2 corner = ToScreen(bb, (Vector2){ 2, 2 }, s, s);
    DrawRectangleRounded(
        (Rectangle){ corner.x, corner.y, 20 * s, 20 * s}, 
        0.6f, 
        16, 
        fill
    );

    Vector2 path[4];
    for(int i = 0; i < 4; i++) path[i] = ToScreen(bb, LOGO_PTS[i], s, s);

    for(int i = 0; i < 3; i++) DrawLineEx(path[i], path[i + 1], 2 * s, stroke);
    for(int i = 0; i < 4; i++) DrawCircleV(path[i], 1 * s, stroke);
}

void DrawSparkle(Rectangle bb, float sx, float sy, Color c) {
    // we can do a fan because the sparkle has rotational symmetry
    Vector2 fan[10];
    fan[0] = ToScreen(bb, SPARKLE_CENTER, sx, sy);
    for(int i = 0; i < 8; i++) fan[i + 1] = ToScreen(bb, SPARKLE_PTS[7 - i], sx, sy);
    fan[9] = fan[1];

    DrawTriangleFan(fan, 10, c);
}

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Clay error: %s\n", errorData.errorText.chars);
}

// most confusing fucking block in the whole program
static void RenderCustomCommands(Clay_RenderCommandArray commands) {
    for (int32_t i = 0; i < commands.length; i++) {
        Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(&commands, i);
        if (cmd->commandType != CLAY_RENDER_COMMAND_TYPE_CUSTOM) continue;

        void *data = cmd->renderData.custom.customData;
        if (!data) continue;

        Clay_BoundingBox box = cmd->boundingBox;
        Rectangle bb = { box.x, box.y, box.width, box.height };

        switch(*(ShapeType *)data) {
            case SHAPE_LOGO: {
                LogoData *logo = (LogoData *)data;
                DrawLogo((Vector2){ bb.x, bb.y }, bb.width,
                    ToRaylib(logo->fill), ToRaylib(logo->stroke));
                break;
            }
            case SHAPE_SPARKLE: {
                SparkleData *sparkle = (SparkleData *)data;
                DrawSparkle(bb, bb.width / 24, bb.height / 24,
                    ToRaylib(sparkle->color));
                break;
            }
        }
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
    Clay_Raylib_Initialize(screenWidth, screenHeight, "raylib-design-experiment", FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    SearchAndSetResourceDir("resources");

    Font fonts[5];
    fonts[INTER_REGULAR]    = LoadFontEx("Inter-Regular.ttf", 144, NULL, 0);
    fonts[INTER_MEDIUM]     = LoadFontEx("Inter-Medium.ttf", 144, NULL, 0);
    fonts[INTER_SEMIBOLD]   = LoadFontEx("Inter-SemiBold.ttf", 144, NULL, 0);
    fonts[INTER_BOLD]       = LoadFontEx("Inter-Bold.ttf", 144, NULL, 0);
    fonts[JB_MONO]          = LoadFontEx("JetBrainsMono-Medium.ttf", 144, NULL, 0);
    for (int i = 0; i < 5; i++) {
        GenTextureMipmaps(&fonts[i].texture);
        SetTextureFilter(fonts[i].texture, TEXTURE_FILTER_TRILINEAR);
    }
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
                        .childGap = 5,
                        .sizing = {
                            .width = CLAY_SIZING_GROW(0, 1280),
                            .height = CLAY_SIZING_FIXED(30)
                        },
                        .padding = CLAY_PADDING_ALL(5)
                    },
                    //.backgroundColor = { 50, 50, 50, 255 }
                }) {
                    CLAY(CLAY_ID("Logo"), {
                        .layout = { .sizing = { CLAY_SIZING_FIXED(16), CLAY_SIZING_FIXED(16) } },
                        .custom = { .customData = &svgLogo }
                    }) {}
                    CLAY_TEXT(CLAY_STRING("Aurora"), CLAY_TEXT_CONFIG({
                        .fontId = INTER_SEMIBOLD, .fontSize = 17,
                        .textColor = { 255, 255, 255, 255 }
                    }));

                    SPACER_HORIZONTAL;

                    // right side: page indicator
                    CLAY_TEXT(CLAY_STRING("page indicator placeholder"), CLAY_TEXT_CONFIG({
                        .fontId = 0, .fontSize = 14,
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
                        .cornerRadius = CLAY_CORNER_RADIUS(15),
                        .border = {
                            .color = { 255, 255, 255, 35.7f },
                            .width = CLAY_BORDER_OUTSIDE(1)
                        }
                    }) {
                        CLAY(CLAY_ID("Sparkle"), {
                            .layout = { .sizing = { CLAY_SIZING_FIXED(16), CLAY_SIZING_FIXED(16) } },
                            .custom = { .customData = &svgSparkle }
                        }) {}

                        CLAY_TEXT(CLAY_STRING("Aurora AI is now in your workspace"), CLAY_TEXT_CONFIG({
                            .fontId = INTER_REGULAR, .fontSize = 13,
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
                            .textColor = { 255, 255, 255, 255 }
                        }));

                        CLAY_TEXT(CLAY_STRING("Ship it."), CLAY_TEXT_CONFIG({
                            .fontId = INTER_BOLD, 
                            .fontSize = (uint16_t)Clamp(VW(7), 42, 76), 
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
                    .cornerRadius = CLAY_CORNER_RADIUS(25),
                    .border = {
                        .color = { 255, 255, 255, 30.6f },
                        .width = { 1, 1, 2, 1 }
                    }
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