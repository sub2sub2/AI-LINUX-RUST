#include <chrono>
#include <functional>
#include <thread>

namespace AIModel::Utils {

class Timer {
public:
    Timer() : running(false) {}

    void start(int seconds, std::function<void()> callback) {
        running = true;
        if (callback != nullptr)
            savedCallback = callback;
        std::thread([=]() {
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
            if (running) {
                callback();
                running = false;
            }
        }).detach();
    }

    void stop() {
        running = false;
    }

    void reset(int seconds) {
        if (running) {
            start(seconds, savedCallback);
        }
    }
    bool isRunning() {
        return running;
    }
private:
    bool running;
    std::function<void()> savedCallback;
};
}
