#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FRAMES_PER_SECOND = 60.0f;
const float TOTAL_TIME = 60.0f;

// global animation time, point of reference / master clock for whole animation
float globalTime = 0.0f;


/* --------------------------   Moving Object That Exist    ----------------------------- */

// typedef struct {.....} Name; -> creates a custom type

// for star
struct Star {
    float positionX;
    float positionY;
    float movementSpeed;
    float visualSize;
};

// for planet
struct Planet {
    float positionX;
    float positionY;
    float radius;
    float movementSpeed;
    float colorRed;
    float colorGreen;
    float colorBlue;
};

//Global Arrays
std::vector<Star> stars;
std::vector<Planet> planets;


/* --------------------------   Helper Function    ----------------------------- */

// for randomizing things such as sizes, position, colors, etc.
float randomFloat(float minimum, float maximum) {
    return minimum + (maximum - minimum) * ((float) rand() / (float) RAND_MAX);
}

// to keep a number within a range, useful for smooth animation
float clampFloat(float value, float minimum, float maximum) {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

// creates smooth transition
float smoothstep(float minimum, float maximum, float input) {
    float t = clampFloat((input - minimum) / (maximum - minimum), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

 /* --------------------------   Drawing Helper Function    ----------------------------- */
 
 //to draw RECTANGLE, using 4 corner vertices (for walls, buildings, subtitle box, ect.)
void drawRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS); //draws quadrilaterals
        glVertex2f(x1,y1);
        glVertex2f(x2,y1);
        glVertex2f(x2,y2);
        glVertex2f(x1,y2);
    glEnd();
}

//Draws a FILLED circle, using Triangle Fan
void drawCircle(float centerX, float centerY, float radius, int segment = 60) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centerX, centerY);
        for(int i = 0; i<= segment; i++) {
            float a = 2.0f * 3.1415926f * i / segment;

            glVertex2f(centerX + cosf(a) * radius, centerY + sinf(a) * radius);
        }
    glEnd();
}

//just for circle outline, basically a normal circle
void drawCircleOutline(float centerX, float centerY, float radius, float segment) {
    glBegin(GL_LINE_LOOP);
            for(int i=0; i<= segment; i++) {
                float a = 2.0f * 3.14152926f * i / segment;
                glVertex2f(centerX + cosf(a) * radius, centerY + sinf(a) * radius);
            }
        glEnd();
}

void drawWindowViewBack(float warpFactor) {
    glColor4f(0.82f, 0.90f, 1.0f, 0.06f); // very transparent
    glLineWidth(1.0f);

    for (const auto& star : stars) {
        // limit reflection to face/window center region
        if (star.positionX > -0.75f && star.positionX < 0.75f &&
            star.positionY > -0.75f && star.positionY < 0.75f) {

            // shorter reflection streak than the real star streak
            float len = star.movementSpeed * 0.006f * warpFactor;

            // scale down and shift reflection so it feels like a glass reflection,
            // not the real star position
            float rx = star.positionX * 0.35f + 0.02f;
            float ry = star.positionY * 0.35f + 0.05f;

            glBegin(GL_LINES);
                glVertex2f(rx, ry);
                glVertex2f(rx + len, ry);
            glEnd();
        }
    }
}


// Create Text
void drawText(float x, float y, void* font, const std:: string& s) {
    glRasterPos2f(x,y);
    for(char c: s) glutBitmapCharacter(font,c); 
}

// Draw random stars
void initializeStars() {
    stars.clear();
    stars.reserve(300);
    for(int i = 0; i < 300; i++) {
        stars.push_back({
            randomFloat(-1.4f, 1.4f), randomFloat(-1.0f, 1.0f), randomFloat(0.8f, 3.0f), randomFloat(1.0f, 3.0f)
        });
    }
}

void initializePlanets() {
    planets.clear();
    for(int i = 0; i < 4; i++) {
        Planet planet;
        planet.positionX = randomFloat(1.5f, 3.5f) + i * 1.6f;
        planet.positionY = randomFloat(-0.4f, 0.5f);
        planet.radius = randomFloat(0.08f, 0.16f);
        planet.movementSpeed = randomFloat(0.08f, 0.18f);
        planet.colorRed = randomFloat(0.2f, 0.8f);
        planet.colorGreen = randomFloat(0.2f, 0.8f);
        planet.colorBlue = randomFloat(0.3f, 0.9f);
        planets.push_back(planet);
    }
}


// STARFIELD EFFECT -> zooming through the space in hyperspeed.
void updateStars(float deltaTime, float warpFactor) {
    for (auto& currentStar : stars) {
        // Move the star to the left based on its individual speed, the time elapsed, and the warp multiplier
        currentStar.positionX -= currentStar.movementSpeed * deltaTime * warpFactor;

        // if moved past the left boundary
        if (currentStar.positionX < -1.5f) {
            // Reset the star to the right
            currentStar.positionX = 1.5f;

            // Randomize the vertical position, speed, and size of stars
            currentStar.positionY = randomFloat(-1.0f, 1.0f);
            currentStar.movementSpeed = randomFloat(1.2f, 4.2f);
            currentStar.visualSize = randomFloat(1.0f, 3.0f);
        }
    }
}


// PLANETARY MOTION -> planets moving across the screen, with random sizes, speeds, and colors. When they move off the left side of the screen, they respawn on the right with new random attributes.
void updatePlanets(float deltaTime, float warpMultiplier) {
    for (auto& planet : planets) {
        // Move the planet to the left based on its speed and time elapsed
        planet.positionX -= planet.movementSpeed * deltaTime * warpMultiplier * 0.75f;

        // Check if the planet has moved off the left side of the screen
        if (planet.positionX < -1.8f) {
            // Respawn the planet on the right with new random attributes
            planet.positionX = randomFloat(1.4f, 4.8f);
            planet.positionY = randomFloat(-0.50f, 0.58f);
            planet.radius = randomFloat(0.06f, 0.18f);
            planet.movementSpeed = randomFloat(0.10f, 0.24f);
            
            // Randomize the Planet Colors (Red, Green, Blue)
            planet.colorRed = randomFloat(0.25f, 0.95f);
            planet.colorGreen = randomFloat(0.20f, 0.80f);
            planet.colorBlue = randomFloat(0.30f, 0.95f);
        }
    }
}

// Draw the space background gradient, transitioning from a deep blue at the top to a darker shade at the bottom, creating a sense of depth and vastness in space.
void drawSpaceBackgroundGradient() {
    glBegin(GL_QUADS);
    glColor3f(0.00f, 0.00f, 0.06f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.03f, 0.02f, 0.13f);
    glVertex2f( 1,-1);
    glVertex2f(-1,-1);
    glEnd();
}

void drawPlanets() {
    // Loop through each planet in the planets vector 
    for (const auto& planet : planets) {

        // Draw the planet itself with its unique color and size 
        glColor3f(planet.colorRed, planet.colorGreen, planet.colorBlue);
        drawCircle(planet.positionX, planet.positionY, planet.radius, 50);

        // Draw the subtle halo effect around the planet 
        glColor4f(1.0f, 1.0f, 1.0f, 0.18f);
        drawCircle(planet.positionX - planet.radius *0.25f, planet.positionY + planet.radius *0.20f, planet.radius * 0.48f, 30);

        // Draw the subtle glow around the planet
        glColor4f(1.0f, 1.0f, 1.0f, 0.12f);
        drawCircleOutline(planet.positionX, planet.positionY, planet.radius * 1.08f, 50);
    }
}

void drawStars(bool streaks, float warpFactor) {
    for (const auto& star : stars) {
        glColor3f(0.88f, 0.92f, 1.0f);

        // Draw the star as a small circle, with size based on its visualSize attribute
        if (streaks) {
            float len = star.movementSpeed * 0.02f * warpFactor; // length of the streak based on speed and warp factor
            glLineWidth(star.visualSize * 0.5f); // line width based on visual size
            glBegin(GL_LINES);
                glVertex2f(star.positionX + len, star.positionY);// start point of the streak (ahead of the star)
                glVertex2f(star.positionX - len, star.positionY);// end point of the streak (behind the star)
            glEnd();
        } else {
            glPointSize(star.visualSize); // point size based on visual size
            glBegin(GL_POINTS);
                glVertex2f(star.positionX, star.positionY); // draw the star as a point at its current position
            glEnd();
    }
}
}

// Draw inside of ship, Window Frame and Interior
void drawWindowFrameAndInterior() {

    // Top Frame 
    glBegin(GL_QUADS);
    glColor3f(0.98f, 0.98f, 1.0f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.82f, 0.84f, 0.90f);
    glVertex2f( 1, 0.78f);
    glVertex2f(-1, 0.78f);
    glEnd();

    //Bottom Frame
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.82f, 0.88f);
    glVertex2f(-1, -0.78f);
    glVertex2f( 1, -0.78f);
    glColor3f(0.68f, 0.72f, 0.80f);
    glVertex2f( 1, -1);
    glVertex2f(-1, -1);
    glEnd();

    //Left Side Frame
    glBegin(GL_QUADS);
    glColor3f(0.92f, 0.93f, 0.97f);
    glVertex2f(-1, 0.78f);
    glVertex2f(-0.78f, 0.78f);
    glColor3f(0.76f, 0.79f, 0.86f);
    glVertex2f(-0.78f, -0.78f);
    glVertex2f(-1, -0.78f);
    glEnd();

    //Right side Frame 
    glBegin(GL_QUADS);
    glColor3f(0.92f, 0.93f, 0.97f);
    glVertex2f(0.78f, 0.78f);
    glVertex2f(1, 0.78f);
    glColor3f(0.76f, 0.79f, 0.86f);
    glVertex2f(1, -0.78f);
    glVertex2f(0.78f, -0.78f);
    glEnd();

    //Inner shadow pieces, draw thin darker strips around the edge of the window frame
    glColor4f(0.60f, 0.64f, 0.72f, 0.55f);
    drawRect(-0.82f,  0.78f, 0.82f,  0.72f);
    drawRect(-0.82f, -0.72f, 0.82f, -0.78f);
    drawRect(-0.82f, -0.78f, -0.72f, 0.78f);
    drawRect( 0.72f, -0.78f,  0.82f, 0.78f);

    // Thick white inner outline, draws a thick white rectangular outline around the glass area.
    glColor3f(0.98f, 0.98f, 1.0f);
    glLineWidth(12.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.78f, -0.78f);
    glVertex2f( 0.78f, -0.78f);
    glVertex2f( 0.78f,  0.78f);
    glVertex2f(-0.78f,  0.78f);
    glEnd();

    //This draws a smaller thin outline inside the glass,adds detail and makes the window look more layered.
    glColor3f(0.64f, 0.69f, 0.78f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.70f, -0.70f);
    glVertex2f( 0.70f, -0.70f);
    glVertex2f( 0.70f,  0.70f);
    glVertex2f(-0.70f,  0.70f);
    glEnd();

    //extra bottom sill
    glColor3f(0.80f, 0.83f, 0.90f);
    drawRect(-1, -1, 1, -0.80f);

    //Hanging string, for the light bulb 
    glColor3f(0.60f, 0.64f, 0.72f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.55f, 0.95f);
    glVertex2f(0.55f, 0.72f);
    glEnd();
    // this is the light bulb, it bobs up and down slightly to add a bit of traveling feeling.
    glColor3f(1.0f, 0.75f, 0.25f);
    drawCircle(0.55f, 0.68f + 0.02f * sin(globalTime * 2.0f), 0.03f, 24); // bobbing effect using sine wave, based on global time 

}

void drawGlassOverlay() {
    glColor4f(0.70f, 0.85f, 1.0f, 0.05f);

    glBegin(GL_QUADS);
        glVertex2f(-1.0f,  1.0f);
        glVertex2f( 1.0f,  1.0f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
    glEnd();
}

// Draw the alien from the back, showing its torso, head, and fins. Add a subtle breathing motion, same method as the bulb  
void drawAlien() {
    float breathe = sin(globalTime * 1.6f) * 0.0025f;

    glPushMatrix();
    glTranslatef(-0.03f, -0.58f + breathe, 0.0f); // much lower

    // seat hint / floor contact
    glColor3f(0.55f, 0.60f, 0.70f);
    drawRect(-0.24f, -0.18f, 0.24f, -0.12f);

    // torso
    glColor3f(0.18f, 0.24f, 0.35f);
    glBegin(GL_POLYGON);
    //bottom point
    glVertex2f(-0.08f, -0.10f);
    glVertex2f( 0.08f, -0.10f);

    //top point 
    glVertex2f( 0.10f,  0.18f);
    glVertex2f(-0.10f,  0.18f);
    glEnd();


    //legs
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.08f, -0.22f, 0.08f, -0.06f);
    
    //leg jeans
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.08f, -0.18f, 0.08f,-0.06f);

    //leg pants outline
    glColor3f(0.13f, 0.18f, 0.28f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(-0.08f, -0.10f);
    glVertex2f( 0.08f, -0.10f);
    glEnd();


    // neck
    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(-0.04f, 0.38f, 0.04f, 0.18f);

    // head
    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(0.0f, 0.35f, 0.11f, 50);

    //mouth
    glColor3f(0.0f, 0.40f, 0.05f);
    glLineWidth(2.5f);

    float mouthX = 0.05f;   // positive = move right, negative = move left
    float mouthY = 0.325f;  // positive = move up, negative = move down
    float width  = 0.025f;  // smaller than 0.06, so mouth is smaller
    float curve  = 8.0f;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++) {
        float x = -width + i * ((2.0f * width) / 30.0f);
        float y = mouthY - curve * x * x;

        glVertex2f(x + mouthX, y);
    }
    glEnd();

    // eyes
    glColor3f(1.00f, 1.00f, 1.00f);
    drawCircle( 0.04f, 0.38f, 0.027f, 20);

    // eye ball highlight
    glColor3f(0.05f, 0.05f, 0.05f);
    drawCircle( 0.045f, 0.385f, 0.013f, 20);

    // head shading
    glColor4f(0.12f, 0.45f, 0.15f, 0.35f);
    drawCircle(-0.03f, 0.38f, 0.08f, 40);

    // alien fins
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.09f, 0.39f); glVertex2f(-0.17f, 0.43f); glVertex2f(-0.11f, 0.31f);
    glEnd();

   

    // side arm
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.03f, -0.02f, 0.03f, 0.15f);

    //side arm shoulder sleeve
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.03f, 0.08f, 0.03f, 0.15f);

    /*
     // side arm outline
    glColor3f(0.01f, 0.01f, 0.01f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.03f, -0.02f);
    glVertex2f( 0.03f, -0.02f);
    glVertex2f( 0.03f,  0.15f);
    glVertex2f(-0.03f,  0.15f);
    glEnd();
    */
   

    glPopMatrix();
}

void drawAlienShooter(float currentTime) {
    float recoilAmount = 0.012f * sin(currentTime * 12.0f);
    float muzzleFlashStrength = fabs(sin(currentTime * 18.0f));

    glPushMatrix();

    // Smaller size and slightly adjusted position
    glTranslatef(-0.35f, -0.20f + recoilAmount, 0.0f);
    glScalef(0.78f, 0.78f, 1.0f);


    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.08f, -0.28f, -0.02f, -0.10f); // left leg
    drawRect( 0.02f, -0.28f,  0.08f, -0.10f); // right leg

    // pants / jeans
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.09f, -0.16f, 0.09f, -0.08f);

    // pants divider
    glColor3f(0.13f, 0.18f, 0.28f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(-0.09f, -0.08f);
        glVertex2f( 0.09f, -0.08f);
    glEnd();

  
    glColor3f(0.18f, 0.24f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.10f, -0.10f);
        glVertex2f( 0.10f, -0.10f);
        glVertex2f( 0.13f,  0.16f);
        glVertex2f(-0.09f,  0.16f);
    glEnd();


    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(-0.035f, 0.14f, 0.035f, 0.22f);


    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(0.0f, 0.31f, 0.11f, 50);

    // head shading
    glColor4f(0.12f, 0.45f, 0.15f, 0.35f);
    drawCircle(-0.03f, 0.34f, 0.08f, 40);

    // alien fins
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.08f, 0.35f);
        glVertex2f(-0.16f, 0.40f);
        glVertex2f(-0.10f, 0.27f);

        glVertex2f(0.08f, 0.35f);
        glVertex2f(0.16f, 0.40f);
        glVertex2f(0.10f, 0.27f);
    glEnd();


    // eyes
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-0.035f, 0.33f, 0.022f, 20);
    drawCircle( 0.045f, 0.33f, 0.022f, 20);

    // pupils looking to the right, toward the enemy
    glColor3f(0.05f, 0.05f, 0.05f);
    drawCircle(-0.026f, 0.33f, 0.010f, 12);
    drawCircle( 0.054f, 0.33f, 0.010f, 12);

    // angry eyebrows
    glColor3f(0.05f, 0.25f, 0.08f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(-0.060f, 0.365f);
        glVertex2f(-0.015f, 0.350f);

        glVertex2f(0.020f, 0.350f);
        glVertex2f(0.070f, 0.365f);
    glEnd();

    // determined mouth
    glColor3f(0.0f, 0.40f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(-0.025f, 0.275f);
        glVertex2f( 0.035f, 0.275f);
    glEnd();

    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.14f, -0.03f, -0.09f, 0.12f);

    // left sleeve
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.14f, 0.06f, -0.09f, 0.12f);

 
    // upper arm / sleeve
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(0.08f, 0.04f, 0.17f, 0.10f);

    // forearm extended forward
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(0.15f, 0.02f, 0.28f, 0.08f);

    // hand
    glColor3f(0.20f, 0.65f, 0.22f);
    drawCircle(0.285f, 0.05f, 0.025f, 16);

    // gun body
    glColor3f(0.55f, 0.58f, 0.65f);
    drawRect(0.26f, 0.01f, 0.42f, 0.09f);

    // gun barrel
    glColor3f(0.35f, 0.38f, 0.45f);
    drawRect(0.40f, 0.035f, 0.52f, 0.065f);

    // top detail
    glColor3f(0.25f, 0.30f, 0.38f);
    drawRect(0.32f, 0.09f, 0.43f, 0.12f);

    // handle
    glColor3f(0.30f, 0.30f, 0.34f);
    drawRect(0.30f, -0.07f, 0.35f, 0.01f);

    // trigger guard
    glColor3f(0.12f, 0.13f, 0.15f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.32f, 0.005f);
        glVertex2f(0.38f, 0.005f);
        glVertex2f(0.38f, -0.035f);
        glVertex2f(0.32f, -0.035f);
    glEnd();

    // muzzle flash
    if (muzzleFlashStrength > 0.55f) {
        glColor4f(1.0f, 0.85f, 0.20f, 0.85f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0.52f, 0.05f);
            glVertex2f(0.64f, 0.10f);
            glVertex2f(0.64f, 0.00f);
        glEnd();

        glColor4f(1.0f, 0.35f, 0.10f, 0.55f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0.52f, 0.05f);
            glVertex2f(0.60f, 0.075f);
            glVertex2f(0.60f, 0.025f);
        glEnd();
    }

    glPopMatrix();
}


void drawAlienFaceScene(float t) {
    //so that the starfield effect still works.
    float warp = 1.2f;
    updateStars(1.0f / FRAMES_PER_SECOND, warp);
    updatePlanets(1.0f / FRAMES_PER_SECOND, warp);




    //rectangle that fills the whole screen, act as background.
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.06f, 0.10f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.02f, 0.03f, 0.05f);
    glVertex2f( 1,-1);
    glVertex2f(-1,-1);
    glEnd();

    //Window View from face perspective
    drawPlanets();
    drawStars(true, warp);
    drawGlassOverlay();
    drawWindowViewBack(1.5f);



    
    float blink = 0.0006f * sin(t * 2.2f);// eyes blink with a sine wave, very subtle

    // neck - moved lower so it connects under the head
    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(-0.10f, -0.38f, 0.10f, -0.24f);

    // left sleeve (upper arm / bicep area)
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.30f, -0.55f, -0.20f, -0.38f);

    // left arm
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.30f, -0.98f, -0.20f, -0.55f);

    // right sleeve (upper arm / bicep area)
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(0.20f, -0.55f, 0.30f, -0.38f);

    // right arm
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(0.20f, -0.98f, 0.30f, -0.55f);

    // body - moved lower
    glColor3f(0.18f, 0.24f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.20f, -0.38f);
        glVertex2f( 0.20f, -0.38f);
        glVertex2f( 0.20f, -1.00f);
        glVertex2f(-0.20f, -1.00f);
    glEnd();


    //Alien head shape 
    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(0.0f, 0.05f, 0.40f, 80);

    //Head shading
    glColor4f(0.12f, 0.45f, 0.15f, 0.30f);
    drawCircle(-0.08f, 0.12f, 0.30f, 60);

    //Alien Ears
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES); // Every group of three vertices forms a triangle, so we can draw the ears with just two triangles. 
        //Left Ear
        glVertex2f(-0.28f, 0.22f); glVertex2f(-0.43f, 0.34f); glVertex2f(-0.30f, 0.05f);
        //Right Ear
        glVertex2f( 0.28f, 0.22f); glVertex2f( 0.43f, 0.34f); glVertex2f( 0.30f, 0.05f);
    glEnd();


    //White part of eyeball + blinking animation 
    glColor3f(0.92f, 0.94f, 0.97f);
    drawCircle(-0.13f, 0.12f + blink, 0.07f, 30);
    drawCircle( 0.13f, 0.12f - blink, 0.07f, 30);

    

    //pupils
    glColor3f(0.08f, 0.10f, 0.12f);
    drawCircle(-0.11f, 0.12f + blink, 0.03f, 20);
    drawCircle( 0.15f, 0.12f - blink, 0.03f, 20);

    //eye highlights
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-0.12f, 0.14f + blink, 0.01f, 10);
    drawCircle( 0.14f, 0.14f - blink, 0.01f, 10);

    //nose 
    glColor3f(0.10f, 0.55f, 0.15f);
    drawRect(-0.025f, -0.02f, 0.025f, 0.10f);

    
    // nose
    glColor3f(0.10f, 0.55f, 0.15f);
    drawRect(-0.025f, -0.02f, 0.025f, 0.10f);

    


    float smile = smoothstep(0.0f, 3.5f, t);// smile grows from 0 to 3.5 as t goes from 0 to 1 
    //SMILE (SAD ALSO )ANIMATIONNNN
    glLineWidth(3.0f);
    glColor3f(0.10f, 0.35f, 0.10f);

    glBegin(GL_LINE_STRIP); // this draws a continuous connected line through all vertices in order, basically if many points, it makes one connected curve
    for (int i = 0; i <= 40; i++) {
        float u = -0.14f + i * (0.28f / 40.0f);
        float normalized = u / 0.14f;               // -1 to 1
        float curve = normalized * normalized;      // 0 center, 1 edges

        //change here to change the smile curve
        float v = -0.17f + smile * (0.010f-0.045f * curve);
        glVertex2f(u, v);
    }
    glEnd();
 
}


// Animation for the first 14 seconds, where the alien is travelling through space. All Space effect are implemented.
void drawSceneWindowFast() {
    float warp = 2.0f; // control how fast the starfield and planets move, higher => faster
    updateStars(1.0f / FRAMES_PER_SECOND, warp);
    updatePlanets(3.7f / FRAMES_PER_SECOND, warp); // 'f' here adjust the speed of planet showing.

    drawSpaceBackgroundGradient();

    drawPlanets();
    drawStars(true, warp);

    //subtle window glow
    glColor4f(0.7f, 0.85f, 1.0f, 0.04f);
    drawRect(-0.78f, -0.78f, 0.78f, 0.78f);

    drawWindowFrameAndInterior();
    drawAlien();
}


void drawSciFiFlashbackBattle(float t) {
    // sky
    glBegin(GL_QUADS);
    glColor3f(0.03f, 0.02f, 0.08f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.10f, 0.03f, 0.10f);
    glVertex2f( 1,-1);
    glVertex2f(-1,-1);
    glEnd();

     // horizon glow
    glColor4f(0.55f, 0.15f, 0.55f, 0.22f);
    drawRect(-1.0f, -0.24f, 1.0f, -0.18f);

    // STRAIGHT GROUND
    glColor3f(0.18f, 0.07f, 0.16f);
    drawRect(-1.0f, -1.0f, 1.0f, -0.24f);

    // distant city skyline - now aligned to straight ground
    for (int i = 0; i < 12; i++) {
        float x = -1.0f + i * 0.18f;
        float h = 0.12f + 0.18f * ((i % 4) / 3.0f);

        glColor3f(0.14f, 0.18f, 0.26f);
        drawRect(x, -0.24f, x + 0.10f, -0.24f + h);

        glColor3f(0.0f, 0.75f, 0.95f);
        for (int w = 0; w < 3; w++) {
            float wx = x + 0.02f + w * 0.025f;
            drawRect(wx, -0.18f, wx + 0.01f, -0.15f + 0.02f * (w % 2));
        }
    }


    // foreground towers - also stable
    for (int i = 0; i < 6; i++) {
        float x = -0.9f + i * 0.34f;
        float h = 0.30f + 0.08f * (i % 3);

        glColor3f(0.22f, 0.32f, 0.45f);
        drawRect(x, -0.25f, x + 0.08f, h);

        glColor3f(0.0f, 0.9f, 1.0f);
        drawRect(x + 0.02f, 0.0f, x + 0.04f, h - 0.05f);

        // antenna or side detail
        glColor3f(0.6f, 0.7f, 0.85f);
        drawRect(x + 0.055f, h - 0.02f, x + 0.065f, h + 0.07f);
    }

    // little flying traffic to sell "city"
    for (int i = 0; i < 5; i++) {
        float px = -1.2f + fmod(t * (0.18f + 0.03f * i) + i * 0.3f, 2.6f);
        float py = 0.15f + 0.08f * sin(t * 1.2f + i);

        glColor3f(1.0f, 0.55f, 0.2f);
        drawRect(px, py, px + 0.04f, py + 0.01f);

        glColor4f(1.0f, 0.55f, 0.2f, 0.35f);
        drawRect(px - 0.03f, py + 0.002f, px, py + 0.008f);
    }

    // alien shooter
    drawAlienShooter(t);
    

    // lasers
    for (int i = 0; i < 4; i++) {
        float phase = fmod(t * 0.9f + i * 0.2f, 1.2f);
        float x1 = -0.08f + phase * 0.75f;
        float y1 = -0.14f + 0.015f * sin(t * 6.0f + i);

        glLineWidth(3.0f);
        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x1 + 0.16f, y1 + 0.035f);
        glEnd();
    }

    // enemy drones
    for (int i = 0; i < 3; i++) {
        float x = 0.45f + i * 0.18f;
        float y = 0.25f + 0.06f * sin(t * 1.5f + i * 0.9f);

        glColor3f(0.85f, 0.15f, 0.25f);
        drawCircle(x, y, 0.035f, 24);

        glColor3f(1.0f, 0.85f, 0.2f);
        drawCircle(x + 0.01f, y, 0.008f, 12);
    }
}


std::string currentLine() {
    
     if (globalTime < 4.0f)
        return "I kept chasing the light ahead of me.";

    if (globalTime < 8.0f)
        return "For duty. For glory. For the ones counting on me.";

    if (globalTime < 12.5f)
        return "But every star outside the window pulled me backward.";

    if (globalTime < 16.5f)
        return "Back to the night I thought courage meant fighting.";

    if (globalTime < 21.0f)
        return "I fired until the sky broke into sparks... and still, I wondered what I had won.";


    return "";

    
}

//helper functin for better script pacing
float currentLineStartTime() {
    if (globalTime < 4.0f) return 0.0f;
    if (globalTime < 8.0f) return 4.0f;
    if (globalTime < 12.5f) return 8.0f;
    if (globalTime < 16.5f) return 12.5f;
    if (globalTime < 21.0f) return 16.5f;
    
    
    return 21.0f;
}



void drawChatBox() {
     std::string line = currentLine();
    if (line.empty()) return;

    float segmentStart = currentLineStartTime();

    int visible = std::min(
        (int)line.size(),
        std::max(0, (int)((globalTime - segmentStart) * 35.0f))
    );

    std::string shown = line.substr(0, visible);

    // subtitle background box
    glColor4f(0.0f, 0.0f, 0.0f, 0.68f);
    drawRect(-0.90f, -0.95f, 0.90f, -0.76f);

    // small glowing top border
    glColor4f(0.8f, 0.9f, 1.0f, 0.22f);
    drawRect(-0.90f, -0.76f, 0.90f, -0.745f);

    // speaker name / label
    glColor3f(0.55f, 0.85f, 1.0f);
    drawText(-0.84f, -0.81f, GLUT_BITMAP_HELVETICA_12, "ALIEN MEMORY LOG");

    // actual subtitle text
    glColor3f(0.95f, 0.97f, 1.0f);
    drawText(-0.84f, -0.89f, GLUT_BITMAP_HELVETICA_18, shown);
}


// ============================================================
//  GROUP B  —  Seconds 14–34  (drop-in addition to your file)
//  Paste these functions into your .cpp BEFORE display().
//  Then replace the display() body with the one at the bottom.
// ============================================================

// -------------------------------------------------------
//  SCENE 1 HELPERS  :  Sci-fi Battle  (t = 0..10)
// -------------------------------------------------------

// Draws a futuristic city skyline.
// Buildings are static — they read as a real city backdrop.
void drawSciFiCity(float scrollX) {
    // sky gradient  (dark purple-blue)
    glBegin(GL_QUADS);
        glColor3f(0.04f, 0.02f, 0.12f);
        glVertex2f(-1, 1); glVertex2f(1, 1);
        glColor3f(0.08f, 0.04f, 0.20f);
        glVertex2f(1, -1); glVertex2f(-1, -1);
    glEnd();

    // distant glow on horizon
    glColor4f(0.40f, 0.10f, 0.80f, 0.18f);
    drawRect(-1, -0.55f, 1, -0.20f);

    // ground
    glColor3f(0.06f, 0.04f, 0.10f);
    drawRect(-1, -1, 1, -0.55f);

    // --- building definitions: {x-left, width, height, shade} ---
    struct Bldg { float x, w, h; float r, g, b; };
    Bldg bldgs[] = {
        {-0.95f, 0.10f, 0.60f, 0.18f, 0.14f, 0.30f},
        {-0.82f, 0.14f, 0.45f, 0.22f, 0.16f, 0.36f},
        {-0.65f, 0.08f, 0.70f, 0.14f, 0.10f, 0.26f},
        {-0.55f, 0.18f, 0.50f, 0.20f, 0.15f, 0.34f},
        {-0.34f, 0.10f, 0.80f, 0.16f, 0.12f, 0.28f},
        {-0.22f, 0.20f, 0.38f, 0.24f, 0.18f, 0.38f},
        { 0.01f, 0.12f, 0.65f, 0.18f, 0.13f, 0.30f},
        { 0.16f, 0.16f, 0.55f, 0.20f, 0.16f, 0.32f},
        { 0.35f, 0.09f, 0.72f, 0.15f, 0.11f, 0.27f},
        { 0.47f, 0.18f, 0.42f, 0.22f, 0.17f, 0.35f},
        { 0.68f, 0.10f, 0.60f, 0.17f, 0.13f, 0.29f},
        { 0.81f, 0.14f, 0.48f, 0.21f, 0.15f, 0.33f},
    };

    for (auto& b : bldgs) {
        float bx = b.x + scrollX * 0.08f; // very slight parallax
        float top = -0.55f + b.h;

        // building body
        glColor3f(b.r, b.g, b.b);
        drawRect(bx, -0.55f, bx + b.w, top);

        // window grid  (small cyan dots)
        glColor3f(0.30f, 0.80f, 0.90f);
        int wRows = (int)(b.h / 0.08f);
        int wCols = (int)(b.w / 0.06f);
        for (int wr = 0; wr < wRows; wr++)
            for (int wc = 0; wc < wCols; wc++) {
                float wx = bx + 0.015f + wc * 0.055f;
                float wy = -0.52f + wr * 0.075f;
                if (wy < top - 0.04f)
                    drawCircle(wx, wy, 0.008f, 6);
            }

        // antenna on tall buildings
        if (b.h > 0.58f) {
            glColor3f(0.50f, 0.55f, 0.70f);
            glLineWidth(1.5f);
            glBegin(GL_LINES);
                glVertex2f(bx + b.w * 0.5f, top);
                glVertex2f(bx + b.w * 0.5f, top + 0.07f);
            glEnd();
            // blinking light
            float blink = (sinf(globalTime * 4.0f + bx * 10.0f) > 0.5f) ? 1.0f : 0.3f;
            glColor3f(blink, 0.1f, 0.1f);
            drawCircle(bx + b.w * 0.5f, top + 0.075f, 0.008f, 8);
        }

        // edge highlight
        glColor4f(0.55f, 0.35f, 0.90f, 0.35f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
            glVertex2f(bx, -0.55f); glVertex2f(bx, top);
        glEnd();
    }

    // flying traffic  (small light streaks in mid-air)
    for (int i = 0; i < 6; i++) {
        float tx = fmodf(scrollX * 0.3f + i * 0.35f, 2.2f) - 1.1f;
        float ty = -0.20f + i * 0.08f;
        glColor4f(0.60f, 0.90f, 1.0f, 0.70f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
            glVertex2f(tx, ty);
            glVertex2f(tx + 0.07f, ty);
        glEnd();
    }
}

// Enemy drone  — simple hexagonal body + two side cannons
void drawDrone(float cx, float cy, float size) {
    // body
    glColor3f(0.70f, 0.15f, 0.15f);
    drawCircle(cx, cy, size, 6);

    // eye sensor
    glColor3f(1.0f, 0.30f, 0.10f);
    drawCircle(cx, cy, size * 0.40f, 20);

    // side arms
    glColor3f(0.50f, 0.12f, 0.12f);
    drawRect(cx - size * 1.6f, cy - size * 0.15f, cx - size * 0.9f, cy + size * 0.15f);
    drawRect(cx + size * 0.9f, cy - size * 0.15f, cx + size * 1.6f, cy + size * 0.15f);

    // cannon tips
    glColor3f(0.30f, 0.08f, 0.08f);
    drawRect(cx - size * 1.75f, cy - size * 0.10f, cx - size * 1.55f, cy + size * 0.10f);
    drawRect(cx + size * 1.55f, cy - size * 0.10f, cx + size * 1.75f, cy + size * 0.10f);

    // glow ring
    glColor4f(1.0f, 0.30f, 0.10f, 0.25f);
    drawCircleOutline(cx, cy, size * 1.15f, 30);
}

// Alien warrior  (front-facing, crouching battle stance)
void drawAlienWarrior(float cx, float cy) {
    float breathe = sinf(globalTime * 2.5f) * 0.004f;

    // legs  (crouching spread)
    glColor3f(0.14f, 0.20f, 0.30f);
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.14f, cy - 0.30f);
        glVertex2f(cx - 0.04f, cy - 0.30f);
        glVertex2f(cx - 0.02f, cy - 0.06f);
        glVertex2f(cx - 0.12f, cy - 0.06f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(cx + 0.04f, cy - 0.30f);
        glVertex2f(cx + 0.14f, cy - 0.30f);
        glVertex2f(cx + 0.12f, cy - 0.06f);
        glVertex2f(cx + 0.02f, cy - 0.06f);
    glEnd();

    // boots
    glColor3f(0.25f, 0.30f, 0.40f);
    drawRect(cx - 0.17f, cy - 0.36f, cx - 0.01f, cy - 0.28f);
    drawRect(cx + 0.01f, cy - 0.36f, cx + 0.17f, cy - 0.28f);

    // torso
    glColor3f(0.18f, 0.24f, 0.36f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.18f, cy - 0.06f);
        glVertex2f(cx + 0.18f, cy - 0.06f);
        glVertex2f(cx + 0.14f, cy + 0.20f + breathe);
        glVertex2f(cx - 0.14f, cy + 0.20f + breathe);
    glEnd();

    // suit stripe
    glColor3f(0.0f, 0.85f, 0.85f);
    drawRect(cx - 0.015f, cy - 0.04f, cx + 0.015f, cy + 0.18f + breathe);

    // shoulder pads
    glColor3f(0.30f, 0.40f, 0.56f);
    drawRect(cx - 0.22f, cy + 0.06f, cx - 0.12f, cy + 0.18f + breathe);
    drawRect(cx + 0.12f, cy + 0.06f, cx + 0.22f, cy + 0.18f + breathe);

    // weapon arm  (outstretched right arm holding gun)
    glColor3f(0.20f, 0.60f, 0.22f); // green skin
    drawRect(cx + 0.14f, cy + 0.10f + breathe, cx + 0.38f, cy + 0.17f + breathe);

    // gun body
    glColor3f(0.35f, 0.38f, 0.42f);
    drawRect(cx + 0.36f, cy + 0.08f + breathe, cx + 0.52f, cy + 0.19f + breathe);

    // gun barrel
    glColor3f(0.22f, 0.25f, 0.28f);
    drawRect(cx + 0.50f, cy + 0.11f + breathe, cx + 0.65f, cy + 0.16f + breathe);

    // left arm  (bent upward for balance)
    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(cx - 0.38f, cy + 0.12f + breathe, cx - 0.14f, cy + 0.19f + breathe);

    // neck
    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(cx - 0.04f, cy + 0.19f + breathe, cx + 0.04f, cy + 0.27f + breathe);

    // head
    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(cx, cy + 0.35f + breathe, 0.10f, 50);

    // helmet visor
    glColor4f(0.30f, 0.70f, 1.0f, 0.45f);
    drawRect(cx - 0.07f, cy + 0.27f + breathe, cx + 0.07f, cy + 0.38f + breathe);

    // eyes (visible through visor)
    glColor3f(0.90f, 0.95f, 1.0f);
    drawCircle(cx - 0.035f, cy + 0.355f + breathe, 0.018f, 16);
    drawCircle(cx + 0.035f, cy + 0.355f + breathe, 0.018f, 16);

    // alien fins
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(cx - 0.08f, cy + 0.40f + breathe);
        glVertex2f(cx - 0.16f, cy + 0.46f + breathe);
        glVertex2f(cx - 0.10f, cy + 0.30f + breathe);
        glVertex2f(cx + 0.08f, cy + 0.40f + breathe);
        glVertex2f(cx + 0.16f, cy + 0.46f + breathe);
        glVertex2f(cx + 0.10f, cy + 0.30f + breathe);
    glEnd();
}

// Laser bolt  — a short bright line
void drawLaserBolt(float x, float y, float len, float r, float g, float b) {
    glColor4f(r, g, b, 0.9f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + len, y);
    glEnd();
    // glow core
    glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
        glVertex2f(x + 0.01f, y);
        glVertex2f(x + len - 0.01f, y);
    glEnd();
}

// Full battle scene  (t = local time 0..10)
void drawBattleScene(float t) {
    float scroll = t * 0.4f; // city slowly scrolls left

    drawSciFiCity(scroll);

    // three enemy drones at different positions / phases
    struct DroneData { float bx, by, phase, speed; };
    DroneData drones[] = {
        { 0.55f,  0.45f, 0.0f, 1.1f},
        { 0.30f,  0.15f, 1.8f, 0.8f},
        { 0.72f, -0.05f, 3.4f, 1.4f},
    };
    for (auto& d : drones) {
        float dx = d.bx + sinf(globalTime * d.speed + d.phase) * 0.06f;
        float dy = d.by + cosf(globalTime * d.speed * 0.7f + d.phase) * 0.04f;
        drawDrone(dx, dy, 0.045f);

        // drone fires red bolt at alien
        if (fmodf(t + d.phase, 2.2f) < 0.35f) {
            drawLaserBolt(dx - 0.05f, dy - 0.02f, -0.30f, 1.0f, 0.20f, 0.10f);
        }
    }

    // alien warrior on the left side
    drawAlienWarrior(-0.55f, -0.30f);

    // alien fires cyan bolt  (periodic)
    float shotPhase = fmodf(t, 1.8f);
    if (shotPhase < 0.25f) {
        float boltX = -0.55f + 0.65f + shotPhase * 2.4f;
        drawLaserBolt(boltX, -0.13f, 0.14f, 0.10f, 0.90f, 0.90f);
    }

    // explosion sparks when bolt hits a drone (simple radial lines)
    if (fmodf(t, 3.6f) < 0.40f) {
        float ex = 0.32f, ey = 0.12f;
        glColor4f(1.0f, 0.60f, 0.10f, 0.80f);
        glLineWidth(2.0f);
        for (int s = 0; s < 8; s++) {
            float a = s * 3.14159f / 4.0f;
            float len2 = 0.06f;
            glBegin(GL_LINES);
                glVertex2f(ex, ey);
                glVertex2f(ex + cosf(a) * len2, ey + sinf(a) * len2);
            glEnd();
        }
    }
}


// -------------------------------------------------------
//  SCENE 2 HELPERS  :  Chasing the Standard  (t = 0..8)
// -------------------------------------------------------

// Dark abstract background with falling grid lines
void drawPressureBackground() {
    // deep dark background
    glBegin(GL_QUADS);
        glColor3f(0.02f, 0.02f, 0.06f);
        glVertex2f(-1,1); glVertex2f(1,1);
        glColor3f(0.04f, 0.02f, 0.10f);
        glVertex2f(1,-1); glVertex2f(-1,-1);
    glEnd();

    // perspective grid lines receding to horizon
    glColor4f(0.30f, 0.10f, 0.55f, 0.22f);
    glLineWidth(1.0f);
    float offset = fmodf(globalTime * 0.18f, 0.18f); // scrolling down slightly
    for (int i = 0; i < 10; i++) {
        float y = -0.55f + (i * 0.18f) - offset;
        if (y > 0.6f) continue;
        glBegin(GL_LINES);
            glVertex2f(-1, y); glVertex2f(1, y);
        glEnd();
    }
    // converging vertical lines
    for (int i = -5; i <= 5; i++) {
        float topX = i * 0.20f;
        glBegin(GL_LINES);
            glVertex2f(topX * 0.1f, 0.6f);
            glVertex2f(topX, -0.55f);
        glEnd();
    }
}

// Floating platform  — a glowing ledge
void drawPlatform(float cx, float cy, float w, float glowR, float glowG, float glowB) {
    // glow beneath
    glColor4f(glowR, glowG, glowB, 0.18f);
    drawRect(cx - w * 0.5f - 0.02f, cy - 0.06f, cx + w * 0.5f + 0.02f, cy);

    // solid platform
    glColor3f(0.22f, 0.25f, 0.35f);
    drawRect(cx - w * 0.5f, cy, cx + w * 0.5f, cy + 0.025f);

    // top edge highlight
    glColor3f(glowR * 0.8f, glowG * 0.8f, glowB * 0.8f);
    glLineWidth(1.8f);
    glBegin(GL_LINES);
        glVertex2f(cx - w * 0.5f, cy + 0.025f);
        glVertex2f(cx + w * 0.5f, cy + 0.025f);
    glEnd();
}

// Alien running figure  (simplified side-view silhouette with limb animation)
void drawAlienRunning(float cx, float cy, float runCycle) {
    float legSwing = sinf(runCycle) * 0.12f;
    float armSwing = -sinf(runCycle) * 0.10f;

    // back leg
    glColor3f(0.14f, 0.20f, 0.30f);
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.02f, cy);
        glVertex2f(cx + 0.02f, cy);
        glVertex2f(cx + 0.04f + legSwing, cy - 0.18f);
        glVertex2f(cx - 0.01f + legSwing, cy - 0.18f);
    glEnd();
    // front leg
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.02f, cy);
        glVertex2f(cx + 0.02f, cy);
        glVertex2f(cx + 0.02f - legSwing, cy - 0.18f);
        glVertex2f(cx - 0.04f - legSwing, cy - 0.18f);
    glEnd();

    // torso
    glColor3f(0.18f, 0.24f, 0.36f);
    drawRect(cx - 0.08f, cy, cx + 0.08f, cy + 0.22f);

    // suit stripe
    glColor3f(0.0f, 0.85f, 0.85f);
    drawRect(cx - 0.008f, cy + 0.02f, cx + 0.008f, cy + 0.20f);

    // back arm
    glColor3f(0.20f, 0.60f, 0.22f);
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.07f, cy + 0.16f);
        glVertex2f(cx - 0.04f, cy + 0.16f);
        glVertex2f(cx - 0.04f + armSwing, cy + 0.06f);
        glVertex2f(cx - 0.10f + armSwing, cy + 0.06f);
    glEnd();
    // front arm  (reaching forward)
    glBegin(GL_QUADS);
        glVertex2f(cx + 0.04f, cy + 0.16f);
        glVertex2f(cx + 0.08f, cy + 0.16f);
        glVertex2f(cx + 0.14f - armSwing, cy + 0.22f);
        glVertex2f(cx + 0.10f - armSwing, cy + 0.22f);
    glEnd();

    // head
    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(cx + 0.04f, cy + 0.30f, 0.07f, 40);

    // fin
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(cx, cy + 0.34f);
        glVertex2f(cx - 0.08f, cy + 0.40f);
        glVertex2f(cx + 0.01f, cy + 0.26f);
    glEnd();

    // eye
    glColor3f(0.90f, 0.95f, 1.0f);
    drawCircle(cx + 0.07f, cy + 0.31f, 0.015f, 16);
    glColor3f(0.05f, 0.05f, 0.05f);
    drawCircle(cx + 0.075f, cy + 0.31f, 0.007f, 12);
}

// Glowing star-shaped target
void drawGlowingTarget(float cx, float cy, float pulse) {
    float r = 0.055f + pulse * 0.015f;

    // outer glow rings
    for (int g = 3; g >= 1; g--) {
        glColor4f(1.0f, 0.82f, 0.20f, 0.06f * g);
        drawCircle(cx, cy, r * (1.0f + g * 0.5f), 40);
    }

    // main golden orb
    glColor3f(1.0f, 0.85f, 0.25f);
    drawCircle(cx, cy, r, 40);

    // bright core
    glColor3f(1.0f, 1.0f, 0.80f);
    drawCircle(cx, cy, r * 0.45f, 20);

    // 4-point star rays
    glColor4f(1.0f, 0.90f, 0.40f, 0.75f);
    glLineWidth(2.0f);
    for (int i = 0; i < 4; i++) {
        float a = i * 3.14159f / 2.0f;
        float rayLen = r * (1.8f + pulse * 0.6f);
        glBegin(GL_LINES);
            glVertex2f(cx, cy);
            glVertex2f(cx + cosf(a) * rayLen, cy + sinf(a) * rayLen);
        glEnd();
    }
}

// Dashed line from alien hand toward target  (shows reach / longing)
void drawReachLine(float ax, float ay, float tx, float ty) {
    glColor4f(0.70f, 0.85f, 1.0f, 0.40f);
    glLineWidth(1.4f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    glBegin(GL_LINES);
        glVertex2f(ax, ay);
        glVertex2f(tx, ty);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

// Full chasing scene  (t = local time 0..8)
void drawChasingScene(float t) {
    drawPressureBackground();

    // --- platforms ---
    // platform 1: alien stands / runs on it
    float p1x = -0.35f;
    float p1y = -0.45f;
    drawPlatform(p1x, p1y, 0.55f, 0.30f, 0.60f, 1.0f);

    // platform 2: intermediate (bobbing)
    float p2y = -0.15f + sinf(globalTime * 1.2f) * 0.05f;
    drawPlatform(0.15f, p2y, 0.30f, 0.55f, 0.30f, 0.90f);

    // platform 3: target platform  (higher up, drifting right)
    float p3x = 0.42f + sinf(globalTime * 0.7f) * 0.08f;
    float p3y = 0.18f + cosf(globalTime * 0.5f) * 0.04f;
    drawPlatform(p3x, p3y, 0.28f, 1.0f, 0.82f, 0.20f);

    // --- glowing target  (always slightly ahead of where alien can reach) ---
    float pulse = (sinf(globalTime * 2.8f) + 1.0f) * 0.5f;
    float targetX = p3x + 0.06f;
    float targetY = p3y + 0.13f;
    drawGlowingTarget(targetX, targetY, pulse);

    // --- alien running on platform 1 ---
    float runCycle = globalTime * 6.0f;
    float alienX = p1x - 0.06f;
    float alienY = p1y + 0.025f;
    drawAlienRunning(alienX, alienY, runCycle);

    // reach line  (from alien hand toward target)
    float handX = alienX + 0.14f + (-sinf(runCycle)) * 0.10f;
    float handY = alienY + 0.22f;
    drawReachLine(handX, handY, targetX, targetY);

    // shadow beneath alien  (grounding)
    glColor4f(0.0f, 0.0f, 0.0f, 0.30f);
    drawRect(alienX - 0.10f, p1y - 0.01f, alienX + 0.10f, p1y + 0.01f);
}


// -------------------------------------------------------
//  CROSSFADE OVERLAY  (blendT = 0..1)
// -------------------------------------------------------
void drawFadeOverlay(float alpha) {
    glColor4f(0.0f, 0.0f, 0.0f, alpha);
    drawRect(-1, -1, 1, 1);
}

<<<<<<< ryan-scene

// -------------------------------------------------------
//  UPDATED display()  — paste this over your existing one
// -------------------------------------------------------
/*

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // --- Group A  (0 – 14 s) ---
    if (globalTime < 14.0f) {
        drawSceneWindowFast();
    }

    // --- Group B  (14 – 34 s) ---
    else if (globalTime < 34.0f) {
        float bTime = globalTime - 14.0f; // local time 0..20

        // Scene 1 : Battle  (local 0..10)
        if (bTime < 10.0f) {
            drawBattleScene(bTime);
        }

        // Crossfade  (local 10..12)
        else if (bTime < 12.0f) {
            float blend = (bTime - 10.0f) / 2.0f; // 0→1
            // draw battle fading out
            drawBattleScene(10.0f);
            drawFadeOverlay(blend);

            // when blend > 0.5 start drawing chasing scene underneath
            if (blend > 0.5f) {
                float cBlend = (blend - 0.5f) * 2.0f; // 0→1
                drawChasingScene(0.0f);
                drawFadeOverlay(1.0f - cBlend);
            }
        }

        // Scene 2 : Chasing  (local 12..20)
        else {
            float cTime = bTime - 12.0f; // 0..8
            drawChasingScene(cTime);
        }
    }

    // --- Group C placeholder  (34 s +) ---
    else {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.05f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "FIN");
    }
=======
    if (globalTime < 7.0f) {
        drawSceneWindowFast();
    } else if (globalTime < 14.0f) {
        drawAlienFaceScene(globalTime - 7.0f);
    } else if (globalTime < 21.0f) {
        drawSciFiFlashbackBattle(globalTime - 14.0f);
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.05f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "FIN");
    }
     
    drawChatBox();
>>>>>>> main

    glutSwapBuffers();
}

*/








void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
 
    // --- Group A  (0 – 14 s) ---
    if (globalTime < 14.0f) {
        drawSceneWindowFast();
    }
 
    // --- Group B  (14 – 34 s) ---
    else if (globalTime < 34.0f) {
        float bTime = globalTime - 14.0f; // local time 0..20
 
        // Scene 1 : Battle  (local 0..10)
        if (bTime < 10.0f) {
            drawBattleScene(bTime);
        }
 
        // Crossfade  (local 10..12)
        else if (bTime < 12.0f) {
            float blend = (bTime - 10.0f) / 2.0f; // 0→1
            // draw battle fading out
            drawBattleScene(10.0f);
            drawFadeOverlay(blend);
 
            // when blend > 0.5 start drawing chasing scene underneath
            if (blend > 0.5f) {
                float cBlend = (blend - 0.5f) * 2.0f; // 0→1
                drawChasingScene(0.0f);
                drawFadeOverlay(1.0f - cBlend);
            }
        }
 
        // Scene 2 : Chasing  (local 12..20)
        else {
            float cTime = bTime - 12.0f; // 0..8
            drawChasingScene(cTime);
        }
    }
 
    // --- Group C placeholder  (34 s +) ---
    else {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.05f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "FIN");
    }
 
    glutSwapBuffers();
}




<<<<<<< ryan-scene


=======
>>>>>>> main
void initGL() {
    glClearColor(0.01f, 0.01f, 0.03f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    srand((unsigned int)time(nullptr));
    initializeStars();
    initializePlanets();
}

void timer(int) {
    globalTime += 1.0f / FRAMES_PER_SECOND;
    glutPostRedisplay();
    glutTimerFunc((unsigned int)(1000.0f / FRAMES_PER_SECOND), timer, 0);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("The Final Coordinate - Revised");

    glutFullScreen();

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
<<<<<<< ryan-scene
=======












>>>>>>> main
