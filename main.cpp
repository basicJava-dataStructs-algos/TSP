#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <cmath>
#include <unordered_set>
#include <limits>

#define COLS 2

using namespace std;

struct Point {
    double x, y;
};

int numCities;
vector<vector<double>> matrix;
vector<int> tour;

//calculate euclidean distance
double calculateDistance(const Point& p1, const Point& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Function to create a graph based on the input coordinates
vector<vector<double>> createMatrix(const vector<Point>& points, int size) {
    int n = size;
    vector<vector<double>> matrix(n, vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                // Calculate Euclidean distance between points i and j
                matrix[i][j] = calculateDistance(points[i], points[j]);
            }
        }
    }

    // Print the distance matrix
   cout << "Distance Matrix:" << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    } 

    return matrix;
}

vector<int> greedyTSP(vector<vector<double>> matrix, int numCities) {

    vector<int> tour;
    vector<bool> visited(numCities, false);

    int start = 0;
    tour.push_back(start);
    visited[start] = true;

    while(tour.size() < numCities) {
        int nearestCity = -1;
        double minDistance = numeric_limits<double>::max();

        for(int i = 0; i < numCities; i++) {
            if(!visited[i] && matrix[start][i] < minDistance) {
                minDistance = matrix[start][i];
                nearestCity = i;
            }
        }

        if(nearestCity != -1) {
            tour.push_back(nearestCity);
            visited[nearestCity] = true;
            start = nearestCity;
        }
    }

    return tour;
}

/**
 * This function returns the adjacent index based on the specified direction.
 * If direction is -1, it returns the previous index; if direction is 1, it returns the next index.
 * If the current index is at the boundary, it wraps around accordingly.
*/
int getAdjacentIdx(int index, int direction) {
    if (direction == -1) {
        return (index == 0) ? numCities - 1 : index - 1;
    }
    if (direction == 1) {
        return (index + 1) % numCities;
    }
}

//Get the list of edges from the t index based on the specified mode
//deriveX/Y in one function
vector<tuple<int, int>> deriveEdges(vector<int>& changes, char mode) {
    vector<tuple<int, int>> edges;

    int start = (mode == 'X') ? 1 : 2;
    int end = (mode == 'X') ? changes.size() - 2 : changes.size() - 1;

    for (int i = start; i < end; i += 2) {
        tuple<int, int> edge = make_tuple(changes[i], changes[i + 1]);
        edges.push_back(edge);
    }

    return edges;
}

vector<tuple<int, int>> deriveEdgesFromTour(vector<int> tour) {
    vector<tuple<int, int>> edges;
    for(int i = 0; i < tour.size(); i++) {
        tuple<int , int> edge = make_tuple(tour[i], tour[(i+1)%tour.size()]);
        edges.push_back(edge);
    }
    return edges;
}

vector<int> constructNewTour(vector<int> tour, vector<int> changes) {
    vector<tuple<int, int>> currentEdges = deriveEdgesFromTour(tour);

    vector<tuple<int, int>> X = deriveEdges(changes, 'X');
    vector<tuple<int, int>> Y = deriveEdges(changes, 'Y');

    for (int i = 0; i < X.size(); i++) {
        for(int j = 0; j < currentEdges.size(); j++) {
            if(X[i] == currentEdges[j]) {
                currentEdges.erase(currentEdges.begin()+j);
                break;
            }
        }
    }
    currentEdges.insert(currentEdges.end(), Y.begin(), Y.end());
    return createTourFromEdges(currentEdges);
}

vector<int> createTourFromEdges(vector<tuple<int, int>> edges) {
    vector<int> tour;
    tuple<int, int> edge = edges[0];
    tour.push_back(get<0>(edge));
    tour.push_back(get<1>(edge));

    int last = get<1>(edge);
    edges.erase(edges.begin()+0);

    while(!edges.empty()) {
        for(int i = 0; i < edges.size(); i++) {
            int a = get<0>(edges[i]);
            int b = get<1>(edges[i]);
            if(a == last || b == last) {
                last = (a == last) ? b : a;
                tour.push_back(last);
                edges.erase(edges.begin()+i);
                break;
            }
        }
    }
    if(tour[0] == tour[tour.size()-1]){
        tour.pop_back();
    }

    return tour;

}

bool isDisjunctive(vector<int> tIndex, int x, int y) {
    if(x == y) {
        return false;
    }
    for(int i = 0; i < tIndex.size()-1;i++) {
        int currentNode = tIndex[i];
        int nextNode = tIndex[i + 1];

        if ((currentNode == x && nextNode == y) || (currentNode == y && nextNode == x)) {
            return false;
        }
    }
    return true;
}

bool isTour(vector<int> tour) {
    if (tour.size() != numCities) {
        return false;
    }

    unordered_set<int> visitedVertices;

    for (int i = 0; i < tour.size(); i++) {
        if (!visitedVertices.insert(tour[i]).second) {
            return false;
        }
    }

    return true;
}

bool isPositiveGain(vector<int> tIndex, int ti) {
    int gainValue = 0;
    int t3;
    for(int i = 1; i < tIndex.size()-2; i++) {
        int t1 = tIndex[i];
        int t2 = tIndex[i+1];
        if (i == tIndex.size() - 3) {
            t3 = ti;
        } else {
             t3 = tIndex[i + 2];
        }
        
        gainValue += matrix[tour[t2]][tour[t3]] - matrix[tour[t1]][tour[t2]];
    }
    if(gainValue > 0) {
        return true;
    }
    return false;
}

bool isConnected(vector<int> tIndex, int x, int y) {
    if(x == y) {
        return false;
    }

    for(int i = 1; i < tIndex.size(); i+=2) {
        if((tIndex[i] == x && tIndex[i+1] == y) || (tIndex[i] == y && tIndex[i+1] == x)) {
            return false;
        }
    }

    return true;
}

bool nextXPossible(vector<int> tIndex, int i) {
    int previousIndex = getAdjacentIdx(i, -1);
    int nextIndex = getAdjacentIdx(i, 1);

    bool isConnectedToNext = isConnected(tIndex, i, nextIndex);
    bool isConnectedToPrev = isConnected(tIndex, i, previousIndex);

    return isConnectedToNext || isConnectedToPrev;
}

int getNextPossibleY(vector<int> tIndex) {
    int ti = tIndex[tIndex.size()-1];
    vector<int> possibleYs;
    for(int i = 0; i < numCities; i++) {
        if((!isDisjunctive(tIndex, i, ti)) || (!isPositiveGain(tIndex, i)) || (!nextXPossible(tIndex, i))) {
            continue;
        }
        possibleYs.push_back(i);
    }
    double minimumDistance = numeric_limits<double>::max();
    int closestYIndex = -1;
    for(int i = 0; i < possibleYs.size(); i++) {
        if(matrix[tour[ti]][tour[i]] < minimumDistance) {
            closestYIndex = i;
            minimumDistance = matrix[tour[ti]][tour[i]];
        }
    }
    return closestYIndex;
}

int selectNewT(vector<int> tIndex) {
    int prevNode = getAdjacentIdx(tIndex.size()-1, -1);
    int nextNode = getAdjacentIdx(tIndex.size()-1, 1);

    vector<int> changes;
    changes.insert(changes.end(), tIndex.begin(), tIndex.end());
    changes.push_back(prevNode);
    changes.push_back(changes[1]);
    vector<int> tour1 = constructNewTour(tour, changes);

    if(isTour(tour1)) {
        return prevNode;
    }
    changes.pop_back();
    changes.pop_back();
    changes.push_back(nextNode);
    changes.push_back(changes[1]);
    vector<int> tour2 = constructNewTour(tour, changes);
    if(isTour(tour2)){
        return nextNode;
    }
    else{
        return -1;
    }

}

int main() {

    cin >> numCities;
    vector<Point> coordinates;

    for (int i = 0; i < numCities; i++) {
        Point point;
        cin >> point.x;
        cin >> point.y;
        //add every coordinate to the end
        coordinates.push_back(point);
    }

    matrix = createMatrix(coordinates, numCities);
    tour = greedyTSP(matrix, numCities);

    for (int j = 0; j < tour.size(); ++j) {
         cout << tour[j] << endl;
    }
    //cout << endl;
 

    return 0;
}
