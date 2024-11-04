import unittest
import numpy as np
import numpy.testing as npt


class TestMatrixMultiplication(unittest.TestCase):
    def test_matrix_product(self):
        # Définir les matrices/vecteurs A, x, et un B différent
        A = np.array([[1, 2], [3, 4]])
        x = np.array([1, 1])

        # Définir B different de A @ x
        B = np.random.rand(
            2
        )  # Génère un B aléatoire qui ne devrait pas correspondre à A @ x

        # Calculer le produit A @ x
        result = A @ x

        # Vérifier que A @ x est proche de B pour provoquer l'erreur
        npt.assert_allclose(result, B, rtol=1e-7, atol=0)


if __name__ == "__main__":
    unittest.main()
