import unittest
import numpy as np
import numpy.testing as npt


class TestMatrixMultiplication(unittest.TestCase):
    def test_matrix_product(self):
        # Définir les matrices ou vecteurs A, x, et B
        A = np.array([[1, 2], [3, 4]])
        x = np.array([1, 1])
        B = np.array([3, 7])  # Exemple de B que A @ x devrait égaler

        # Calculer le produit A @ x
        result = A @ x

        # Vérifier que A @ x est proche de B
        npt.assert_allclose(result, B, rtol=1e-5, atol=1e-8)


if __name__ == "__main__":
    unittest.main()
