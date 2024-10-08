#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include "include/Numbers.h"

#define CELLSIZE 10

static EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
CONST EFI_GRAPHICS_OUTPUT_BLT_PIXEL black = {0x00, 0x00, 0x00, 0x00};
CONST EFI_GRAPHICS_OUTPUT_BLT_PIXEL white = {0xFF, 0xFF, 0xFF, 0x00};

typedef struct {
    UINT32 width;
    UINT32 height;
} GameWindow;
GameWindow gameWindow;

typedef struct {
    INT32 x;
    INT32 y;
    INT32 speedX;
    INT32 speedY;
} ball_t;

typedef struct {
    INT32 x;
    INT32 y;
    INT32 speed;
    UINT8 score;
    const UINT32 size;
} bat_t;

VOID drawCell(UINT32 x, UINT32 y, BOOLEAN draw) {
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = white;
    if(!draw) {
        color = black;
    }
    gop->Blt(gop, &color, EfiBltVideoFill,
             0, 0,
             x * CELLSIZE, y * CELLSIZE,
             CELLSIZE, CELLSIZE,
             0);
}

VOID drawBall(ball_t* ball, BOOLEAN draw) {
    drawCell(ball->x, ball->y, draw);
}

VOID drawBat(bat_t* bat, BOOLEAN draw) {
    for(UINT32 i = 0; i < bat->size; i++) {
        drawCell(bat->x, bat->y + i, draw);
    }
}

VOID drawMiddleLine() {
    UINT32 centre = gameWindow.width / 2;
    for(INT32 i = 0; i <= gameWindow.height; i++) {
        if(i % 4 == 0) {
            drawCell(centre, i, TRUE);
            drawCell(centre, i + 1, TRUE);
        }
    }
}

VOID DrawOneNumber(UINT8 score, UINT32 x) {
    for(INT32 i = 0; i < NUMBERHEIGHT; i++) {
        for(INT32 j = 0; j < NUMBERWIDTH; j++) {
            drawCell(x + j, i + 1, NUMBERS[score][i][j]);
        }
    }
}

VOID drawScore(UINT8 leftScore, UINT8 rightScore) {
    UINT32 centre = gameWindow.width / 2;
    DrawOneNumber(leftScore, centre - 10 - NUMBERWIDTH);
    DrawOneNumber(rightScore, centre + 10 + NUMBERWIDTH);
}

VOID initGame(ball_t* ball, bat_t* leftBat, bat_t* rightBat) {
    ball->x = gameWindow.width / 2;
    ball->y = gameWindow.height / 2;
    ball->speedX = -1;
    ball->speedY = 1;

    leftBat->x = 0;
    rightBat->x = gameWindow.width - 1;
    leftBat->y = rightBat->y = (gameWindow.height - leftBat->size) / 2;

    drawBat(leftBat, TRUE);
    drawBat(rightBat, TRUE);
    drawBall(ball, TRUE);
    drawScore(leftBat->score, rightBat->score);
    drawMiddleLine();
}

VOID moveBat(bat_t* bat, BOOLEAN up) {
    drawBat(bat, FALSE);
    INT8 dir = up ? -1 : 1;
    bat->y += dir * bat->speed;
    if(bat->y < bat->speed) {
        bat->y = 0;
    } else if(bat->y > gameWindow.height - bat->size) {
        bat->y = gameWindow.height - bat->size;
    }
    drawBat(bat, TRUE);
}

VOID updateBats(EFI_INPUT_KEY* keyPress, bat_t* leftBat, bat_t* rightBat) {
    if(keyPress->UnicodeChar == 'w') {
        moveBat(leftBat, TRUE);
    } else if(keyPress->UnicodeChar == 's') {
        moveBat(leftBat, FALSE);
    }

    if(keyPress->ScanCode == SCAN_UP) {
        moveBat(rightBat, TRUE);
    } else if(keyPress->ScanCode == SCAN_DOWN) {
        moveBat(rightBat, FALSE);
    }
}

BOOLEAN BatBlockBall(bat_t* bat, ball_t* ball) {
  if(ABS(bat->x - ball->x) != 1) {
    return FALSE;
  }
  return (bat->y <= ball->y) && ((bat->y + bat->size) >= ball->y);
}

BOOLEAN updateBall(ball_t* ball, bat_t* leftBat, bat_t* rightBat) {
    drawBall(ball, FALSE);
    BOOLEAN goal = FALSE;
    if(ball->x == 0) {
        goal = TRUE;
        rightBat->score++;
    } else if(ball->x == gameWindow.width) {
        goal = TRUE;
        leftBat->score++;
    }

    // End game
    if(leftBat->score > 9 || rightBat->score > 9) {
        return TRUE;
    }

    // Reset ball
    if(goal) {
        ball->x = gameWindow.width / 2;
        ball->y = gameWindow.height / 2;
        ball->speedX *= -1;
    }


    // Bounce off walls
    if(ball->y == 0 || ball->y >= gameWindow.height) {
        ball->speedY *= -1;
    }

    // Bounce off bats
    if(BatBlockBall(leftBat, ball) || BatBlockBall(rightBat, ball)) {
        ball->speedX *= -1;
    }

    // Update ball's position
    ball->x += ball->speedX;
    ball->y += ball->speedY;

    drawBall(ball, TRUE);
    drawScore(leftBat->score, rightBat->score);
    drawMiddleLine();
    return FALSE;
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
    gameWindow.width = screenWidth / CELLSIZE;
    gameWindow.height = screenHeight / CELLSIZE;

    // Initialize the game
    ball_t ball = {};
    bat_t leftBat = {.size = 10, .speed = 4};
    bat_t rightBat = {.size = 10, .speed = 4};
    initGame(&ball, &leftBat, &rightBat);

    // Update loop
    BOOLEAN stop = FALSE;
    while (!stop) {
        EFI_INPUT_KEY keyPress;
        EFI_STATUS poll = gST->ConIn->ReadKeyStroke(gST->ConIn, &keyPress);
        stop = updateBall(&ball, &leftBat, &rightBat);
        if(poll == EFI_SUCCESS) {
            updateBats(&keyPress, &leftBat, &rightBat);
            // Exit game
            if(keyPress.UnicodeChar == 'q') {
                stop = TRUE;
            }
        }
        gBS->Stall(50000);
    }

    // Clear screen to black before returing
    gop->SetMode(gop, mode);
    return EFI_SUCCESS;
}