#include "nlohmann/json.hpp"
#include <Eigen/Dense>
#include <chrono>
#include <cpr/cpr.h>
#include <iostream>
#include <thread>
#include <unistd.h>

#define CODE_SUCCESS 200
#define NB_CORES 4

class Minion {
private:
  int identifier;
  int size;
  Eigen::MatrixXd A; // pour les calculs après
  Eigen::VectorXd X;
  Eigen::VectorXd b;
  double execution_time;

public:
  Minion() {
    while (true) {
      while (!initialization()) {
      }
      work();
    }
  }

  // Fonction pour initialiser le Minion en récupérant des données depuis un
  // serveur
  bool initialization() {
    // Faire une requête GET vers le serveur
    cpr::Response response = cpr::Get(cpr::Url{"http://localhost:8000"});

    // Vérifier si la requête a réussi (code de statut HTTP 200)
    if (response.status_code != CODE_SUCCESS) {
      // Si la requête échoue, retourner false pour indiquer un échec
      // d'initialisation
      return false;
    }

    // Analyser la réponse JSON
    nlohmann::json data_json = nlohmann::json::parse(response.text);

    // Extraire l'identifiant et la taille des données JSON
    identifier = data_json["identifier"];
    size = data_json["size"];

    // Redimensionner les matrices A et le vecteur b en fonction de la taille
    // extraite
    A.resize(size, size);
    b.resize(size);

    // Remplir la matrice A et le vecteur b avec les valeurs des données JSON
    for (unsigned long i = 0; i < size; i++) {
      for (unsigned long j = 0; j < size; j++) {
        A(i, j) = data_json["A"][i][j];
      }
      b(i) = data_json["b"][i];
    }
    // Afficher un message indiquant une initialisation réussie
    std::cout << "Minion " << getpid() << " pour la tâche " << identifier
              << " initialisé" << std::endl;

    // Retourner true pour indiquer une initialisation réussie
    return true;
  }
}


int main() {

  // Définir le nombre de threads pour le traitement parallèle en utilisant la
  // bibliothèque Eigen
  Eigen::setNbThreads(NB_CORES); // permet de limiter l'utilisation de eigen

  // Créer une instance de la classe Minion
  Minion minion;

  // Quitter le programme
  exit(0);
}
