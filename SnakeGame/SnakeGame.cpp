#include "iostream"
#include "io.h"
#include "fcntl.h"
#include "conio.h"
#include "windows.h."


using namespace std;

enum ConsoleColor
{
    Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4, Magenta = 5, Brown = 6, LightGray = 7, DarkGray = 8,
    LightBlue = 9, LightGreen = 10, LightCyan = 11, LightRed = 12, LightMagenta = 13, Yellow = 14, White = 15
};


//Функции, которые включают части GameBuilding
void PrepareGame();//начало игры
void PlayGame();//Основной игровой цикл
void WelcomeScreen(); //рисунок экрана приветствия
void DrawMap();//часть предназначена для рисования карты игры
void InputTracker();//Функция, которая отслеживает и реагирует на взаимодействия с пользователем
void GameLogic();//основная игровая логика
void GameOver(int reason);//печатает этикетку gameover

//Общие необходимые переменные
bool game_over; // определяет, идет ли игра

//Размер и дизайн игровой карты
const int map_width = 40;
const int map_height = 20;
const char map_horizontal_border_element = '-';
const char map_vertical_border_element = '|';
const wchar_t* snake_head = L"\x263a";//@
const char snake_body_element = '*';
const wchar_t* fruits[] = { L"\u2665",L"\u2666",L"\u2663",L"\u2660"};//♥ ♦ ♣ ♠
int fruit = 0;
int game_over_reason = 1;

//Змеиный хвост
int x_tail[100], y_tail[100];
int tail_length;

//Координаты
int x, y; //змея
int x_fruit, y_fruit; //фрукты для нашей змейки
//Оценка игры
int score;
//Нумерация направлений движения
enum class Direction {
    STOP = 0,
    LEFT, RIGHT, DOWN, UP
};
Direction where_to_go;


int main() 

{  
    setlocale(0, "");
    //Следующие строки позволяют использовать символы UTF16 в выводе консоли.
    if (_setmode(_fileno(stdout), _O_U16TEXT) == -1) {
        perror("Недоразумения в печати существуют");
    }
        
    game_over = false;
    PrepareGame();//Инициализация запускаемых переменных
    WelcomeScreen();

    while (!game_over)
    {
        if (_kbhit()) { //_kbhit () определяет давление пользователя на клавиатуру
            if (_getch() == 'q') {//_getch () определяет, какая клавиша была нажата 
                game_over = true;
            }
            else
            {
                PlayGame();
            }
        }

    }
    GameOver(game_over_reason);
    
    return 0;
}


void PlayGame() {
    //основной игровой цикл
    while (!game_over)
    {
        DrawMap();
        InputTracker();
        GameLogic();
    }
}

void PrepareGame() {
    //Инициализация запускаемых переменных
    where_to_go = Direction::STOP;
    x = map_width / 2 -1;
    y = map_height / 2 -1;
    x_fruit = rand() % map_width;
    y_fruit = rand() % map_height;
    score = 0;
   

}

void DrawMap() {
    system("cls");
    for (int i = 0; i < map_width; i++) {
        wcout << map_horizontal_border_element;
    }
    wcout << endl;
    for (int i = 0; i < map_height; i++) {
        for (int j = 0; j < map_width; j++) {
            if (j == 0 || j == map_width - 1)
                wcout << map_vertical_border_element;
            else if (i == y && j == x)
                wcout << snake_head;
            else if (i == y_fruit && j == x_fruit)
                wcout << fruits[fruit];
            else {
                bool print = false;
                for (int k = 0; k < tail_length; k++) {
                    if (x_tail[k] == j && y_tail[k] == i) {
                        wcout << snake_body_element;
                        print = true;
                    }
                }
                if (!print) {
                    wcout << " ";
                }                    
            }
                

        }
        wcout << endl;
    }
    for (int i = 0; i < map_width; i++) {
        wcout << map_horizontal_border_element;
    }
    wcout << endl;
    wcout << "Your Score: " << score << endl;

}

void InputTracker() {
    if (_kbhit()) { //_kbhit () определяет давление пользователя на клавиатуру
        switch (_getch ()) { //_getch () определяет, какая клавиша была нажата        
            case '4': 
                where_to_go = Direction::LEFT;
                break;
            case '6':
                where_to_go = Direction::RIGHT;
                break;
            case '8':
                where_to_go = Direction::UP;
                break;
            case '5':
                where_to_go = Direction::DOWN;
                break;
            case 'q':
                game_over = true;
                break;
        }
    }
}

void GameLogic() {
    //переменные предыдущих позиций

    int x_previous = x_tail[0];
    int y_previous = y_tail[0];

    int before_previous_x, before_previous_y;

    x_tail[0] = x;
    y_tail[0] = y;
    // Цикл подготовки данных для рисования хвоста
    for (int i = 1; i < tail_length; i++) {
        before_previous_x = x_tail[i];
        before_previous_y = y_tail[i];
        x_tail[i] = x_previous;
        y_tail[i] = y_previous;
        x_previous = before_previous_x;
        y_previous = before_previous_y;
    }


    //логика движений
    switch (where_to_go) {
        case Direction::UP:
            y--;
            break;
        case Direction::DOWN:
            y++;
            break;
        case Direction::LEFT:
            x--;
            break;
        case Direction::RIGHT:
            x++; 
            break;
    }





    //Логика, позволяющая нашей змейке появляться на противоположной стороне карты в случае перемещения за границу.
    if (x <= 0) {
        x = map_width - 2;
    }
    else if (x >= map_width-1) {
        x = 1;
    }
    
    if (y < 0) {
        y = map_height - 1;
    }
    else if (y >= map_height) {
        y = 0;
    }

    //Логика предотвращения поедания собственного хвоста
    for (int i = 0; i < tail_length; i++) {
        if (x_tail[i] == x && y_tail[i] == y) {
            game_over = true;
            game_over_reason = 2;
        }
    }
    //Логика действий после того, как фрукт был съеден
    if (x == x_fruit && y == y_fruit) {
        score += 10;
        tail_length++;
        fruit = rand() % 4;// создано новое изображение фруктов
        x_fruit = rand() % map_width;
        y_fruit = rand() % map_height;
        while (x_fruit == 0 || x_fruit == map_width - 1) {
            x_fruit = rand() % map_width;
        }
        while (y_fruit == 0 || y_fruit == map_height - 1) {
            y_fruit = rand() % map_height;
        }
    }
}

void WelcomeScreen() {
    system("cls");
    wcout << L"*                * ****** *       ******  ****  **     ** ******" << endl;
    wcout << L" *              *  *      *      *       *    * * *   * * *     " << endl;
    wcout << L"  *     **     *   *      *      *       *    * *  * *  * *     " << endl;
    wcout << L"   *   *  *   *    ****** *      *       *    * *   *   * ******" << endl;
    wcout << L"    * *    * *     *      *    * *       *    * *       * *     " << endl;
    wcout << L"     *      *      ****** ******  ******* ****  *       * ******" << endl;
    wcout << L"♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣" << endl;
    wcout << L"                               '8' - UP                         " << endl;
    wcout << L"                     '4' - LEFT        '6' - RIGHT              " << endl;
    wcout << L"                              '5' - DOWN                        " << endl << endl;
    wcout << L"                           'q' - GAME OVER                      " << endl;
    wcout << L"♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣" << endl;
    wcout << L"Для выхода нажмите 'q', Чтобы НАЧАТЬ, нажмите любую другую кнопку" << endl;

}

void GameOver(int reason) {

    

    system("cls");
    wcout << L" ****  ***  **     ** *****  ****  *         * ***** ***** " << endl;
    wcout << L"*     *   * * *   * * *     *    *  *       *  *     *    *" << endl;
    wcout << L"*     *   * *  * *  * *     *    *   *     *   *     *    *" << endl;
    wcout << L"*  ** ***** *   *   * ***** *    *    *   *    ***** ***** " << endl;
    wcout << L"*   * *   * *       * *     *    *     * *     *     *    *" << endl;
    wcout << L" **** *   * *       * *****  ****       *      ***** *    *" << endl;
    wcout << L"♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣" << endl;
    wcout << L"                    Ваша оценка была: " << score << endl;
    switch (reason)
    {
    case 1:
        wcout << L"         Вы перестали играть по собственному желанию!" << endl;
        break;
    case 2:
        wcout << L"   Какая жалость! Ты свой хвост съел и умер!"<< endl;
        break;
    }
}