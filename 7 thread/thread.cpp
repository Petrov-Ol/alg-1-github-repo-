#include <thread> // Для работы с патоками
#include <iostream>
#include <vector>
#include <mutex> //Для работы с блокировками (для предотвращения состояния гонки)

using namespace std;

mutex coutMutex; // Глобальный объект блокировки

//Все параметры, передаваемые в конструктор копируются
void print_hello(int id, double& sum)
{
    //Добавил блокировку
    coutMutex.lock();
    //Выводим ID потока
    sum +=id;
    cout<<"Thread: "<<id<<" sum: "<<sum<<endl;
    coutMutex.unlock();
}

//Версия кода с блокировками
int main()
{
    double sum =0;
    cout <<"codes"<<endl;
    vector<thread> threads;
    for (int i=0; i<8; i++)
    {
        //Передаём по ссылке значение
        threads.push_back(thread(print_hello, i, ref(sum)));
    }
    //Ожидаем присоединения потока
    for (auto& th: threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    return 0;
}