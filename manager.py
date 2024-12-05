from multiprocessing.managers import BaseManager
from multiprocessing import Queue


class QueueManager(BaseManager):
    pass


class QueueClient:
    def __init__(self):
        QueueManager.register("get_task_queue")
        QueueManager.register("get_result_queue")

        manager = QueueManager(address=("localhost", 50000), authkey=b"queue")
        manager.connect()

        self.task_queue = manager.get_task_queue()
        self.result_queue = manager.get_result_queue()


if __name__ == "__main__":
    task_queue = Queue()
    result_queue = Queue()

    QueueManager.register("get_task_queue", callable=lambda: task_queue)
    QueueManager.register("get_result_queue", callable=lambda: result_queue)

    manager = QueueManager(address=("localhost", 50000), authkey=b"queue")
    server = manager.get_server()
    print("QueueManager server is running...")
    server.serve_forever()
