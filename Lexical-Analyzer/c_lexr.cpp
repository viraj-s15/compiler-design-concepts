#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// All the libraries needed for reading from a file and using the string compare function have been included
// Only these libraries have been used, no external function has been used to help the code
// all the keywords of c have been included

using namespace std;

int is_keyword(char file_buffer[])
{
    char keywords[32][10] = {"auto", "break", "case", "char", "const", "continue", "default",
                             "do", "double", "else", "enum", "extern", "float", "for", "goto",
                             "if", "int", "long", "register", "return", "short", "signed",
                             "sizeof", "static", "struct", "switch", "typedef", "union",
                             "unsigned", "void", "volatile", "while"};
    int i, flag = 0;
    for (i = 0; i < 32; ++i)
    {
        if (strcmp(keywords[i], file_buffer) == 0)
        {
            flag = 1;
            break;
        }
    }
    return flag;
}

int main()
{
    char ch, file_buffer[15], operator_array[] = "+-*/%=";
    ifstream fin("lab1.txt");
    int i, j = 0;
    if (!fin.is_open())
    {
        cout << "There was an issue with opening the file, this is probably because the file does not exist\n";
        exit(0);
    }
    while (!fin.eof())
    {
        ch = fin.get();

        for (i = 0; i < 6; ++i)
        {
            if (ch == operator_array[i])
                cout << ch << " -> operator\n";
        }

        if (isalnum(ch))
        {
            file_buffer[j++] = ch;
        }
        else if ((ch == ' ' || ch == '\n') && (j != 0))
        {
            file_buffer[j] = '\0';
            j = 0;

            if (is_keyword(file_buffer) == 1)
                cout << file_buffer << " -> keyword\n";
            else
                cout << file_buffer << " -> indentifier\n";
        }
    }
    fin.close();
    return 0;
}

