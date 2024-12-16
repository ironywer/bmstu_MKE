import matplotlib.pyplot as plt
import argparse
import os

# Константы
RESULT_DIR = "./results/"

def read_values(filename):
    """Считывает значения x и y из файла. Возвращает два списка: x и y."""
    if not os.path.exists(filename):
        raise FileNotFoundError(f"Файл {filename} не найден.")
    x_values = []
    y_values = []
    with open(filename, 'r') as file:
        for line in file:
            # Разделяем строку на два столбца, ожидаем, что данные разделены пробелом
            columns = line.strip().split()
            if len(columns) == 2:
                try:
                    x_values.append(float(columns[0]))
                    y_values.append(float(columns[1]))
                except ValueError:
                    continue  # Если не удается преобразовать в числа, пропускаем строку
    return x_values, y_values

def main(cubic_file, linear_file, another_file, num_nodes):
    # Формируем пути для файлов
    cubic_path = os.path.join(RESULT_DIR, cubic_file if cubic_file else f"cubicSolution_{num_nodes}nodes.txt")
    linear_path = os.path.join(RESULT_DIR, linear_file if linear_file else f"linearSolution_{num_nodes}nodes.txt")
    another_path = os.path.join(RESULT_DIR, another_file if another_file else f"analyticalSolution_{num_nodes}nodes.txt")

    # Считываем данные из файлов
    x1, y1 = read_values(cubic_path)
    x2, y2 = read_values(linear_path)
    x3, y3 = read_values(another_path)

    # Проверка, что все x одинаковые для всех решений (для корректного построения графика)
    if not (len(x1 )== len(x2) == len(x3)):
        raise ValueError("Количество x в файлах должны совпадать.")

    # Проверка длины данных
    if len(x1) != num_nodes or len(y1) != num_nodes or len(y2) != num_nodes or len(y3) != num_nodes:
        raise ValueError(f"Количество значений x и y в файлах должно совпадать с количеством узлов ({num_nodes}).")

    # Построение графика
    plt.figure(figsize=(10, 6))
    plt.plot(x1, y1, label="Cubic Solution", marker='o', linestyle='--', color='blue')
    plt.plot(x2, y2, label="Linear Solution", marker='s', linestyle='-', color='orange')
    plt.plot(x3, y3, label="Analytical Solution", marker='^', linestyle=':', color='green')

    # Настройки графика
    plt.title(f"Решения для {num_nodes} КЭ", fontsize=14)
    plt.xlabel("Значения Х", fontsize=12)
    plt.ylabel("ЗЗначеенниия У", fontsize=12)
    plt.legend(fontsize=10)
    plt.grid(True, linestyle='--', alpha=0.7)

    # Сохранение и отображение графика
    os.makedirs(RESULT_DIR, exist_ok=True)  # Создать папку, если её нет
    plt.savefig(os.path.join(RESULT_DIR, f"graph_methods_{num_nodes}.png"), dpi=300)
    plt.show()

if __name__ == "__main__":
    # Парсер аргументов командной строки
    parser = argparse.ArgumentParser(description="Построение графика абсолютных погрешностей.")
    parser.add_argument("--cubic", type=str, help="Файл с данными для cubicSolution (по умолчанию cubicSolution_<nodes>nodes.txt)")
    parser.add_argument("--linear", type=str, help="Файл с данными для linearSolution (по умолчанию linearSolution_<nodes>nodes.txt)")
    parser.add_argument("--another", type=str, help="Файл с данными для analyticalSolution (по умолчанию analyticalSolution_<nodes>nodes.txt)")
    parser.add_argument("--nodes", type=int, required=True, help="Количество узлов (значений в файлах)")

    # Считываем аргументы
    args = parser.parse_args()

    # Запуск программы с заданными параметрами
    main(args.cubic, args.linear, args.another, args.nodes)
