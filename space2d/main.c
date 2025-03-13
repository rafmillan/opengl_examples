#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif


#define TARGET_FPS 60
#define FRAME_TIME (1.0 / TARGET_FPS)  // 1/60 seconds per frame

// Time Scale Macro: Modify this value to speed up (>1) or slow down (<1) simulation time.
#define TIME_SCALE 1000.0

#define SCALE_FACTOR 1000.0

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define NUM_SEGMENTS 100  // Number of segments for smoothness

// Physics Constants
#define G       0.01
#define GRAVITY -9.8          // Gravity (pixels per second^2)
#define RESTITUTION 1       // Bounce energy loss (0 = no bounce, 1 = perfect bounce)
#define DRAG_COEFF 0.00       // Air resistance factor
#define FRICTION 0.00       // Ground friction (reduces horizontal speed on bounce)
#define BALL_RADIUS  50        // Ball size


typedef struct {
    float r;
    float g;
    float b;
} _color;

// Ball Properties
typedef struct {
    float x, y;
    float vx, vy;
    float mass;
    float radius;
    _color color;
} Body;

// Timing Variables
double get_time() {
    return glfwGetTime();  // Get time in seconds
}

void drawCircle(float cx, float cy, float r, int num_segments, _color color) {
    glBegin(GL_TRIANGLE_FAN);
    //glColor3f(1.0f, 0.5f, 0.0f);
    glColor3f(color.r, color.g, color.b);
    glVertex2f(cx, cy);  // Center of the circle

    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * M_PI * i / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void updatePhysics(Body* m2, Body* m1, float dt) {
    float dx = m1->x - m2->x;
    float dy = m1->y - m2->y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance < (m1->radius + m2->radius)) {
        return;
    }

    float force = G * (m1->mass * m2->mass) / (distance * distance);
    float ax = force * (dx / distance) / m2->mass;
    float ay = force * (dy / distance) / m2->mass;

    m2->vx += ax * dt;
    m2->vy += ay * dt;
    m2->x += m2->vx * dt;
    m2->y += m2->vy * dt;
}

float get_init_vy(float m1, float m2, float r) {
    return sqrt( G * m2 / r);
}

void drawBody(Body* body) {
    drawCircle(body->x, body->y, body->radius, NUM_SEGMENTS, body->color);
}

int main() {

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

#if ENABLE_VSYNC
    // Enable VSYNC
    glfwSwapInterval(1);
#endif

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);  // 2D Projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float s_m = 100000;
    float p0_m = 100;
    float m0_m = 1;

    float r0 = 400;
    float r1 = 200;

    float s_x0 = SCREEN_WIDTH / 2;
    float s_y0 = SCREEN_HEIGHT / 2;
    float p0_x0 = s_x0 + r0;
    float p0_y0 = s_y0;
    float m0_x0 = p0_x0 - r1;
    float m0_y0 = p0_y0;

    float p0_vy0 = get_init_vy(p0_m, s_m, r0);
    //float m0_vy0 = get_init_vy(m0_m, p0_m, r1);

    printf("planet v0 = %f\n", p0_vy0);
    //printf("moon v0 = %f\n", m0_vy0);

    Body star = {s_x0, s_y0, 0, 0, s_m, 10, {0.9f, 0.5f, 0.0f}};
    Body planet = {p0_x0, p0_y0, 0, p0_vy0, p0_m, 4, {0.0f, 0.2f, 0.8f}};
    //Body moon = {m0_x0, m0_y0, 0, m0_vy0 + p0_vy0, m0_m, 1, {0.0f, 0.9f, 0.9f}};

    double lastTime = get_time();
    double fpsTime = 0;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = get_time();
        float dt = (float)(currentTime - lastTime);
        lastTime = currentTime;

        // Apply time scale
        dt *= TIME_SCALE;

        // FPS calculation
        frameCount++;
        fpsTime += dt;
        if (fpsTime >= 1.0) {
            double fps = frameCount / fpsTime;
            char title[256];
            snprintf(title, sizeof(title), "FPS: %.1f", fps);
            glfwSetWindowTitle(window, title);
            frameCount = 0;
            fpsTime = 0;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        drawBody(&star);
        drawBody(&planet);

        updatePhysics(&planet, &star, dt);
        updatePhysics(&star, &planet, dt);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double endTime = get_time();
        double frameDuration = endTime - currentTime;
        if (frameDuration < FRAME_TIME) {
            glfwWaitEventsTimeout(FRAME_TIME - frameDuration);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
