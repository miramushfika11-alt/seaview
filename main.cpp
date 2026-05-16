#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// ==================== CAMERA VARIABLES ====================
float cameraDistance = 40.0f;
float cameraAngleX = 20.0f;   // Up-down angle
float cameraAngleY = 0.0f;    // Left-right angle
float cameraPanX = 0.0f;
float cameraPanY = 5.0f;
int lastMouseX, lastMouseY;
int mouseButton = -1;

// ==================== ANIMATION VARIABLES ====================
float timeParam = 0.0f;
float shipX = -25.0f;
float shipZ = 5.0f;
float birdX = -30.0f;

// Fish variables
typedef struct {
    float x, y, z;
    float speed;
    float phase;
    float scale;
    float r, g, b;
} Fish;

#define NUM_FISH 12
Fish fishes[NUM_FISH];

// Bird variables
typedef struct {
    float xOffset, yOffset, zOffset;
    float speed;
    float flapPhase;
} Bird;

#define NUM_BIRDS 7
Bird birds[NUM_BIRDS];

// Human walking phase
float humanWalkPhase = 0.0f;
float humanPosOnDeck = 0.0f;
int humanDirection = 1;

// Wave parameters
#define WAVE_GRID 80
float waveHeights[WAVE_GRID + 1][WAVE_GRID + 1];

// Cloud variables
typedef struct {
    float x, y, z;
    float scale;
    float speed;
} Cloud;

#define NUM_CLOUDS 8
Cloud clouds[NUM_CLOUDS];

// Dolphin variables
typedef struct {
    float x, z;
    float speed;
    float phase;
    int jumping;
} Dolphin;

#define NUM_DOLPHINS 3
Dolphin dolphins[NUM_DOLPHINS];

// Lighthouse rotation
float lighthouseBeamAngle = 0.0f;

// Day-night cycle
float dayTime = 1.5f; // Start at noon-ish

// ==================== MOUSE FUNCTIONS ====================
void mouseFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        mouseButton = button;
        lastMouseX = x;
        lastMouseY = y;
    } else {
        mouseButton = -1;
    }
}

void motionFunc(int x, int y) {
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    if (mouseButton == GLUT_LEFT_BUTTON) {
        // Rotate camera
        cameraAngleY += dx * 0.3f;
        cameraAngleX += dy * 0.3f;
        if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
        if (cameraAngleX < -10.0f) cameraAngleX = -10.0f;
    } else if (mouseButton == GLUT_RIGHT_BUTTON) {
        // Pan camera
        cameraPanX -= dx * 0.05f;
        cameraPanY += dy * 0.05f;
    }

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

void mouseWheelFunc(int button, int dir, int x, int y) {
    // Some systems use button 3/4 for scroll
    if (dir > 0) {
        cameraDistance -= 2.0f;
    } else {
        cameraDistance += 2.0f;
    }
    if (cameraDistance < 5.0f) cameraDistance = 5.0f;
    if (cameraDistance > 100.0f) cameraDistance = 100.0f;
    glutPostRedisplay();
}

// Keyboard zoom fallback
void keyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
        case '+': case '=':
            cameraDistance -= 2.0f;
            if (cameraDistance < 5.0f) cameraDistance = 5.0f;
            break;
        case '-': case '_':
            cameraDistance += 2.0f;
            if (cameraDistance > 100.0f) cameraDistance = 100.0f;
            break;
        case 'w': cameraPanY += 1.0f; break;
        case 's': cameraPanY -= 1.0f; break;
        case 'a': cameraPanX -= 1.0f; break;
        case 'd': cameraPanX += 1.0f; break;
        case 'r': // Reset camera
            cameraDistance = 40.0f;
            cameraAngleX = 20.0f;
            cameraAngleY = 0.0f;
            cameraPanX = 0.0f;
            cameraPanY = 5.0f;
            break;
        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

void specialKeyFunc(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: cameraAngleX += 2.0f; break;
        case GLUT_KEY_DOWN: cameraAngleX -= 2.0f; break;
        case GLUT_KEY_LEFT: cameraAngleY -= 2.0f; break;
        case GLUT_KEY_RIGHT: cameraAngleY += 2.0f; break;
    }
    if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
    if (cameraAngleX < -10.0f) cameraAngleX = -10.0f;
    glutPostRedisplay();
}
// ==================== INITIALIZATION ====================
void initFishes() {
    for (int i = 0; i < NUM_FISH; i++) {
        fishes[i].x = ((float)(rand() % 800) / 10.0f) - 40.0f;
        fishes[i].y = -((float)(rand() % 30) / 10.0f) - 0.5f;
        fishes[i].z = ((float)(rand() % 600) / 10.0f) - 30.0f;
        fishes[i].speed = 0.02f + (float)(rand() % 100) / 2000.0f;
        fishes[i].phase = (float)(rand() % 628) / 100.0f;
        fishes[i].scale = 0.3f + (float)(rand() % 50) / 100.0f;
        // Random fish colors
        switch (rand() % 5) {
            case 0: fishes[i].r = 1.0f; fishes[i].g = 0.5f; fishes[i].b = 0.0f; break; // Orange
            case 1: fishes[i].r = 1.0f; fishes[i].g = 0.8f; fishes[i].b = 0.0f; break; // Gold
            case 2: fishes[i].r = 0.0f; fishes[i].g = 0.8f; fishes[i].b = 0.4f; break; // Green
            case 3: fishes[i].r = 0.8f; fishes[i].g = 0.2f; fishes[i].b = 0.2f; break; // Red
            case 4: fishes[i].r = 0.3f; fishes[i].g = 0.3f; fishes[i].b = 0.9f; break; // Blue
        }
    }
}

void initBirds() {
    for (int i = 0; i < NUM_BIRDS; i++) {
        birds[i].xOffset = (float)(rand() % 200) / 10.0f - 10.0f;
        birds[i].yOffset = (float)(rand() % 40) / 10.0f - 2.0f;
        birds[i].zOffset = (float)(rand() % 200) / 10.0f - 10.0f;
        birds[i].speed = 0.1f + (float)(rand() % 50) / 500.0f;
        birds[i].flapPhase = (float)(rand() % 628) / 100.0f;
    }
}

void initClouds() {
    for (int i = 0; i < NUM_CLOUDS; i++) {
        clouds[i].x = (float)(rand() % 1000) / 10.0f - 50.0f;
        clouds[i].y = 18.0f + (float)(rand() % 100) / 10.0f;
        clouds[i].z = -20.0f - (float)(rand() % 200) / 10.0f;
        clouds[i].scale = 1.5f + (float)(rand() % 200) / 100.0f;
        clouds[i].speed = 0.005f + (float)(rand() % 20) / 4000.0f;
    }
}

void initDolphins() {
    for (int i = 0; i < NUM_DOLPHINS; i++) {
        dolphins[i].x = (float)(rand() % 400) / 10.0f - 20.0f;
        dolphins[i].z = (float)(rand() % 300) / 10.0f - 15.0f;
        dolphins[i].speed = 0.03f + (float)(rand() % 20) / 1000.0f;
        dolphins[i].phase = (float)(rand() % 628) / 100.0f;
        dolphins[i].jumping = 0;
    }
}

// ==================== WAVE SYSTEM ====================
void calculateWaves() {
    float gridSize = 100.0f;
    float cellSize = gridSize / WAVE_GRID;

    for (int i = 0; i <= WAVE_GRID; i++) {
        for (int j = 0; j <= WAVE_GRID; j++) {
            float x = -gridSize / 2.0f + i * cellSize;
            float z = -gridSize / 2.0f + j * cellSize;

            // Multiple wave layers for realistic look
            float wave1 = sin(x * 0.15f + timeParam * 1.5f) * 0.3f;
            float wave2 = sin(z * 0.2f + timeParam * 1.2f) * 0.2f;
            float wave3 = sin((x + z) * 0.1f + timeParam * 0.8f) * 0.15f;
            float wave4 = sin(x * 0.3f - timeParam * 2.0f) * 0.1f;
            float wave5 = cos(z * 0.25f + timeParam * 1.8f) * 0.12f;

            waveHeights[i][j] = wave1 + wave2 + wave3 + wave4 + wave5;
        }
    }
}

float getWaveHeight(float x, float z) {
    float wave1 = sin(x * 0.15f + timeParam * 1.5f) * 0.3f;
    float wave2 = sin(z * 0.2f + timeParam * 1.2f) * 0.2f;
    float wave3 = sin((x + z) * 0.1f + timeParam * 0.8f) * 0.15f;
    float wave4 = sin(x * 0.3f - timeParam * 2.0f) * 0.1f;
    float wave5 = cos(z * 0.25f + timeParam * 1.8f) * 0.12f;
    return wave1 + wave2 + wave3 + wave4 + wave5;
}

void drawWaterSurface() {
    float gridSize = 100.0f;
    float cellSize = gridSize / WAVE_GRID;

    calculateWaves();

    for (int i = 0; i < WAVE_GRID; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= WAVE_GRID; j++) {
            float x0 = -gridSize / 2.0f + i * cellSize;
            float x1 = x0 + cellSize;
            float z = -gridSize / 2.0f + j * cellSize;

            float h0 = waveHeights[i][j];
            float h1 = waveHeights[i + 1][j];

            // Calculate normals for lighting
            float nx0 = 0.0f, ny0 = 1.0f, nz0 = 0.0f;
            float nx1 = 0.0f, ny1 = 1.0f, nz1 = 0.0f;

            if (i > 0 && i < WAVE_GRID && j > 0 && j < WAVE_GRID) {
                nx0 = waveHeights[i - 1][j] - waveHeights[i + 1][j];
                nz0 = waveHeights[i][j - 1] - waveHeights[i][j + 1];
                ny0 = 2.0f * cellSize;
                float len0 = sqrt(nx0 * nx0 + ny0 * ny0 + nz0 * nz0);
                if (len0 > 0) { nx0 /= len0; ny0 /= len0; nz0 /= len0; }
            }

            // Vary water color based on depth and wave height
            float deepFactor = 0.5f + h0 * 0.5f;
            float r = 0.0f + deepFactor * 0.05f;
            float g = 0.25f + deepFactor * 0.15f;
            float b = 0.5f + deepFactor * 0.3f;

            // Add foam on wave peaks
            if (h0 > 0.5f) {
                float foam = (h0 - 0.5f) * 2.0f;
                r += foam * 0.5f;
                g += foam * 0.5f;
                b += foam * 0.3f;
            }

            glColor4f(r, g, b, 0.85f);
            glNormal3f(nx0, ny0, nz0);
            glVertex3f(x0, h0, z);

            glColor4f(r + 0.02f, g + 0.02f, b + 0.02f, 0.85f);
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, h1, z);
        }
        glEnd();
    }
}

// Draw underwater floor
void drawSeaFloor() {
    glColor3f(0.6f, 0.5f, 0.3f); // Sandy color
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-50.0f, -5.0f, 50.0f);
    glVertex3f(50.0f, -5.0f, 50.0f);
    glVertex3f(50.0f, -5.0f, -50.0f);
    glVertex3f(-50.0f, -5.0f, -50.0f);
    glEnd();

    // Draw some seaweed
    for (int i = 0; i < 20; i++) {
        float sx = sin(i * 3.7f) * 30.0f;
        float sz = cos(i * 2.3f) * 25.0f;
        float sway = sin(timeParam * 1.5f + i) * 0.3f;

        glColor3f(0.0f, 0.4f + (i % 3) * 0.1f, 0.1f);
        glPushMatrix();
        glTranslatef(sx, -5.0f, sz);
        for (int seg = 0; seg < 5; seg++) {
            glPushMatrix();
            glTranslatef(sway * seg * 0.3f, seg * 0.4f, 0.0f);
            glScalef(0.1f, 0.4f, 0.1f);
            glutSolidCube(1.0);
            glPopMatrix();
        }
        glPopMatrix();
    }
}
// ==================== HUMAN WITH WALKING ANIMATION ====================
void drawHuman(float walkPhase, float scale) {
    float armSwing = sin(walkPhase) * 35.0f;
    float legSwing = sin(walkPhase) * 30.0f;
    float bodyBob = fabs(sin(walkPhase)) * 0.05f;

    glPushMatrix();
    glScalef(scale, scale, scale);
    glTranslatef(0.0f, bodyBob, 0.0f);

    // Head
    glColor3f(1.0f, 0.82f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, 2.05f, 0.0f);
    glutSolidSphere(0.22, 12, 12);

    // Hat
    glColor3f(0.2f, 0.2f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.25, 0.2, 8, 4);
    glPopMatrix();

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.07f, 0.05f, 0.18f);
    glutSolidSphere(0.03, 6, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.07f, 0.05f, 0.18f);
    glutSolidSphere(0.03, 6, 6);
    glPopMatrix();

    glPopMatrix(); // End head

    // Neck
    glColor3f(1.0f, 0.82f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, 1.75f, 0.0f);
    glScalef(0.1f, 0.15f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Torso (Sailor shirt - white with blue stripes effect)
    glColor3f(0.9f, 0.9f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.0f);
    glScalef(0.5f, 0.7f, 0.3f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Belt
    glColor3f(0.3f, 0.15f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, 0.93f, 0.0f);
    glScalef(0.52f, 0.08f, 0.32f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left Arm
    glColor3f(1.0f, 0.82f, 0.65f);
    glPushMatrix();
    glTranslatef(-0.35f, 1.55f, 0.0f);
    glRotatef(armSwing, 1.0f, 0.0f, 0.0f);

    // Upper arm (shirt)
    glColor3f(0.9f, 0.9f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.0f);
    glScalef(0.15f, 0.35f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Lower arm (skin)
    glColor3f(1.0f, 0.82f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    glScalef(0.12f, 0.3f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Hand
    glPushMatrix();
    glTranslatef(0.0f, -0.68f, 0.0f);
    glutSolidSphere(0.06, 6, 6);
    glPopMatrix();

    glPopMatrix(); // End left arm

    // Right Arm
    glPushMatrix();
    glTranslatef(0.35f, 1.55f, 0.0f);
    glRotatef(-armSwing, 1.0f, 0.0f, 0.0f);

    glColor3f(0.9f, 0.9f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.0f);
    glScalef(0.15f, 0.35f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(1.0f, 0.82f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    glScalef(0.12f, 0.3f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.68f, 0.0f);
    glutSolidSphere(0.06, 6, 6);
    glPopMatrix();

    glPopMatrix(); // End right arm

    // Left Leg
    glColor3f(0.15f, 0.15f, 0.4f); // Dark blue pants
    glPushMatrix();
    glTranslatef(-0.13f, 0.85f, 0.0f);
    glRotatef(legSwing, 1.0f, 0.0f, 0.0f);

    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f);
    glScalef(0.18f, 0.5f, 0.18f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Shoe
    glColor3f(0.2f, 0.1f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, -0.58f, 0.05f);
    glScalef(0.18f, 0.1f, 0.25f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix(); // End left leg

    // Right Leg
    glColor3f(0.15f, 0.15f, 0.4f);
    glPushMatrix();
    glTranslatef(0.13f, 0.85f, 0.0f);
    glRotatef(-legSwing, 1.0f, 0.0f, 0.0f);

    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f);
    glScalef(0.18f, 0.5f, 0.18f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.2f, 0.1f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, -0.58f, 0.05f);
    glScalef(0.18f, 0.1f, 0.25f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix(); // End right leg

    glPopMatrix(); // End scale
}

// ==================== ADVANCED SHIP ====================
void drawShip() {
    float waveH = getWaveHeight(shipX, shipZ);
    float shipRoll = sin(timeParam * 1.5f) * 3.0f;
    float shipPitch = sin(timeParam * 1.2f) * 2.0f;

    glPushMatrix();
    glTranslatef(shipX, waveH + 0.5f, shipZ);
    glRotatef(shipRoll, 0.0f, 0.0f, 1.0f);
    glRotatef(shipPitch, 1.0f, 0.0f, 0.0f);

    // ---- Hull Bottom (Dark) ----
    glColor3f(0.3f, 0.15f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.0f);
    glScalef(8.0f, 0.6f, 2.8f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Hull bow (front taper)
    glColor3f(0.4f, 0.2f, 0.08f);
    glPushMatrix();
    glTranslatef(4.5f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.4f, 0.5f, 2.0f);
    glutSolidCone(1.0, 1.0, 8, 4);
    glPopMatrix();

    // Hull stern (back)
    glColor3f(0.35f, 0.18f, 0.06f);
    glPushMatrix();
    glTranslatef(-4.2f, 0.2f, 0.0f);
    glScalef(0.5f, 1.0f, 2.6f);
    glutSolidCube(1.0);
    glPopMatrix();

    // ---- Main Deck ----
    glColor3f(0.7f, 0.5f, 0.25f); // Wood color
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(8.0f, 0.15f, 2.8f);
    glutSolidCube(1.0);
    glPopMatrix();

    // ---- Deck Rails ----
    glColor3f(0.8f, 0.7f, 0.5f);
    for (int side = -1; side <= 1; side += 2) {
        for (float rx = -3.5f; rx <= 3.5f; rx += 1.0f) {
            glPushMatrix();
            glTranslatef(rx, 0.8f, side * 1.35f);
            glScalef(0.05f, 0.5f, 0.05f);
            glutSolidCube(1.0);
            glPopMatrix();
        }
        // Top rail
        glPushMatrix();
        glTranslatef(0.0f, 1.0f, side * 1.35f);
        glScalef(7.5f, 0.05f, 0.05f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // ---- Cabin / Bridge ----
    glColor3f(0.85f, 0.85f, 0.85f);
    glPushMatrix();
    glTranslatef(-1.5f, 1.3f, 0.0f);
    glScalef(2.5f, 1.2f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Cabin windows
    glColor3f(0.3f, 0.5f, 0.8f);
    for (int w = 0; w < 3; w++) {
        glPushMatrix();
        glTranslatef(-2.0f + w * 0.7f, 1.4f, 1.01f);
        glScalef(0.4f, 0.35f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-2.0f + w * 0.7f, 1.4f, -1.01f);
        glScalef(0.4f, 0.35f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // Cabin roof
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(-1.5f, 1.95f, 0.0f);
    glScalef(2.7f, 0.1f, 2.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // ---- Smoke Stack ----
    glColor3f(0.7f, 0.1f, 0.1f); // Red stack
    glPushMatrix();
    glTranslatef(-1.5f, 2.5f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.35, 0.3, 1.2, 12, 4);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Stack band
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(-1.5f, 3.5f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    quad = gluNewQuadric();
    gluCylinder(quad, 0.36, 0.36, 0.15, 12, 2);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // ---- Smoke particles ----
    glColor4f(0.6f, 0.6f, 0.6f, 0.5f);
    for (int s = 0; s < 5; s++) {
        float smokeAge = fmod(timeParam * 2.0f + s * 0.5f, 3.0f);
        float smokeY = 3.8f + smokeAge * 1.5f;
        float smokeSize = 0.2f + smokeAge * 0.3f;
        float smokeAlpha = 1.0f - smokeAge / 3.0f;
        float smokeDrift = smokeAge * 0.5f;

        glColor4f(0.7f, 0.7f, 0.7f, smokeAlpha * 0.4f);
        glPushMatrix();
        glTranslatef(-1.5f - smokeDrift, smokeY, sin(timeParam + s) * 0.3f);
        glutSolidSphere(smokeSize, 8, 8);
        glPopMatrix();
    }

    // ---- Mast with flag ----
    glColor3f(0.6f, 0.4f, 0.2f);
    glPushMatrix();
    glTranslatef(1.5f, 2.5f, 0.0f);
    glScalef(0.08f, 3.0f, 0.08f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Flag (animated)
    float flagWave = sin(timeParam * 5.0f) * 10.0f;
    glColor3f(0.9f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(1.5f, 3.7f, 0.0f);
    glRotatef(flagWave, 0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, -0.4f, 0.0f);
    glVertex3f(0.6f + sin(timeParam * 3.0f) * 0.1f, -0.2f, 0.1f);
    glEnd();
    glPopMatrix();

    // ---- Lifebuoys ----
    glColor3f(1.0f, 0.3f, 0.0f);
    for (int lb = 0; lb < 3; lb++) {
        glPushMatrix();
        glTranslatef(-1.0f + lb * 2.0f, 0.7f, 1.42f);
        glutSolidTorus(0.05, 0.15, 8, 12);
        glPopMatrix();
    }

    // ---- Anchor (front) ----
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(3.8f, 0.0f, 1.0f);
    glScalef(0.08f, 0.6f, 0.08f);
    glutSolidCube(1.0);
    glPopMatrix();

    // ---- Humans on deck ----
    // Captain at bridge
    //glPushMatrix();
    //glTranslatef(-1.5f, 1.9f, 0.0f);
    //glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    //drawHuman(0.0f, 0.5f); // Standing still
    //glPopMatrix();

    // Walking sailor on deck
    glPushMatrix();
    glTranslatef(humanPosOnDeck, 0.6f, 0.0f);
    glRotatef(humanDirection > 0 ? 90.0f : -90.0f, 0.0f, 1.0f, 0.0f);
    drawHuman(humanWalkPhase, 0.5f);
    glPopMatrix();

    // Lookout person at front
    glPushMatrix();
    glTranslatef(3.0f, 0.6f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawHuman(sin(timeParam * 0.5f) * 0.5f, 0.5f);
    glPopMatrix();

    glPopMatrix(); // End ship
}
// ==================== FISH ====================
void drawSingleFish(Fish* f) {
    float swimWobble = sin(timeParam * 5.0f + f->phase) * 15.0f;
    float tailWag = sin(timeParam * 8.0f + f->phase) * 25.0f;

    glPushMatrix();
    glTranslatef(f->x, f->y, f->z);
    glScalef(f->scale, f->scale, f->scale);
    glRotatef(swimWobble, 0.0f, 1.0f, 0.0f);

    // Body
    glColor3f(f->r, f->g, f->b);
    glPushMatrix();
    glScalef(1.0f, 0.4f, 0.3f);
    glutSolidSphere(0.5, 10, 8);
    glPopMatrix();

    // Tail
    glPushMatrix();
    glTranslatef(-0.5f, 0.0f, 0.0f);
    glRotatef(tailWag, 0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.4f, 0.15f, 0.0f);
    glVertex3f(-0.4f, -0.15f, 0.0f);
    glEnd();
    glPopMatrix();

    // Dorsal fin
    glColor3f(f->r * 0.8f, f->g * 0.8f, f->b * 0.8f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.2f, 0.0f);
    glVertex3f(-0.15f, 0.4f, 0.0f);
    glVertex3f(0.15f, 0.2f, 0.0f);
    glEnd();

    // Eye
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.3f, 0.05f, 0.12f);
    glutSolidSphere(0.05, 6, 6);
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.02f, 0.0f, 0.02f);
    glutSolidSphere(0.025, 6, 6);
    glPopMatrix();

    // Other eye
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.3f, 0.05f, -0.12f);
    glutSolidSphere(0.05, 6, 6);
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.02f, 0.0f, -0.02f);
    glutSolidSphere(0.025, 6, 6);
    glPopMatrix();

    glPopMatrix();
}

void drawAllFish() {
    for (int i = 0; i < NUM_FISH; i++) {
        drawSingleFish(&fishes[i]);
    }
}

// ==================== BIRDS ====================
void drawSingleBird(Bird* b) {
    float wingFlap = sin(timeParam * 10.0f + b->flapPhase) * 35.0f;
    float bodyBob = sin(timeParam * 5.0f + b->flapPhase) * 0.2f;

    glPushMatrix();
    glTranslatef(birdX + b->xOffset, 12.0f + b->yOffset + bodyBob, b->zOffset);
    glRotatef(-10.0f, 0.0f, 0.0f, 1.0f); // Slight forward tilt

    // Body
    glColor3f(0.15f, 0.15f, 0.15f);
    glPushMatrix();
    glScalef(0.6f, 0.15f, 0.15f);
    glutSolidSphere(1.0, 8, 8);
    glPopMatrix();

    // Head
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.5f, 0.1f, 0.0f);
    glutSolidSphere(0.12, 8, 8);
    glPopMatrix();

    // Beak
    glColor3f(1.0f, 0.7f, 0.0f);
    glPushMatrix();
    glTranslatef(0.65f, 0.08f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCone(0.03, 0.15, 6, 2);
    glPopMatrix();

    // Right Wing
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.1f);
    glRotatef(wingFlap, 1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.2f, 0.0f, 0.0f);
    glVertex3f(-0.3f, 0.0f, 0.9f);
    glVertex3f(0.3f, 0.0f, 0.7f);
    glEnd();
    // Wing feather details
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.1f, 0.0f, 0.0f);
    glVertex3f(-0.4f, 0.0f, 0.6f);
    glVertex3f(0.0f, 0.0f, 0.5f);
    glEnd();
    glPopMatrix();

    // Left Wing
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.1f);
    glRotatef(-wingFlap, 1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.2f, 0.0f, 0.0f);
    glVertex3f(-0.3f, 0.0f, -0.9f);
    glVertex3f(0.3f, 0.0f, -0.7f);
    glEnd();
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.1f, 0.0f, 0.0f);
    glVertex3f(-0.4f, 0.0f, -0.6f);
    glVertex3f(0.0f, 0.0f, -0.5f);
    glEnd();
    glPopMatrix();

    // Tail
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5f, 0.0f, 0.0f);
    glVertex3f(-0.8f, 0.1f, 0.15f);
    glVertex3f(-0.8f, 0.1f, -0.15f);
    glEnd();

    glPopMatrix();
}

void drawAllBirds() {
    for (int i = 0; i < NUM_BIRDS; i++) {
        drawSingleBird(&birds[i]);
    }
}

// ==================== DOLPHINS ====================
void drawDolphin(Dolphin* d) {
    float jumpPhase = sin(timeParam * 2.0f + d->phase);
    float y = 0.0f;
    float rotation = 0.0f;

    if (jumpPhase > 0.3f) {
        y = (jumpPhase - 0.3f) * 4.0f;
        rotation = (jumpPhase - 0.3f) * 60.0f;
    }

    glPushMatrix();
    glTranslatef(d->x, y, d->z);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    // Body
    glColor3f(0.4f, 0.5f, 0.6f);
    glPushMatrix();
    glScalef(1.2f, 0.35f, 0.35f);
    glutSolidSphere(1.0, 12, 10);
    glPopMatrix();

    // Nose
    glColor3f(0.45f, 0.55f, 0.65f);
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCone(0.2, 0.6, 8, 4);
    glPopMatrix();

    // Dorsal fin
    glColor3f(0.35f, 0.45f, 0.55f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.35f, 0.0f);
    glVertex3f(-0.3f, 0.7f, 0.0f);
    glVertex3f(0.3f, 0.35f, 0.0f);
    glEnd();

    // Tail fluke
    float tailFlap = sin(timeParam * 6.0f + d->phase) * 20.0f;
    glPushMatrix();
    glTranslatef(-1.2f, 0.0f, 0.0f);
    glRotatef(tailFlap, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.4f, 0.3f, 0.15f);
    glVertex3f(-0.4f, -0.3f, 0.15f);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.4f, 0.3f, -0.15f);
    glVertex3f(-0.4f, -0.3f, -0.15f);
    glEnd();
    glPopMatrix();

    // Belly (lighter)
    glColor3f(0.8f, 0.85f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, -0.15f, 0.0f);
    glScalef(1.0f, 0.2f, 0.3f);
    glutSolidSphere(1.0, 10, 8);
    glPopMatrix();

    // Eye
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.6f, 0.1f, 0.3f);
    glutSolidSphere(0.04, 6, 6);
    glPopMatrix();

    glPopMatrix();

    // Splash effect when entering water
    if (jumpPhase > 0.25f && jumpPhase < 0.4f) {
        glDisable(GL_LIGHTING);
        glColor4f(0.8f, 0.9f, 1.0f, 0.5f);
        glPushMatrix();
        glTranslatef(d->x, 0.1f, d->z);
        for (int sp = 0; sp < 8; sp++) {
            float angle = sp * 45.0f;
            float splashR = 0.5f + (jumpPhase - 0.25f) * 5.0f;
            glPushMatrix();
            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            glTranslatef(splashR, 0.0f, 0.0f);
            glutSolidSphere(0.1, 6, 6);
            glPopMatrix();
        }
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }
}

void drawAllDolphins() {
    for (int i = 0; i < NUM_DOLPHINS; i++) {
        drawDolphin(&dolphins[i]);
    }
}

// ==================== CLOUDS ====================
void drawCloud(Cloud* c) {
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 0.85f);

    glPushMatrix();
    glTranslatef(c->x, c->y, c->z);
    glScalef(c->scale, c->scale * 0.4f, c->scale * 0.5f);

    // Build cloud from multiple spheres
    glutSolidSphere(1.0, 10, 10);

    glPushMatrix();
    glTranslatef(1.2f, 0.2f, 0.0f);
    glutSolidSphere(0.8, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.0f, 0.1f, 0.3f);
    glutSolidSphere(0.9, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5f, 0.4f, -0.2f);
    glutSolidSphere(0.7, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5f, -0.1f, -0.3f);
    glutSolidSphere(0.6, 10, 10);
    glPopMatrix();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawAllClouds() {
    for (int i = 0; i < NUM_CLOUDS; i++) {
        drawCloud(&clouds[i]);
    }
}

// ==================== SUN WITH GLOW ====================
void drawSun() {
    float sunOrbitRadius = 35.0f;
    float sunAngle = timeParam * 0.1f;
    float sunX = cos(sunAngle) * sunOrbitRadius;
    float sunY = fabs(sin(sunAngle)) * sunOrbitRadius + 5.0f;
    float sunZ = -30.0f;

    // Set light position
    GLfloat lightPos[] = { sunX, sunY, sunZ, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Adjust light color based on "time of day"
    float dayFactor = sin(sunAngle);
    if (dayFactor < 0) dayFactor = 0;

    GLfloat ambColor[] = { 0.2f + dayFactor * 0.3f, 0.2f + dayFactor * 0.25f, 0.2f + dayFactor * 0.15f, 1.0f };
    GLfloat diffColor[] = { 0.5f + dayFactor * 0.5f, 0.4f + dayFactor * 0.5f, 0.3f + dayFactor * 0.3f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffColor);

    // Draw sun
    glDisable(GL_LIGHTING);

    // Sun glow (multiple transparent spheres)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Outer glow
    glColor4f(1.0f, 0.9f, 0.3f, 0.1f);
    glPushMatrix();
    glTranslatef(sunX, sunY, sunZ);
    glutSolidSphere(5.0, 20, 20);
    glPopMatrix();

    // Middle glow
    glColor4f(1.0f, 0.85f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(sunX, sunY, sunZ);
    glutSolidSphere(3.5, 20, 20);
    glPopMatrix();

    // Sun core
    glColor4f(1.0f, 0.95f, 0.4f, 1.0f);
    glPushMatrix();
    glTranslatef(sunX, sunY, sunZ);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}
// ==================== LIGHTHOUSE ON ISLAND ====================
void drawLighthouse() {
    glPushMatrix();
    glTranslatef(-30.0f, 0.0f, -15.0f);

    // Small Island
    glColor3f(0.6f, 0.5f, 0.2f); // Sandy
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(4.0f, 0.5f, 4.0f);
    glutSolidSphere(1.0, 12, 12);
    glPopMatrix();

    // Island vegetation
    glColor3f(0.2f, 0.5f, 0.1f);
    glPushMatrix();
    glTranslatef(1.5f, 0.3f, 1.0f);
    glScalef(1.5f, 0.8f, 1.5f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    // Palm tree trunk
    glColor3f(0.5f, 0.35f, 0.15f);
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, -1.0f);
    float palmCurve = 0.0f;
    for (int p = 0; p < 8; p++) {
        glPushMatrix();
        palmCurve += 0.08f;
        glTranslatef(palmCurve, p * 0.5f, 0.0f);
        glScalef(0.12f, 0.5f, 0.12f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // Palm leaves
    glColor3f(0.1f, 0.6f, 0.1f);
    glPushMatrix();
    glTranslatef(0.64f, 4.0f, 0.0f);
    for (int leaf = 0; leaf < 6; leaf++) {
        glPushMatrix();
        glRotatef(leaf * 60.0f + sin(timeParam * 2.0f) * 5.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(40.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(2.0f, -0.5f, 0.2f);
        glVertex3f(2.0f, -0.5f, -0.2f);
        glEnd();
        glPopMatrix();
    }
    glPopMatrix();
    glPopMatrix(); // End palm tree

    // Lighthouse tower
    glColor3f(0.9f, 0.9f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.8, 0.5, 5.0, 12, 6);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Red stripes on lighthouse
    glColor3f(0.8f, 0.1f, 0.1f);
    for (int stripe = 0; stripe < 3; stripe++) {
        glPushMatrix();
        glTranslatef(0.0f, 1.5f + stripe * 1.5f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        quad = gluNewQuadric();
        float baseR = 0.75f - stripe * 0.08f;
        gluCylinder(quad, baseR, baseR - 0.05f, 0.5, 12, 2);
        gluDeleteQuadric(quad);
        glPopMatrix();
    }

    // Lighthouse top platform
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, 5.5f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    quad = gluNewQuadric();
    gluDisk(quad, 0.0, 1.0, 12, 2);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Light room (glass)
    glColor4f(1.0f, 1.0f, 0.5f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 6.0f, 0.0f);
    glutSolidSphere(0.5, 10, 10);
    glPopMatrix();

    // Lighthouse dome
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 6.5f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.4, 0.5, 10, 4);
    glPopMatrix();

    // Rotating light beam
    lighthouseBeamAngle = fmod(timeParam * 50.0f, 360.0f);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 0.5f, 0.15f);

    glPushMatrix();
    glTranslatef(0.0f, 6.0f, 0.0f);
    glRotatef(lighthouseBeamAngle, 0.0f, 1.0f, 0.0f);

    // Light beam cone
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int b = -5; b <= 5; b++) {
        float angle = b * 2.0f * 3.14159f / 180.0f;
        glVertex3f(cos(angle) * 20.0f, sin(angle) * 2.0f - 2.0f, sin(angle) * 20.0f);
    }
    glEnd();
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glPopMatrix(); // End lighthouse
}

// ==================== ROCKS & DETAILS ====================
void drawRocks() {
    // Scattered rocks in the water
    float rockPositions[][3] = {
        {15.0f, -0.3f, -10.0f},
        {20.0f, -0.2f, 8.0f},
        {-10.0f, -0.4f, 20.0f},
        {25.0f, -0.1f, -5.0f},
        {-20.0f, -0.3f, 12.0f}
    };

    for (int i = 0; i < 5; i++) {
        float wh = getWaveHeight(rockPositions[i][0], rockPositions[i][2]);
        glColor3f(0.4f + i * 0.03f, 0.38f + i * 0.02f, 0.35f);
        glPushMatrix();
        glTranslatef(rockPositions[i][0], wh + rockPositions[i][1], rockPositions[i][2]);
        glScalef(0.8f + i * 0.1f, 0.6f + i * 0.05f, 0.7f + i * 0.08f);
        glutSolidSphere(1.0, 8, 8);
        glPopMatrix();

        // Smaller rock nearby
        glColor3f(0.45f, 0.4f, 0.35f);
        glPushMatrix();
        glTranslatef(rockPositions[i][0] + 1.0f, wh - 0.2f, rockPositions[i][2] + 0.5f);
        glScalef(0.4f, 0.3f, 0.4f);
        glutSolidSphere(1.0, 6, 6);
        glPopMatrix();
    }
}

// ==================== BUOY ====================
void drawBuoys() {
    float buoyPositions[][2] = {
        {10.0f, 15.0f},
        {-15.0f, -8.0f},
        {5.0f, -18.0f}
    };

    for (int i = 0; i < 3; i++) {
        float bx = buoyPositions[i][0];
        float bz = buoyPositions[i][1];
        float wh = getWaveHeight(bx, bz);
        float buoyBob = wh + 0.3f;

        glPushMatrix();
        glTranslatef(bx, buoyBob, bz);

        // Buoy body
        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glutSolidSphere(0.3, 10, 10);
        glPopMatrix();

        // White stripe
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidTorus(0.05, 0.3, 8, 12);
        glPopMatrix();

        // Top stick
        glColor3f(0.3f, 0.3f, 0.3f);
        glPushMatrix();
        glTranslatef(0.0f, 0.5f, 0.0f);
        glScalef(0.03f, 0.6f, 0.03f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Blinking light on top
        float blink = sin(timeParam * 5.0f + i * 2.0f);
        if (blink > 0.0f) {
            glDisable(GL_LIGHTING);
            glColor3f(1.0f, 1.0f, 0.0f);
            glPushMatrix();
            glTranslatef(0.0f, 0.8f, 0.0f);
            glutSolidSphere(0.05, 6, 6);
            glPopMatrix();
            glEnable(GL_LIGHTING);
        }

        glPopMatrix();
    }
}
// ==================== SKY GRADIENT ====================
void drawSkyDome() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float sunAngle = timeParam * 0.1f;
    float dayFactor = fabs(sin(sunAngle));

    // Sky gradient quad behind everything
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glBegin(GL_QUADS);
    // Top of sky - deeper blue
    float topR = 0.1f + dayFactor * 0.15f;
    float topG = 0.2f + dayFactor * 0.3f;
    float topB = 0.4f + dayFactor * 0.5f;
    // Bottom of sky - lighter / sunset
    float botR = 0.4f + dayFactor * 0.4f;
    float botG = 0.5f + dayFactor * 0.3f;
    float botB = 0.5f + dayFactor * 0.4f;

    glColor3f(topR, topG, topB);
    glVertex3f(-1.0f, 1.0f, 0.999f);
    glVertex3f(1.0f, 1.0f, 0.999f);

    glColor3f(botR, botG, botB);
    glVertex3f(1.0f, 0.0f, 0.999f);
    glVertex3f(-1.0f, 0.0f, 0.999f);

    // Horizon to bottom
    glColor3f(botR, botG, botB);
    glVertex3f(-1.0f, 0.0f, 0.999f);
    glVertex3f(1.0f, 0.0f, 0.999f);

    glColor3f(0.3f, 0.5f + dayFactor * 0.2f, 0.7f + dayFactor * 0.2f);
    glVertex3f(1.0f, -1.0f, 0.999f);
    glVertex3f(-1.0f, -1.0f, 0.999f);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ==================== FOAM / WAKE BEHIND SHIP ====================
void drawShipWake() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int w = 0; w < 15; w++) {
        float wakeAge = w * 0.8f;
        float wx = shipX - 4.0f - wakeAge;
        float wz = shipZ;
        float wh = getWaveHeight(wx, wz) + 0.05f;
        float alpha = 1.0f - wakeAge / 12.0f;
        float spread = wakeAge * 0.3f;

        if (alpha < 0) alpha = 0;

        glColor4f(0.8f, 0.9f, 1.0f, alpha * 0.4f);

        // Left wake line
        glPushMatrix();
        glTranslatef(wx, wh, wz + spread);
        glutSolidSphere(0.15f + wakeAge * 0.05f, 6, 6);
        glPopMatrix();

        // Right wake line
        glPushMatrix();
        glTranslatef(wx, wh, wz - spread);
        glutSolidSphere(0.15f + wakeAge * 0.05f, 6, 6);
        glPopMatrix();

        // Center foam
        glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.3f);
        glPushMatrix();
        glTranslatef(wx, wh, wz);
        glutSolidSphere(0.1f + wakeAge * 0.03f, 6, 6);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ==================== WATER SURFACE REFLECTION HINT ====================
void drawWaterReflectionEffect() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Sparkles on water surface
    glDisable(GL_LIGHTING);
    int numSparkles = 30;
    for (int i = 0; i < numSparkles; i++) {
        float sx = sin(i * 7.3f + timeParam * 0.5f) * 40.0f;
        float sz = cos(i * 4.7f + timeParam * 0.3f) * 30.0f;
        float wh = getWaveHeight(sx, sz) + 0.1f;
        float sparkle = sin(timeParam * 10.0f + i * 3.0f);
        if (sparkle > 0.7f) {
            float intensity = (sparkle - 0.7f) / 0.3f;
            glColor4f(1.0f, 1.0f, 0.8f, intensity * 0.6f);
            glPushMatrix();
            glTranslatef(sx, wh, sz);
            glutSolidSphere(0.05, 4, 4);
            glPopMatrix();
        }
    }
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

// ==================== HUD / INFO TEXT ====================
void drawHUD() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);

    // Draw text
    char info1[] = "Mouse: Left=Rotate, Right=Pan, Scroll=Zoom";
    char info2[] = "Keys: WASD=Pan, +/-=Zoom, R=Reset, ESC=Exit";
    char info3[] = "3D Sea View - Advanced Animation";

    glRasterPos2f(10, 580);
    for (int i = 0; info3[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, info3[i]);

    glRasterPos2f(10, 30);
    for (int i = 0; info1[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info1[i]);

    glRasterPos2f(10, 15);
    for (int i = 0; info2[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info2[i]);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ==================== MAIN DISPLAY ====================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Draw sky background first
    drawSkyDome();

    glLoadIdentity();

    // Camera setup using mouse-controlled variables
    float camX = cameraDistance * cos(cameraAngleX * 3.14159f / 180.0f) * sin(cameraAngleY * 3.14159f / 180.0f);
    float camY = cameraDistance * sin(cameraAngleX * 3.14159f / 180.0f);
    float camZ = cameraDistance * cos(cameraAngleX * 3.14159f / 180.0f) * cos(cameraAngleY * 3.14159f / 180.0f);

    gluLookAt(camX + cameraPanX, camY + cameraPanY, camZ,
              cameraPanX, cameraPanY * 0.3f, 0.0,
              0.0, 1.0, 0.0);

    // Draw everything
    drawSun();
    drawAllClouds();

    // Enable blending for water transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawSeaFloor();
    drawAllFish();
    drawAllDolphins();
    drawWaterSurface();
    drawWaterReflectionEffect();

    glDisable(GL_BLEND);

    drawShip();
    drawShipWake();
    drawAllBirds();
    drawLighthouse();
    drawRocks();
    drawBuoys();

    drawHUD();

    glutSwapBuffers();
}

// ==================== UPDATE FUNCTION ====================
void update(int value) {
    timeParam += 0.016f;

    // Move ship
    shipX += 0.05f;
    if (shipX > 35.0f) {
        shipX = -35.0f;
    }

    // Human walking on deck
    humanWalkPhase += 0.15f;
    humanPosOnDeck += humanDirection * 0.02f;
    if (humanPosOnDeck > 2.5f) {
        humanDirection = -1;
    } else if (humanPosOnDeck < -2.5f) {
        humanDirection = 1;
    }

    // Move birds
    birdX += 0.12f;
    if (birdX > 40.0f) {
        birdX = -40.0f;
    }

    // Move fish
    for (int i = 0; i < NUM_FISH; i++) {
        fishes[i].x += fishes[i].speed;
        if (fishes[i].x > 40.0f) fishes[i].x = -40.0f;
        // Gentle vertical swimming
        fishes[i].y = -((float)(abs((int)(fishes[i].phase * 100)) % 30) / 10.0f) - 0.5f
                      + sin(timeParam * 1.5f + fishes[i].phase) * 0.3f;
    }

    // Move dolphins
    for (int i = 0; i < NUM_DOLPHINS; i++) {
        dolphins[i].x += dolphins[i].speed;
        if (dolphins[i].x > 35.0f) dolphins[i].x = -35.0f;
    }

    // Move clouds
    for (int i = 0; i < NUM_CLOUDS; i++) {
        clouds[i].x += clouds[i].speed;
        if (clouds[i].x > 55.0f) clouds[i].x = -55.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ==================== RESHAPE ====================
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, aspect, 0.5, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

// ==================== INIT ====================
void init() {
    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // Fog for distance atmosphere
    glEnable(GL_FOG);
    GLfloat fogColor[] = { 0.6f, 0.75f, 0.85f, 1.0f };
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 40.0f);
    glFogf(GL_FOG_END, 90.0f);

    GLfloat ambientLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    // Initialize all objects
    srand(42); // Fixed seed for reproducibility
    initFishes();
    initBirds();
    initClouds();
    initDolphins();
}

// ==================== MAIN ====================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Advanced 3D Sea View - Ship, Fish, Birds, Dolphins");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);

    // Mouse controls
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Keyboard controls
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialKeyFunc);

    printf("\n====================================\n");
    printf("  Advanced 3D Sea View Animation\n");
    printf("====================================\n");
    printf("  Mouse Controls:\n");
    printf("    Left Button + Drag  = Rotate Camera\n");
    printf("    Right Button + Drag = Pan Camera\n");
    printf("    Scroll Wheel        = Zoom In/Out\n");
    printf("  Keyboard Controls:\n");
    printf("    +/- = Zoom In/Out\n");
    printf("    W/A/S/D = Pan Camera\n");
    printf("    Arrow Keys = Rotate Camera\n");
    printf("    R = Reset Camera\n");
    printf("    ESC = Exit\n");
    printf("====================================\n\n");

    glutMainLoop();
    return 0;
}
