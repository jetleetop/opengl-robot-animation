#define STB_IMAGE_IMPLEMENTATION
#include <stdlib.h>
#include <glut.h>  // Include the GLUT heade
#include <stdio.h>
#include <math.h>
#include "stb_image.h" // stb_image 라이브러리 헤더 파일

GLuint headTextures[6], torsoTextures[6], armTextures[6], handTextures[6], legTextures[6], footTextures[6], block1Textures[6], block2Textures[6];

////// Global Variables
float angle = 0.0f;  // current angle of the rotating triangle
bool fullScreen = false;

static float zPos = 12.0f;
static float yPos = 0.0f;
static float rotationAngle = 0.0f;
static bool moving = true;
static bool jmoving = true;

static bool leg1 = true;  // robot's leg states
static bool leg2 = false;  // true = forward, false = back

static bool arm1 = false;
static bool arm2 = true;

////// Robot variables
static float legAngle[2] = { 0.0f, 0.0f };
static float armAngle[2] = { 0.0f, 0.0f };

static int ptri = 0;
int btri = 0;
int blocktri = 0;
int blocktri2 = 0;
int dencetri = 0;
int jtri = 0;
GLuint LoadGLTexture(const char* szFilePath) {
    int width, height, channels;
    unsigned char* image = stbi_load(szFilePath, &width, &height, &channels, STBI_rgb_alpha);

    if (image) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(image); // 이미지 데이터 메모리 해제
        return textureID;
    }
    else {
        fprintf(stderr, "Failed to load texture: %s\n", szFilePath);
        return 0;
    }
}

void LoadTextures() {
    headTextures[0] = LoadGLTexture("asset\head_texture0.bmp");
    headTextures[1] = LoadGLTexture("asset\head_texture1.bmp");
    headTextures[2] = LoadGLTexture("asset\head_texture2.bmp");
    headTextures[3] = LoadGLTexture("asset\head_texture3.bmp");
    headTextures[4] = LoadGLTexture("asset\head_texture4.bmp");
    headTextures[5] = LoadGLTexture("asset\head_texture0.bmp");

    torsoTextures[0] = LoadGLTexture("asset\torso_texture0.bmp");
    torsoTextures[1] = LoadGLTexture("asset\torso_texture1.bmp");
    torsoTextures[2] = LoadGLTexture("asset\torso_texture2.bmp");
    torsoTextures[3] = LoadGLTexture("asset\torso_texture3.bmp");
    torsoTextures[4] = LoadGLTexture("asset\torso_texture2.bmp");
    torsoTextures[5] = LoadGLTexture("asset\torso_texture0.bmp");

    armTextures[0] = LoadGLTexture("asset\arm_texture0.bmp");
    armTextures[1] = LoadGLTexture("asset\arm_texture3.bmp");
    armTextures[2] = LoadGLTexture("asset\arm_texture2.bmp");
    armTextures[3] = LoadGLTexture("asset\arm_texture3.bmp");
    armTextures[4] = LoadGLTexture("asset\arm_texture4.bmp");
    armTextures[5] = LoadGLTexture("asset\arm_texture0.bmp");

    handTextures[0] = LoadGLTexture("asset\hand_texture.bmp");
    handTextures[1] = LoadGLTexture("asset\hand_texture.bmp");
    handTextures[2] = LoadGLTexture("asset\hand_texture.bmp");
    handTextures[3] = LoadGLTexture("asset\hand_texture.bmp");
    handTextures[4] = LoadGLTexture("asset\hand_texture.bmp");
    handTextures[5] = LoadGLTexture("asset\hand_texture5.bmp");

    legTextures[0] = LoadGLTexture("asset\leg_texture0.bmp");
    legTextures[1] = LoadGLTexture("asset\leg_texture3.bmp");
    legTextures[2] = LoadGLTexture("asset\leg_texture4.bmp");
    legTextures[3] = LoadGLTexture("asset\leg_texture3.bmp");
    legTextures[4] = LoadGLTexture("asset\leg_texture4.bmp");
    legTextures[5] = LoadGLTexture("asset\leg_texture0.bmp");

    footTextures[0] = LoadGLTexture("asset\foot_texture.bmp");
    footTextures[1] = LoadGLTexture("asset\foot_texture.bmp");
    footTextures[2] = LoadGLTexture("asset\foot_texture.bmp");
    footTextures[3] = LoadGLTexture("asset\foot_texture.bmp");
    footTextures[4] = LoadGLTexture("asset\foot_texture.bmp");
    footTextures[5] = LoadGLTexture("asset\foot_texture.bmp");

    block1Textures[0] = LoadGLTexture("asset\block1_texture0.bmp");
    block1Textures[1] = LoadGLTexture("asset\block1_texture1.bmp");
    block1Textures[2] = LoadGLTexture("asset\block1_texture1.bmp");
    block1Textures[3] = LoadGLTexture("asset\block1_texture1.bmp");
    block1Textures[4] = LoadGLTexture("asset\block1_texture1.bmp");
    block1Textures[5] = LoadGLTexture("asset\block2_texture0.bmp");

    block2Textures[0] = LoadGLTexture("asset\block2_texture0.bmp");
    block2Textures[1] = LoadGLTexture("asset\block2_texture0.bmp");
    block2Textures[2] = LoadGLTexture("asset\block2_texture0.bmp");
    block2Textures[3] = LoadGLTexture("asset\block2_texture0.bmp");
    block2Textures[4] = LoadGLTexture("asset\block2_texture0.bmp");
    block2Textures[5] = LoadGLTexture("asset\block2_texture0.bmp");
}

// DrawCube
void DrawCube(GLuint textures[6], float xPos, float yPos, float zPos) {
    glPushMatrix();
    glTranslatef(xPos, yPos, zPos);

    // 텍스처 좌표 생성 활성화
    glEnable(GL_TEXTURE_2D);

    // 위쪽 면
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);
    glEnd();

    // 앞 면
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();

    // 좌측 면
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_POLYGON);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);
    glEnd();

    // 뒷 면
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, -1.0f);
    glEnd();

    // 우측 면
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glBegin(GL_POLYGON);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, -1.0f);
    glEnd();

    // 아랫면
    glBindTexture(GL_TEXTURE_2D, textures[5]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();

    // 텍스처 좌표 생성 비활성화
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void DrawBlock1(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f); 
    glTranslatef(xPos, yPos, zPos);
    glScalef(3.0f, 3.0f, 3.0f);  
    DrawCube(block1Textures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

void DrawBlock2(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(xPos, yPos, zPos);
    glScalef(4.0f, 4.0f, 4.0f);  
    DrawCube(block2Textures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

void DrawBlock3(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(xPos, yPos, zPos);
    glScalef(0.4f, 0.4f, 0.4f);
    DrawCube(block2Textures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

// DrawArm
void DrawArm(float xPos, float yPos, float zPos) {

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);  // 흰색
    glTranslatef(xPos, yPos, zPos);
    glScalef(1.0f, 4.0f, 1.0f);  // arm is a 1x4x1 cube
    DrawCube(armTextures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

void DrawHand(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);  // 흰색
    glTranslatef(xPos, yPos, zPos);
    glScalef(1.0f, 1.0f, 1.0f);
    DrawCube(handTextures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

// DrawHead
void DrawHead(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);  // 흰색
    glTranslatef(xPos, yPos, zPos);
    glScalef(2.0f, 2.0f, 2.0f);  // 머리는 2x2x2 크기의 정육면체
    DrawCube(headTextures, 0.0f, 0.0f, 0.0f);  // 정육면체 그리기
    glPopMatrix();
}


// DrawTorso
void DrawTorso(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);  // 흰색
    glTranslatef(xPos, yPos, zPos);
    glScalef(3.0f, 5.0f, 2.0f);  // torso is a 3x5x2 cube
    DrawCube(torsoTextures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

// DrawLeg
void DrawLeg(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);  // 흰색
    glTranslatef(xPos, yPos, zPos);
    glScalef(1.0f, 5.0f, 1.0f);  // leg is a 1x5x1 cube
    DrawCube(legTextures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

void DrawFoot(float xPos, float yPos, float zPos) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);  // 흰색
    glTranslatef(xPos, yPos, zPos);
    glScalef(1.0f, 1.0f, 2.0f);
    DrawCube(footTextures, 0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

void move() {
    if (ptri == 0) {
        if (arm1)
            armAngle[0] = armAngle[0] + 0.25f;
        else
            armAngle[0] = armAngle[0] - 0.25f;

        if (armAngle[0] >= 15.0f)
            arm1 = false;
        if (armAngle[0] <= -15.0f)
            arm1 = true;

        if (arm2)
            armAngle[1] = armAngle[1] + 0.25f;
        else
            armAngle[1] = armAngle[1] - 0.25f;

        if (armAngle[1] >= 15.0f)
            arm2 = false;
        if (armAngle[1] <= -15.0f)
            arm2 = true;

        if (leg1)
            legAngle[0] = legAngle[0] + 0.25f;
        else
            legAngle[0] = legAngle[0] - 0.25f;

        if (legAngle[0] >= 15.0f)
            leg1 = false;
        if (legAngle[0] <= -15.0f)
            leg1 = true;

        if (leg2)
            legAngle[1] = legAngle[1] + 0.25f;
        else
            legAngle[1] = legAngle[1] - 0.25f;

        if (legAngle[1] >= 15.0f)
            leg2 = false;
        if (legAngle[1] <= -15.0f)
            leg2 = true;
    }
    else {
        if (arm1)
            armAngle[0] = armAngle[0] + 1.5f;
        else
            armAngle[0] = armAngle[0] - 1.5f;

        if (armAngle[0] >= 45.0f)
            arm1 = false;
        if (armAngle[0] <= -0.5f)
            arm1 = true;

        if (arm2) {
            armAngle[1] = 0;
            armAngle[1] = armAngle[1];
        }
        else {
            armAngle[1] = 0;
            armAngle[1] = armAngle[1];
        }

        if (armAngle[1] >= 15.0f)
            arm2 = false;
        if (armAngle[1] <= -15.0f)
            arm2 = true;

        if (leg1) {
            legAngle[0] = 0;
            legAngle[0] = legAngle[0];
        }
        else {

            legAngle[0] = legAngle[0] - 0.25f;
        }

        if (legAngle[0] >= 15.0f)
            leg1 = false;
        if (legAngle[0] <= -15.0f)
            leg1 = true;

        if (leg2) {
            legAngle[1] = 0;
            legAngle[1] = legAngle[1];
        }
        else {
            legAngle[1] = 0;
            legAngle[1] = legAngle[1];
        }

        if (legAngle[1] >= 15.0f)
            leg2 = false;
        if (legAngle[1] <= -15.0f)
            leg2 = true;
    }
}

void dencemove() {
    armAngle[0] = 45.0f;
    armAngle[1] = -45.0f;
    legAngle[0] = -10.0f;
    legAngle[1] = 10.0f;
    // 다리와 관련된 코드는 수정하지 않습니다
}

void jumpmove() {
    if (jtri == 1) {
        jmoving = true;
        if (jmoving) {
            yPos += 0.4f;
            if (yPos > 4.0f) {
                yPos = 4.0f;
            }
            jmoving = false;
        }
        else {
            yPos -= 0.4f;
            if (yPos <= 3.0f) {
                yPos = 3.0f;  // 3에 도달하면
            }
        }
    }
    else if (jtri == 2) {
        jmoving = true;
        if (jmoving) {
            yPos += 0.4f;
            if (yPos > 7.0f) {
                yPos = 7.0f;
            }
            jmoving = false;
        }
        else {
            yPos -= 0.4f;
            if (yPos <= 6.0f) {
                yPos = 6.0f;
            }
        }
    }
    else if (jtri == 3) {
        jmoving = true;
        if (jmoving) {
            yPos += 0.4f;
            if (yPos > 10.0f) {
                yPos = 10.0f;
            }
            jmoving = false;
        }
        else {
            yPos -= 0.4f;
            if (yPos <= 9.0f) {
                yPos = 9.0f;  // 3에 도달하면
            }
        }
    }
}

// DrawRobot
void DrawRobot(float xPos, float yPos, float zPos, float rotationAngle) {
    if (dencetri == 0) move();
    else dencemove();

    jumpmove();

    glPushMatrix();

    glTranslatef(xPos, yPos, zPos);  // draw robot at desired coordinates

    if (rotationAngle != 0.0f) {
        glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around y-axis
    }

    // draw components
    DrawHead(1.0f, 2.0f, 0.0f);
    DrawTorso(1.5f, 0.0f, 0.0f);

    glPushMatrix();
    

    glTranslatef(0.0f, -0.5f, 0.0f);
    glRotatef(armAngle[0], 1.0f, 0.0f, 0.0f);
    DrawArm(2.5f, 0.0f, -0.5f);
    DrawHand(2.5f, -4.0f, -0.5f);
    if (btri > 0) DrawBlock3(2.5f, -4.5f, -1.5f);
    glPopMatrix();

    glPushMatrix();
    

    glTranslatef(0.0f, -0.5f, 0.0f);
    glRotatef(armAngle[1], 1.0f, 0.0f, 0.0f);
    DrawArm(-1.5f, 0.0f, -0.5f);
    DrawHand(-1.5f, -4.0f, -0.5f);
    glPopMatrix();

    glPushMatrix();
    

    glTranslatef(0.0f, -0.5f, 0.0f);
    glRotatef(legAngle[0], 1.0f, 0.0f, 0.0f);
    DrawLeg(1.5f, -4.5f, 0.0f);
    DrawFoot(1.5f, -9.5f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    

    glTranslatef(0.0f, -0.5f, 0.0f);
    glRotatef(legAngle[1], 1.0f, 0.0f, 0.0f);
    DrawLeg(-0.5f, -4.5f, 0.0f);
    DrawFoot(-0.5f, -9.5f, 0.0f);
    glPopMatrix();

    glPopMatrix();
}
bool rotating = true; // 회전 중인지 상태를 나타내는 변수

void resetPtri(int value) {
    ptri = 0; // ptri를 0으로 재설정
    rotating = true; // Resume rotation
    blocktri += 1;
    btri += 1;
}

void newrest(int value) {
    ptri = 0; // ptri를 0으로 재설정
    rotating = true; // Resume rotation
    blocktri2 += 1;
    btri -= 1;
    jtri += 1;
}

void Myidle() {
    if (rotating) {
        if (moving) {
            zPos -= 0.1f; // Decrement z position for movement
            if (zPos < -8.0f) {
                zPos = -8.0f;
                moving = false; // Stop moving and start rotating
            }
        }
        else {
            rotationAngle += 1.0f; // Increment rotation angle
            if (rotationAngle == 45.0f || rotationAngle == 90.0f || rotationAngle == 135.0f) {
                rotating = false; // Stop rotation
                ptri = 1; // Set ptri to 1
                glutTimerFunc(3200, resetPtri, 0); // 2초 후에 resetPtri 호출
            }
            else if (rotationAngle == 180.0f || rotationAngle == 235.0f || rotationAngle == 270.0f) {
                rotating = false; // Stop rotation
                ptri = 1;
                glutTimerFunc(1000, newrest, 0);
                jumpmove();
            }
            else if (rotationAngle == 360.0f) {
                dencetri = 1;
            }
        } 
    }
    glutPostRedisplay();
}

GLuint floorTextureID;
GLuint backgroundTextureID;

void InitTextures() {
    // 바닥에 사용할 텍스처 로드
    floorTextureID = LoadGLTexture("floor_texture.jpg");

    // 배경에 사용할 텍스처 로드
    backgroundTextureID = LoadGLTexture("background_texture.jpg");
}

void DrawBackground() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 색상 버퍼와 깊이 버퍼를 지우고 새로 그림

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTextureID);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-100.0f, -100.0f, 15.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(100.0f, -100.0f, 15.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(100.0f, 100.0f, 15.0f);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-100.0f, 100.0f, 15.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
// Render
void Render() {
    DrawBackground();  // 배경을 그립니다.
    glLoadIdentity();  // reset the view

    gluLookAt(10.0f, 10.0f, -45.0f,  // eye position
            0.0f, 0.0f, 0.0f,  // center position
            0.0f, 1.0f, 0.0f);  // up direction
    

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    DrawRobot(0.0f, yPos, zPos, rotationAngle);  // draw robot

    if (blocktri < 1) {
        DrawBlock1(-5.0f, -3.0f, -10.0f);  // 첫 번째 블록 그리기
        DrawBlock1(-8.0f, -2.0f, -8.0f);   // 두 번째 블록 그리기
        DrawBlock1(-6.0f, -2.0f, -3.0f);   // 세 번째 블록 그리기
    }
    else if (blocktri == 1) {
        // blocktri가 1일 때 첫 번째 블록을 제외한 두 번째와 세 번째 블록만 그립니다.
        DrawBlock1(-8.0f, -2.0f, -8.0f);   // 두 번째 블록 그리기
        DrawBlock1(-6.0f, -2.0f, -3.0f);   // 세 번째 블록 그리기
    }
    else if (blocktri == 2) {
        // blocktri가 2일 때 첫 번째와 두 번째 블록을 제외한 세 번째 블록만 그립니다.
        DrawBlock1(-6.0f, -2.0f, -3.0f);   // 세 번째 블록 그리기
    }
    else {}

    if (blocktri2 >= 1) {
        DrawBlock2(2.0f, -7.0f, -6.6f);   // 첫 번째 블록 그리기
    }

    if (blocktri2 >= 2) {
        DrawBlock2(2.0f, -4.0f, -6.6f);   // 두 번째 블록 그리기
    }

    if (blocktri2 >= 3) {
        DrawBlock2(2.0f, -1.0f, -6.6f);   // 두 번째 블록 그리기
    }

    glutSwapBuffers();  // swap the buffers
}

// Reshape
void Reshape(int w, int h) {
    if (h == 0) h = 1;  // prevent divide by zero
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);  // set the viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);  // set perspective
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SetupRC()
{
    // Light values and coordinates
    GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat  specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    

    // Enable lighting
    glEnable(GL_LIGHTING);

    // Setup and enable light 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHT0);

    // Enable color tracking
    glEnable(GL_COLOR_MATERIAL);

    // Set Material properties to follow glColor values
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // All materials hereafter have full specular reflectivity
    // with a high shine
    glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    // Light blue background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);  // initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 1080);  // initial window size
    glutInitWindowPosition(0, 0);  // initial window position
    glutCreateWindow("Robot Animation with GLUT");  // create window
    LoadTextures();
    InitTextures();
    glutDisplayFunc(Render);  // set display callback
    glutReshapeFunc(Reshape);  // set reshape callback
    glutIdleFunc(Myidle);  // set idle callback for continuous rendering
    SetupRC();

    glutMainLoop();  // enter the GLUT main loop

    return 0;
}