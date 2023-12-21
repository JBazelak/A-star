#include <iostream>
#include <fstream>
#include <list>
#include <cmath>

using namespace std;

const int ROWS = 20;
const int COLS = 20;

// Struktura reprezentująca punkt na planszy
struct Point {
    int x, y;
    int cost;      // koszt dotarcia do tego punktu
    int heuristic; // heurystyka
    int totalCost; // koszt całkowity (koszt + heurystyka)
    Point* parent; // wskaźnik do rodzica
};

// Funkcja obliczająca heurystykę (odległość euklidesową) pomiędzy dwoma punktami
int calculateHeuristic(const Point& current, const Point& goal) {
    return (sqrt(pow(goal.x - current.x, 2) + pow(goal.y - current.y, 2)));
}

// Funkcja sprawdzająca, czy dany punkt jest ważny (nie wychodzi poza zakres planszy i nie jest ścianą)
bool isValid(int** grid, int x, int y) {
    return (x >= 0 && x < ROWS && y >= 0 && y < COLS && grid[x][y] != 5);
}

// Funkcja wykonująca algorytm A*
void AStar(int** grid, Point start, Point goal) {
    // Tworzymy listę otwartą i zamkniętą
    list<Point> openList;
    list<Point> closedList;

    // Dodajemy punkt początkowy do listy otwartej
    openList.push_back(start);

    while (!openList.empty()) {
        // Wybieramy punkt o najniższym koszcie całkowitym z listy otwartej
        auto current = openList.begin();
        for (auto it = openList.begin(); it != openList.end(); ++it) {
            if (it->totalCost < current->totalCost) {
                current = it;
            }
        }

        // Przenosimy wybrany punkt z listy otwartej do listy zamkniętej
        closedList.push_back(*current);
        openList.erase(current);

        // Jeśli dotarliśmy do celu, kończymy algorytm
        if (closedList.back().x == goal.x && closedList.back().y == goal.y) {
            break;
        }

        // Sprawdzamy sąsiadów aktualnego punktu
        for (int i = 0; i < 4; ++i) {
            int newX = closedList.back().x;
            int newY = closedList.back().y;

            switch (i) {
            case 0: // Góra
                newX--;
                break;
            case 1: // Prawo
                newY++;
                break;
            case 2: // Dół
                newX++;
                break;
            case 3: // Lewo
                newY--;
                break;
            }

            // Sprawdzamy, czy nowy punkt jest ważny
            if (isValid(grid, newX, newY)) {
                // Sprawdzamy, czy nowy punkt jest już na liście zamkniętej
                bool inClosedList = false;
                for (auto& point : closedList) {
                    if (point.x == newX && point.y == newY) {
                        inClosedList = true;
                        break;
                    }
                }

                // Jeśli nowy punkt nie jest na liście zamkniętej, to go przetwarzamy
                if (!inClosedList) {
                    // Obliczamy koszt ruchu i heurystykę dla nowego punktu
                    int newCost = closedList.back().cost + 1;
                    int newHeuristic = calculateHeuristic({ newX, newY }, goal);
                    int newTotalCost = newCost + newHeuristic;

                    // Sprawdzamy, czy nowy punkt jest już na liście otwartej
                    bool inOpenList = false;
                    for (const auto& point : openList) {
                        if (point.x == newX && point.y == newY) {
                            inOpenList = true;
                            break;
                        }
                    }

                    // Jeśli nowy punkt nie jest na liście otwartej lub ma niższy koszt niż dotychczasowy, to go dodajemy
                    if (!inOpenList || newTotalCost < closedList.back().totalCost) {
                        Point* newPoint = new Point{ newX, newY, newCost, newHeuristic, newTotalCost, &closedList.back() };
                        openList.push_back(*newPoint);
                    }
                }
            }
        }
    }

    // Wypełniamy planszę cyframi 3 na podstawie znalezionego rozwiązania
    Point* current = &closedList.back();
    while (current != nullptr) {
        grid[current->x][current->y] = 3;
        current = current->parent;
    }
    cout << endl;
    // Wyświetlamy planszę z oznaczoną ścieżką
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}

//funkcja wyświetlajaca wygenerowaną tablicę
void printGrid(int** array, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << array[i][j] << " ";
        }
        cout << "\n";
    }
}
//funkcja tworząca tablicę

int** makeTab(int x, int y) {
    int** tab = new int* [x];
    for (unsigned int i = 0; i < x; ++i) {
        tab[i] = new int[y];
    }
    return tab;
}
//funkcja ładująca wygenerowane dane z pliku "grid.txt" do tablicy
void loadGridData(const string& fileName, int** array, int rows, int cols) {
    ifstream gridFile(fileName);
    if (!gridFile.is_open()) {
        cerr << "Blad, nie udalo sie otworzyc pliku: " << fileName << endl;
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (!(gridFile >> array[i][j])) {
                cerr << "Błąd podczas wczytywania danych" << endl;
                gridFile.close();
                exit(1);
            }
        }
    }

    gridFile.close();
}

int main() {
    int** grid_tab = makeTab(ROWS, COLS);
    loadGridData("grid.txt", grid_tab, ROWS, COLS);
    printGrid(grid_tab, ROWS, COLS);

    Point start = { 0, 0, 0, 0, 0, nullptr }; // Początkowy punkt
    Point goal = { 19, 19, 0, 0, 0, nullptr }; // Cel

    AStar(grid_tab, start, goal);

    return 0;
}
