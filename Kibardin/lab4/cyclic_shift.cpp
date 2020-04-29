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

int cyclicShift(std::string& text, std::string& shiftText)
{
    bool cycle = false;
    int result = -1;
    std::vector<int> prefix;
    if(text.size() != shiftText.size())                        //-выходим из функции, так как дилны строк не равны
    {
#ifdef debug
        std::cout << "strings have different lengths "<< std::endl;
#endif
        return -1;
    }
    prefix = prefix_function(text);                            //-вычисляем префикс-функцию
    for(int k = 0, l = 0; k < shiftText.size();)
        if(shiftText[k] == text[l])                            //-при нахождении одинаковых символов сдвигаем счетчики
        {
            if(result == -1)                                   //-если до текущего шага не было найдено совпадений, то
                result = k;                                    //записываем индекс первого символа в сдвиге
#ifdef debug
            std::cout << "found equal symbols, shiftText[" << k << "] = " << shiftText[k] << ", " << "text[" << l << "] = " << text[l] << std::endl;
#endif
            k++;
            l++;
            if(l == text.size())                               //-при достижении конца строки Т возвращаем индекс первого
            {                                                  //символа из Т в строке сдвига
#ifdef debug
                std::cout << "end text reached at k = " << k - 1 << std::endl;
                if(cycle)
                {
                    std::cout << "result with cycle: " << result << std::endl;
                } else
                {
                    std::cout << "result without cycle: " << result << std::endl;
                }
#endif
                return result;
            }
            if(k == shiftText.size())                       //-при достижении конца строки сдвига, обнуляем ее счетчик
            {
                k = 0;
                if(cycle)                                   //при повторном достижении конца строки завершаем работу
                    return -1;
                cycle = true;
#ifdef debug
                std::cout << "end shiftText reached , k = " << k << ", l = " << l << std::endl;
#endif
            }
        } else
        {
#ifdef debug
            std::cout << "found different symbol, shiftText[" << k << "] = " << shiftText[k] << ", " << "text[" << l << "] = " << text[l] << std::endl;
#endif
            result = -1;                                    //-при несовпадении символов, удаляем запись о нахождении
            //первого символа
            if(l == 0)                                      //-увеличение счетчика строки сдвига
            {
                k++;
#ifdef debug
                std::cout << "k++, k = " << k << ", l = " << l << std::endl;
#endif
            } else
            {                                              //-присваиваем счетчику искомой строки предыдущее значение
                l = prefix[l-1];                           // префикс-функции
#ifdef debug
                std::cout << "shift l, k = " << k << ", l = " << l << std::endl;
#endif
            }
        }
#ifdef debug
    std::cout << "not a cycle shift " << std::endl;
#endif
    return result;
}

int main() {
    std::string shiftText, text;
    std::cin >> shiftText;
    std::cin >> text;
    std::cout <<  cyclicShift(text, shiftText);
    return 0;
}