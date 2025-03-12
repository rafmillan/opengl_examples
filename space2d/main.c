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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NUM_SEGMENTS 100  // Number of segments for smoothness

// Physics Constants
#define GRAVITY -9.8          // Gravity (pixels per second^2)
#define RESTITUTION 1       // Bounce energy loss (0 = no bounce, 1 = perfect bounce)
#define DRAG_COEFF 0.00       // Air resistance factor
#define FRICTION 0.00       // Ground friction (reduces horizontal speed on bounce)
#define BALL_RADIUS  50        // Ball size

// Ball Properties
typedef struct {
    float x, y;         // Position
    float vx, vy;       // Velocity
} Ball;

// Timing Variables
double get_time() {
    return glfwGetTime();  // Get time in seconds
}

void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.0f, 0.0f);  // Red color
    glVertex2f(cx, cy);  // Center of the circle

    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * M_PI * i / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void updatePhysics(Ball *ball, float dt) {
    // Apply gravity (acceleration affects velocity)
    ball->vy += GRAVITY * dt;  

    // Apply air resistance (drag)
    ball->vx *= (1.0 - DRAG_COEFF * dt);
    ball->vy *= (1.0 - DRAG_COEFF * dt);

    // Update position based on velocity
    ball->x += ball->vx * dt;
    ball->y += ball->vy * dt;

    // Collision with ground
    if (ball->y - BALL_RADIUS <= 0) {
        ball->y = BALL_RADIUS;  // Keep above ground
        ball->vy = -ball->vy * RESTITUTION;  // Bounce with energy loss

        // Apply ground friction
        ball->vx *= (1.0 - FRICTION);
    }

    // Collision with ceiling
    if (ball->y + BALL_RADIUS >= SCREEN_HEIGHT) {
        ball->y = SCREEN_HEIGHT - BALL_RADIUS;
        ball->vy = -ball->vy * RESTITUTION;

        ball->vx *= (1.0 - FRICTION);
    }

    // Collision with walls
    if (ball->x - BALL_RADIUS <= 0) {
        ball->x = BALL_RADIUS;
        ball->vx = -ball->vx * RESTITUTION;
    }
    if (ball->x + BALL_RADIUS >= SCREEN_WIDTH) {
        ball->x = SCREEN_WIDTH - BALL_RADIUS;
        ball->vx = -ball->vx * RESTITUTION;
    }
}

int main() {
    Ball ball = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - BALL_RADIUS, 50, -200}; // Initial velocity

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

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);  // 2D Projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double lastTime = get_time();
    double fpsTime = 0;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = get_time();
        float dt = (float)(currentTime - lastTime);
        lastTime = currentTime;

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

        updatePhysics(&ball, dt);
        drawCircle(ball.x, ball.y, BALL_RADIUS, NUM_SEGMENTS);

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

