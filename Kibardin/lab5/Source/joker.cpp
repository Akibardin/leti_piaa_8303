#include <iostream>
#include <map>
#include <vector>
//#define debug

struct Vertex {
    std::map<char,int> next;                // множество ребер бора
    std::map<char,int> go;                  // состояния автомата
    bool leaf;                              // флаг для проверки является ли вершина терминальной
    int parent;                             // индекс родительской вершины
    char parentChar;                        // символ родительской вершины
    int link;                               // суффиксная ссылка

    std::vector<int> listPatterns;          // множество индексов шаблонов
};

class Trie
{
    std::vector<Vertex> vertices;           // контейнер для хранения бор
    std::vector<std::string> patterns;      // множество подстрок

    std::vector<int> C;                     // контейнер для записи результата
    std::vector<int> patternsIndex;         // индексы вхождения подстрок в строке с джокером
public:
    void init() {                           // иницализация бора
        Vertex head;
        head.parent = head.link = -1;
        head.leaf = false;
        vertices.push_back(head);
    }

    void addString (std::string s) {
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
        vertices[v].listPatterns.push_back(patterns.size() - 1);               // добавляем индекс паттерна
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

    void ahoCorasickJoker(std::string& text, std::string& pattern)
    {
#ifdef debug
        std::cout << "------------------\n";
#endif
        int state = 0;
        C.resize(text.size());                                                          // инициализация вектора результата
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
#ifdef debug
                    std::cout << "found pattern "  << std::endl;
#endif
                    for(auto Li : vertices[tmp].listPatterns)                          // цикл для вычисления индексов
                    {                                                                  // вхождений
                        int buffer = i + 1 - patterns[Li].size()- patternsIndex[Li];
                        if(buffer >=0 && buffer <= text.size() - pattern.size())
                        {
#ifdef debug
                            std::cout << "match at position: " << buffer << std::endl;
#endif
                            C[buffer]++;
                        }
                    }
#ifdef debug
                    std::cout << "------------------\n";
#endif
                }
            }
        }
    }

    void printResult(std::string pattern)
    {
#ifdef debug
        std::cout << "result:" << std::endl;
#endif
        for(size_t i = 0; i < C.size(); i++)
        {
                if(C[i] == patterns.size())
                std::cout << i + 1<< "\n";
        }
    }

    void splitString(std::string& pattern, char joker)                  //метод разбиения строки
    {
        size_t currentPos, prevPos;
        for(size_t i = 0; i < pattern.size() && currentPos != std::string::npos;)
        {
            std::string buffer;
            while(pattern[i] == joker) i++;                             // пропуск джокеров в строке
            prevPos = i;
            currentPos = pattern.find(joker, i);                        //поиск следующего джокера
            if(currentPos == std::string::npos)                         //создание подстроки
                buffer = pattern.substr(i, pattern.size() - i);
            else
                buffer = pattern.substr( prevPos,currentPos - prevPos);
            if(!buffer.empty())
            {
                patternsIndex.push_back(prevPos);                       // запись индекса подстроки и добавление ее в бор
                addString(buffer);
            }
            i = currentPos;
        }
    }
};

int main() {
    //std::freopen("test", "r", stdin);
    Trie trie;
    trie.init();
    std::string text, pattern;
    char joker;
    std::cin >> text;
    std::cin >> pattern;
    std::cin >> joker;

    trie.splitString(pattern, joker);
    trie.ahoCorasickJoker(text, pattern);
    trie.printResult(pattern);

    return 0;
}
