#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>

using namespace std;

enum Color { WHITE, BROWN, ORANGE, GREY, BLUE, DARK_GREY, YELLOW, GREEN, RED};
enum Direction { LEFT, RIGHT, UP, DOWN, FORWARD, BACKWARD };

// translacao
float X = 0, Y = 0, Z = 0;
// rotacao
float I = 0, J = 0, K = 0;

float SCALE = 1;
int WIDTH = 1280;
int HEIGTH = 720;

// luz ligada ou desligada
bool lightEnabled = true;

// posição da luz
//GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightStep = 0.1f;
GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_direction[] = { 0.0, -1.0, 0.0 };
GLfloat light_spot_cutoff = 30.0;
GLfloat light_spot_exponent = 2.0;
GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

void applyTranslationRotation() {
    glTranslatef(X, Y, Z);
    glRotatef(I, 1, 0, 0);
    glRotatef(J, 0, 1, 0);
    glRotatef(K, 0, 0, 1);
}

void nextColor(Color color) {
    switch (color) {
    case WHITE:
        glColor3f(0.90, 0.90, 0.90);
        break;
    case BROWN:
        glColor3f(0.45, 0.35, 0.25);
        break;
    case GREY:
        glColor3f(0.8, 0.8, 0.8);
        break;
    case DARK_GREY:
        glColor3f(0.4, 0.4, 0.4);
        break;
    case BLUE:
        glColor3f(0.6, 0.75, 0.85);
        break;
    case ORANGE:
        glColor3f(0.75, 0.25, 0);
        break;
    case YELLOW:
        glColor3f(1, 1, 0);
        break;
    case GREEN:
        glColor3f(0, 0.5, 0);
        break;
    case RED:
        glColor3f(1.0, 0.0, 0.0);
        break;
    }
}

// Ajustando a perspectiva para as mudanças de tamanho da janela
void changeSize(int w, int h) {
    if (h == 0)
        h = 1;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(float x, float y, float z, float length) {
    GLfloat h = length * 0.5f;

    GLfloat vertices[] = {
        // frontal
        x + h, y + h, z + h,
        x + h, y - h, z + h,
        x - h, y - h, z + h,
        x - h, y + h, z + h,
        // traseira
        x + h, y + h, z - h,
        x + h, y - h, z - h,
        x - h, y - h, z - h,
        x - h, y + h, z - h,
        // esquerda
        x - h, y - h, z + h,
        x - h, y + h, z + h,
        x - h, y + h, z - h,
        x - h, y - h, z - h,
        // direita
        x + h, y - h, z + h,
        x + h, y + h, z + h,
        x + h, y + h, z - h,
        x + h, y - h, z - h,
        // superior
        x - h, y + h, z + h,
        x - h, y + h, z - h,
        x + h, y + h, z - h,
        x + h, y + h, z + h,
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glDrawArrays(GL_QUADS, 0, 20);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawRoof(float x, float y, float z,
    float b, float length, float height) {

    float bh = b * 0.55;
    float lh = length * 0.55;
    float hh = height * 0.55;

    GLfloat vertices[] = {
        // face esquerda retangular
        x + lh, y + hh, z,
        x + lh, y - hh, z + bh,
        x - lh, y - hh, z + bh,
        x - lh, y + hh, z,
        // face direita retangular
        x + lh, y + hh, z,
        x + lh, y - hh, z - bh,
        x - lh, y - hh, z - bh,
        x - lh, y + hh, z,
    };

    nextColor(ORANGE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 8);

    // os triângulos devem estar adjacentes às paredes
    lh = length * 0.5;

    GLfloat triangles[]{
        x + lh, y + hh, z,
        x + lh, y - hh, z - bh,
        x + lh, y - hh, z + bh,

        x - lh, y + hh, z,
        x - lh, y - hh, z + bh,
        x - lh, y - hh, z - bh,
    };

    nextColor(YELLOW);
    glVertexPointer(3, GL_FLOAT, 0, triangles);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawWindow(float scale, int xAxis, int zAxis) {
    glPushMatrix();
    applyTranslationRotation();
    nextColor(BLUE);
    glTranslatef(0.4 * scale * xAxis, 0, zAxis * scale / 2 + 0.01 * zAxis);
    glRectf(-0.2 * scale, -0.2 * scale, 0.2 * scale, 0.2 * scale);
    glPopMatrix();
}

void drawHouse(const float scale) {

    GLfloat matSpecular[] = { 1.f, 1.f, 1.f, 1.f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMateriali(GL_FRONT, GL_SHININESS, 60);

    glPushMatrix();
    applyTranslationRotation();
    nextColor(YELLOW);
    drawCube(-0.5 * scale, 0, 0, 1 * scale);
    drawCube(0.5 * scale, 0, 0, 1 * scale);
    drawRoof(0, 0.7 * scale, 0, 1.2 * scale, 2 * scale, 0.5 * scale);
    // chaminé
    nextColor(GREY);
    drawCube(0.5 * scale, 0.65 * scale, 0.3 * scale, 0.15 * scale);
    drawCube(0.5 * scale, 0.8 * scale, 0.3 * scale, 0.15 * scale);
    // porta
    nextColor(BROWN);
    glRotatef(90, 0, scale, 0);
    glTranslatef(0, 0, scale + 0.01);
    glRectf(-0.15 * scale, -0.5 * scale, 0.15 * scale, 0.1 * scale);

    nextColor(YELLOW);
    glTranslatef(-0.1 * scale, -0.2 * scale, 0.008);
    glutSolidSphere(0.02f * scale, 10, 10);

    glPopMatrix();

    // janelas
    drawWindow(scale, 1, 1);
    drawWindow(scale, -1, 1);
    drawWindow(scale, 1, -1);
    drawWindow(scale, -1, -1);
}

void writeMessage(float x, float y, std::string message) {
    // escreve as teclas na tela
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para o texto
    glRasterPos2f(x, y);        // Posição do texto na tela

    for (const char& character : message) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, character);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(light_position[0], light_position[1], light_position[2]);

    // Desenhe uma representação visual da luz
    nextColor(BLUE); // Amarelo
    glutSolidSphere(0.1f, 10, 10);

    glPopMatrix();

    // redefinir transformações
    glLoadIdentity();
    gluLookAt(6, 3, 2, // localização da câmera
        0, 0, 0,       // ponto de visão
        0, 1, 0);      // Vetor de orientação vertical

    // desenha a casa
    drawHouse(SCALE);

    // espaço / chão
    nextColor(GREEN);
    glBegin(GL_QUADS);
    glVertex3f(-5, -0.5, -5);
    glVertex3f(-5, -0.5, 5);
    glVertex3f(5, -0.5, 5);
    glVertex3f(5, -0.5, -5);
    glEnd();

    // escreve na tela
    std::string messageKeys2 = " seta esquerda = Gira para esquerda /\n"
        " seta direita = Gira para direita /\n"
        " seta cima	= Gira para cima /\n"
        " seta baixo = Gira para baixo";
    writeMessage(10, 10, messageKeys2);

    std::string messageKeys = "Teclas para casa (setas):\n"
        " shift + seta esquerda = Rotaciona para esquerda /\n"
        " shift + seta direita = Rotaciona para direita";
    writeMessage(10, 40, messageKeys);

    std::string messageHouse = "Teclas para casa:\n"
        " w = Mover para frente /\n"
        " s = Mover para tras /\n"
        " a = Mover para esquerda /\n"
        " d = Mover para direita /\n"
        " W = Mover para acima /\n"
        " S = Mover para baixo /\n"
        " + = Aumenta o tamanho /\n"
        " - = Diminui o tamanho";
    writeMessage(10, 70, messageHouse);

    std::string messageLight = "Teclas para iluminação:\n"
        " l/L = liga/desliga a luz /\n"
        " h = Mover para esquerda /\n"
        " k - Mover para direita /\n"
        " j - Mover para frente /\n"
        " u - Mover para tras /\n"
        " y - Mover para cima /\n"
        " i - Mover para baixo";
    writeMessage(10, 100, messageLight);

    glutSwapBuffers();
}

void moveLight(Direction direction) {
    switch (direction)
    {
    case LEFT:
        light_position[2] += lightStep;
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        break;
    case RIGHT:
        light_position[2] -= lightStep;
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        break;
    case UP:
        light_position[1] += lightStep;
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        break;
    case DOWN:
        light_position[1] -= lightStep;
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        break;
    case FORWARD:
        light_position[0] -= lightStep;
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        break;
    case BACKWARD:
        light_position[0] += lightStep;
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        break;
    default:
        break;
    }
}

void toggleLight() {
    lightEnabled = !lightEnabled;
    if (lightEnabled) {
        glEnable(GL_LIGHTING);
    }
    else {
        glDisable(GL_LIGHTING);
    }
}

void processNormalKeys(unsigned char key, int x, int y) {
    float threshold = 0.1;
    switch (key) {
    case 27: // escape
        exit(0);
        break;
        // para a tradução de combinações de teclas do vim
    case 'a': // esquerda
        Z += threshold;
        break;
    case 'd': // direita
        Z -= threshold;
        break;
    case 'w': // frente
        X -= threshold;
        break;
    case 's': // voltar
        X += threshold;
        break;
    case 'W': // acima
        Y += threshold;
        break;
    case 'S': // abaixo
        Y -= threshold;
        break;
    case '+': // aumenta o tamanho
        SCALE += threshold;
        break;
    case '-': // diminui o tamanho
        if (SCALE > threshold)
            SCALE -= threshold;
        break;
    case 'l':
    case 'L': // liga/desliga iluminação
        toggleLight();
        break;
    case 'h': // move a iluminação para esquerda
        moveLight(LEFT);
        break;
    case 'k': // move a iluminação para direita
        moveLight(RIGHT);
        break;
    case 'y': // move a iluminação para cima
        moveLight(UP);
        break;
    case 'i': // move a iluminação para baixo
        moveLight(DOWN);
        break;
    case 'u': // move a iluminação para frente
        moveLight(FORWARD);
        break;
    case 'j': // move a iluminação para tras
        moveLight(BACKWARD);
        break;
    }
}

void processSpecialKeys(int key, int x, int y) {
    float threshold = 7.5;

    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
        switch (key) {
        case GLUT_KEY_LEFT: // rotaciona para esquerda
            I += threshold;
            break;
        case GLUT_KEY_RIGHT: // rotaciona para direita
            I -= threshold;
            break;
        }
        return;
    }

    switch (key) {
    case GLUT_KEY_LEFT: // gira para esquerda
        J -= threshold;
        break;
    case GLUT_KEY_RIGHT: // gira para direita
        J += threshold;
        break;
    case GLUT_KEY_UP: // gira para cima
        K += threshold;
        break;
    case GLUT_KEY_DOWN: // gira para baixo
        K -= threshold;
        break;
    }
}

// depreciada
/*void iluminarCenario() {

    GLfloat position[] = { 0.0f, 0.0f, 5.0f, 1.0f }; // Posição da fonte de luz
    GLfloat direction[] = { 0.0f, 0.0f, -1.0f }; // Direção do spot
    GLfloat cutoff = 30.0f; // Ângulo de abertura do spot
    GLfloat exponent = 2.0f; // Expoente de atenuação do spot

    // iluminação
    glEnable(GL_LIGHTING); // Habilitar iluminação
    glEnable(GL_LIGHT0);   // Habilitar luz 0
    glEnable(GL_LIGHT1);   // Habilitar luz 1

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    GLfloat luzAmbiente[4] = { 0.7, 0.7, 0.7, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    GLfloat light0[4][4] = {
        {0.1f, 0.1f, 0.1f, 1.f}, // ambiente
        {0.8f, 0.8f, 0.8f, 1.f}, // diffuse
        {1.f, 1.f, 1.f, 1.f},    // specular
        {0.f, 0.f, 1.f, 1.f},    // position
    };
    
    //glLightfv(GL_LIGHT0, GL_AMBIENT, &light0[0][0]);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, &light0[1][0]);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, &light0[2][0]);
    //glLightfv(GL_LIGHT0, GL_POSITION, &light0[3][0]);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); // Definir posição da luz
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction); // Definir direção do spot
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff); // Definir ângulo de abertura do spot
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent); // Definir expoente de atenuação do spot
}*/

void initLights() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  // pontual
    glEnable(GL_LIGHT1);  // spot
    glEnable(GL_LIGHT2);  // direcional

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Define a posição, direção e propriedades da iluminação pontual
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Define a posição, direção, ângulo de abertura e propriedades da iluminação spot
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction);
    glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, &light_spot_cutoff);
    glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, &light_spot_exponent);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    // Define a direção e propriedades da iluminação direcional
    glLightfv(GL_LIGHT2, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);

    //glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

int main(int argc, char** argv) {

    // inicia GLUT e cria a janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(150, 150);
    glutInitWindowSize(WIDTH, HEIGTH);
    glutCreateWindow("Casa 3d Com pontos de luz");

    // registrar callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

    // para renderizar gráficos back-to-back
    glEnable(GL_DEPTH_TEST); // Habilitar teste de profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // iluminação
    //iluminarCenario(); // depreciada
    initLights();

    glutMainLoop();
    return 1;
}