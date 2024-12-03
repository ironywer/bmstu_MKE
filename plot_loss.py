import matplotlib.pyplot as plt

LOSS_CUBIC = "absoluteErrors_cubicSolution_by_nodesCount.txt"
LOSS_LINEAR = "absoluteErrors_linearSolution_by_nodesCount.txt"
RESULT_DIR = "./results/"

def read_values(filename):
    """Считывает значения из файла."""
    with open(filename, 'r') as file:
        return [float(line.strip()) for line in file]

# Чтение данных из файлов
y1 = read_values(RESULT_DIR + LOSS_CUBIC)
y2 = read_values(RESULT_DIR + LOSS_LINEAR)

# Параметры
x = list(range(100,len(y1)*100 + 100, 100))  # Значения x от 1 до 20



# Проверка длины данных
if len(y1) != len(x) or len(y2) != len(x):
    raise ValueError("Количество значений y в файлах должно совпадать с длиной x (20).")

# Построение графика
plt.figure(figsize=(8, 6))
plt.plot(x, y1, label="cubicSolution", marker='o')
plt.plot(x, y2, label="linearSolution", marker='s')

# Настройки графика
plt.title("График абсолютных погрешностей")
plt.xlabel("Количесто КЭ")
plt.ylabel("Абсолютная погрешность")
plt.legend()
plt.grid(True)

# Сохранение и отображение графика
plt.savefig(RESULT_DIR+"graph__loss.png", dpi=300)

