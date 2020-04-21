#include <iostream>
#include <vector>
#include <map>

#define debug

class Edge
{
public:
    std::vector<char> backEdge;                       // -контейнер для хранения обратных путей
    std::map<char, std::pair<int, int>> forwardEdge;  // -контейнер для хранения множества прямых путей и их пропускных способностей
};
class Graph
{
    char start, end;                                   // -исток и сток графа
    std::map<char,  Edge> graph;                       // -контейнер графа
    std::vector<char> way;                             // -найденный путь
    std::vector<char> backNeighborList;                // -контейнер для хранения множества вершин, которые добавлялись в
public:                                                // путь при обратном проходе
    Graph()
    {
        int edgeNumber;
        std::cin >> edgeNumber;
        std::cin >> start;
        std::cin >> end;
        for(int i = 0; i < edgeNumber; i++)
        {
            int distance;
            char from, where;
            std::cin >> from >> where >> distance;
            graph[from].forwardEdge[where].first = distance;
            graph[from].forwardEdge[where].second = 0;

        }
    }

    bool dfs()                                          // -метод поиска в глубину
    {
        std::vector<char> closed;
#ifdef debug
        std::cout << "-------------------" << std::endl;
        std::cout << "searching way" << std::endl;
#endif
        while (!way.empty()) {
            char currentTop = way.back();
            if(currentTop == end)                       // -если на очередной итерации был добавлен сток, то путь найден
                return true;
            bool flag = false;
            bool backNeighborFounded = false;
                                                        // ищем прямые пути
#ifdef debug
            std::cout << "forward available ways:"<< std::endl;
#endif
            for (const auto &iter : graph[currentTop].forwardEdge) {
                bool inWay = false, inClosed = false;
                for (auto way_iter: way) {              // -проверка на наличии рассматриваемой вершины в пути
                    if (way_iter == iter.first) {
                        inWay = true;
                        break;
                    }
                }
                for(auto closed_iter: closed)           // -проверка на наличии рассматриваемой вершины в множетсве
                    if(closed_iter == iter.first)       // просмотренных вершин
                    {
                        inClosed = true;
                        break;
                    }
                if(inClosed || inWay)
                    continue;

                if (iter.second.first - iter.second.second) {
#ifdef debug
                    std::cout <<currentTop << "->" <<iter.first  << " " <<iter.second.first << "/"  << iter.second.second <<std::endl;
#endif
                    currentTop = iter.first;
                    flag = true;
                    break;
                }
            }

            // -рассматриваем множество обратных ребер, которые не входят в путь
#ifdef debug
            std::cout <<"backward available way"<<std::endl;
#endif
            for (auto backNeighbor: graph[way.back()].backEdge) {   // -аналогично прямым путям, проверяем
                bool inWay = false, inClosed = false;               // есть ли данное ребро в пути и в множестве
                for (auto way_iter: way) {                          // просмотренных вершин
                    if (way_iter == backNeighbor) {
                        inWay = true;
                        break;
                    }
                }
                for(auto closed_iter: closed)
                    if(closed_iter == backNeighbor)
                    {
                        inClosed = true;
                        break;
                    }
                if(inClosed || inWay)
                    continue;
                if (graph[backNeighbor].forwardEdge[way.back()].second) {
#ifdef debug
                    std::cout <<way.back() << "->" <<backNeighbor  << " " << graph[backNeighbor].forwardEdge[way.back()].second <<std::endl;
#endif
                    currentTop = backNeighbor;
                    flag = true;
                    backNeighborFounded = true;
                    break;
                }
            }

            if (flag) {                                             // -Если путь найден, то заносим его в контейнер и
                                                                    // Так же производим проверку, что эта найденная
                                                                    // связана с предыдущей обратным ребром
#ifdef debug
                std::cout <<"next top: "  << currentTop <<std::endl;
#endif
                way.push_back(currentTop);
                if (backNeighborFounded)
                {
                    bool inList = false;
                    for(auto list_iter : backNeighborList)
                        if(list_iter == currentTop)
                        {
                            inList = true;
                            break;
                        }
                    if(!inList)
                        backNeighborList.push_back(currentTop);
                }
            } else {
                closed.push_back(way.back());                       // -Если путь не найден, то помечаем вершину как
                                                                    // просмотренную и удаляем ее из пути
#ifdef debug
                std::cout << "no way from: " << way.back() << std::endl;
                std::cout << "go back to prev top\n" << std::endl;
                std::cout << "-------------------" << std::endl;
#endif
                way.pop_back();
            }

        }
        return false;

    }

    int searchFlow()
    {
        int currentFlow = 0;
        way.push_back(start);

        //пока есть путь до стока и ребра исходящие из истока
        //находим ребро
        //добавляем его в путь. Пока не дошли до стока, ищем новое наибольшее ребро, которое не входит в путь.
        // Дойдя до стока, ищем наименьшее значение потока в текущем пути.
        // Откатываемся назад, до вершины, которя находится перед ребром с наименьшей пропускной способностью
        while(dfs())
        {

#ifdef debug
            std::cout << "founded ";
            printPatch();
            std::cout << "start search bottle neck " << std::endl;
#endif
            int minFlow = INT32_MAX;
            char minTop = end;

            for(auto way_iter = way.begin(); way_iter != way.end() - 1; way_iter++) // -поиск минимального потока
            {
                bool inList = false;
                for(auto list_iter : backNeighborList)
                    if(list_iter == *(way_iter+1) )
                    {
                        inList = true;
                        break;
                    }
                if(!inList)
                {
                    for(const auto& next_iter : graph[*way_iter].forwardEdge)
                    {

                        if(next_iter.first == *(way_iter + 1) && minFlow > next_iter.second.first - next_iter.second.second)
                            minFlow = next_iter.second.first - next_iter.second.second;
                    }
                }
                else
                {
                    for(const auto& next_iter : graph[*(way_iter + 1)].forwardEdge)
                    {

                        if(next_iter.first == *way_iter && minFlow > next_iter.second.second)
                        {
                            minFlow = next_iter.second.second;
#ifdef debug
                            std::cout << "bottle neck: " << minFlow << std::endl;
#endif
                        }
                    }
                }


            }
            currentFlow += minFlow;
#ifdef debug
            std::cout << "current flow: " << currentFlow << std::endl;
#endif
            bool flag = true;
            bool backTop = true;
            for(auto way_iter = way.begin(); way_iter != way.end() - 1; way_iter++) // -заполняем путь потоком
            {
                bool inList = false;
                for(auto list_iter : backNeighborList)
                    if(list_iter == *(way_iter+1) )
                    {
                        inList = true;
                        break;
                    }
                if(!inList)                                                         //-если путь прямой, то прибавляем поток
                {
                    graph[*way_iter].forwardEdge[*(way_iter + 1)].second += minFlow;
#ifdef debug
                    std::cout << "flow of " << *way_iter<< "->" << *(way_iter+1)  << " = " << graph[*way_iter].forwardEdge[*(way_iter + 1)].second << std::endl;
#endif
                    bool hasThatNeighbor = false;
                    for(auto& backNeighbor: graph[*(way_iter + 1)].backEdge)        //-проверка на наличие
                    {                                                               //вершины в множестве обратных путей
                        if(*way_iter == backNeighbor)                               //следующей вершины
                        {
                            hasThatNeighbor = true;
                            break;
                        }
                    }
                    if(!hasThatNeighbor)
                        graph[*(way_iter + 1)].backEdge.push_back(*way_iter);

                    if(flag && backTop && graph[*way_iter].forwardEdge[*(way_iter + 1)].second == graph[*way_iter].forwardEdge[*(way_iter + 1)].first)
                    {
                        minTop = *way_iter;
                        flag = false;
                    }
                } else                                                              //-из обратного ребра вычитаем
                {                                                                   //минимальный поток
                    graph[*(way_iter + 1)].forwardEdge[*way_iter].second -= minFlow;
                    minTop = *way_iter;
                    backTop = false;
#ifdef debug
                    std::cout << "flow of " << *(way_iter+1)<< "->" << *way_iter << " = " << graph[*way_iter].forwardEdge[*(way_iter + 1)].second << std::endl;
#endif
                }
            }
#ifdef debug
            std::cout << "go back" << std::endl;
#endif
            while(way.back() != minTop)                               //-откатываемся назад до вершины,
            {                                                         // из которой выходит наименьший поток
#ifdef debug
                std::cout << "pop " << way.back() << std::endl;
#endif
                if(!backNeighborList.empty() && way.back() == backNeighborList.back())
                    backNeighborList.pop_back();
                way.pop_back();
            }

        }
        return currentFlow;
    }

    void print()                        // -метод печати максимального потока и ребер графа
    {
        for(const auto& iter: graph) {
            for (const auto& next_iter: iter.second.forwardEdge)
                std::cout << iter.first << " " << next_iter.first << " " << next_iter.second.second << std::endl;

        }
    }

    void printPatch()                   // -метод печати пути
    {
        std::cout << "way: ";
        for(auto iter:way)
            std::cout << iter;
        std::cout << std::endl;
    }

};

int main() {
    Graph graph;
    std::cout << graph.searchFlow() << std::endl;
    graph.print();
    return 0;
}