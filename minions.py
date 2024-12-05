from queue_system import QueueManager


class Minion:
    def __init__(self, task_queue, result_queue):
        self.task_queue = task_queue
        self.result_queue = result_queue

    def run(self):
        while True:
            try:
                # Récupérer une tâche de la task_queue
                task = self.task_queue.get(timeout=5)
                print(f"Minion: Received task {task.identifier}. Processing...")

                # Simuler le traitement de la tâche
                task.work()
                print(f"Minion: Task {task.identifier} completed. Sending result back.")

                # Envoyer le résultat dans la result_queue
                self.result_queue.put(task)
            except Exception:
                print("Minion: No more tasks. Exiting...")
                break


if __name__ == "__main__":
    QueueManager.register("get_task_queue")
    QueueManager.register("get_result_queue")

    # Connexion au serveur
    manager = QueueManager(address=("localhost", 50000), authkey=b"queue")
    manager.connect()

    # Récupérer les queues depuis le serveur
    task_queue = manager.get_task_queue()
    result_queue = manager.get_result_queue()

    # Créer une instance de Minion
    minion_role = Minion(task_queue, result_queue)
    minion_role.run()
