#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <fstream>
#include <filesystem>

namespace RabbitAI {
    class Task {
    public:
        std::string name;
        std::function<void()> action;

        Task(const std::string& taskName, std::function<void()> taskAction)
            : name(taskName), action(taskAction) {}

        void run() {
            std::cout << "Running task: " << name << std::endl;
            try {
                action();
            } catch (const std::exception& e) {
                std::cerr << "Error in task " << name << ": " << e.what() << std::endl;
            }
        }
    };

    class Scheduler {
    private:
        std::vector<Task> tasks;
        std::mutex taskMutex;

    public:
        void addTask(const Task& task) {
            std::lock_guard<std::mutex> lock(taskMutex);
            tasks.push_back(task);
        }

        void runAll() {
            std::cout << "Starting all tasks..." << std::endl;
            std::vector<std::thread> threads;
            for (const auto& task : tasks) {
                threads.emplace_back([&]() { task.run(); });
            }
            for (auto& thread : threads) {
                thread.join();
            }
            std::cout << "All tasks completed." << std::endl;
        }
    };

    class Environment {
    public:
        std::map<std::string, std::string> variables;

        void loadFromFile(const std::string& filepath) {
            if (std::filesystem::exists(filepath)) {
                std::ifstream file(filepath);
                std::string line;
                while (std::getline(file, line)) {
                    auto delimiterPos = line.find('=');
                    auto key = line.substr(0, delimiterPos);
                    auto value = line.substr(delimiterPos + 1);
                    variables[key] = value;
                }
            } else {
                std::cerr << "Environment file not found: " << filepath << std::endl;
            }
        }

        std::string getVariable(const std::string& key) const {
            auto it = variables.find(key);
            return it != variables.end() ? it->second : "";
        }

        void print() const {
            for (const auto& [key, value] : variables) {
                std::cout << key << "=" << value << std::endl;
            }
        }
    };
}

int main() {
    using namespace RabbitAI;

    Scheduler scheduler;
    Environment env;

    env.loadFromFile(".env");
    env.print();

    scheduler.addTask(Task("Print Welcome Message", []() {
        std::cout << "Welcome to RabbitAI Engine!" << std::endl;
    }));

    scheduler.addTask(Task("Example Task", []() {
        std::cout << "Executing Example Task..." << std::endl;
    }));

    scheduler.runAll();

    return 0;
}
