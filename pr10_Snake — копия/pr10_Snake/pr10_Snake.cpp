// pr10_Snake.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <conio.h>
#include <fstream>
#include <windows.h>

const int width = 20;
const int height = 20;
int fruinX, fruitY, score, length;

// односвязный список
struct Node 
{
    int x;
    int y;
    Node* next;

};

// Структура для таблицы рекордов
struct Record 
{
    char name[50];
    int score;
};

// Каждый сегмент змейки хранит координаты и указатель на следующий сегмент
Node* head = nullptr;

enum eDirection 
{ 
    STOP = 0, 
    LEFT, 
    RIGHT, 
    UP, 
    DOWN 

};

eDirection dir;


void freeSnake() 
{
    while (head != nullptr) 
    {
        Node* temp = head;
        head = head->next;
        delete temp;

    }

}

// Добавление нового сегмента в начало
void addHead(int nx, int ny) 
{
    Node* newNode = new Node;

    newNode->x = nx;
    newNode->y = ny;

    newNode->next = head;

    head = newNode;

}

// Удаление последнего сегмента при движении
void removeTail() 
{
    if (head == nullptr || head->next == nullptr)
    {
        return;
    }

    Node* curr = head;

    while (curr->next->next != nullptr) 
    {
        curr = curr->next;

    }

    delete curr->next;
    curr->next = nullptr;
}

// Сохранение и загрузка игры из бинарного файла
void save(const char* filename) 
{
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open())
    {
        return;
    }

    // В сохранение счёт, направление движения, положение еды, длина змейки
    out.write((char*)&score, sizeof(score));
    int d = static_cast<int>(dir);
    out.write((char*)&d, sizeof(d));
    out.write((char*)&fruinX, sizeof(fruinX));
    out.write((char*)&fruitY, sizeof(fruitY));
    out.write((char*)&length, sizeof(length));

    // Координаты всех сегментов змейки
    Node* curr = head;
    while (curr != nullptr) 
    {
        out.write((char*)&(curr->x), sizeof(int));
        out.write((char*)&(curr->y), sizeof(int));
        curr = curr->next;

    }
    out.close();
}

bool loadGame(const char* filename) 
{
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;

    freeSnake();

    in.read((char*)&score, sizeof(score));
    int d;
    in.read((char*)&d, sizeof(d));
    dir = static_cast<eDirection>(d);
    in.read((char*)&fruinX, sizeof(fruinX));
    in.read((char*)&fruitY, sizeof(fruitY));
    in.read((char*)&length, sizeof(length));


    Node* tail = nullptr;
    for (int i = 0; i < length; ++i) 
    {
        Node* newNode = new Node;
        in.read((char*)&(newNode->x), sizeof(int));
        in.read((char*)&(newNode->y), sizeof(int));
        newNode->next = nullptr;

        if (head == nullptr) 
        {
            head = newNode;
            tail = newNode;

        }
        else 
        {
            tail->next = newNode;
            tail = newNode;

        }
    }
    in.close();

    return true;

}

// Бинарный файл таблицы рекордов
void appendSnakeRecord(const char* filename, const char* name, int currentScore) 
{
    std::ofstream out(filename, std::ios::binary | std::ios::app);
    if (!out)
    {
        return;
    }

    Record r;
    int i = 0;
    while (name[i] != 0 && i < 49) 
    {
        r.name[i] = name[i];
        i++;

    }
    r.name[i] = 0;
    r.score = currentScore;

    out.write((char*)&r, sizeof(Record));
    out.close();
}

void showLeaderboard(const char* filename) 
{
    std::ifstream in(filename, std::ios::binary);
    system("cls"); // Очистка консоли
    std::cout << "ТАБЛИЦА РЕКОРДОВ\n";
    if (!in) 
    {
        _getch();
        return;

    }
    Record r;
    while (in.read((char*)&r, sizeof(Record))) 
    {
        std::cout << r.name << ": " << r.score << "\n";

    }
    in.close();
    _getch();
}

void Setup(bool isLoaded)
{
    if (!isLoaded) 
    {
        freeSnake();
        dir = STOP;
        score = 0;
        length = 3;

        // Начальная инициализация списка змейки в центре
        addHead(width / 2 - 2, height / 2);
        addHead(width / 2 - 1, height / 2);
        addHead(width / 2, height / 2);

        fruinX = 1 + rand() % (width - 2);
        fruitY = 1 + rand() % (height - 2);

    }
}

void Draw()
{
    system("cls"); // Очистка консоли
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Границы поля
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
            {
                std::cout << "#";
            }
            else if (i == fruitY && j == fruinX)
            {
                std::cout << "F";
            }
            else
            {
                bool print = false;
                Node* curr = head;

                if (curr != nullptr && curr->x == j && curr->y == i) 
                {
                    std::cout << "0"; // Голова
                    print = true;

                }
                else 
                {
                    curr = (curr != nullptr) ? curr->next : nullptr;
                    while (curr != nullptr) 
                    {
                        if (curr->x == j && curr->y == i) 
                        {
                            std::cout << "o"; // Тело
                            print = true;
                            break;

                        }
                        curr = curr->next;

                    }
                }
                if (!print)
                {
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl;
    }

    std::cout << "Score: " << score << std::endl;
    std::cout << "Нажмите 'P' во время игры для сохранения состояния\n";
}

bool Input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a': 
            if (dir != RIGHT)
            {
                dir = LEFT;
            }
            break;
        case 'd': 
            if (dir != LEFT)
            {
                dir = RIGHT;
            }
            break;
        case 'w': 
            if (dir != DOWN)
            {
                dir = UP;
            }
            break;
        case 's': 
            if (dir != UP)
            {
                dir = DOWN;
            }
            break;
        case 'p':
            save("save.dat");
            return true;

        }
    }
    return false;

}

bool Logic()
{
    if (dir == STOP)
    {
        return false;
    }

    // Вычисляем новые координаты головы
    int nextX = head->x;
    int nextY = head->y;

    switch (dir)
    {
    case LEFT:
        nextX--;
        break;
    case RIGHT:
        nextX++; 
        break;
    case UP:
        nextY--; 
        break;
    case DOWN:
        nextY++; 
        break;

    }

    // При столкновении с границей поля игра завершается
    if (nextX <= 0 || nextX >= width - 1 || nextY <= 0 || nextY >= height - 1)
    {
        return true;

    }

    // При столкновении с собой игра завершается
    Node* curr = head;
    while (curr != nullptr) 
    {
        if (curr->x == nextX && curr->y == nextY) 
        {
            return true;

        }
        curr = curr->next;

    }

    // Добавляем новую голову вперед списка
    addHead(nextX, nextY);

    // При съедании еды змейка увеличивается, счёт повышается
    if (nextX == fruinX && nextY == fruitY)
    {
        score += 10;
        length++;
        fruinX = 1 + rand() % (width - 2);
        fruitY = 1 + rand() % (height - 2);

    }
    else
    {
        removeTail(); // Убираем хвост, если еда не съедена

    }

    return false;
}

void RunGame(bool isLoaded) 
{
    Setup(isLoaded);
    bool gameEnd = false;

    while (!gameEnd)
    {
        Draw();
        if (Input())
        {
            return; // Выход в меню по кнопке P
        }

        gameEnd = Logic();

        Sleep(200);

    }

    system("cls");

    std::cout << "Игра окончена! Score: " << score << "\nВведите ваше имя: ";
    char name[50];
    std::cin >> name;

    appendSnakeRecord("records.dat", name, score);
    freeSnake();
}

int main()
{
    setlocale(NULL, "Russian");

    while (true) 
    {
        system("cls"); // Очистка консоли

        std::cout << "1.Новая игра\n";
        std::cout << "2.Загрузить игру\n";
        std::cout << "3.Таблица рекордов\n";
        std::cout << "4.Выход\n";
        std::cout << "Выберите пункт: ";

        char choice = _getch();
        if (choice == '1')
        {
            RunGame(false);
        }
        if (choice == '2') 
        {
            if (loadGame("save.dat")) 
            {
                RunGame(true);

            }

        }
        if (choice == '3')
        {
            showLeaderboard("records.dat");

        }
        if (choice == '4') 
        {
            freeSnake();
            return 0;

        }

    }

}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
