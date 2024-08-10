#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>

#define CELLSIZE 10

static EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
CONST EFI_GRAPHICS_OUTPUT_BLT_PIXEL black = {0x00, 0x00, 0x00, 0x00};
CONST EFI_GRAPHICS_OUTPUT_BLT_PIXEL white = {0xFF, 0xFF, 0xFF, 0x00};

typedef struct {
    UINT32 width;
    UINT32 height;
} GameWindow;

typedef struct {
    UINT32 x;
    UINT32 y;
    UINT32 speedX;
    UINT32 speedY;
} ball_t;

typedef struct {
    UINT32 x;
    UINT32 y;
    const UINT32 size;
    const UINT32 speed;
} bat_t;

VOID drawCell(UINT32 x, UINT32 y, BOOLEAN erase) {
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = white;
    if(erase) {
        color = black;
    }
    gop->Blt(gop, &color, EfiBltVideoFill,
             0, 0,
             x * CELLSIZE, y * CELLSIZE,
             CELLSIZE, CELLSIZE,
             0);
}

VOID drawBall(ball_t* ball, BOOLEAN erase) {
    drawCell(ball->x, ball->y, erase);
}

VOID drawBat(bat_t* bat, BOOLEAN erase) {
    for(UINT32 i = 0; i < bat->size; i++) {
        drawCell(bat->x, bat->y + i, erase);
    }
}


VOID initGame(GameWindow gameWindow, ball_t* ball, bat_t* leftBat, bat_t* rightBat) {
    ball->x = gameWindow.width / 2;
    ball->y = gameWindow.height / 2;
    ball->speedX = -1;
    ball->speedY = 1;

    leftBat->x = 0;
    rightBat->x = gameWindow.width - 1;
    leftBat->y = rightBat->y = (gameWindow.height - leftBat->size) / 2;

    drawBat(leftBat, FALSE);
    drawBat(rightBat, FALSE);
    drawBall(ball, FALSE);
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE imageHandle, IN EFI_SYSTEM_TABLE *systemTable) {
    // Initialize the screen
    EFI_STATUS status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,
                                            NULL,
                                            (VOID**) &gop);
    if(EFI_ERROR(status)) {
        Print(L"Unable to locate GOP\n");
        Exit(1);
    }

    // Set mode and clear screen to black
    UINT32 mode = 0;
    status = gop->SetMode(gop, mode);
    if(EFI_ERROR(status)) {
        Print(L"Unable to set mode %03d\r\n", mode);
    }

    // Define the game window
    UINT32 screenWidth = gop->Mode->Info->HorizontalResolution;
    UINT32 screenHeight = gop->Mode->Info->VerticalResolution;
    GameWindow gameWindow = {screenWidth / CELLSIZE, screenHeight / CELLSIZE};

    // Initialize the game
    ball_t ball = {};
    bat_t leftBat = {.size = 10, .speed = 4};
    bat_t rightBat = {.size = 10, .speed = 4};
    initGame(gameWindow, &ball, &leftBat, &rightBat);


    gBS->Stall(100000000);
    // Clear screen to black before returing
    gop->SetMode(gop, mode);
    return EFI_SUCCESS;
}