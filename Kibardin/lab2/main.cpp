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
        for(int i = 0; i < MATRIX_SIZE; i++) {
            for(int j = 0; j < MATRIX_SIZE; j++)
            {
                float buff = fabs(EndTop - (j + 'a'));
                matrix[i][j].hX = fabs(EndTop - (j + 'a'));
            }

        }

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
            {
                for(int j = 0; j < MATRIX_SIZE; j++)
                    if(matrix[i][j].distance != 0)
                    {
                        flag = true;
                        break;
                    }
            }
            if(!flag) {
                continue;
            }
            float value = -1;
            while(value < 0)                   // задание эвристики
            {
                printf("enter heuristic for top (%c): ", 'a' + i);
                std::cin >> value;
                printf("%g\n", value);
            }
            for(int j = 0; j < MATRIX_SIZE; j++)
                matrix[j][i].hX = value;
        }
    }

    void setClosed(std::vector<char>& open, std::vector<char>& closed, char top) const        // процедура для добавления вершины
    {                                                                               // в обработанное множество
        for(auto iter = open.begin(); iter != open.end();iter++)
        {
            if(*iter == top)
            {
                printf("all tops from (%c) visited\nadd (%c) to closed\n", *iter, *iter);
                puts("----------------------");
                closed.push_back(*iter);
                open.erase(iter);
                break;
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

    bool isInMany(std::vector<char>& many, char symbol) const           // метод для проверки наличия вершины
    {                                                                   // в заданном множестве
        for(auto iter : many)
            if(iter == symbol)
                return true;
        return false;
    }

    void printQueue(std::vector<Queue>& queue) const
    {
        puts("----------------------");
        printf("in queue:\n");
        for(auto iter : queue)
        {
            printf("from (%c) to (%c) = %g\n",iter.prev, iter.next, iter.gX);
        }
        puts("----------------------");
    }

    bool aStar(char startTop, char endTop)                              // метод реализующий алгоритм а*
    {
        result.clear();
        if(startTop == endTop)
        {
            result.push_back(startTop);
            return true;
        }
        std::vector<Queue> queue;
        std::vector<char> closed_char, open;
        unsigned int currentTop;
        open.push_back(startTop);
        for(int nextTop = 0; nextTop < MATRIX_SIZE; nextTop++)  //  добавляем в очередь элементы из стартовой вершины
        {
            currentTop = startTop - 'a';
            if(matrix[currentTop][nextTop].distance != 0)
            {
                Queue buffer;
                buffer.next = nextTop + 'a';
                buffer.gX = matrix[currentTop][nextTop].distance + matrix[currentTop][nextTop].gX;
                buffer.prev = startTop;
                matrix[currentTop][nextTop].visited = true;
                setG_X(nextTop, matrix[currentTop][nextTop].distance + matrix[currentTop][nextTop].gX , startTop);//куда, дистанция от старта, откуда
                queue.push_back(buffer);
            }
        }

        setClosed(open, closed_char,startTop);
        while(!queue.empty())                                           // пока есть хотя бы одно необработанное ребро
        {                                                               // идет поиск пути
            puts("new iteration");
            printQueue(queue);
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
            printf("min value in queue: from (%c) to (%c) cost g(x) = %g\nerased\n", minValue.prev, minValue.next, minValue.gX);
            if(minValue.next == endTop) {                              // если найденное значение является конечной вершиной
                setG_X(minValue.next - 'a', minValue.gX, minValue.prev);// восстанавливаем путь
                restoreWay(startTop, endTop);
                return true;
            }

            queue.erase(iterPosition);
            setClosed(open, closed_char, minValue.prev);
            for(int i = 0; i < MATRIX_SIZE; i++)        // рассматриваем всех соседей данной вершины
            {

                if(matrix[minValue.next - 'a'][i].distance != 0)
                {
                    float tentativeGScore = minValue.gX + matrix[minValue.next - 'a'][i].distance;
                    Queue buffer;
                    buffer.next = 'a' + i;
                    buffer.prev = minValue.next;
                    buffer.gX = tentativeGScore;
                    if(isInMany(closed_char, i + 'a'))                         // Если элемент ведет в обработанную вершину,
                        continue;                                                        // пропускаем его
                    if(!isInMany(open, i + 'a'))                               // Если элемент не в множестве
                    {                                                                    // обрабатываемых вершин, то добавляем его в это множество
                        setG_X(buffer.next - 'a', tentativeGScore, buffer.prev);
                        open.push_back(buffer.next);
                    }
                    else
                    {
                        float matrixValue = matrix[i][0].gX;
                        if(matrix[i][0].gX > tentativeGScore)
                        {
                            printf("update g(x) for top %c\nold = %g, new = %g\n", 'a' + i, matrix[i][0].gX, tentativeGScore);
                            setG_X(i, tentativeGScore, minValue.next);     // Обновляем путь, если он оказался меньше
                        }
                    }
                    printf("add way from (%c) to (%c) cost g(x) = %g\n", buffer.prev, buffer.next, buffer.gX);
                    queue.push_back(buffer);
                }
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
        graph.printResult();
    else
       std::cout << "don't exist" << std::endl;
    puts("\n\n\n");
    graph.setNotVisited();
    //graph.setHeuristicStepik(endTop);
    graph.setOwnHeuristic();
    puts("search way with a*: ");
    if(graph.aStar(startTop, endTop))
    {
        printf("result: ");
        graph.printResult();
    }
    else
        std::cout << "don't exist" << std::endl;
    return 0;
}