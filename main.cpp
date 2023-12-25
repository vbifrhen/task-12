#include <iostream>
#include <thread>
#include <chrono>

void performTask(int totalSteps) {
    for (int i = 0; i < totalSteps; ++i) {

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        float progress = static_cast<float>(i + 1) / totalSteps * 100.0f;
        std::cout << "\rProgress: " << progress << "%";
        std::cout.flush();
    }


    std::cout << std::endl;
}

int main() {
    const int totalSteps = 10;

    std::thread taskThread(performTask, totalSteps);

    taskThread.join();

    std::cout << "Task completed!" << std::endl;

    return 0;
}
