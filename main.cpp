#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <list>

using namespace std;

class Graph
{
private:
    vector<vector<bool>> matrix; //Macierz sąsiedztwa./home/marcin/Gold Mercedes Racing/cmake-build-debug
    unsigned int vertices; //Ilosc wierzcholkow.
    unsigned int edges; //Ilosc krawedzi.

public:
    friend bool are_isomorphic(Graph *, Graph *, vector<int> & T);
    friend void permutations(vector<int> & T, int ix, Graph * graph1, Graph * graph2, bool & isomorphic);

    Graph(unsigned int size)
    {
        vertices = size;
        edges=0;
        matrix.resize(vertices);
        for (int i = 0; i < vertices; i++)
        {
            matrix[i].resize(vertices);
            for (int j = 0; j < vertices; j++)
            {
                matrix[i][j] = false;
            }
        }
    }

    void add_edge(unsigned int ver1, unsigned int ver2)
    {
        matrix[ver1][ver2] = true;
        matrix[ver2][ver1] = true;
        edges++;
    }

    static Graph * load_from_file(string filename)
    {
        ifstream file;
        file.open(filename);
        if (!file.good())
            throw ("Nie mozna otworzyc pliku " + filename);

        //Sprawdzam czy plik zawiera inne znaki niz cyfry czy biale znaki.
        char znak;
        while (!file.eof()) {
            file >> znak;
            if (!isspace(znak) && !isdigit(znak)) {
                throw ("Plik " + filename + " zawiera niedozwolone znaki.");
            }
        }
        file.close();

        file.open(filename);
        if (!file.good())
            throw ("Nie mozna otworzyc pliku " + filename);

        string line;
        getline(file, line);
        if (line.empty())
            throw ("Plik " + filename + " jest pusty");
        istringstream tiss(line);
        unsigned int ver;
        tiss >> ver;
        Graph * graph = new Graph(ver);

        while (getline(file, line))
        {
            if (line.empty())
                break;
            int x, y;
            istringstream iss(line);
            iss >> x >> y;
            if (x < 0 || x >= ver || y < 0 || y >= ver)
            {
                string error = "Podano bledny wierzcholek.";
                throw (error);
            }
            graph->add_edge(x,y);
        }

        file.close();
        return graph;
    }
};

void permutations(vector<int> & T, int ix, Graph * graph1, Graph * graph2, bool & isomorphic)
{
    //T - tablica permutacji, i-ty wierzcholek jest permutowany na T[i]-ty wierzcholek
    if( ix < graph1->vertices )
    {
        //Rekurencyjna permutacja wierzcholkow
        for(int i = ix; i < graph1->vertices; i++)
        {
            swap(T[ix], T[i]);
            permutations(T, ix+1, graph1, graph2, isomorphic);
            if (isomorphic) //Jesli sa izomorficzne, koncze.
                return;
            swap(T[ix], T[i]);
        }
    }
    else
    {
        bool are_similar=true;
        //Permutuje macierz sasiedztwa jednego grafu, po czym je porownuje
        //Jesli sa takie same, to grafy sa izomorficzne.
        for (int i=0;i<graph1->vertices;i++)
        {
            for (int j = i; j < graph1->vertices; j++)
            {
                if (graph1->matrix[T[i]][T[j]] != graph2->matrix[i][j])
                {
                    are_similar = false;
                    return;
                }
            }
        }
        if (are_similar)
            isomorphic=true;
    }
}

bool are_isomorphic(Graph * graph1, Graph * graph2, vector<int> & T)
{
    //Jesli grafy maja rozna ilosc wierzcholkow lub krawedzi sa nieizomorficzne.
    if (graph1->vertices != graph2->vertices)
        return false;
    if (graph1->edges != graph2->edges)
        return false;

    //Tworze wektory stopni wierzcholkow obu grafow.
    vector<int> degrees1, degrees2;
    degrees1.resize(graph1->vertices);
    degrees2.resize(graph1->vertices);
    //Zliczam wszystkie krawedzie dla danego wierzcholka i wychodzi stopien.
    for (int i=0;i<degrees1.size();i++)
    {
        degrees1[i]=0;
        degrees2[i]=0;
        for (int j=0;j<degrees1.size();j++)
        {
            degrees1[i]+=graph1->matrix[i][j];
            degrees2[i]+=graph2->matrix[i][j];
        }
    }
    //Sortuje wektory.
    sort(degrees1.begin(),degrees1.end());
    sort(degrees2.begin(),degrees2.end());
    //Jesli wektory sa rozne, grafy sa nieizomorficzne.
    for (int i=0;i<degrees1.size();i++)
    {
        if (degrees1[i] != degrees2[i])
            return false;
    }

    //Ostatecznie trzeba sprawdzic permutacje macierzy sasiedztwa.
    T.resize(graph1->vertices);
    for (int i=0;i<graph1->vertices;i++)
        T[i]=i;
    bool isomorphic=false;
    permutations(T,0,graph1,graph2,isomorphic);

    if (isomorphic)
        return true;
    else return false;
}


int main(int argc, char * argv[])
{
    vector<int> T;
    Graph * graph1 = nullptr;
    Graph * graph2 = nullptr;
    try
    {
        cout << endl;
        graph1 = Graph::load_from_file(argv[1]);
        graph2 = Graph::load_from_file(argv[2]);
        cout << "Wczytano grafy: " << argv[1] << " " << argv[2] << endl;
        if (are_isomorphic(graph1, graph2, T)) {
            cout << "Izomorficzne" << endl;
            for (int i = 0; i < T.size(); i++)
                cout << i << " --> " << T[i] << endl;
        } else
            cout << "Nieizomorficzne" << endl;
        delete graph1;
        delete graph2;
        return 0;
    }
    catch(string w)
    {
        cout << w << endl;
        if (graph1)
            delete graph1;
        if (graph2)
            delete graph2;
        return 69;
    }
}
