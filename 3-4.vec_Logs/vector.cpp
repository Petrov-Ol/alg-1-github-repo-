// Подключаем библиотеку iostream для управления выводом и вводом
#include <iostream>
#include <fstream> // Для работы с файлами. Комбинация ofstream и ifstream: создание, чтение и запись в файлы.
#include <sstream>  // Add this for istringstream
#include <string> //Для использования getline
#include <vector> // Для работы с векторами
#include <chrono> // Для работы с датой
#include <ctime>  // Для работы с временем
#include <iomanip> //Используется для управления форматированием ввода и вывода.
#include <stdexcept>  // Добавил для обработки исключений
#include <cmath> // Добавил для использования функций математической библиотеки

// Подключаем пространство имен std 
using namespace std; 

//Класс для сохранения веременных данных при работе с матрицами
template<typename T = double>
class MatrixDense {
    T* __data; // Указатель на массив данных типа T
    unsigned _m, _n; // Размеры матрицы: количество строк (_m) и столбцов (_n)
    bool __is_valid; // Флаг валидности матрицы

public:
    // Конструктор, принимающий размеры матрицы
    MatrixDense(unsigned m = 0, unsigned n = 0) : _m(m), _n(n), __is_valid(false) {
        // Безопасное выделение памяти
        try {
            if (m > 0 && n > 0) {
                __data = new T[_m * _n](); // Инициализация нулями
                __is_valid = true;
            } else {
                __data = nullptr;
            }
        } catch (const bad_alloc& e) {
            __data = nullptr;
            __is_valid = false;
        }
    }

    // Конструктор копирования
    MatrixDense(const MatrixDense& other) : _m(other._m), _n(other._n), __is_valid(false) {
        try {
            if (other.__is_valid && _m > 0 && _n > 0) {
                __data = new T[_m * _n];
                copy(other.__data, other.__data + _m * _n, __data);
                __is_valid = true;
            } else {
                __data = nullptr;
            }
        } catch (const bad_alloc& e) {
            __data = nullptr;
            __is_valid = false;
        }
    }

    // Оператор присваивания
    MatrixDense& operator=(const MatrixDense& other) {
        if (this != &other) {
            // Освободить существующую память
            if (__data) {
                delete[] __data;
            }

            // Скопировать данные
            _m = other._m;
            _n = other._n;
            __is_valid = false;

            try {
                if (other.__is_valid && _m > 0 && _n > 0) {
                    __data = new T[_m * _n];
                    copy(other.__data, other.__data + _m * _n, __data);
                    __is_valid = true;
                } else {
                    __data = nullptr;
                }
            } catch (const bad_alloc& e) {
                __data = nullptr;
                __is_valid = false;
            }
        }
        return *this;
    }

    // Деструктор, вызываемый при уничтожении объекта
    ~MatrixDense() {
        // Безопасное освобождение памяти
        if (__data != nullptr) {
            delete[] __data;
            __data = nullptr;
        }
    }

    // Метод для доступа к элементам матрицы с проверкой границ
    T& getElement(unsigned i, unsigned j) {
        // Проверка валидности матрицы и границ
        if (!__is_valid) {
            throw runtime_error("Доступ к элементам недопустимой матрицы");
        }
        
        if (i >= _m || j >= _n) {
            throw out_of_range("Индекс матрицы выходит за рамки допустимого");
        }
        
        return __data[j + i * _n];
    }

    // Константная версия getElement
    const T& getElement(unsigned i, unsigned j) const {
        if (!__is_valid) {
            throw runtime_error("Доступ к элементам недопустимой матрицы");
        }
        
        if (i >= _m || j >= _n) {
            throw out_of_range("Индекс матрицы выходит за рамки допустимого");
        }
        
        return __data[j + i * _n];
    }

    // Проверка валидности матрицы
    bool isValid() const { return __is_valid; }

    unsigned rows() const { return _m; }
    unsigned cols() const { return _n; }
};

struct MatrixData {
    MatrixDense<double> matrix;
    unsigned rows;
    unsigned cols;

    // Конструктор по умолчанию
    MatrixData() : rows(0), cols(0), matrix(MatrixDense<double>(0, 0)) {}
};

struct CalcProblemParams
{
   string filePath1;
   string filePath2;
   string exportPath;
   enum class operations {vv_sum, vv_sub, mm_sum};
   operations op;
};

//Структура для хранения значений векторов и их размерностей:
struct VectorData {
    vector<double> values;
    int size = 0;
};

struct ExportConfig {
    string path; // Путь к файлу
};

struct CalcResults {
    VectorData result; // Результат вычислений для векторов
    MatrixData matrixResult; // Результат вычислений для матриц
};

int logAll(string data)
{
    ofstream logFile("log.txt", ios::app); // Открываем файл в режиме добавления
    if (!logFile) {
        return -1;
    }
    //ios::app — это флаг, который указывает, что файл должен быть открыт в режиме добавления
    time_t t = time(0); // Получаю текущее время
    struct tm *now = localtime(&t); // Преобразую в локальное время

    // Записываем дату и время в файл
    logFile <<put_time(now, "%Y-%m-%d %H:%M:%S") << " - "; // Форматирую и записываю дату и время

    logFile << data << endl; // Записываю данные в файл
    logFile.close(); // Закрываю файл
    return 0; 
}

//Функция для чтения из файлов и проверки значений
VectorData readDataFromFile(const string& filepath) {
    string line;
    VectorData vectorData;
    ifstream dataFile(filepath); // Открываем файл в режиме чтения
    if (!dataFile) {
        logAll("Ошибка открытия файла");
        return vectorData; // Возвращаем пустой вектор
    }

    while (getline(dataFile, line)) {
        if (line == "vector") {
            logAll("В файле обнаружено значение vector");
            // Читаем размер вектора
            if (getline(dataFile, line)) {
                vectorData.size = stoi(line);
                vectorData.values.resize(vectorData.size);
                // Читаем значения вектора
                if (getline(dataFile, line)) {
                    /*Istringstream - это строковой поток ввода. Конструктор istringstream () со строковым аргументом прикрепляет создаваемый объект потока к указанной строке. То есть теперь информация будет считываться из стандартной строки точно таким же образом, как и из cin, объекта класса потока ввода istream.*/
                    istringstream iss(line);
                    for (int i = 0; i < vectorData.size; ++i) {
                        iss >> vectorData.values[i];
                    }
                }
            }
        }
    }
    dataFile.close(); // Закрываем файл
    return vectorData; 
}

//Функция для чтения данных матрицы из файла
MatrixData readMatrixFromFile(const string& filepath) {
    string line;
    MatrixData matrixData;
    ifstream dataFile(filepath);
    
    // Расширенная проверка файла
    if (!dataFile) {
        logAll("Ошибка: Невозможно открыть файл: " + filepath);
        return matrixData;
    }

    // Проверка размера файла
    dataFile.seekg(0, ios::end);
    streampos fileSize = dataFile.tellg();
    dataFile.seekg(0, ios::beg);

    if (fileSize == 0) {
        logAll("Ошибка: Файл пуст: " + filepath);
        return matrixData;
    }

    // Отладочный лог полного содержимого файла
    logAll("Попытка чтения матрицы из файла: " + filepath);
    
    try {
        while (getline(dataFile, line)) {
            // Пропуск пустых строк и комментариев
            if (line.empty() || line[0] == '#') continue;

            logAll("Чтение строки: " + line);
            
            if (line == "matrix") {
                logAll("Найдено значение 'matrix'");
                
                // Чтение размерности
                if (!getline(dataFile, line)) {
                    logAll("Ошибка: Неожиданный конец файла после 'matrix' маркера");
                    return matrixData;
                }

                logAll("Парсинг размерности: " + line);
                
                size_t xPos = line.find('x');
                if (xPos == string::npos) {
                    logAll("Ошибка: Неверный формат размерности матрицы. Ожидается 'MxN'.");
                    return matrixData;
                }

                try {
                    matrixData.rows = stoi(line.substr(0, xPos));
                    matrixData.cols = stoi(line.substr(xPos + 1));
                    
                    // Проверка разумных ограничений размера матрицы
                    const unsigned MAX_MATRIX_SIZE = 10000;
                    if (matrixData.rows == 0 || matrixData.cols == 0 || 
                        matrixData.rows > MAX_MATRIX_SIZE || 
                        matrixData.cols > MAX_MATRIX_SIZE) {
                        logAll("Ошибка: Неверные размеры матрицы. Строки: " + 
                               to_string(matrixData.rows) + 
                               ", Колонны: " + to_string(matrixData.cols));
                        return matrixData;
                    }
                    
                    logAll("Размеры матрицы: " + 
                           to_string(matrixData.rows) + "x" + 
                           to_string(matrixData.cols));
                    
                    // Безопасное выделение памяти
                    matrixData.matrix = MatrixDense<double>(matrixData.rows, matrixData.cols);
                    
                    // Чтение значений матрицы
                    for (unsigned i = 0; i < matrixData.rows; ++i) {
                        if (!getline(dataFile, line)) {
                            logAll("Ошибка: Неожиданный конец файла. Ожидается " + 
                                   to_string(matrixData.rows) + " строк.");
                            return matrixData;
                        }

                        logAll("Чтение строки матрицы: " + line);
                        istringstream iss(line);
                        
                        for (unsigned j = 0; j < matrixData.cols; ++j) {
                            double value;
                            if (!(iss >> value)) {
                                logAll("Ошибка: Не удалось прочитать элемент матрицы в позиции (" + 
                                       to_string(i) + ", " + to_string(j) + ")");
                                return matrixData;
                            }
                            
                            // Дополнительная проверка значений
                            if (isnan(value) || isinf(value)) {
                                logAll("Внимание: Подозрительное значение matrix в позиции (" + 
                                       to_string(i) + ", " + to_string(j) + 
                                       "): " + to_string(value));
                            }
                            
                            matrixData.matrix.getElement(i, j) = value;
                            
                            logAll("Элемент matrix в позиции (" + 
                                   to_string(i) + ", " + to_string(j) + 
                                   ") is: " + to_string(value));
                        }

                        // Проверка, что в строке нет лишних элементов
                        double extraValue;
                        if (iss >> extraValue) {
                            logAll("Внимание: В строке " + 
                                   to_string(i) + " остались лишние элементы. Их игнорируется.");
                        }
                    }
                    
                    // Успешное чтение матрицы
                    logAll("Матрица успешно прочитана");
                    return matrixData;
                    
                } catch (const exception& e) {
                    logAll("Ошибка парсинга матрицы: " + string(e.what()));
                    return matrixData;
                }
            }
        }

        // Если маркер матрицы не найден
        logAll("Ошибка: Нет 'matrix' маркера в файле.");
        return matrixData;

    } catch (const exception& e) {
        logAll("Ошибка: обработка матрицы: " + string(e.what()));
        return matrixData;
    }
}

// Функция для сложения матриц
MatrixData Calck_mm_sum(const MatrixData& mat1, const MatrixData& mat2) {
    // Проверка валидности входных матриц
    if (!mat1.matrix.isValid() || !mat2.matrix.isValid()) {
        logAll("Ошибка: Одна или обе матрицы недействительны");
        return MatrixData();
    }

    // Проверка совпадения размерностей
    if (mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
        logAll("Ошибка: Размерности матриц не совпадают для сложения. " + 
               string("Matrix 1: ") + to_string(mat1.rows) + "x" + to_string(mat1.cols) + 
               ", Matrix 2: " + to_string(mat2.rows) + "x" + to_string(mat2.cols));
        return MatrixData();
    }

    // Проверка разумных ограничений размера
    const unsigned MAX_MATRIX_SIZE = 10000;
    if (mat1.rows == 0 || mat1.cols == 0 || 
        mat1.rows > MAX_MATRIX_SIZE || mat1.cols > MAX_MATRIX_SIZE) {
        logAll("Ошибка: Размеры матрицы превышают допустимые ограничения");
        return MatrixData();
    }

    // Создание результирующей матрицы
    MatrixData resultMatrix;
    resultMatrix.rows = mat1.rows;
    resultMatrix.cols = mat1.cols;
    resultMatrix.matrix = MatrixDense<double>(resultMatrix.rows, resultMatrix.cols);

    // Выполнение суммирования с обработкой исключений
    try {
        for (unsigned i = 0; i < mat1.rows; ++i) {
            for (unsigned j = 0; j < mat1.cols; ++j) {
                double val1 = mat1.matrix.getElement(i, j);
                double val2 = mat2.matrix.getElement(i, j);

                // Проверка на переполнение
                double sumVal = val1 + val2;
                if (isnan(sumVal) || isinf(sumVal)) {
                    logAll("Внимание: Переполнение или неверный результат в (" + 
                           to_string(i) + ", " + to_string(j) + 
                           "): " + to_string(val1) + " + " + to_string(val2));
                    sumVal = 0; // Безопасное значение по умолчанию
                }

                resultMatrix.matrix.getElement(i, j) = sumVal;

                // Отладочный лог каждой операции
                logAll("Сумма матрицы в (" + to_string(i) + ", " + to_string(j) + 
                       "): " + to_string(val1) + " + " + to_string(val2) + 
                       " = " + to_string(sumVal));
            }
        }

        logAll("Суммирование матрицы завершено успешно");
        return resultMatrix;

    } catch (const exception& e) {
        logAll("Ошибка суммирования матриц: " + string(e.what()));
        return MatrixData();
    } catch (...) {
        logAll("Ошибка: Неизвестная ошибка суммирования матриц");
        return MatrixData();
    }
}

// Функция для сложения двух векторов
VectorData Calck_vv_sum(const VectorData& vec1, const VectorData& vec2) {
    // Проверка на равенство размерностей
    if (vec1.size != vec2.size) {
        logAll("Ошибка! Размерность векторов не совпадает");
        return VectorData();
    }

    VectorData result; // Создаем объект для хранения результата сложения векторов
    result.size = vec1.size; // Устанавливаем размер результирующего вектора равным размеру первого вектора
    result.values.resize(result.size); // Изменяем размер массива значений результирующего вектора

    // Сложение векторов
    for (int i = 0; i < result.size; ++i) {
        result.values[i] = vec1.values[i] + vec2.values[i];
    }

    return result;
}

//Функция для вычитания векторов
VectorData Calck_vv_sub(const VectorData& vec1, const VectorData& vec2) {
    // Проверка на равенство размерностей
    if (vec1.size != vec2.size) {
        logAll("Ошибка! Размерность векторов не совпадает");
        return VectorData();
    }

    VectorData result; // Создаем объект для хранения результата вычитания векторов
    result.size = vec1.size; // Устанавливаем размер результирующего вектора равным размеру первого вектора
    result.values.resize(result.size); // Изменяем размер массива значений результирующего вектора

    // Вычитание векторов
    for (int i = 0; i < result.size; ++i) {
        result.values[i] = vec1.values[i] - vec2.values[i];
    }

    return result;
}

// Функция для экспорта результатов рассчётов в файл
int Export(const CalcResults& calcResults, const ExportConfig& config) {
    // Создаю файл и открываю его на дозапись
    logAll("Открываю " + config.path);
    ofstream dataFile(config.path, ios::app); // Открываем файл в режиме добавления
    if (!dataFile) {
        logAll("Ошибка открытия файла: " + config.path);
        return -1;
    }

    // Проверка на наличие данных для записи в вектор
    if (calcResults.result.size > 0) {
        dataFile << "Результат вычисления векторов: "; // Заголовок для вектора

        // Записываем данные вектора в файл
        for (const auto& value : calcResults.result.values) {
            dataFile << value << " "; // Записываем значения вектора
        }
        dataFile << endl; // Переход на новую строку
    } else {
        logAll("Нет данных для записи вектора, ничего не записывается в файл.");
    }

    // Подсчет элементов матрицы
    unsigned countMatrixElements = calcResults.matrixResult.rows * calcResults.matrixResult.cols;

    // Проверка на наличие данных для записи в матрицу
    if (countMatrixElements > 0) {
        dataFile << "Результат вычисления матриц:" << endl; // Заголовок для матрицы
        for (unsigned i = 0; i < calcResults.matrixResult.rows; ++i) {
            for (unsigned j = 0; j < calcResults.matrixResult.cols; ++j) {
                dataFile << calcResults.matrixResult.matrix.getElement(i, j) << " "; // Записываем элементы матрицы
            }
            dataFile << endl; // Переход на новую строку после каждой строки матрицы
        }
    } else {
        logAll("Нет данных для записи матрицы.");
    }

    dataFile.close(); // Закрываю файл
    return 0; 
}

int main(int argc, char* argv[]) {
    FILE* forcedLog = fopen("log.txt", "a");
    if (forcedLog) {
        fprintf(forcedLog, "\n--- НОВЫЙ ЗАПУСК ПРОГРАММЫ ---\n");
        fprintf(forcedLog, "Программа запущена с %d аргументами\n", argc);
        
        for (int k = 0; k < argc; ++k) {
            fprintf(forcedLog, "Аргументы %d: %s\n", k, argv[k] ? argv[k] : "NULL");
        }
        
        fclose(forcedLog);
    }

    for (int k = 0; k < argc; ++k) {
        if (argv[k] == nullptr) {
            fprintf(stderr, "Ошибка: Null аргумент в индексе %d\n", k);
            return 1;
        }
    }

    VectorData vector1, vector2;
    MatrixData matrix1, matrix2;
    CalcProblemParams calcParams;
    CalcResults calcResult;

    for (int k = 0; k < argc; ++k) {
        logAll("Полный Аргумент " + to_string(k) + ": " + string(argv[k]));
    }

    logAll("Запуск функции main с " + to_string(argc) + " arguments");
    
    bool fp1_processed = false;
    bool fp2_processed = false;
    bool op_processed = false;
    bool exp_processed = false;

    try {
        for (int i = 1; i < argc; i++) {
            logAll("Обработка аргумента " + to_string(i) + ": " + string(argv[i]));
            
            if (string(argv[i]) == "--fp1" || string(argv[i]) == "--matrix_fp1") {
                if (i + 1 < argc) {
                    calcParams.filePath1 = argv[++i];
                    logAll("Путь к первому файлу: " + calcParams.filePath1);
                    
                    // Проверка файла
                    FILE* testFile = fopen(calcParams.filePath1.c_str(), "r");
                    if (!testFile) {
                        logAll("Ошибка: Не удалось открыть первый файл: " + calcParams.filePath1);
                        return 1;
                    }
                    fclose(testFile);
                    
                    // Попытка чтения как вектора, если не матрица
                    vector1 = readDataFromFile(calcParams.filePath1);
                    if (vector1.size == 0) {
                        matrix1 = readMatrixFromFile(calcParams.filePath1);
                        if (matrix1.rows == 0 || matrix1.cols == 0) {
                            logAll("Ошибка: Не удалось прочитать данные из первого файла");
                            return 1;
                        }
                    }
                    
                    fp1_processed = true;
                } else {
                    logAll("Ошибка: нет аргумента после --fp1 или --matrix_fp1");
                    return 1;
                }
            } else if (string(argv[i]) == "--fp2" || string(argv[i]) == "--matrix_fp2") {
                if (i + 1 < argc) {
                    calcParams.filePath2 = argv[++i];
                    logAll("Путь ко второму файлу: " + calcParams.filePath2);
                    
                    FILE* testFile = fopen(calcParams.filePath2.c_str(), "r");
                    if (!testFile) {
                        logAll("Ошибка: Не удалось открыть второй файл: " + calcParams.filePath2);
                        return 1;
                    }
                    fclose(testFile);
                    
                    // Попытка чтения как вектора, если не матрица
                    vector2 = readDataFromFile(calcParams.filePath2);
                    if (vector2.size == 0) {
                        matrix2 = readMatrixFromFile(calcParams.filePath2);
                        if (matrix2.rows == 0 || matrix2.cols == 0) {
                            logAll("Ошибка: Не удалось прочитать данные из второго файла");
                            return 1;
                        }
                    }
                    
                    fp2_processed = true;
                } else {
                    logAll("Ошибка: нет аргумента после --fp2 или --matrix_fp2");
                    return 1;
                }
            } else if (string(argv[i]) == "--op") {
                if (i + 1 < argc) {
                    string opStr = argv[++i];
                    logAll("Операция: " + opStr);
                    
                    if (opStr == "vv_sum") {
                        calcParams.op = CalcProblemParams::operations::vv_sum;
                    } else if (opStr == "vv_sub") {
                        calcParams.op = CalcProblemParams::operations::vv_sub;
                    } else if (opStr == "mm_sum") {
                        calcParams.op = CalcProblemParams::operations::mm_sum;
                    } else {
                        logAll("Неизвестная операция: " + opStr);
                        return 1;
                    }
                    
                    op_processed = true;
                } else {
                    logAll("Ошибка: нет аргумента после --op");
                    return 1;
                }
            } else if (string(argv[i]) == "--exp") {
                if (i + 1 < argc) {
                    calcParams.exportPath = argv[++i];
                    logAll("Путь для экспорта: " + calcParams.exportPath);
                    exp_processed = true;
                } else {
                    logAll("Ошибка: нет аргумента после --exp");
                    return 1;
                }
            } else {
                logAll("Неожиданный аргумент: " + string(argv[i]));
                return 1;
            }
        }

        // Проверка, что все необходимые аргументы обработаны
        if (!fp1_processed || !fp2_processed || !op_processed || !exp_processed) {
            logAll("Ошибка: Не все обязательные аргументы обработаны");
            return 1;
        }

        // Выполнение операции в зависимости от типа
        switch (calcParams.op) {
            case CalcProblemParams::operations::vv_sum:
                if (vector1.size != vector2.size) {
                    logAll("Ошибка: Размеры векторов не совпадают");
                    return 1;
                }
                calcResult.result = Calck_vv_sum(vector1, vector2);
                break;
            case CalcProblemParams::operations::vv_sub:
                if (vector1.size != vector2.size) {
                    logAll("Ошибка: Размеры векторов не совпадают");
                    return 1;
                }
                calcResult.result = Calck_vv_sub(vector1, vector2);
                break;
            case CalcProblemParams::operations::mm_sum:
                if (matrix1.rows != matrix2.rows || matrix1.cols != matrix2.cols) {
                    logAll("Ошибка: Размеры матриц не совпадают");
                    return 1;
                }
                calcResult.matrixResult = Calck_mm_sum(matrix1, matrix2);
                break;
        }

        // Экспорт результатов
        ExportConfig exportConfig;
        exportConfig.path = calcParams.exportPath;
        Export(calcResult, exportConfig);

        logAll("Операция успешно выполнена");
        return 0;
    } catch (const exception& e) {
        logAll("Ошибка: Ошибка обработки аргументов: " + string(e.what()));
        return 1;
    } catch (...) {
        logAll("Ошибка: Ошибка обработки аргументов");
        return 1;
    }
}
