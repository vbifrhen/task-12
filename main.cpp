#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iomanip>
#include <cmath>

std::mutex consoleMutex;
std::mutex mtx;
std::condition_variable cv;
bool canUpdate = false;

void clearConsoleLine() {
    std::cout << std::setw(80) << '\r';
}

void showProgress(std::atomic<bool>& done, std::atomic<int>& progress, int totalSteps) {
    int steps = 0;
    while (!done) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return canUpdate; });
        canUpdate = false;
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        {
            std::lock_guard<std::mutex> lock(consoleMutex);
            clearConsoleLine();
            float prog = 100.0f / totalSteps * (steps + 1);
            progress = static_cast<int>(prog);
            std::cout << "Progress: " << progress << "%";
            std::cout.flush();
        }

        steps++;
    }
}

void showMessage(std::atomic<bool>& done) {
    while (!done) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return canUpdate; });
        canUpdate = false;
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        {
            std::lock_guard<std::mutex> lock(consoleMutex);
            clearConsoleLine();
            std::cout << "You can drink some coffee while the program is working";
            std::cout.flush();
        }
    }
}

void fileCopy(const std::string& source, const std::string& destination, std::atomic<bool>& done, std::atomic<int>& progress, int totalSteps) {
    std::ifstream sourceFile(source, std::ios::binary);
    std::ofstream destFile(destination, std::ios::binary);

    if (!sourceFile.is_open() || !destFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        done = true;
        return;
    }

    sourceFile.seekg(0, std::ios::end);
    std::streampos fileSize = sourceFile.tellg();
    sourceFile.seekg(0, std::ios::beg);

    const int bufferSize = static_cast<int>(std::ceil(static_cast<float>(fileSize) / totalSteps));
    char buffer[bufferSize];

    for (int i = 0; i < totalSteps; ++i) {
        sourceFile.read(buffer, sizeof(buffer));
        destFile.write(buffer, sourceFile.gcount());

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        {
            std::lock_guard<std::mutex> lock(mtx);
            canUpdate = true;
            cv.notify_all();
        }
    }

    sourceFile.close();
    destFile.close();
    done = true;
}

int main() {
    const std::string sourceFileName = "source.txt";
    const std::string destFileName = "destination.txt";
    const int totalSteps = 20;

    std::atomic<bool> done(false);
    std::atomic<int> progress(0);

    std::thread progressThread(showProgress, std::ref(done), std::ref(progress), totalSteps);
    std::thread messageThread(showMessage, std::ref(done));
    std::thread fileCopyThread(fileCopy, sourceFileName, destFileName, std::ref(done), std::ref(progress), totalSteps);

    while (!done) {
        std::cout << "\rProgress: " << progress << "%";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (progressThread.joinable()) {
        progressThread.join();
    }
    if (messageThread.joinable()) {
        messageThread.join();
    }
    if (fileCopyThread.joinable()) {
        fileCopyThread.join();
    }

    {
        std::lock_guard<std::mutex> lock(consoleMutex);
        clearConsoleLine();
        std::cout << "File copy completed!\n";
    }

    return 0;
}
