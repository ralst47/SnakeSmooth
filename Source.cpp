#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <cmath>
#include <conio.h>
#include <iostream>
#include <windows.h>
#include <ctime>
#include <GLFW/glfw3.h>
#include <stb_easy_font.h>
#include <string>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

enum TDir { RIGHT, UP, LEFT, DOWN };
const int width = 30;
const int height = width;
const float offsetStep = 0.2;

float offset = 0.0f;
bool breakPlay = false;
bool flag = false;
bool flag1 = false;
char str[35];
int quant = 3;
void putApl();

struct TSnk
{
    bool hd;
    bool bd;
    int nseg;
};

struct TCell  //слоёная клетка
{
    bool apl;
    bool brd;
    TSnk snk;
};

TCell map[height][width];

struct TColor
{
    byte r, g, b;
};

struct TCoord
{
    int x;
    int y;
    int dir;
    TColor color;

    TCoord& operator = (const TCoord& other)//Перегрузка оператора присваивания
    {
        this->x = other.x;
        this->y = other.y;
        this->dir = other.dir;
        return *this;
    }
};

struct TSnake
{
    int delay = 8000 * pow(offsetStep, 2) / pow(width * height, 0.5);
    TCoord seg[height * width];
    TDir dir;
    int length;
    bool mouth;
    int step;
    TSnake() { dir = RIGHT; seg[0] = { 5, 5 }; length = 1; mouth = false; step = 2; }

    void Move()
    {
        map[seg[0].y][seg[0].x].snk.hd = true;

        for (int i = 1; i < length; i++)
        {
            map[seg[i].y][seg[i].x].snk.bd = true;
            map[seg[i].y][seg[i].x].snk.nseg = i;
        }

        map[seg[length].y][seg[length].x].snk.bd = false;
        map[seg[1].y][seg[1].x].snk.hd = false;

        for (int i = length; i > 0; i--)
            seg[i] = seg[i - 1];
    }

    void IsBreak()
    {
        //        for (int i = 2; i <= length; i++)
        //            if (map[seg[i].y][seg[i].x].snk.hd)
        if (map[seg[0].y][seg[0].x].snk.bd)
            breakPlay = true;
    }

    void appEvent()
    {
        if (map[seg[0].y][seg[0].x].apl)
        {
            //           PlaySound("apple.wav", NULL, SND_ASYNC);
            Sleep(5);
            map[seg[0].y][seg[0].x].apl = false;
            seg[length].color.r = rand() % 255;
            seg[length].color.g = rand() % 255;
            seg[length].color.b = rand() % 255;
            length++;
            quant = rand() % 1 + 1;
            //if (length < delay / 2 + 10) delay -= 2;
            putApl();
        }
    }

    void Control(char key_LEFT, char key_RIGHT, char key_DOWN, char key_UP)
    {
        int a, b;
        a = seg[0].x;
        b = seg[0].y;
        if ((GetKeyState(VK_UP) & 0x80 || GetKeyState(key_UP) & 0x80) && !flag && dir != DOWN)
        {
            flag = true;
            dir = UP;
        }
        if ((GetKeyState(VK_DOWN) & 0x80 || GetKeyState(key_DOWN) & 0x80) && !flag && dir != UP)
        {
            flag = true;
            dir = DOWN;
        }
        if ((GetKeyState(VK_RIGHT) & 0x80 || (GetKeyState(key_RIGHT) & 0x80)) && !flag && dir != LEFT)
        {
            flag = true;
            dir = RIGHT;
        }
        if ((GetKeyState(VK_LEFT) & 0x80 || GetKeyState(key_LEFT) & 0x80) && !flag && dir != RIGHT)
        {
            flag = true;
            dir = LEFT;
        }

        if (dir == RIGHT) if (map[b][a + 1].brd) a = 1; else { ++a; }
        if (dir == LEFT) if (map[b][a - 1].brd) a = width - 2; else { --a; }
        if (dir == UP) if (map[b + 1][a].brd) b = 1; else { ++b; }
        if (dir == DOWN) if (map[b - 1][a].brd) b = height - 2; else { --b; }

        if (GetKeyState(VK_ESCAPE) < 0)  breakPlay = true;

        seg[0].x = a;
        seg[0].y = b;
        mouth = (a + b) % 2;
        flag = false;
        Move();
        IsBreak();
        appEvent();
    }
};

TSnake snake;

void putApl()
{
    int ax;
    int ay;
    for (int i = 0; i < quant; i++)
    {
        do {
            ax = rand() % (width - 2) + 1;
            ay = rand() % (height - 2) + 1;

        } while (map[ay][ax].apl || map[ay][ax].snk.hd || map[ay][ax].snk.bd);
        map[ay][ax].apl = true;
    }
}

void Border()
{
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1)
            {
                map[j][i].brd = true;
            }
    }
}

void Rotate(int Alfa)
{
    glTranslatef(0.5f, 0.5f, 0.0f);
    glRotatef(Alfa, 0.0f, 0.0f, 1.0f);
    glTranslatef(-0.5f, -0.5f, 0.0f);
}

void ShowHeader()
{
    int dir;
    const float gap = 0;//0-0.5 - зазор
    float x, y;
    float step = (2 * M_PI / 16);//шаг и ширина открытия рта

    if (snake.dir == RIGHT)  dir = 0;//0,1,2,3 - направление
    if (snake.dir == UP)  dir = 1;//0,1,2,3 - направление
    if (snake.dir == LEFT)  dir = 2;//0,1,2,3 - направление
    if (snake.dir == DOWN)  dir = 3;//0,1,2,3 - направление

    Rotate(dir * 90);//поворот головы
    glTranslatef(offset, 0.0f, 0.0f);//смещение
    glBegin(GL_POLYGON);
    glColor3f(0, 0.8, 0.2);
    if (snake.mouth) glVertex2f(0.1, 0.5);
    for (double i = step; i < 2 * M_PI; i += step)
    {
        x = cos(i) * 0.5 * (1 - gap * 2) + 0.5;
        y = sin(i) * 0.5 * (1 - gap * 2) + 0.5;
        glVertex2f(x, y);
    }
    if (snake.mouth) glVertex2f(0.1, 0.5);
    glEnd();
}

void ShowHeader0()
{
    int dir;
    const float gap = 0;//0-0.5 - зазор

    if (snake.dir == RIGHT)  dir = 0;//0,1,2,3 - направление
    if (snake.dir == UP)  dir = 1;//0,1,2,3 - направление
    if (snake.dir == LEFT)  dir = 2;//0,1,2,3 - направление
    if (snake.dir == DOWN)  dir = 3;//0,1,2,3 - направление

    float rotation = M_PI / 2 * dir;
    float x, y;
    float step = (2 * M_PI / 16);//шаг и ширина открытия рта

    glBegin(GL_POLYGON);
    glColor3f(0, 0.8, 0.2);
    if (snake.mouth) glVertex2f(0.5, 0.5);
    for (double i = step + rotation; i < 2 * M_PI + rotation; i += step)
    {
        x = cos(i) * 0.5 * (1 - gap * 2) + 0.5;
        y = sin(i) * 0.5 * (1 - gap * 2) + 0.5;
        glVertex2f(x, y);
    }
    if (snake.mouth) glVertex2f(0.5, 0.5);
    glEnd();
}

void ShowBody(int i, int j)
{
    const float gap = 0;//0-0.5 - зазор
    float x, y;
    float step = (2 * M_PI / 16);//шаг
    int nseg = (map[j][i].snk.nseg);
    if (i == 1 && snake.seg[nseg].dir == 0)
        ShowBody(width - 1, j);
    if (nseg > 0) Rotate(snake.seg[nseg].dir * 90);//поворот головы
    //Rotate(-snake.seg->dir * 90);//поворот головы
    glTranslatef(offset, 0.0f, 0.0f);//смещение
    glBegin(GL_POLYGON);
    glColor3ub(snake.seg[nseg].color.r, snake.seg[nseg].color.g, snake.seg[nseg].color.b);
    //glVertex2f(0.5, 0.5);
    for (double i = 0; i < 2 * M_PI; i += step)
    {
        x = cos(i) * 0.5 * (1 - gap * 2) + 0.5;
        y = sin(i) * 0.5 * (1 - gap * 2) + 0.5;
        glVertex2f(x, y);
    }
    glEnd();
}

void ShowBorder()
{
    const float gap = 0;
    glBegin(GL_QUADS);
    glColor3f(0.5, 0, 0.5); glVertex2f(gap, gap);
    glColor3f(0.4, 0, 0.4); glVertex2f(1 - gap, gap);
    glColor3f(0.5, 0, 0.5); glVertex2f(1 - gap, 1 - gap);
    glColor3f(0.3, 0, 0.3); glVertex2f(gap, 1 - gap);
    glEnd();
}

void ShowApple()
{
    glBegin(GL_POLYGON);        glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.68621f, 1.0f);            glVertex2f(0.647646f, 0.99425f);
    glVertex2f(0.577895f, 0.968148f);            glVertex2f(0.532538f, 0.936295f);
    glVertex2f(0.49945f, 0.899834f);            glVertex2f(0.47815f, 0.863372f);
    glVertex2f(0.466685f, 0.83152f);            glVertex2f(0.460886f, 0.799667f);
    glVertex2f(0.49945f, 0.805417f);            glVertex2f(0.569201f, 0.83152f);
    glVertex2f(0.614558f, 0.863372f);            glVertex2f(0.647646f, 0.899834f);
    glVertex2f(0.668946f, 0.936295f);            glVertex2f(0.680411f, 0.968148f);
    glEnd();
    glBegin(GL_POLYGON);        glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.5f, 0.756049f);            glVertex2f(0.452958f, 0.7585f);
    glVertex2f(0.361591f, 0.777858f);            glVertex2f(0.289014f, 0.797216f);
    glVertex2f(0.25215f, 0.799667f);            glVertex2f(0.215287f, 0.797216f);
    glVertex2f(0.144644f, 0.777858f);            glVertex2f(0.067679f, 0.726477f);
    glVertex2f(0.028942f, 0.675095f);            glVertex2f(0.006203f, 0.606557f);
    glVertex2f(0.0f, 0.517704f);            glVertex2f(0.005338f, 0.435259f);
    glVertex2f(0.021568f, 0.352814f);            glVertex2f(0.04939f, 0.270369f);
    glVertex2f(0.086819f, 0.19372f);            glVertex2f(0.137289f, 0.11707f);
    glVertex2f(0.177281f, 0.06864f);            glVertex2f(0.233331f, 0.02021f);
    glVertex2f(0.285019f, 0.002451f);            glVertex2f(0.314113f, 0.0f);
    glVertex2f(0.343207f, 0.002451f);            glVertex2f(0.396312f, 0.02021f);
    glVertex2f(0.463137f, 0.03797f);            glVertex2f(0.5f, 0.040421f);
    glVertex2f(0.536863f, 0.03797f);            glVertex2f(0.603688f, 0.02021f);
    glVertex2f(0.656793f, 0.002451f);            glVertex2f(0.685887f, 0.0f);
    glVertex2f(0.714981f, 0.002451f);            glVertex2f(0.766669f, 0.02021f);
    glVertex2f(0.822719f, 0.06864f);            glVertex2f(0.862711f, 0.11707f);
    glVertex2f(0.913181f, 0.19372f);            glVertex2f(0.95061f, 0.270369f);
    glVertex2f(0.978432f, 0.352814f);            glVertex2f(0.994662f, 0.435259f);
    glVertex2f(1.0f, 0.517704f);            glVertex2f(0.993797f, 0.606557f);
    glVertex2f(0.971058f, 0.675095f);            glVertex2f(0.932321f, 0.726477f);
    glVertex2f(0.855356f, 0.777858f);
    glVertex2f(0.784713f, 0.797216f);            glVertex2f(0.74785f, 0.799667f);
    glVertex2f(0.710986f, 0.797216f);            glVertex2f(0.638409f, 0.777858f);
    glVertex2f(0.547042f, 0.7585f);    glEnd();
}

void ShowField()
{
    const float gap = 0.04;
    glBegin(GL_QUADS);
    glColor3ub(50, 50, 50); glVertex2f(gap, gap);
    glColor3ub(30, 30, 30); glVertex2f(1 - gap, gap);
    glColor3ub(50, 50, 50); glVertex2f(1 - gap, 1 - gap);
    glColor3ub(70, 70, 70); glVertex2f(gap, 1 - gap);
    glEnd();
}

void ChekCellField(TCell cell, int i, int j)
{
    if (j > 0 && j < height - 1 && i > 0 && i < width - 1) ShowField();
}

void ChekCell(TCell cell, int i, int j)
{
    int nseg = (map[j][i].snk.nseg);
    if (cell.apl) ShowApple();
    if (cell.snk.bd) ShowBody(i, j);
    if (cell.snk.hd) ShowHeader();
    if (cell.brd) ShowBorder();
}

void ChekCellBorder(TCell cell, int i, int j)
{
    if (cell.brd) ShowBorder();
}

void Game_Show()
{
    glLoadIdentity();
    glScaled(2.0 / width, 2.0 / height, 1);
    glTranslatef(-width * 0.5, -height * 0.5, 0);

    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            glPushMatrix();
            glTranslatef(i, j, 0);
            ChekCellField(map[j][i], i, j);
            glPopMatrix();
        }
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            glPushMatrix();
            glTranslatef(i, j, 0);
            ChekCell(map[j][i], i, j);
            glPopMatrix();
        }
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            glPushMatrix();
            glTranslatef(i, j, 0);
            ChekCellBorder(map[j][i], i, j);
            glPopMatrix();
        }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true, 
    // и приложение после этого закроется
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


void print_string(float x, float y, char* text, float r, float g, float b)
{
    char buffer[99999]; // ~500 chars
    int num_quads;

    num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Score()
{
    glTranslatef(1 + 0 * (width - 2 - 4) / 2, 8 + (height - 2 - 7) / 2, 0);
    //glTranslatef(5, 8 + 11 * (height - 2) / 36, 0);//20
    //glTranslatef(5, 24, 0);
    glScalef(1, -1, 1);
    snprintf(str, sizeof str, "%d", snake.length - 1);
    print_string(0, 0, str, 0, 1, 0);
}

void GameOver()
{
    glTranslatef(2, height - 2 - 3, 0);
    glScalef(0.5, -0.5, 1);
    snprintf(str, sizeof str, "%s%d", "  GAME\n  OVER!\nSCORE: ", snake.length - 1);
    print_string(0, 0, str, 1, 0.5, 0);
}

int main(void)
{
    srand(time(NULL));
    Border();
    putApl();
    GLFWwindow* window;
    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(700, 700, "SNAKE", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        snake.Control('A', 'D', 'S', 'W');
        snake.seg[0].dir = snake.dir;
        for (offset = -1; offset < 0; offset += offsetStep)
        {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glfwPollEvents();
            Score();
            /* Render here */
            Game_Show();
            glfwSwapBuffers(window);
            glClear(GL_COLOR_BUFFER_BIT);
            Sleep(snake.delay);

        }
        if (breakPlay)
        {
            GameOver();
            glfwSwapBuffers(window);
            Sleep(3000);
            break;
        }
    }
    glfwTerminate();
    return 0;
}