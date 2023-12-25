#include <iostream>
#include <thread>
#include <chrono>

// Функция, выполняющая задачу и обновляющая прогресс
void performTask(int totalSteps) {
    for (int i = 0; i < totalSteps; ++i) {
        // Имитация выполнения шага задачи
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Вывод прогресса в консоль
        float progress = static_cast<float>(i + 1) / totalSteps * 100.0f;
        std::cout << "\rProgress: " << progress << "%";
        std::cout.flush();
    }

    // Добавляем новую строку после завершения задачи
    std::cout << std::endl;
}

int main() {
    const int totalSteps = 10;

    // Запускаем задачу в отдельном потоке
    std::thread taskThread(performTask, totalSteps);

    // Ждем завершения потока задачи
    taskThread.join();

    std::cout << "Task completed!" << std::endl;

    return 0;
}
