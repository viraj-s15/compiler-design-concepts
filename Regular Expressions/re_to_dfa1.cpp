#include <iostream>

int state = 0;

void start(char c) {
  switch (c) {
  case '0':
    state = 1;
    break;
  case '1':
    state = 2;
    break;
  default:
    state = -1;
  }
}
void state1(char c) {
  switch (c) {
  case '0':
    state = 0;
    break;
  case '1':
    state = 3;
    break;
  default:
    state = -1;
  }
}
void state2(char c) {
  switch (c) {
  case '0':
    state = 3;
    break;
  case '1':
    state = 0;
    break;
  default:
    state = -1;
  }
}

void state3(char c) {
  switch (c) {
  case '0':
    state = 2;
    break;
  case '1':
    state = 1;
    break;
  default:
    state = -1;
  }
}

int dfa(std::string input) {
  int str_len = input.length();
  if (input[0] != '0' and input[1] != '1') {
    std::cout << "The entered symbols are incorrect\n";
    return 0;
  }

  for (int i = 0; i < str_len; i++) {
    switch (state) {
    case 0:
      start(input[i]);
      break;
    case 1:
      state1(input[i]);
      break;
    case 2:
      state2(input[i]);
      break;
    case 3:
      state3(input[i]);
      break;
    default:
      return 0;
    }
  }

  if (state == 3)
    return 1;
  return 0;
}

int main() {
  std::string input;
  std::cin >> input;
  int solution = dfa(input);
  if (solution == 1)
    std::cout << "The string has been accepted\n";
  else
    std::cout << "The string has not been accepted\n";
  return 0;
}
