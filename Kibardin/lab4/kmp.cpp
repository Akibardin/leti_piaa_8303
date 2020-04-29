#include <iostream>
#include <vector>

#define debug

std::vector<int> prefix_function (std::string s) {
    int n =  s.length();
    std::vector<int> pi(n, 0);                             //-создаем вектор необходимого размера
    for (int i=1; i<n; ++i) {
        int j = pi[i-1];                                        //-вычисляем новое значение п-ф, про
        while (j > 0 && s[i] != s[j])                           //-проверяем до тех пор, пока не дойдем до 0, либо пока
            j = pi[j-1];                                        //символы не совпадут
        if (s[i] == s[j])                                       //-при совпадении символов увеличиваем п-ф на единицу
            ++j;
        pi[i] = j;
    }
#ifdef debug
    std::cout << "prefix function: ";
    for(auto iter : pi)
        std::cout << iter << " ";
    std::cout << std::endl;
#endif
    return pi;
}

std::vector<int> kmp(std::string& pattern, std::string& text)            //алгоритм кнута-морриса-пратта
{
    std::vector<int> result;
    std::vector<int> prefix;
    if(pattern.size() > text.size())                                     //проверка на корректность длины шаблона
    {
#ifdef debug
        std::cout << "length of text less than pattern" << std::endl;
#endif
        result.push_back(-1);
        return result;
    }
    prefix = prefix_function(pattern);                                //-задаем префикс-функцию
    for(int k = 0, l = 0; k < text.size();)
        if(text[k] == pattern[l])                                        //-при нахождении одинаковых символов сдвигаем счетчики
        {
#ifdef debug
            std::cout << "found equal symbols, pattern[" << k << "] = " << pattern[l] << ", " << "text[" << l << "] = " << text[k] << std::endl;
#endif
            k++;
            l++;
            if(l == pattern.size())                                   //-обнуляем счетчик шаблона, если он найден в тексте
            {
#ifdef debug
                std::cout << "found pattern at k = " <<  k - l << std::endl;
#endif
                result.push_back(k - l);
                l = 0;
            }
        } else
        {
#ifdef debug
            std::cout << "found different symbol, pattern[" << k << "] = " << pattern[l] << ", " << "text[" << l << "] = " << text[k] << std::endl;
#endif
            if(l == 0)                                        //-при несовпадении шаблона и текста с первого символа
            {                                                 //увеличиваем счетчик текста, иначе счетчику шаблона
                k++;                                          //присваиваем предыдущее значение префикс-функции
#ifdef debug
                std::cout << "k++, k = " << k << ", l = " << l << std::endl;
#endif
            } else
            {
                l = prefix[l-1];
#ifdef debug
                std::cout << "shift l, k = " << k << ", l = " << l << std::endl;
#endif
            }
        }
    if(result.empty())
    {
        result.push_back(-1);
    }
    return result;
}

int main() {
    std::vector<int> result;
    std::string P, T;
    std::cin >> P;
    std::cin >> T;
    result = kmp(P, T);
    for(auto iter = result.begin(); iter != result.end(); iter++)
    {
        std::cout << *iter;
        if(iter + 1 != result.end())
            std::cout << ",";
    }
    return 0;
}