from manager import QueueClient
from task import Task
import time
import queue
import threading

T = 10


class Boss:
    def __init__(self):
        self.client = QueueClient()
        self.temps_general = 0
        self.T = 10

    def assign_task(self, task_id, size=None):
        task = Task(identifier=task_id, size=size)
        print(f"Boss task : {task.identifier} sended to the queue.")
        self.client.task_queue.put(task)

    def track_results(self):
        for i in range(0, T + 1):
            try:
                result = self.client.result_queue.get(timeout=3)
                print(f"Boss task : {result.identifier} completed.")
                self.temps_general = self.temps_general + result.time
            except queue.Empty:
                print("Boss : No results yet, waiting...")
                time.sleep(1)
        print(f"General execution time: {self.temps_general} seconds.")


if __name__ == "__main__":
    boss = Boss()

    # Lancer un thread pour écouter les résultats
    result_thread = threading.Thread(target=boss.track_results, daemon=True)
    result_thread.start()

    task_id = 0
    for i in range(0, T + 1):
        boss.assign_task(task_id, 10)
        task_id += 1
        time.sleep(2)
