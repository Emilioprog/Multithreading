import unittest
from task import Task
import numpy.testing as npt


class TaskTest(unittest.TestCase):
    def test_task(self):
        t = Task()
        t.work()  # Ajouté pour que ça marche
        npt.assert_allclose(t.a @ t.x, t.b)
