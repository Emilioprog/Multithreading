from manager import QueueClient
from task import Task
import time
import queue
import threading


class Boss:
    def __init__(self):
        self.client = QueueClient()

    def assign_task(self, task_id, size=None):
        task = Task(identifier=task_id, size=size)
        print(f"Boss task : {task.identifier} sended to the queue.")
        self.client.task_queue.put(task)

    def track_results(self):
        while True:
            try:
                result = self.client.result_queue.get(timeout=3)
                print(f"Boss task : {result.identifier} completed")
            except queue.Empty:
                print("Boss : No results yet, waiting...")
                time.sleep(1)


if __name__ == "__main__":
    boss = Boss()

    # Lancer un thread pour écouter les résultats
    result_thread = threading.Thread(target=boss.track_results, daemon=True)
    result_thread.start()

    task_id = 0
    while True:
        boss.assign_task(task_id,10)
        task_id += 1
        time.sleep(2)
