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
const int WIN_SCORE = 10; // Конечное значение рекорда

enum Difficulty { EASY, MEDIUM, HARD };
Difficulty currentDifficulty = EASY;

struct TMap {
    char map1[mHeight][mWidth + 1];
    void Clear();
    void Show();
    void DrawBorders();
    void DrawObstacles();
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
    int speed;
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
    void SetSpeed(int s) { speed = s; }
    int GetSpeed() const { return speed; }
    int GetScore() const { return score; }
};

void TSnake::IncScore()
{
    score++;
    if (score % 5 == 0 && speed > 50) {
        speed -= 10;
    }
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

    if (GetAsyncKeyState(w)) dir = dirUp;
    if (GetAsyncKeyState(s)) dir = dirDown;
    if (GetAsyncKeyState(a) ) dir = dirLeft;
    if (GetAsyncKeyState(d) ) dir = dirRight;

    if (dir == dirLeft) x--;
    if (dir == dirRight) x++;
    if (dir == dirUp) y--;
    if (dir == dirDown) y++;

    if ((x <= 0) || (x >= mWidth - 1) ||
        (y <= 0) || (y >= mHeight - 1) ||
        (mp->map1[y][x] == '+') || (mp->map1[y][x] == '#'))
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
    speed = 150;
}

void TMap::DrawBorders()
{
    for (int x = 0; x < mWidth; x++)
    {
        map1[0][x] = '#';
        map1[mHeight - 1][x] = '#';
    }

    for (int y = 1; y < mHeight - 1; y++)
    {
        map1[y][0] = '#';
        map1[y][mWidth - 1] = '#';
    }
}

void TMap::DrawObstacles()
{
    for (int y = 1; y < mHeight - 1; y++)
    {
        for (int x = 1; x < mWidth - 1; x++)
        {
            if (map1[y][x] == '#') map1[y][x] = ' ';
        }
    }

    switch (currentDifficulty)
    {
    case MEDIUM:
        for (int y = 5; y < mHeight - 5; y++)
        {
            map1[y][mWidth / 2] = '#';
        }
        break;

    case HARD:
        for (int y = 5; y < mHeight - 5; y++)
        {
            map1[y][mWidth / 3] = '#';
            map1[y][2 * mWidth / 3] = '#';
        }

        for (int x = 10; x < mWidth - 10; x++)
        {
            map1[mHeight / 3][x] = '#';
            map1[2 * mHeight / 3][x] = '#';
        }
        break;
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
    for (int i = 0; i < mWidth; i++)
        map1[0][i] = ' ';
    map1[0][mWidth] = '\0';
    for (int j = 1; j < mHeight; j++)
        strncpy_s(map1[j], map1[0], mWidth + 1);

    DrawBorders();
    DrawObstacles();

    map1[12][20] = '*';
    map1[12][60] = '*';
    map1[5][40] = '*';
    map1[20][40] = '*';
}

void ShowDifficultyMenu()
{
    system("cls");
    cout << "Select difficulty level:" << endl;
    cout << "1. Easy (no obstacles)" << endl;
    cout << "2. Medium (some obstacles)" << endl;
    cout << "3. Hard (many obstacles)" << endl;
    cout << "Press ESC to exit" << endl;
}

void SelectDifficulty()
{
    ShowDifficultyMenu();

    while (true)
    {
        if (GetAsyncKeyState('1') )
        {
            currentDifficulty = EASY;
            break;
        }
        else if (GetAsyncKeyState('2') )
        {
            currentDifficulty = MEDIUM;
            break;
        }
        else if (GetAsyncKeyState('3'))
        {
            currentDifficulty = HARD;
            break;
        }
        else if (GetAsyncKeyState(VK_ESCAPE) )
        {
            exit(0);
        }
        Sleep(100);
    }
}

void ShowWinner(const char* winner)
{
    system("cls");
    cout << "=================================" << endl;
    cout << "          GAME OVER!" << endl;
    cout << "        " << winner << " WINS!" << endl;
    cout << "=================================" << endl;
    cout << "Press any key to exit..." << endl;
    _getch();
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

    SelectDifficulty();

    int userCnt = 2;
    TMap map1;
    TSnake snake1(&map1, 3, 1);
    TSnake snake2(&map1, 60, 1);

    switch (currentDifficulty)
    {
    case EASY:
        snake1.SetSpeed(150);
        snake2.SetSpeed(150);
        break;
    case MEDIUM:
        snake1.SetSpeed(100);
        snake2.SetSpeed(100);
        break;
    case HARD:
        snake1.SetSpeed(70);
        snake2.SetSpeed(70);
        break;
    }

    snake1.Init(10, 5, dirRight);
    snake2.Init(70, 5, dirLeft);

    bool gameOver = false;
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

        // Проверка на победу
        if (snake1.GetScore() >= WIN_SCORE)
        {
            ShowWinner("PLAYER 1 (SNAKE1)");
            gameOver = true;
            break;
        }
        else if (snake2.GetScore() >= WIN_SCORE)
        {
            ShowWinner("PLAYER 2 (SNAKE2)");
            gameOver = true;
            break;
        }

        map1.Clear();
        snake1.PutOnMap();
        if (userCnt >= 2) snake2.PutOnMap();
        map1.Show();
        Sleep(snake1.GetSpeed());
    } while (!(GetAsyncKeyState(VK_ESCAPE)) && !gameOver);

    return 0;
}
