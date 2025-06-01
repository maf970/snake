#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <cstring>
#include <ctime>

using namespace std;

void MoveXY(int x, int y)
{
    COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

const int mWidth = 80;
const int mHeight = 25;

struct TMap {
    char map1[mHeight][mWidth + 1];
    void Clear();
    void Show();
    void DrawBorders(); // Новая функция для рисования границ
};

enum TDirection { dirLeft, dirRight, dirUp, dirDown };
enum TResult { reOk, reKill };

const int sLen = 1000;

class TSnake {
    int x, y;
    TMap* mp;
    TDirection dir;
    POINT tail[sLen];
    int len;
    int score;
    POINT OutTextPos;
    bool isAlive;
public:
    void AddTail(int _x, int _y);
    void MoveTail(int _x, int _y);
    TSnake(TMap* _mp, int outX, int outY);
    void Init(int _x, int _y, TDirection _dir);
    void PutOnMap();
    TResult UserControl(int w, int s, int a, int d);
    void IncScore();
    bool IsAlive() const { return isAlive; }
    void Revive() { isAlive = true; }
};

void TSnake::IncScore()
{
    score++;
}

void TSnake::MoveTail(int _x, int _y)
{
    for (int i = len - 1; i >= 0; i--)
        tail[i + 1] = tail[i];
    tail[0].x = _x;
    tail[0].y = _y;
}

void TSnake::AddTail(int _x, int _y)
{
    MoveTail(_x, _y);
    len++;
    if (len >= sLen) len = sLen - 1;
}

TResult TSnake::UserControl(int w, int s, int a, int d)
{
    if (!isAlive) return reOk;

    POINT old;
    old.x = x;
    old.y = y;

    if (GetAsyncKeyState(w) & 0x8000) dir = dirUp;
    if (GetAsyncKeyState(s) & 0x8000) dir = dirDown;
    if (GetAsyncKeyState(a) & 0x8000) dir = dirLeft;
    if (GetAsyncKeyState(d) & 0x8000) dir = dirRight;

    if (dir == dirLeft) x--;
    if (dir == dirRight) x++;
    if (dir == dirUp) y--;
    if (dir == dirDown) y++;

    // Проверка столкновения с границами или хвостом
    if ((x <= 0) || (x >= mWidth - 1) ||
        (y <= 0) || (y >= mHeight - 1) ||
        (mp->map1[y][x] == '+'))
    {
        isAlive = false;
        return reKill;
    }

    if (mp->map1[y][x] == '*')
    {
        AddTail(old.x, old.y);
        mp->map1[y][x] = ' ';
    }
    else
    {
        MoveTail(old.x, old.y);
    }

    return reOk;
}

void TSnake::Init(int _x, int _y, TDirection _dir)
{
    x = _x;
    y = _y;
    dir = _dir;
    len = 0;
    isAlive = true;
}

void TSnake::PutOnMap()
{
    char str[20];

    if (OutTextPos.x < mWidth / 2) {
        sprintf_s(str, "P1 Score: %d", score);
    }
    else {
        sprintf_s(str, "P2 Score: %d", score);
    }

    for (int i = 0; i < strlen(str); i++)
        mp->map1[OutTextPos.y][OutTextPos.x + i] = str[i];

    if (isAlive)
    {
        mp->map1[y][x] = '@';
        for (int i = 0; i < len; i++)
            mp->map1[tail[i].y][tail[i].x] = '+';
    }
}

TSnake::TSnake(TMap* _mp, int outX, int outY)
{
    mp = _mp;
    Init(0, 0, dirRight);
    score = 0;
    OutTextPos.x = outX;
    OutTextPos.y = outY;
    isAlive = true;
}

void TMap::DrawBorders()
{
    // Рисуем горизонтальные границы
    for (int x = 0; x < mWidth; x++)
    {
        map1[0][x] = '#';
        map1[mHeight - 1][x] = '#';
    }

    // Рисуем вертикальные границы
    for (int y = 1; y < mHeight - 1; y++)
    {
        map1[y][0] = '#';
        map1[y][mWidth - 1] = '#';
    }
}

void TMap::Show()
{
    MoveXY(0, 0);
    map1[mHeight - 1][mWidth - 1] = '\0';
    for (int j = 0; j < mHeight; j++)
        printf("%s\n", map1[j]);
}

void TMap::Clear()
{
    // Заполняем все поле пробелами
    for (int i = 0; i < mWidth; i++)
        map1[0][i] = ' ';
    map1[0][mWidth] = '\0';
    for (int j = 1; j < mHeight; j++)
        strncpy_s(map1[j], map1[0], mWidth + 1);

    // Рисуем границы
    DrawBorders();

    // Размещаем еду на карте
    map1[12][20] = '*';
    map1[12][60] = '*';
    map1[5][40] = '*';
    map1[20][40] = '*';
}

int main()
{
    system("mode con cols=80 lines=25");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    srand(static_cast<unsigned int>(time(nullptr)));

    int userCnt = 2;
    TMap map1;
    TSnake snake1(&map1, 3, 1);
    TSnake snake2(&map1, 60, 1);
    snake1.Init(10, 5, dirRight);
    snake2.Init(70, 5, dirLeft);

    do
    {
        bool kill1 = snake1.UserControl('W', 'S', 'A', 'D') == reKill;
        bool kill2 = false;

        if (userCnt >= 2)
        {
            kill2 = snake2.UserControl(VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT) == reKill;
        }

        if (kill1)
        {
            snake2.IncScore();
            snake1.Init(10, 5, dirRight);
        }
        if (kill2)
        {
            snake1.IncScore();
            snake2.Init(70, 5, dirLeft);
        }

        map1.Clear();
        snake1.PutOnMap();
        if (userCnt >= 2) snake2.PutOnMap();
        map1.Show();
        Sleep(100);
    } while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000));

    return 0;
}
