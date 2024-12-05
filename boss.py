from queue_system import QueueManager
from task import Task
import time
import queue
import threading


class Boss:
    def __init__(self, task_queue, result_queue):
        self.task_queue = task_queue
        self.result_queue = result_queue

    def assign_task(self, task_id):
        task = Task(identifier=task_id)
        print(f"Boss task : {task.identifier} sended to the queue.")
        self.task_queue.put(task)

    def track_results(self):
        while True:
            try:
                result = self.result_queue.get(timeout=3)
                print(f"Boss task : {result.identifier} completed")
            except queue.Empty:
                print("Boss : No results yet, waiting...")
                time.sleep(1)


if __name__ == "__main__":
    QueueManager.register("get_task_queue")
    QueueManager.register("get_result_queue")

    manager = QueueManager(address=("localhost", 50000), authkey=b"queue")
    manager.connect()

    task_queue = manager.get_task_queue()
    result_queue = manager.get_result_queue()

    manager_role = Boss(task_queue, result_queue)

    # Lancer un thread pour écouter les résultats
    result_thread = threading.Thread(target=manager_role.track_results, daemon=True)
    result_thread.start()

    task_id = 0
    while True:
        manager_role.assign_task(task_id)
        task_id += 1
        time.sleep(2)
