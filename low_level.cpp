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
  int time;
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
    cpr::Response response = cpr::Get(cpr::Url{"http://localhost:8000"});

    // Vérifiez si la réponse est vide
    if (response.text.empty()) {
      std::cerr << "Réponse vide du serveur" << std::endl;
      return false;
    }

    // Vérifiez les erreurs réseau
    if (response.error) {
      std::cerr << "Erreur réseau : " << response.error.message << std::endl;
      return false;
    }

    // Vérifiez le code HTTP
    if (response.status_code != CODE_SUCCESS) {
      std::cerr << "Erreur HTTP : " << response.status_code << std::endl;
      return false;
    }

    try {
      nlohmann::json data_json = nlohmann::json::parse(response.text);

      if (!data_json.contains("A") || !data_json.contains("b") ||
          !data_json.contains("identifier") || !data_json.contains("size") ||
          !data_json.contains("time")) {
        std::cerr << "Données JSON incomplètes" << std::endl;
        return false;
      }

      identifier = data_json["identifier"];
      size = data_json["size"];
      time = data_json["time"];

      if (!data_json["A"].is_array() || !data_json["b"].is_array() ||
          data_json["A"].size() != size || data_json["b"].size() != size) {
        std::cerr << "Dimensions invalides dans le JSON" << std::endl;
        return false;
      }

      A.resize(size, size);
      b.resize(size);

      for (unsigned long i = 0; i < size; i++) {
        for (unsigned long j = 0; j < size; j++) {
          A(i, j) = data_json["A"][i][j].is_number()
                        ? data_json["A"][i][j].get<double>()
                        : 0.0;
        }
        b(i) = data_json["b"][i].is_number() ? data_json["b"][i].get<double>()
                                             : 0.0;
      }

      std::cout << "Minion " << getpid() << " pour la tâche " << identifier
                << " initialisé" << std::endl;

      return true;

    } catch (const nlohmann::json::exception &e) {
      std::cerr << "Erreur JSON : " << e.what() << std::endl;
      return false;
    }
  }

  // Fonction pour effectuer le travail du Minion en résolvant un système
  // linéaire AX=b
  double work() {
    // Résoudre le système linéaire ax=b et mesurer le temps de début et de fin
    const auto start_time = std::chrono::high_resolution_clock::now();
    X = A.lu().solve(b);
    const auto end_time = std::chrono::high_resolution_clock::now();

    // Calculer le temps d'exécution
    execution_time = std::chrono::duration_cast<std::chrono::duration<double>>(
                         end_time - start_time)
                         .count();

    // Afficher l'identifiant de la tâche et le temps d'exécution
    std::cout << "Minion " << getpid() << " a terminé la tâche " << identifier
              << " en " << execution_time << std::endl;

    // créer le json pour le POSt
    nlohmann::json data_json = {
        {"identifier", identifier},
        {"size", size},
        {"time", execution_time},
    };

    for (int i = 0; i < size; i++) {
      data_json["b"][i] = b[i];
      for (int j = 0; j < size; j++) {
        data_json["A"][i][j] = A(i, j);
      }
    }
    // envoyer données sur serveur (POST)
    cpr::Post(cpr::Url{"http://localhost:8000"},
              cpr::Body{data_json.dump()}); // 127.0.01:8000

    // Retourner le temps d'exécution
    return execution_time;
  }
};

int main() {

  // Définir le nombre de threads pour le traitement parallèle en utilisant la
  // bibliothèque Eigen
  Eigen::setNbThreads(NB_CORES); // permet de limiter l'utilisation de eigen

  // Créer une instance de la classe Minion
  Minion minion;

  // Quitter le programme
  exit(0);
}
