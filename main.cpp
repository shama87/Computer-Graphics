#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <string>
#include <vector>

#define PI 3.1416

int segments = 60;
GLfloat twoPi = 2.0f * PI;

float boatX = 0.0f;
float shipX = 0.0f;
float cloud1X = 0.0f, cloud2X = 0.5f, cloud3X = -0.3f;
float dronePosX = 0.0f, dronePosY = 0.7f;
float propellerAngle = 0.0f;
float carX = -0.8f;
float car2X = -1.2f;
float car3X = -1.6f;
float boat2X = -0.8f;
float boat3X = 0.6f;

bool boatActive = true;
bool nightMode = false;
bool droneActive = true;
bool carActive = true;
bool lightingEnabled = true;
bool lamppostLightOn = true;
bool carHeadlightOn = true;
bool wavesActive = true;
float droneVelocity = 0.002f;
float carSpeed = 0.008f;
float car2Speed = 0.006f;
float car3Speed = 0.01f;
float boat2Speed = 0.004f;
float boat3Speed = 0.003f;

int currentLightingEffect = 0;
int lamppostLightColor = 0;

GLfloat light0_position[] = {0.0f, 2.0f, 2.0f, 1.0f};
GLfloat light0_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.8f, 1.0f};
GLfloat light0_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat light1_position[] = {-1.0f, 0.5f, 1.0f, 1.0f};
GLfloat light1_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat light1_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
GLfloat light1_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};

GLfloat lamppost_light_positions[6][4] = {
    {-0.9f, 0.85f, 0.0f, 1.0f},
    {-0.6f, 0.85f, 0.0f, 1.0f},
    {-0.3f, 0.85f, 0.0f, 1.0f},
    {0.3f, 0.85f, 0.0f, 1.0f},
    {0.6f, 0.85f, 0.0f, 1.0f},
    {0.9f, 0.85f, 0.0f, 1.0f}
};

GLfloat lamppost_light_diffuse[4] = {1.0f, 1.0f, 0.8f, 1.0f};
GLfloat lamppost_light_specular[4] = {1.0f, 1.0f, 0.8f, 1.0f};
GLfloat lamppost_spot_direction[] = {0.0f, -1.0f, 0.0f};

GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat shininess = 50.0f;

float waveOffset = 0.0f;

struct Point2D {
    float x, y;
};

Point2D calculateBezierPoint(Point2D p0, Point2D p1, Point2D p2, Point2D p3, float t) {
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Point2D p;
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    return p;
}

void drawBezierWave(float yOffset, float amplitude, float frequency, float phase) {
    if (!wavesActive) {
        glLineWidth(2.0f);
        glColor3f(0.0f, 0.5f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(-1.0f, yOffset, 0.0f);
        glVertex3f(1.0f, yOffset, 0.0f);
        glEnd();
        return;
    }

    int numSegments = 20;
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.5f, 1.0f);

    for (int segment = 0; segment < numSegments; segment++) {
        float xStart = -1.0f + segment * (2.0f / numSegments);
        float xEnd = xStart + (2.0f / numSegments);

        Point2D p0, p1, p2, p3;
        p0.x = xStart;
        p0.y = yOffset + amplitude * sin(frequency * xStart + phase + waveOffset);

        p1.x = xStart + (xEnd - xStart) * 0.3f;
        p1.y = yOffset + amplitude * sin(frequency * p1.x + phase + waveOffset + 0.5f);

        p2.x = xStart + (xEnd - xStart) * 0.7f;
        p2.y = yOffset + amplitude * sin(frequency * p2.x + phase + waveOffset - 0.5f);

        p3.x = xEnd;
        p3.y = yOffset + amplitude * sin(frequency * xEnd + phase + waveOffset);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) {
            float t = i / 20.0f;
            Point2D p = calculateBezierPoint(p0, p1, p2, p3, t);
            glVertex3f(p.x, p.y, 0.0f);
        }
        glEnd();
    }
}

void drawBankWaves() {
    if (!wavesActive) {
        glLineWidth(3.0f);
        glColor3f(0.1f, 0.4f, 0.8f);
        glBegin(GL_LINES);
        glVertex3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glEnd();
        return;
    }

    float bankY = 0.0f;
    int numSegments = 30;

    glLineWidth(3.0f);
    glColor3f(0.1f, 0.4f, 0.8f);

    for (int segment = 0; segment < numSegments; segment++) {
        float xStart = -1.0f + segment * (2.0f / numSegments);
        float xEnd = xStart + (2.0f / numSegments);

        Point2D p0, p1, p2, p3;
        p0.x = xStart;
        p0.y = bankY + 0.025f * sin(xStart * 8.0f + waveOffset * 2.0f) * (1.0f - abs(xStart) * 0.3f);

        p1.x = xStart + (xEnd - xStart) * 0.3f;
        p1.y = bankY + 0.025f * sin(p1.x * 8.0f + waveOffset * 2.0f + 0.5f) * (1.0f - abs(p1.x) * 0.3f);

        p2.x = xStart + (xEnd - xStart) * 0.7f;
        p2.y = bankY + 0.025f * sin(p2.x * 8.0f + waveOffset * 2.0f - 0.5f) * (1.0f - abs(p2.x) * 0.3f);

        p3.x = xEnd;
        p3.y = bankY + 0.025f * sin(xEnd * 8.0f + waveOffset * 2.0f) * (1.0f - abs(xEnd) * 0.3f);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 10; i++) {
            float t = i / 10.0f;
            Point2D p = calculateBezierPoint(p0, p1, p2, p3, t);
            glVertex3f(p.x, p.y, 0.0f);
        }
        glEnd();
    }

    glLineWidth(2.0f);
    glColor3f(0.2f, 0.5f, 0.9f);

    for (int segment = 0; segment < numSegments; segment++) {
        float xStart = -1.0f + segment * (2.0f / numSegments);
        float xEnd = xStart + (2.0f / numSegments);

        Point2D p0, p1, p2, p3;
        p0.x = xStart;
        p0.y = bankY + 0.015f * sin(xStart * 12.0f + waveOffset * 1.5f + 1.0f) * (1.0f - abs(xStart) * 0.3f);

        p1.x = xStart + (xEnd - xStart) * 0.3f;
        p1.y = bankY + 0.015f * sin(p1.x * 12.0f + waveOffset * 1.5f + 1.5f) * (1.0f - abs(p1.x) * 0.3f);

        p2.x = xStart + (xEnd - xStart) * 0.7f;
        p2.y = bankY + 0.015f * sin(p2.x * 12.0f + waveOffset * 1.5f + 0.5f) * (1.0f - abs(p2.x) * 0.3f);

        p3.x = xEnd;
        p3.y = bankY + 0.015f * sin(xEnd * 12.0f + waveOffset * 1.5f + 1.0f) * (1.0f - abs(xEnd) * 0.3f);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 10; i++) {
            float t = i / 10.0f;
            Point2D p = calculateBezierPoint(p0, p1, p2, p3, t);
            glVertex3f(p.x, p.y * 0.7f, 0.0f);
        }
        glEnd();
    }

    if (wavesActive) {
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for (float x = -0.9f; x <= 0.9f; x += 0.04f) {
            if (rand() % 100 < 40) {
                float foamY = 0.008f * sin(x * 20.0f + waveOffset * 3.0f);
                glColor3f(1.0f, 1.0f, 1.0f);
                glVertex3f(x, bankY + foamY, 0.0f);
            }
        }
        glEnd();
    }
}

void changeLightingEffect() {
    switch(currentLightingEffect) {
        case 0:
            light0_ambient[0] = 0.2f; light0_ambient[1] = 0.2f; light0_ambient[2] = 0.2f;
            light0_diffuse[0] = 0.9f; light0_diffuse[1] = 0.9f; light0_diffuse[2] = 0.8f;
            light0_specular[0] = 1.0f; light0_specular[1] = 1.0f; light0_specular[2] = 1.0f;

            light1_ambient[0] = 0.1f; light1_ambient[1] = 0.1f; light1_ambient[2] = 0.1f;
            light1_diffuse[0] = 0.6f; light1_diffuse[1] = 0.6f; light1_diffuse[2] = 0.6f;
            light1_specular[0] = 0.5f; light1_specular[1] = 0.5f; light1_specular[2] = 0.5f;
            break;

        case 1:
            light0_ambient[0] = 0.25f; light0_ambient[1] = 0.2f; light0_ambient[2] = 0.1f;
            light0_diffuse[0] = 0.95f; light0_diffuse[1] = 0.85f; light0_diffuse[2] = 0.6f;
            light0_specular[0] = 1.0f; light0_specular[1] = 0.9f; light0_specular[2] = 0.7f;

            light1_ambient[0] = 0.15f; light1_ambient[1] = 0.1f; light1_ambient[2] = 0.05f;
            light1_diffuse[0] = 0.7f; light1_diffuse[1] = 0.6f; light1_diffuse[2] = 0.4f;
            light1_specular[0] = 0.6f; light1_specular[1] = 0.5f; light1_specular[2] = 0.3f;
            break;

        case 2:
            light0_ambient[0] = 0.15f; light0_ambient[1] = 0.15f; light0_ambient[2] = 0.25f;
            light0_diffuse[0] = 0.7f; light0_diffuse[1] = 0.7f; light0_diffuse[2] = 0.95f;
            light0_specular[0] = 0.8f; light0_specular[1] = 0.8f; light0_specular[2] = 1.0f;

            light1_ambient[0] = 0.05f; light1_ambient[1] = 0.05f; light1_ambient[2] = 0.15f;
            light1_diffuse[0] = 0.5f; light1_diffuse[1] = 0.5f; light1_diffuse[2] = 0.7f;
            light1_specular[0] = 0.4f; light1_specular[1] = 0.4f; light1_specular[2] = 0.6f;
            break;

        case 3:
            light0_ambient[0] = 0.3f; light0_ambient[1] = 0.3f; light0_ambient[2] = 0.3f;
            light0_diffuse[0] = 1.0f; light0_diffuse[1] = 1.0f; light0_diffuse[2] = 1.0f;
            light0_specular[0] = 1.0f; light0_specular[1] = 1.0f; light0_specular[2] = 1.0f;

            light1_ambient[0] = 0.2f; light1_ambient[1] = 0.2f; light1_ambient[2] = 0.2f;
            light1_diffuse[0] = 0.8f; light1_diffuse[1] = 0.8f; light1_diffuse[2] = 0.8f;
            light1_specular[0] = 0.7f; light1_specular[1] = 0.7f; light1_specular[2] = 0.7f;
            break;
    }
}

void changeLamppostLightColor() {
    switch(lamppostLightColor) {
        case 0:
            lamppost_light_diffuse[0] = 1.0f;
            lamppost_light_diffuse[1] = 1.0f;
            lamppost_light_diffuse[2] = 0.9f;
            lamppost_light_specular[0] = 1.0f;
            lamppost_light_specular[1] = 1.0f;
            lamppost_light_specular[2] = 0.9f;
            break;
        case 1:
            lamppost_light_diffuse[0] = 1.0f;
            lamppost_light_diffuse[1] = 1.0f;
            lamppost_light_diffuse[2] = 0.7f;
            lamppost_light_specular[0] = 1.0f;
            lamppost_light_specular[1] = 1.0f;
            lamppost_light_specular[2] = 0.7f;
            break;
        case 2:
            lamppost_light_diffuse[0] = 1.0f;
            lamppost_light_diffuse[1] = 0.8f;
            lamppost_light_diffuse[2] = 0.5f;
            lamppost_light_specular[0] = 1.0f;
            lamppost_light_specular[1] = 0.8f;
            lamppost_light_specular[2] = 0.5f;
            break;
        case 3:
            lamppost_light_diffuse[0] = 0.7f;
            lamppost_light_diffuse[1] = 0.8f;
            lamppost_light_diffuse[2] = 1.0f;
            lamppost_light_specular[0] = 0.7f;
            lamppost_light_specular[1] = 0.8f;
            lamppost_light_specular[2] = 1.0f;
            break;
    }
}

void setupLighting() {
    if (!lightingEnabled) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        return;
    }

    changeLightingEffect();
    changeLamppostLightColor();

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glEnable(GL_LIGHT1);

    if (nightMode && lamppostLightOn) {
        for (int i = 0; i < 6; i++) {
            int light_id = GL_LIGHT2 + i;
            glLightfv(light_id, GL_POSITION, lamppost_light_positions[i]);
            glLightfv(light_id, GL_DIFFUSE, lamppost_light_diffuse);
            glLightfv(light_id, GL_SPECULAR, lamppost_light_specular);
            glLightf(light_id, GL_SPOT_CUTOFF, 45.0f);
            glLightfv(light_id, GL_SPOT_DIRECTION, lamppost_spot_direction);
            glLightf(light_id, GL_SPOT_EXPONENT, 2.0f);
            glLightf(light_id, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(light_id, GL_LINEAR_ATTENUATION, 0.05f);
            glLightf(light_id, GL_QUADRATIC_ATTENUATION, 0.01f);
            glEnable(light_id);
        }
    } else {
        for (int i = 0; i < 6; i++) {
            glDisable(GL_LIGHT2 + i);
        }
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
}

void drawWaterSurface() {
    glNormal3f(0.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
    if (nightMode) {
        glColor3f(0.0f, 0.1f, 0.3f);
    } else {
        glColor3f(0.0f, 0.3f, 0.8f);
    }
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    if (nightMode) {
        glColor3f(0.0f, 0.2f, 0.5f);
    } else {
        glColor3f(0.0f, 0.5f, 1.0f);
    }
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glEnd();

    if (wavesActive) {
        drawBezierWave(-0.6f, 0.02f, 10.0f, 0.0f);
        drawBezierWave(-0.62f, 0.015f, 12.0f, 1.5f);
    } else {
        glLineWidth(2.0f);
        glColor3f(0.0f, 0.5f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(-1.0f, -0.6f, 0.0f);
        glVertex3f(1.0f, -0.6f, 0.0f);
        glVertex3f(-1.0f, -0.62f, 0.0f);
        glVertex3f(1.0f, -0.62f, 0.0f);
        glEnd();
    }

    drawBankWaves();

    glColor3f(0.0f, 0.5f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (float x = -1.0f; x <= 1.0f; x += 0.01f) {
        float wave = wavesActive ? sin(x * 10.0f + boatX * 5.0f) * 0.02f : 0.0f;
        float normal_x = wavesActive ? -cos(x * 10.0f + boatX * 5.0f) * 0.2f : 0.0f;
        glNormal3f(normal_x, 1.0f, 0.0f);
        glVertex3f(x, wave - 0.6f, 0.0f);
    }
    glEnd();
}

void drawSmallBoat() {
    glPushMatrix();
    glTranslatef(boatX, -0.6f, 0.0f);
    glScalef(0.6f, 0.6f, 1.0f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glBegin(GL_POLYGON);
    glColor3f(0.5f, 0.2f, 0.0f);
    glVertex3f(-0.3f, -0.2f, 0.0f);
    glVertex3f(0.3f, -0.2f, 0.0f);
    glColor3f(0.6f, 0.3f, 0.1f);
    glVertex3f(0.2f, -0.3f, 0.0f);
    glVertex3f(-0.2f, -0.3f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.5f, 0.3f);
    glVertex3f(-0.25f, -0.2f, 0.0f);
    glVertex3f(0.25f, -0.2f, 0.0f);
    glVertex3f(0.25f, -0.1f, 0.0f);
    glVertex3f(-0.25f, -0.1f, 0.0f);
    glEnd();

    glColor3f(0.5f, 0.3f, 0.1f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for(int i = 0; i < 5; i++) {
        float x = -0.23f + i * 0.1f;
        glVertex3f(x, -0.2f, 0.0f);
        glVertex3f(x, -0.1f, 0.0f);
    }
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.6f, 0.4f);
    glVertex3f(-0.15f, -0.1f, 0.0f);
    glVertex3f(0.05f, -0.1f, 0.0f);
    glVertex3f(0.05f, 0.05f, 0.0f);
    glVertex3f(-0.15f, 0.05f, 0.0f);
    glEnd();

    glColor3f(0.3f, 0.5f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(-0.13f, -0.08f, 0.0f);
    glVertex3f(-0.07f, -0.08f, 0.0f);
    glVertex3f(-0.07f, 0.0f, 0.0f);
    glVertex3f(-0.13f, 0.0f, 0.0f);
    glEnd();

    glNormal3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.3f);
    glVertex3f(-0.05f, -0.1f, 0.0f);
    glVertex3f(-0.03f, -0.1f, 0.0f);
    glVertex3f(-0.03f, 0.15f, 0.0f);
    glVertex3f(-0.05f, 0.15f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-0.03f, 0.05f, 0.0f);
    glColor3f(0.9f, 0.9f, 1.0f);
    glVertex3f(-0.03f, 0.15f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.1f, 0.05f, 0.0f);
    glEnd();

    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.03f, 0.05f, 0.0f);
    glVertex3f(-0.03f, 0.15f, 0.0f);
    glVertex3f(-0.03f, 0.15f, 0.0f);
    glVertex3f(0.1f, 0.05f, 0.0f);
    glVertex3f(-0.03f, 0.05f, 0.0f);
    glVertex3f(0.1f, 0.05f, 0.0f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glVertex3f(-0.25f, -0.1f, 0.0f);
    glVertex3f(0.25f, -0.1f, 0.0f);
    glEnd();

    glPopMatrix();
}

void drawBoat2() {
    glPushMatrix();
    glTranslatef(boat2X, -0.75f, 0.0f);
    glScalef(0.4f, 0.4f, 1.0f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glBegin(GL_POLYGON);
    glColor3f(0.2f, 0.4f, 0.1f);
    glVertex3f(-0.3f, -0.2f, 0.0f);
    glVertex3f(0.3f, -0.2f, 0.0f);
    glVertex3f(0.2f, -0.3f, 0.0f);
    glVertex3f(-0.2f, -0.3f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.6f, 0.2f);
    glVertex3f(-0.25f, -0.2f, 0.0f);
    glVertex3f(0.25f, -0.2f, 0.0f);
    glVertex3f(0.25f, -0.05f, 0.0f);
    glVertex3f(-0.25f, -0.05f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.8f, 0.9f);
    glVertex3f(-0.1f, -0.05f, 0.0f);
    glVertex3f(0.1f, -0.05f, 0.0f);
    glVertex3f(0.1f, 0.05f, 0.0f);
    glVertex3f(-0.1f, 0.05f, 0.0f);
    glEnd();

    glColor3f(0.4f, 0.6f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-0.08f, -0.02f, 0.0f);
    glVertex3f(-0.03f, -0.02f, 0.0f);
    glVertex3f(-0.03f, 0.02f, 0.0f);
    glVertex3f(-0.08f, 0.02f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(0.03f, -0.02f, 0.0f);
    glVertex3f(0.08f, -0.02f, 0.0f);
    glVertex3f(0.08f, 0.02f, 0.0f);
    glVertex3f(0.03f, 0.02f, 0.0f);
    glEnd();

    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(0.2f, 0.0f, 0.0f);
    glVertex3f(0.3f, 0.1f, 0.0f);
    glEnd();

    glPopMatrix();
}

void drawBoat3() {
    glPushMatrix();
    glTranslatef(boat3X, -0.65f, 0.0f);
    glScalef(0.5f, 0.5f, 1.0f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glBegin(GL_POLYGON);
    glColor3f(0.6f, 0.3f, 0.0f);
    glVertex3f(-0.3f, -0.2f, 0.0f);
    glVertex3f(0.3f, -0.2f, 0.0f);
    glVertex3f(0.2f, -0.3f, 0.0f);
    glVertex3f(-0.2f, -0.3f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.5f, 0.2f);
    glVertex3f(-0.25f, -0.2f, 0.0f);
    glVertex3f(0.25f, -0.2f, 0.0f);
    glVertex3f(0.25f, -0.1f, 0.0f);
    glVertex3f(-0.25f, -0.1f, 0.0f);
    glEnd();

    glNormal3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.4f, 0.4f);
    glVertex3f(-0.05f, -0.1f, 0.0f);
    glVertex3f(-0.03f, -0.1f, 0.0f);
    glVertex3f(-0.03f, 0.1f, 0.0f);
    glVertex3f(-0.05f, 0.1f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex3f(-0.03f, 0.0f, 0.0f);
    glVertex3f(-0.03f, 0.1f, 0.0f);
    glVertex3f(0.08f, 0.0f, 0.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glVertex3f(-0.2f, -0.15f, 0.0f);
    glVertex3f(0.2f, -0.15f, 0.0f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex3f(0.18f, -0.25f, 0.0f);
    glVertex3f(0.22f, -0.25f, 0.0f);
    glVertex3f(0.22f, -0.2f, 0.0f);
    glVertex3f(0.18f, -0.2f, 0.0f);
    glEnd();

    glPopMatrix();
}

void drawCargoShip() {
    glPushMatrix();
    glTranslatef(shipX, -0.9f, 0.0f);
    glScalef(0.5f, 0.5f, 1.0f);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    glColor3f(0.1f, 0.1f, 0.3f);
    glVertex3f(-0.4f, -0.1f, 0.0f);
    glVertex3f(0.4f, -0.1f, 0.0f);
    glColor3f(0.2f, 0.2f, 0.5f);
    glVertex3f(0.35f, -0.2f, 0.0f);
    glVertex3f(-0.35f, -0.2f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.6f);
    glVertex3f(-0.35f, -0.1f, 0.0f);
    glVertex3f(0.35f, -0.1f, 0.0f);
    glVertex3f(0.35f, 0.1f, 0.0f);
    glVertex3f(-0.35f, 0.1f, 0.0f);
    glEnd();

    float containerColors[][3] = {
        {0.8f, 0.2f, 0.2f},
        {0.2f, 0.8f, 0.2f},
        {0.2f, 0.2f, 0.8f},
        {0.8f, 0.8f, 0.2f},
        {0.8f, 0.2f, 0.8f},
        {0.2f, 0.8f, 0.8f}
    };

    for (int stack = 0; stack < 2; stack++) {
        for (int i = 0; i < 3; i++) {
            glNormal3f(0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS);
            glColor3fv(containerColors[(i+stack)%6]);
            glVertex3f(-0.3f + i * 0.25f, 0.1f + stack * 0.15f, 0.0f);
            glVertex3f(-0.15f + i * 0.25f, 0.1f + stack * 0.15f, 0.0f);
            glVertex3f(-0.15f + i * 0.25f, 0.2f + stack * 0.15f, 0.0f);
            glVertex3f(-0.3f + i * 0.25f, 0.2f + stack * 0.15f, 0.0f);
            glEnd();

            glColor3f(0.1f, 0.1f, 0.1f);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            glVertex3f(-0.3f + i * 0.25f, 0.1f + stack * 0.15f, 0.0f);
            glVertex3f(-0.15f + i * 0.25f, 0.1f + stack * 0.15f, 0.0f);
            glVertex3f(-0.15f + i * 0.25f, 0.2f + stack * 0.15f, 0.0f);
            glVertex3f(-0.3f + i * 0.25f, 0.2f + stack * 0.15f, 0.0f);
            glEnd();
        }
    }

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.4f, 0.4f);
    glVertex3f(0.05f, 0.2f, 0.0f);
    glVertex3f(0.15f, 0.2f, 0.0f);
    glVertex3f(0.15f, 0.3f, 0.0f);
    glVertex3f(0.05f, 0.3f, 0.0f);
    glEnd();

    glColor3f(0.7f, 0.8f, 1.0f);
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            glBegin(GL_QUADS);
            glVertex3f(0.06f + i * 0.04f, 0.22f + j * 0.03f, 0.0f);
            glVertex3f(0.09f + i * 0.04f, 0.22f + j * 0.03f, 0.0f);
            glVertex3f(0.09f + i * 0.04f, 0.25f + j * 0.03f, 0.0f);
            glVertex3f(0.06f + i * 0.04f, 0.25f + j * 0.03f, 0.0f);
            glEnd();
        }
    }

    glPushMatrix();
    glTranslatef(0.1f, 0.35f, 0.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.05f * cos(angle), 0.05f * sin(angle), 0.0f);
    }
    glEnd();

    if(boatActive) {
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i <= segments; i++) {
            float angle = i * twoPi / segments;
            glVertex3f(0.03f * cos(angle), 0.1f + 0.02f * sin(angle), 0.0f);
        }
        glEnd();
    }
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.08f, -0.05f);
    std::string shipName = "CARGO";
    for (char c : shipName) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);

    glPopMatrix();
}

void drawPrivateCar() {
    glPushMatrix();
    glTranslatef(carX, 0.48f, 0.0f);
    glScalef(0.3f, 0.2f, 1.0f);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.8f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
    glVertex3f(-0.4f, -0.08f, 0.0f);
    glVertex3f(0.4f, -0.08f, 0.0f);
    glVertex3f(0.4f, 0.12f, 0.0f);
    glVertex3f(-0.4f, 0.12f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.9f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-0.3f, 0.12f, 0.0f);
    glVertex3f(0.3f, 0.12f, 0.0f);
    glVertex3f(0.3f, 0.25f, 0.0f);
    glVertex3f(-0.3f, 0.25f, 0.0f);
    glEnd();

    if (lightingEnabled) {
        GLfloat window_specular[] = {0.8f, 0.8f, 1.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, window_specular);
    }

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.3f, 0.3f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-0.25f, 0.12f, 0.0f);
    glVertex3f(0.0f, 0.12f, 0.0f);
    glVertex3f(0.0f, 0.25f, 0.0f);
    glVertex3f(-0.25f, 0.25f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(0.05f, 0.12f, 0.0f);
    glVertex3f(0.25f, 0.12f, 0.0f);
    glVertex3f(0.25f, 0.25f, 0.0f);
    glVertex3f(0.05f, 0.25f, 0.0f);
    glEnd();

    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    }

    glColor3f(0.1f, 0.1f, 0.1f);

    glPushMatrix();
    glTranslatef(-0.25f, -0.1f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.08f * cos(angle), 0.08f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.25f, -0.1f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.08f * cos(angle), 0.08f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    if (nightMode && carHeadlightOn) {
        if (lightingEnabled) {
            GLfloat headlight_emission[] = {0.8f, 0.8f, 0.4f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, headlight_emission);
        }

        glColor3f(1.0f, 1.0f, 0.8f);
        glBegin(GL_TRIANGLES);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.4f, 0.02f, 0.0f);
        glVertex3f(0.5f, 0.08f, 0.0f);
        glVertex3f(0.5f, -0.04f, 0.0f);
        glEnd();

        glColor3f(0.9f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.38f, -0.04f, 0.0f);
        glVertex3f(-0.35f, -0.04f, 0.0f);
        glVertex3f(-0.35f, -0.01f, 0.0f);
        glVertex3f(-0.38f, -0.01f, 0.0f);
        glEnd();

        if (lightingEnabled) {
            GLfloat no_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
        }
    }

    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.15f, 0.12f, 0.0f);
    glVertex3f(-0.15f, 0.25f, 0.0f);
    glVertex3f(0.15f, 0.12f, 0.0f);
    glVertex3f(0.15f, 0.25f, 0.0f);
    glEnd();

    glPopMatrix();
}

void drawCar2() {
    glPushMatrix();
    glTranslatef(car2X, 0.48f, 0.0f);
    glScalef(0.25f, 0.18f, 1.0f);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.1f, 0.6f, 0.2f);
    glBegin(GL_QUADS);
    glVertex3f(-0.4f, -0.08f, 0.0f);
    glVertex3f(0.4f, -0.08f, 0.0f);
    glVertex3f(0.4f, 0.12f, 0.0f);
    glVertex3f(-0.4f, 0.12f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.9f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-0.3f, 0.12f, 0.0f);
    glVertex3f(0.3f, 0.12f, 0.0f);
    glVertex3f(0.3f, 0.25f, 0.0f);
    glVertex3f(-0.3f, 0.25f, 0.0f);
    glEnd();

    if (lightingEnabled) {
        GLfloat window_specular[] = {0.8f, 0.8f, 1.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, window_specular);
    }

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.3f, 0.3f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-0.2f, 0.12f, 0.0f);
    glVertex3f(0.1f, 0.12f, 0.0f);
    glVertex3f(0.1f, 0.25f, 0.0f);
    glVertex3f(-0.2f, 0.25f, 0.0f);
    glEnd();

    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    }

    glColor3f(0.1f, 0.1f, 0.1f);

    glPushMatrix();
    glTranslatef(-0.25f, -0.1f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.07f * cos(angle), 0.07f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.25f, -0.1f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.07f * cos(angle), 0.07f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    if (nightMode && carHeadlightOn) {
        if (lightingEnabled) {
            GLfloat headlight_emission[] = {0.8f, 0.8f, 0.4f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, headlight_emission);
        }

        glColor3f(1.0f, 1.0f, 0.8f);
        glBegin(GL_TRIANGLES);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.4f, 0.02f, 0.0f);
        glVertex3f(0.5f, 0.08f, 0.0f);
        glVertex3f(0.5f, -0.04f, 0.0f);
        glEnd();

        glColor3f(0.9f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.38f, -0.04f, 0.0f);
        glVertex3f(-0.35f, -0.04f, 0.0f);
        glVertex3f(-0.35f, -0.01f, 0.0f);
        glVertex3f(-0.38f, -0.01f, 0.0f);
        glEnd();

        if (lightingEnabled) {
            GLfloat no_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
        }
    }

    glPopMatrix();
}

void drawCar3() {
    glPushMatrix();
    glTranslatef(car3X, 0.48f, 0.0f);
    glScalef(0.35f, 0.22f, 1.0f);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.0f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(-0.4f, -0.08f, 0.0f);
    glVertex3f(0.4f, -0.08f, 0.0f);
    glVertex3f(0.4f, 0.12f, 0.0f);
    glVertex3f(-0.4f, 0.12f, 0.0f);
    glEnd();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.9f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-0.3f, 0.12f, 0.0f);
    glVertex3f(0.3f, 0.12f, 0.0f);
    glVertex3f(0.3f, 0.25f, 0.0f);
    glVertex3f(-0.3f, 0.25f, 0.0f);
    glEnd();

    if (lightingEnabled) {
        GLfloat window_specular[] = {0.8f, 0.8f, 1.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, window_specular);
    }

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(0.3f, 0.3f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-0.25f, 0.12f, 0.0f);
    glVertex3f(0.0f, 0.12f, 0.0f);
    glVertex3f(0.0f, 0.25f, 0.0f);
    glVertex3f(-0.25f, 0.25f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(0.05f, 0.12f, 0.0f);
    glVertex3f(0.25f, 0.12f, 0.0f);
    glVertex3f(0.25f, 0.25f, 0.0f);
    glVertex3f(0.05f, 0.25f, 0.0f);
    glEnd();

    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    }

    glColor3f(0.1f, 0.1f, 0.1f);

    glPushMatrix();
    glTranslatef(-0.25f, -0.1f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.09f * cos(angle), 0.09f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.25f, -0.1f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.09f * cos(angle), 0.09f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    if (nightMode && carHeadlightOn) {
        if (lightingEnabled) {
            GLfloat headlight_emission[] = {0.8f, 0.8f, 0.4f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, headlight_emission);
        }

        glColor3f(1.0f, 1.0f, 0.8f);
        glBegin(GL_TRIANGLES);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.4f, 0.02f, 0.0f);
        glVertex3f(0.5f, 0.08f, 0.0f);
        glVertex3f(0.5f, -0.04f, 0.0f);
        glEnd();

        glColor3f(0.9f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.38f, -0.04f, 0.0f);
        glVertex3f(-0.35f, -0.04f, 0.0f);
        glVertex3f(-0.35f, -0.01f, 0.0f);
        glVertex3f(-0.38f, -0.01f, 0.0f);
        glEnd();

        if (lightingEnabled) {
            GLfloat no_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
        }
    }

    glPopMatrix();
}

void drawTreeAt(float x, float y, float size, bool isPine) {
    glPushMatrix();
    glTranslatef(x, y + 0.5f, 0.0f);
    glScalef(size, size, 1.0f);

    glBegin(GL_QUADS);
    if (nightMode) glColor3f(0.3f, 0.15f, 0.0f);
    else glColor3f(0.4f, 0.2f, 0.0f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.02f, 0.0f, 0.0f);
    glVertex3f(0.02f, 0.0f, 0.0f);
    glVertex3f(0.02f, 0.2f, 0.0f);
    glVertex3f(-0.02f, 0.2f, 0.0f);
    glEnd();

    if (isPine) {
        glBegin(GL_TRIANGLES);
        if (nightMode) glColor3f(0.0f, 0.25f, 0.0f);
        else glColor3f(0.0f, 0.5f, 0.0f);

        glNormal3f(0.0f, -0.5f, 1.0f);
        glVertex3f(0.0f, 0.35f, 0.0f);
        glVertex3f(-0.15f, 0.2f, 0.0f);
        glVertex3f(0.15f, 0.2f, 0.0f);

        glNormal3f(0.0f, -0.3f, 1.0f);
        glVertex3f(0.0f, 0.45f, 0.0f);
        glVertex3f(-0.12f, 0.3f, 0.0f);
        glVertex3f(0.12f, 0.3f, 0.0f);

        glNormal3f(0.0f, -0.1f, 1.0f);
        glVertex3f(0.0f, 0.55f, 0.0f);
        glVertex3f(-0.09f, 0.4f, 0.0f);
        glVertex3f(0.09f, 0.4f, 0.0f);
        glEnd();
    } else {
        if (nightMode) glColor3f(0.0f, 0.3f, 0.0f);
        else glColor3f(0.0f, 0.6f, 0.0f);

        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i <= segments; i++) {
            float angle = i * twoPi / segments;
            float nx = cos(angle);
            float ny = sin(angle);
            glNormal3f(nx, ny, 0.5f);
            glVertex3f(0.12f * cos(angle), 0.25f + 0.12f * sin(angle), 0.0f);
        }
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i <= segments; i++) {
            float angle = i * twoPi / segments;
            float nx = cos(angle);
            float ny = sin(angle);
            glNormal3f(nx, ny, 0.5f);
            glVertex3f(0.05f + 0.1f * cos(angle), 0.35f + 0.1f * sin(angle), 0.0f);
        }
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i <= segments; i++) {
            float angle = i * twoPi / segments;
            float nx = cos(angle);
            float ny = sin(angle);
            glNormal3f(nx, ny, 0.5f);
            glVertex3f(-0.05f + 0.1f * cos(angle), 0.35f + 0.1f * sin(angle), 0.0f);
        }
        glEnd();
    }
    glPopMatrix();
}

void drawStreetLamp(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y + 0.5f, 0.0f);

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.01f, 0.0f, 0.0f);
    glVertex3f(0.01f, 0.0f, 0.0f);
    glVertex3f(0.01f, 0.3f, 0.0f);
    glVertex3f(-0.01f, 0.3f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.01f, 0.25f, 0.0f);
    glVertex3f(0.1f, 0.25f, 0.0f);
    glVertex3f(0.1f, 0.26f, 0.0f);
    glVertex3f(0.01f, 0.26f, 0.0f);
    glEnd();

    if (nightMode && lamppostLightOn) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(lamppost_light_diffuse[0],
                 lamppost_light_diffuse[1],
                 lamppost_light_diffuse[2], 0.3f);
        glBegin(GL_TRIANGLES);
        glVertex3f(0.1f, 0.255f, 0.0f);
        glVertex3f(0.15f, 0.1f, 0.0f);
        glVertex3f(0.05f, 0.1f, 0.0f);
        glEnd();

        glColor4f(lamppost_light_diffuse[0],
                 lamppost_light_diffuse[1],
                 lamppost_light_diffuse[2], 0.15f);
        glBegin(GL_TRIANGLES);
        glVertex3f(0.1f, 0.255f, 0.0f);
        glVertex3f(0.2f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glEnd();

        glDisable(GL_BLEND);
    }

    if (nightMode && lamppostLightOn) {
        glColor3f(lamppost_light_diffuse[0],
                 lamppost_light_diffuse[1],
                 lamppost_light_diffuse[2]);
    } else if (nightMode) {
        glColor3f(0.4f, 0.4f, 0.3f);
    } else {
        glColor3f(0.6f, 0.6f, 0.4f);
    }

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.1f + 0.03f * cos(angle), 0.255f + 0.03f * sin(angle), 0.0f);
    }
    glEnd();

    glPopMatrix();
}

void drawCloud(float x, float y, float size, float intensity) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(size, size, 1.0f);

    glColor3f(intensity, intensity, intensity);

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.1f * cos(angle), 0.08f * sin(angle), 0.0f);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.08f + 0.07f * cos(angle), 0.03f + 0.06f * sin(angle), 0.0f);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(-0.08f + 0.07f * cos(angle), 0.03f + 0.06f * sin(angle), 0.0f);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(0.06f * cos(angle), 0.06f + 0.05f * sin(angle), 0.0f);
    }
    glEnd();

    glPopMatrix();
}

void drawRoadSection() {
    glBegin(GL_QUADS);
    if (nightMode) glColor3f(0.2f, 0.2f, 0.2f);
    else glColor3f(0.3f, 0.3f, 0.3f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.5f, 0.0f);
    glVertex3f(-1.0f, 0.5f, 0.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    for (float mark = -0.9f; mark < 0.9f; mark += 0.2f) {
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(mark, 0.25f, 0.01f);
        glVertex3f(mark + 0.1f, 0.25f, 0.01f);
        glVertex3f(mark + 0.1f, 0.26f, 0.01f);
        glVertex3f(mark, 0.26f, 0.01f);
        glEnd();
    }

    if (nightMode) glColor3f(0.4f, 0.4f, 0.4f);
    else glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.5f, 0.0f);
    glVertex3f(1.0f, 0.5f, 0.0f);
    glVertex3f(1.0f, 0.55f, 0.0f);
    glVertex3f(-1.0f, 0.55f, 0.0f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-1.0f, -0.05f, 0.0f);
    glVertex3f(1.0f, -0.05f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glEnd();

    if (nightMode) glColor3f(0.0f, 0.3f, 0.0f);
    else glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 0.55f, 0.0f);
    glVertex3f(1.0f, 0.55f, 0.0f);
    glVertex3f(1.0f, 0.7f, 0.0f);
    glVertex3f(-1.0f, 0.7f, 0.0f);

    glVertex3f(-1.0f, -0.2f, 0.0f);
    glVertex3f(1.0f, -0.2f, 0.0f);
    glVertex3f(1.0f, -0.05f, 0.0f);
    glVertex3f(-1.0f, -0.05f, 0.0f);
    glEnd();
}

void drawFlyingDrone() {
    glPushMatrix();
    glTranslatef(dronePosX, dronePosY, 0.0f);

    glBegin(GL_QUADS);
    if (nightMode) glColor3f(0.3f, 0.3f, 0.3f);
    else glColor3f(0.1f, 0.1f, 0.1f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.03f, -0.01f, 0.0f);
    glVertex3f(0.03f, -0.01f, 0.0f);
    glVertex3f(0.03f, 0.01f, 0.0f);
    glVertex3f(-0.03f, 0.01f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.05f, 0.0f, 0.0f);
    glVertex3f(0.05f, 0.0f, 0.0f);
    glVertex3f(0.05f, 0.005f, 0.0f);
    glVertex3f(-0.05f, 0.005f, 0.0f);

    glVertex3f(0.0f, -0.05f, 0.0f);
    glVertex3f(0.005f, -0.05f, 0.0f);
    glVertex3f(0.005f, 0.05f, 0.0f);
    glVertex3f(0.0f, 0.05f, 0.0f);
    glEnd();

    glPushMatrix();
    glTranslatef(0.05f, 0.0f, 0.0f);
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments + propellerAngle;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.015f * cos(angle), 0.015f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.05f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments - propellerAngle;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.015f * cos(angle), 0.015f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments + propellerAngle;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.015f * cos(angle), 0.015f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments - propellerAngle;
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.015f * cos(angle), 0.015f * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    if (nightMode && lightingEnabled) {
        GLfloat drone_light_emission[] = {0.9f, 0.8f, 0.3f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, drone_light_emission);
    }

    glBegin(GL_TRIANGLE_FAN);
    if (nightMode) glColor3f(1.0f, 0.8f, 0.2f);
    else glColor3f(1.0f, 0.9f, 0.3f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = i * twoPi / segments;
        glVertex3f(0.035f + 0.006f * cos(angle), 0.006f * sin(angle), 0.0f);
    }
    glEnd();

    if (nightMode && lightingEnabled) {
        GLfloat no_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
    }

    glPopMatrix();
}

void updateScene(int value) {
    if (boatActive) {
        boatX += 0.005f;
        shipX -= 0.003f;
        boat2X += 0.004f;
        boat3X -= 0.003f;

        if (boatX > 1.3f) boatX = -1.3f;
        if (shipX < -1.3f) shipX = 1.3f;
        if (boat2X > 1.3f) boat2X = -1.3f;
        if (boat3X < -1.3f) boat3X = 1.3f;
    }

    cloud1X -= 0.001f;
    cloud2X -= 0.0008f;
    cloud3X -= 0.0012f;

    if (cloud1X < -1.5f) cloud1X = 1.5f;
    if (cloud2X < -1.5f) cloud2X = 1.5f;
    if (cloud3X < -1.5f) cloud3X = 1.5f;

    if (droneActive) {
        propellerAngle += 0.2f;
        dronePosX += droneVelocity;
        dronePosY = 0.7f + 0.05f * sin(dronePosX * 3.0f);

        if (dronePosX > 0.9f || dronePosX < -0.9f) {
            droneVelocity = -droneVelocity;
        }
    }

    if (carActive) {
        carX += carSpeed;
        car2X += car2Speed;
        car3X += car3Speed;
        if (carX > 1.5f) carX = -1.5f;
        if (car2X > 1.5f) car2X = -1.5f;
        if (car3X > 1.5f) car3X = -1.5f;
    }

    if (wavesActive) {
        waveOffset += 0.02f;
    }

    if (nightMode) {
        light0_position[0] = -0.5f;
        light0_position[1] = 1.5f;
    } else {
        light0_position[0] = 0.5f;
        light0_position[1] = 1.8f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, updateScene, 0);
}

void renderDisplay() {
    if (nightMode) glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
    else glClearColor(0.4f, 0.69f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupLighting();

    drawWaterSurface();
    drawCargoShip();
    drawSmallBoat();
    drawBoat2();
    drawBoat3();

    glBegin(GL_QUADS);
    if (nightMode) glColor3f(0.0f, 0.1f, 0.2f);
    else glColor3f(0.4f, 0.69f, 1.0f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 0.7f, 0.0f);
    glVertex3f(1.0f, 0.7f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();

    float cloudBright = nightMode ? 0.3f : 0.95f;
    drawCloud(cloud1X, 0.85f, 0.8f, cloudBright);
    drawCloud(cloud2X - 0.5f, 0.9f, 0.6f, cloudBright * 0.9f);
    drawCloud(cloud3X + 0.3f, 0.88f, 0.7f, cloudBright * 0.8f);
    drawCloud(cloud1X + 0.7f, 0.83f, 0.5f, cloudBright * 0.7f);

    glBegin(GL_QUADS);
    if (nightMode) glColor3f(0.0f, 0.2f, 0.0f);
    else glColor3f(0.0f, 0.5f, 0.2f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.7f, 0.0f);
    glVertex3f(-1.0f, 0.7f, 0.0f);
    glEnd();

    drawFlyingDrone();
    drawRoadSection();
    drawPrivateCar();
    drawCar2();
    drawCar3();

    drawTreeAt(-0.8f, 0.21f, 0.5f, true);
    drawTreeAt(-0.6f, 0.22f, 0.6f, false);
    drawTreeAt(-0.4f, 0.21f, 0.45f, true);
    drawTreeAt(-0.2f, 0.23f, 0.65f, false);
    drawTreeAt(0.8f, 0.22f, 0.55f, true);
    drawTreeAt(0.6f, 0.21f, 0.48f, false);
    drawTreeAt(0.4f, 0.23f, 0.62f, true);
    drawTreeAt(0.2f, 0.22f, 0.5f, false);

    for(float lampPos = -0.9f; lampPos <= 0.9f; lampPos += 0.3f) {
        drawStreetLamp(lampPos, 0.05f);
    }

    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2f(-0.95f, 0.9f);
    std::string nightText = "Night: " + std::string(nightMode ? "ON" : "OFF") + " (N)";
    for (char c : nightText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, 0.85f);
    std::string lightText = "Lighting: " + std::string(lightingEnabled ? "ON" : "OFF") + " (L)";
    for (char c : lightText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, 0.8f);
    std::string carText = "Car: " + std::string(carActive ? "Moving" : "Stopped") + " (C)";
    for (char c : carText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, 0.75f);
    std::string lamppostText = "Lamppost: " + std::string(lamppostLightOn ? "ON" : "OFF") + " (P)";
    for (char c : lamppostText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, 0.7f);
    std::string headlightText = "Headlights: " + std::string(carHeadlightOn ? "ON" : "OFF") + " (H)";
    for (char c : headlightText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, 0.65f);
    std::string effectText = "Light Effect: " + std::to_string(currentLightingEffect) + " (E)";
    for (char c : effectText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, 0.6f);
    std::string lampColorText = "Lamp Color: ";
    switch(lamppostLightColor) {
        case 0: lampColorText += "White"; break;
        case 1: lampColorText += "Yellow"; break;
        case 2: lampColorText += "Orange"; break;
        case 3: lampColorText += "Blue"; break;
    }
    lampColorText += " (K)";
    for (char c : lampColorText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, 0.55f);
    std::string wavesText = "Waves: " + std::string(wavesActive ? "ON" : "OFF") + " (W)";
    for (char c : wavesText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.2f, 0.9f);
    std::string statusText = "Boats: " + std::string(boatActive ? "Moving" : "Stopped") + " (SPACE)";
    for (char c : statusText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    glRasterPos2f(0.6f, 0.9f);
    std::string droneText = "Drone: " + std::string(droneActive ? "On" : "Off") + " (D)";
    for (char c : droneText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glRasterPos2f(-0.95f, -0.9f);
    std::string coordText = "Boat: " + std::to_string(boatX).substr(0,4) +
                           "  Ship: " + std::to_string(shipX).substr(0,4) +
                           "  Car: " + std::to_string(carX).substr(0,4);
    for (char c : coordText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glutSwapBuffers();
}

void handleKeyboard(unsigned char key, int x, int y) {
    if (key == ' ') boatActive = !boatActive;
    if (key == 'n' || key == 'N') nightMode = !nightMode;
    if (key == 'd' || key == 'D') droneActive = !droneActive;
    if (key == 'c' || key == 'C') carActive = !carActive;
    if (key == 'l' || key == 'L') lightingEnabled = !lightingEnabled;
    if (key == 'p' || key == 'P') lamppostLightOn = !lamppostLightOn;
    if (key == 'h' || key == 'H') carHeadlightOn = !carHeadlightOn;
    if (key == 'e' || key == 'E') {
        currentLightingEffect = (currentLightingEffect + 1) % 4;
        changeLightingEffect();
    }
    if (key == 'k' || key == 'K') {
        lamppostLightColor = (lamppostLightColor + 1) % 4;
        changeLamppostLightColor();
    }
    if (key == 'w' || key == 'W') wavesActive = !wavesActive;  // Toggle all waves
    if (key == 27) exit(0);
    glutPostRedisplay();
}

void initializeGL() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glPointSize(3.0f);
    glLineWidth(2.0f);

    setupLighting();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Marine Cox's Bazar ");

    currentLightingEffect = 0;
    changeLightingEffect();

    lamppostLightColor = 0;
    changeLamppostLightColor();

    initializeGL();
    glutDisplayFunc(renderDisplay);
    glutKeyboardFunc(handleKeyboard);
    glutTimerFunc(16, updateScene, 0);

    glutMainLoop();
    return 0;
}
