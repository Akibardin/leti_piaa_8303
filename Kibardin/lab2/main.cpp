#include <iostream>
#include <vector>
#include <cfloat>
#include <cmath>
#define MATRIX_SIZE 26
struct Top                                          // структура для описания вершины
{
    float distance = 0;
    bool visited = false;

    // a* modification
    float gX = 0;
    float hX = 0;
    char prevTop = '\0';
};

struct Queue                                        // структура очереди с приоритетом
{
    char next;
    float gX;
    char prev;
};

class Graph {                                       // класс графа
    std::vector<std::vector<Top>> matrix;          // матрица графа
    std::vector<char> result;
public:
    Graph() {
        matrix.resize(26);
        for (int i = 0; i < MATRIX_SIZE; i++)
            matrix[i].resize(26);
    }

    void setNotVisited()                            // метод для работы в одной программе с двумя методами поиска пути
    {
        for(int i = 0; i < MATRIX_SIZE; i++)
            for(int j = 0; j < MATRIX_SIZE; j++)
                matrix[i][j].visited = false;
    }

    void addRib(char start, char end, float distance)// метод для добавления ребра в матрицу
    {
        if(start - 'a' >= MATRIX_SIZE || end - 'a' >= MATRIX_SIZE || matrix[start - 'a'][end - 'a'].distance != 0)
            return;
        matrix[start - 'a'][end - 'a'].distance = distance;

    }

    bool isInSolution(int tmpTop)                   // метод для проверки наличия вершины в результате
    {
        for(auto iter : result)
            if(tmpTop + 'a' == iter)
                return true;
        return false;
    }

///////////////////////////////////////////////////////////////////////////////////

    bool greedyAlgorithm(char startTop, char endTop)    // метод описывающий жадный алгоритм
    {
        result.resize(0);
        result.push_back(startTop);
        printf("start top (%c)\n", startTop);
        puts("--------------------------");
        int bufferTop = startTop - 'a';
        while(true)
        {
            float min = FLT_MAX;
            int indexMin = -1;
            for(int i = 0; i < MATRIX_SIZE; i++)
            {
                if(i == endTop - 'a' && matrix[bufferTop][i].distance != 0)
                {
                    i += 'a';
                    result.push_back(i);
                    return true;
                }
                if(matrix[bufferTop][i].distance < min  && matrix[bufferTop][i].distance !=0 && !isInSolution(i)  && !matrix[bufferTop][i].visited)
                {
                    indexMin = i;   // выборка минимального пути
                    min = matrix[bufferTop][i].distance;
                }
            }

            char symbol = 'a';
            if(indexMin != -1)      // добавление вершины в решение
            {
                bufferTop = indexMin;

                symbol += indexMin;
                result.push_back(symbol);
                printf("add (%c) to result\ncurrent result: ", symbol);
                printResult();
                puts("--------------------------");
                if(symbol == endTop) {
                    return true;
                }

            } else
            {
                if(!result.empty())     // из текщей вершины некуда идти, откат назад
                {
                    symbol = result.back();
                    result.pop_back();
                    printf("no way from the top (%c)\ncurrent result: ", symbol);
                    printResult();
                    puts("--------------------------");
                    //printResult();
                    for(int i = 0; i < MATRIX_SIZE; i++)
                        matrix[i][symbol - 'a'].visited = true;
                    if(result.empty())
                        return false;
                    bufferTop = result.back() - 'a';
                }else
                {
                    return false;
                }
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////////

    void setHeuristicStepik(char EndTop)// эвристика для степика
    {
        for(int i = 0; i < MATRIX_SIZE; i++)
            for(int j = 0; j < MATRIX_SIZE; j++)
                    matrix[i][j].hX = fabs(EndTop - (j + 'a'));
    }

    void setOwnHeuristic()              // метод для задания собственной эвристики
    {
        for(int i = 0; i < MATRIX_SIZE; i++)
        {
            bool flag = false;
            for(int j = 0; j < MATRIX_SIZE; j++)// цикл, в котором проверяется существование вершины
            {                                   // для задания эвристики
                if(matrix[j][i].distance != 0)
                {
                    flag = true;
                    break;
                }
            }
            if(!flag)
                continue;
            float value = -1;
            while(value <= 0)                   // задание эвристики
            {
                printf("enter heuristic for top (%c): ", 'a' + i);
                std::cin >> value;
                printf("%g\n", value);
            }
            for(int j = 0; j < MATRIX_SIZE; j++)
                matrix[j][i].hX = value;
        }
    }

    void setClosed(std::vector<char>& open, std::vector<char>& closed) const        // процедура для добавления вершины
    {                                                                               // в обработанное множество
        for(auto iter: open)
        {
            bool flag = false;
            for(int i = 0; i < MATRIX_SIZE; i++)                // цикл для проверки существования необработанных ребер
                flag |= matrix[iter - 'a'][i].visited;
            if(flag)
            {
                printf("all tops from (%c) visited\nadd (%c) to closed\n", iter, iter);
                puts("--------------------------");
                closed.push_back(open.back());
                open.pop_back();
            }
        }
    }

    void setG_X(int next, float startDistance, char prev)       // метод для задания значения расстояния от старта до
    {                                                           // текущей вершины
        for(int i = 0; i < MATRIX_SIZE; i++)
        {
                matrix[next][i].prevTop = prev;
                matrix[next][i].gX = startDistance;
        }
    }

    void restoreWay(char startTop, char endTop)                 // метод восстановления пути от конечной вершины
    {
        std::vector<char> inverResult;
        char prevTop = endTop;
        while(true)
        {
            if(prevTop == startTop || prevTop == '\0') {
                inverResult.push_back(startTop);
                break;
            }
            inverResult.push_back(prevTop);
            prevTop = matrix[prevTop - 'a'][0].prevTop;
        }

        for(auto i = inverResult.rbegin(); i != inverResult.rend(); i++)
            result.push_back(*i);
    }



    bool isInQueue(std::vector<Queue>& queue, Queue value) const        // метод для проверки наличия ребра
    {                                                                   // в текущей очереди
        for(auto i : queue)
        {
            if(i.next == value.next && i.gX == value.gX)
                return true;
        }
        return false;
    }

    bool isInMany(std::vector<char>& many, char symbol) const           // метод для проверки наличия вершины
    {                                                                   // в заданном множестве
        for(auto iter : many)
            if(iter == symbol)
                return true;
        return false;
    }

    bool aStar(char startTop, char endTop)                              // метод реализующий алгоритм а*
    {
        result.clear();
        int bufferTop = startTop - 'a';

        std::vector<Queue> queue;
        std::vector<char> closed, open;
        open.push_back(startTop);
        while(!open.empty())                                           // пока есть хотя бы одно необработанное ребро
        {                                                               // идет поиск пути
            unsigned int currentTop;
            if(open.back()== endTop) {
                restoreWay(startTop, endTop);
                return true;
            }
            for(auto iter : open)                                       // Цикл для добавления в очередь новых вариантов
            {                                                           // пути. Если текущий путь отсутствует в очереди,
                for(int nextTop = 0; nextTop < MATRIX_SIZE; nextTop++)  //  добавляем в очередь
                {
                    currentTop = iter - 'a';
                    if(matrix[currentTop][nextTop].distance != 0)
                    {
                        Queue buffer;
                        buffer.next = nextTop + 'a';
                        buffer.gX = matrix[currentTop][nextTop].distance + matrix[currentTop][nextTop].gX;
                        buffer.prev = iter;
                        if(!isInQueue(queue, buffer) && !matrix[currentTop][nextTop].visited)
                        {
                            matrix[currentTop][nextTop].visited = true;
                            setG_X(nextTop, matrix[currentTop][nextTop].distance + matrix[currentTop][nextTop].gX , iter);//куда, дистанция от старта, откуда
                            queue.push_back(buffer);
                        }
                    }
                }
            }
            setClosed(open, closed);                                // обновляем множества обработанных и необработанных вершин
            if(queue.empty())
                return false;
            Queue minValue = queue[0];
            auto iterPosition = queue.begin();
            for(auto iter = queue.begin(); iter != queue.end(); iter++)   // поиск минимального значения в очереди и
            {                                                             // последующее извлечение его
                float fXMin = minValue.gX + matrix[minValue.prev - 'a'][minValue.next - 'a'].hX;
                float fXIter = (*iter).gX + matrix[(*iter).prev - 'a'][(*iter).next - 'a'].hX;
                if(fXMin > fXIter || (fXMin == fXIter && minValue.next < (*iter).next))
                {
                    minValue = *iter;
                    iterPosition = iter;
                }
            }
            queue.erase(iterPosition);
            if(isInMany(closed, minValue.next))                         // Если элемент ведет в обработанную вершину,
                continue;                                                  // пропускаем его
            if(!isInMany(open, minValue.next))
            {
                printf("add way from (%c) to (%c)\n", minValue.prev, minValue.next);
                puts("--------------------------");
                open.push_back(minValue.next);                             // Добавление элемента в необработанное множество
            }
            else
            {
                if(matrix[minValue.next - 'a'][0].gX > minValue.gX)
                    setG_X(minValue.next, minValue.gX, minValue.prev);     // Обновляем путь, если он оказался меньше
            }
        }
        return false;
    }

    void printResult() const                                               // вывод результата
    {
        if(!result.empty())
            for(auto iter : result)
                std::cout << iter;
        std::cout << std::endl;
    }
};

int main() {
    char startTop, endTop ,start, end;
    float distance;
    Graph graph;
    //freopen("ribs.txt", "r", stdin);
    std::cin >> startTop >> endTop;

    std::cin >> start;                          // ввод ребер до точки в формате
    while(start != '.') {                       // {a} {b} {distance}
        std::cin >>end >> distance;             // ......
        graph.addRib(start, end, distance);     // {a} {b} {distance}
        std::cin >> start;                      // .
    }
    puts("search way with greedy: ");
    if(graph.greedyAlgorithm(startTop, endTop))
    {
        graph.printResult();
    }
    else
        std::cout << "don't exist" << std::endl;

    puts("\n\n\n");

    graph.setNotVisited();
    //graph.setHeuristicStepik(endTop);
    graph.setOwnHeuristic();


    puts("search way with a*: ");
    if(graph.aStar(startTop, endTop))
        graph.printResult();
    else
        std::cout << "don't exist" << std::endl;
    return 0;
}