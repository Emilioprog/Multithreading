#include "Queue"
#include "Task"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

// Classe représentant un Minion
class Minion {
private:
  std::queue<std::shared_ptr<Task>> &taskQueue; // Référence à la file de tâches
  std::queue<std::shared_ptr<Task>>
      &resultQueue;       // Référence à la file de résultats
  std::mutex &queueMutex; // Mutex pour synchroniser l'accès aux queues
  std::condition_variable
      &queueCV; // Condition variable pour signaler de nouvelles tâches
  std::atomic<bool> &running; // Indicateur de continuation

public:
  Minion(std::queue<std::shared_ptr<Task>> &taskQueue,
         std::queue<std::shared_ptr<Task>> &resultQueue, std::mutex &queueMutex,
         std::condition_variable &queueCV, std::atomic<bool> &running)
      : taskQueue(taskQueue), resultQueue(resultQueue), queueMutex(queueMutex),
        queueCV(queueCV), running(running) {}

  void run() {
    while (running) {
      std::shared_ptr<Task> task;

      {
        // Protéger l'accès à la file avec un verrou
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCV.wait(lock, [&] { return !taskQueue.empty() || !running; });

        if (!running && taskQueue.empty())
          break;

        task = taskQueue.front();
        taskQueue.pop();
      }

      if (task) {
        std::cout << "Minion: Received task " << task->identifier
                  << ". Processing...\n";

        // Simuler le traitement de la tâche
        task->work();

        // Ajouter le résultat à la file des résultats
        {
          std::lock_guard<std::mutex> lock(queueMutex);
          resultQueue.push(task);
        }

        std::cout << "Minion: Task " << task->identifier
                  << " completed. Result sent.\n";
      }
    }

    std::cout << "Minion: No more tasks. Exiting...\n";
  }
};

int main() {
  QueueManager manager;

  // Ajouter des tâches à la file
  {
    std::lock_guard<std::mutex> lock(manager.queueMutex);
    for (int i = 1; i <= 10; ++i) {
      manager.taskQueue.push(std::make_shared<Task>(i));
    }
  }
  manager.queueCV.notify_all();

  // Démarrer les Minions
  std::vector<std::thread> minions;
  for (int i = 0; i < 3; ++i) {
    minions.emplace_back([&manager]() {
      Minion minion(manager.taskQueue, manager.resultQueue, manager.queueMutex,
                    manager.queueCV, manager.running);
      minion.run();
    });
  }

  // Attendre la fin des threads
  for (auto &minion : minions) {
    minion.join();
  }

  // Afficher les résultats
  std::cout << "All tasks processed. Results:\n";
  while (!manager.resultQueue.empty()) {
    auto result = manager.resultQueue.front();
    manager.resultQueue.pop();
    std::cout << "Task " << result->identifier << " completed.\n";
  }

  return 0;
}
