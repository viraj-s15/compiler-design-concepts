#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

#define MAX 5 

char* input;
int i = 0;
char lasthandle[6], stack[50];
char handles[MAX][5];
//(E) becomes )E( when pushed to stack

int top = 0, l;
char prec[6][6] = {
    /*       +   *     i    (    )    $ 
    * + */ '>', '<', '<', '<', '>', '>',
    /* * */ '>','>', '<', '<', '>', '>',
    /* i */ '>','>', 'e', 'e', '>', '>',
    /* ( */ '<','<', '<', '<', '>', 'e',
    /* ) */ '>','>', 'e', 'e', '>', '>',
    /* $ */ '<','<', '<', '<', '<', '>',
};

void createHandles(char handles[MAX][5]) {
    string prod;
    ifstream f("production4.txt");
    int count = 0;
    while (getline(f,prod) && count < 5) {
        strcpy(handles[count],prod.c_str());  
        cout << handles[count]<< endl;
        count++;
    }
}

int getindex(char c)
{
    switch (c)
    {
    case '+':
        return 0;
    case '*':
        return 1;
    case 'i':
        return 2;
    case '(':
        return 3;
    case ')':
        return 4;
    case '$':
        return 5;
    }
    return -1;
}

int shift()
{
    stack[++top] = *(input + i++);
    stack[top + 1] = '\0';
    return 0;
}

int reduce()
{
    int len, found, t;
    for (int i = 0; i < 5; i++) // selecting handles
    {
        len = strlen(handles[i]);
        if (stack[top] == handles[i][0] && top + 1 >= len)
        {
            found = 1;
            for (t = 0; t < len; t++)
            {
                // cout << stack[top - t] << endl;
                //     cout << handles[i][t] << endl;
                if (stack[top - t] != (char)handles[i][t])
                {
                    found = 0;
                    break;
                }
            }
            if (found == 1)
            {
                stack[top - t + 1] = 'E';
                top = top - t + 1;
                strcpy(lasthandle, handles[i]);
                stack[top + 1] = '\0';
                return 1; // successful reduction
            }
        }
    }
    return 0;
}

void dispstack()
{
    for (int j = 0; j <= top; j++)
        cout << stack[j];
}

void dispinput()
{
    for (int j = i; j < l; j++)
        cout << *(input + j);
}

int main()
{
    createHandles(handles);
    input = new char[50];
    cout << "Enter the string: ";
    cin >> input;
    input = strcat(input, "$");
    l = strlen(input);
    strcpy(stack, "$");
    cout << "\nSTACK\tINPUT\tACTION";
    while (i <= l)
    {
        shift();
        cout << "\n";
        dispstack();
        cout << "\t";
        dispinput();
        cout << "\tShift";
        if (prec[getindex(stack[top])][getindex(input[i])] == '>')
        {
            while (reduce())
            {
                cout << "\n";
                dispstack();
                cout << "\t";
                dispinput();
                cout << "\tReduced: E->" << lasthandle;
            }
        }
    }
    if (strcmp(stack, "$E$") == 0)
        cout << "\nAccepted;\n";
    else
        cout << "\nNot Accepted;\n";
    delete[] input;
    return 0;
}
