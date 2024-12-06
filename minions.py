from manager import QueueClient


class Minion:
    def __init__(
        self,
    ):
        self.client = QueueClient()

    def run(self):
        while True:
            try:
                # Récupérer une tâche de la task_queue
                task = self.client.task_queue.get(timeout=5)
                print(f"Minion: Received task {task.identifier}. Processing...")

                # Simuler le traitement de la tâche
                task.work()
                print(
                    f"Minion: Task {task.identifier} completed in {task.time} seconds. Sending result back."
                )

                # Envoyer le résultat dans la result_queue
                self.client.result_queue.put(task)
            except Exception:
                print("Minion: No more tasks. Exiting...")
                break


if __name__ == "__main__":
    # Créer une instance de Minion et runs
    minion = Minion()
    minion.run()
