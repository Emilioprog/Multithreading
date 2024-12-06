import time
import json

import numpy as np


class Task:
    def __init__(self, identifier, size=None):
        self.identifier = identifier
        # choosee the size of the problem
        # self.size = size or np.random.randint(300, 3_000)
        self.size = size or np.random(30,300)
        # Generate the input of the problem
        self.A = np.random.rand(self.size, self.size)
        self.b = np.random.rand(self.size)
        # prepare room for the results
        self.x = np.zeros((self.size))
        self.time = 0

    def work(self):
        start = time.perf_counter()
        self.x = np.linalg.solve(self.A, self.b)
        self.time = time.perf_counter() - start

    def to_json(self) -> str:
        # dumps
        return json.dumps(
            {
                "identifier": self.identifier,
                "size": self.size,
                "A": self.A.tolist(),
                "b": self.b.tolist(),
                "time": self.time,
            }
        )

    @staticmethod
    def from_json(text: str):
        # loads
        # Deserialize a JSON string to a Task object.
        data = json.loads(text)
        task = Task(identifier=data["identifier"], size=data["size"])
        task.A = np.array(data["A"])
        task.b = np.array(data["b"])
        task.time = data["time"]
        return task

    def __eq__(self, other: "Task") -> bool:
        if not isinstance(other, Task):
            return False
        return (
            self.identifier == other.identifier
            and self.size == other.size
            and np.array_equal(self.A, other.A)
            and np.array_equal(self.b, other.b)
        )
