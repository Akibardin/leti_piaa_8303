#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
/*
 * Вариант 2. Подсчитать количество вершин в автомате; вывести список найденных образцов, имеющих пересечения с другими найденными образцами в строке поиска.
*/
//#define debug
bool myCmp(std::pair<int, int> i, std::pair<int, int> j)
{
    return i.second < j.second;
}

struct Vertex {
    std::map<char,int> next;                // множество ребер бора
    std::map<char,int> go;                  // состояния автомата
    bool leaf;                              // флаг для проверки является ли вершина терминальной
    int parent;                             // индекс родительской вершины
    char parentChar;                        // символ родительской вершины
    int link;                               // суффиксная ссылка

    int patternNumber;                      // номер шаблона в терминальной вершине
};

class Trie
{
    std::vector<Vertex> vertices;           // контейнер для хранения бор
    std::vector<std::string> patterns;      // множество подстрок
    std::vector<std::pair<int,int>> result; // контейнер для записи результата
public:
    void init() {                           // иницализация бора
        Vertex head;
        head.parent = head.link = -1;
        head.leaf = false;
        vertices.push_back(head);
    }

    void addString (const std::string & s) {
#ifdef debug
        std::cout << "------------------\n";
#endif
#ifdef debug
        std::cout << "add new string in trie: " << s << std::endl;
#endif
        patterns.push_back(s);
        int v = 0;
        for (auto c : s) {
#ifdef debug
            std::cout << "symbol " << c << " \n";
#endif
            if (vertices[v].next.find(c) == vertices[v].next.end()) {           // если вершины в боре нет, добавляем
#ifdef debug
                std::cout << "add new vertex from " << v << "(" << c <<") \n";
#endif
                Vertex buffer;
                buffer.leaf = false;
                buffer.link = -1;
                buffer.parent = v;
                buffer.parentChar = c;

                vertices.push_back(buffer);
                vertices[v].next[c] = vertices.size() - 1;
            }
            v = vertices[v].next[c];
        }
        vertices[v].leaf = true;                                               // помечаем вершну как терминальную
        vertices[v].patternNumber = patterns.size() - 1;                       // добавляем индекс шаблона
    }

    int go (int v, char c) {                                                   // метод перехода в новое состояние
#ifdef debug
        std::cout << "------------------\n";
        std::cout << "current symbol: " << c << "\n";
#endif
        if (vertices[v].go.find(c) == vertices[v].go.end())                    // переход в новое состояние, при отсутствии
        {                                                                      // в множестве переходов текущего
            if (vertices[v].next.find(c) != vertices[v].next.end())
            {
                vertices[v].go[c] = vertices[v].next[c];

            }
            else
            {
                vertices[v].go[c] = v==0 ? 0 : go (getLink(v), c);         // переход по ссылке
#ifdef debug
                std::cout << "go through link: " << vertices[v].go[c] <<" \n";
#endif
            }
        }
#ifdef debug
        std::cout << "next state: " << vertices[v].go[c] <<" \n";
#endif
        return vertices[v].go[c];
    }

    int getLink (int v) {                                                   // метод получения суффиксной ссылки
#ifdef debug
        std::cout << "------------------\n";
#endif

        if (vertices[v].link == -1)                                         // при отстутствии сслыки вычисляем ее
        {
            if (v == 0 || vertices[v].parent == 0)                          // добавлем ссылку на корень
            {
#ifdef debug
                std::cout << "get link to root: " <<" \n";
#endif
                vertices[v].link = 0;
            }
            else                                                            // вычисление ссылки через родительскую вершину
            {
#ifdef debug
                std::cout << "get link through parent: " <<" \n";
#endif
                vertices[v].link = go (getLink(vertices[v].parent), vertices[v].parentChar);
            }
        }
#ifdef debug
        std::cout << "get link to state: " << vertices[v].link <<" \n";
#endif
        return vertices[v].link;
    }

    void ahoCorasick(std::string& text)
    {
#ifdef debug
        std::cout << "------------------\n";
#endif
        int state = 0;
        for(int i = 0; i < text.size();i++)
        {
            state = go(state, text[i]);                                                 // переход в новое состояние
#ifdef debug
            std::cout << "state: " << state <<" \n";
#endif
            for(size_t tmp = state; tmp != 0; tmp = getLink(tmp))                      // цикл для поиска всех возможных
            {                                                                          // вхождений
                if(vertices[tmp].leaf)
                {
                    std::pair<int,int> buffer;                                         // добавление в результат
                    buffer.first = vertices[tmp].patternNumber;                        // найденного шаблона и его
                    buffer.second = i - patterns[buffer.first].size();                 // позиции в тексте
                    result.push_back(buffer);
#ifdef debug
                    std::cout << "found pattern #" << buffer.second << ": " << patterns[buffer.first] <<" \n";
#endif
#ifdef debug
                    std::cout << "------------------\n";
#endif
                }
            }
        }
    }

    void printResult()
    {
        std::cout << "result: " <<std::endl;
        std::sort(result.begin(), result.end(), myCmp);
        for(auto iter : result)
        {
            std::cout << iter.second + 2 << " " << iter.first + 1 << std::endl;
        }
#ifdef debug
        std::cout << "#verties: " << vertices.size() << std::endl;
        for(size_t i = 0 ; i < result.size() - 1; i++) {
            for(size_t j = i + 1 ; j < result.size(); j++)
                {
                    size_t first, second;
                    first = patterns[result[i].first].size() - 1 + result[i].second;
                    second =  result[j].second;
                    if(first >= second)
                    {
                        std::cout << "crossing of patterns " << patterns[result[i].first] << " and " << patterns[result[j].first] << ". #" << result[i].first + 1<< ",#" << result[j].first + 1 << " at positions " << result[i].second + 2 << "," << result[j].second + 2 << "\n";
                    }
                }
        }
#endif
    }
};

int main() {
    //std::freopen("test", "r", stdin);
    Trie trie;
    trie.init();
    std::string text, pattern;
    int n = 0;
    std::cin >> text;
    std::cin >> n;
    for(int i = 0; i < n ;i++)
    {
        std::cin >> pattern;
        trie.addString(pattern);
    }

    trie.ahoCorasick(text);
    trie.printResult();

    return 0;
}
