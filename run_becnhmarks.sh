#!/bin/bash

# Указание директории для результатов
RESULT_DIR="results"

# Создание директории, если она не существует
mkdir -p $RESULT_DIR

# Компиляция C++ файлов
g++ main.cpp utils.cpp -o program -lm

# Проверка успешности компиляции
if [ $? -ne 0 ]; then
    echo "Ошибка компиляции"
    exit 1
fi

./program

# Запуск Python-скрипта для построения графика
python3 plot_loss.py

# Проверка успешности выполнения Python-скрипта
if [ $? -ne 0 ]; then
    echo "Ошибка выполнения plot_loss.py"
    exit 1
fi

# Сообщение о завершении
echo "DONE"