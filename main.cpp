#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <cmath>

void copyFile(const std::string& source, const std::string& destination, int totalSteps) {
    std::ifstream sourceFile(source, std::ios::binary);
    std::ofstream destFile(destination, std::ios::binary);

    if (!sourceFile.is_open() || !destFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }

    // Определяем размер файла
    sourceFile.seekg(0, std::ios::end);
    std::streampos fileSize = sourceFile.tellg();
    sourceFile.seekg(0, std::ios::beg);

    const int bufferSize = static_cast<int>(std::ceil(static_cast<float>(fileSize) / totalSteps));
    char buffer[bufferSize];

    int steps = 0;

    while (!sourceFile.eof()) {
        // Читаем блок данных из исходного файла
        sourceFile.read(buffer, bufferSize);

        // Записываем блок данных в целевой файл
        destFile.write(buffer, sourceFile.gcount());

        // Имитация выполнения шага задачи
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Вывод прогресса в консоль
        float progress = 100 / totalSteps * (steps + 1);
        std::cout << "\rProgress: " << progress << "%";

        steps++;
    }

    sourceFile.close();
    destFile.close();
}

int main() {
    const std::string sourceFileName = "source.txt";
    const std::string destFileName = "destination.txt";
    const int totalSteps = 20;

    std::thread copyThread(copyFile, sourceFileName, destFileName, totalSteps);

    copyThread.join();

    std::cout << "\nFile copy completed!" << std::endl;

    return 0;
}
