#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include "json.hpp"
#include "CPU.h"
#include "Memory.h"
#include <list>
#include "MockCart.h"

using json = nlohmann::json;

MockCart mockCart(32768);
Memory *mem;
CPU *cpu;
int tests_run = 0;
int failures = 0;
std::vector<std::string> names = {"PC", "SP", "AF", "BC", "DE", "HL"};

bool ends_with_json(char *filename) {
  return strlen(filename) > 5
    && !strcmp(filename + strlen(filename) - 5, ".json");
}

std::list<std::string> get_tests() {
  std::list<std::string> tests;

  DIR *d;
  struct dirent *dir;
  d = opendir("tests");
  if(d) {
    while((dir = readdir(d)) != 0) {
      if(ends_with_json(dir->d_name)) {
	tests.push_back(dir->d_name);
      }
    }
    closedir(d);
  }

  return tests;
}

json read_test(std::string file) {
  std::ifstream infile("tests/" + file);
  json j;
  infile >> j;
  return j;
}

uint8_t get_nibble_value(char c) {
  if(c >= '0' && c <= '9') {
    return c - '0';
  } else if(c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else {
    return c - 'A' + 10;
  }
}
uint8_t get_byte_value(std::string b) {
  return get_nibble_value(b[0]) * 16 + get_nibble_value(b[1]);
}

uint16_t get_word_value(std::string w) {
  return get_nibble_value(w[0]) * 16 * 16 * 16
    + get_nibble_value(w[1]) * 16 * 16
    + get_nibble_value(w[2]) * 16
    + get_nibble_value(w[3]);
}

void insert_program(json &test) {
  for(int i = 0;i < test["program"].size();i++) {
    std::string code = test["program"][i];
    uint8_t byte = get_byte_value(code);
    mockCart.putByte(0x100 + i, byte);
  }
  mem->insertCart(&mockCart);
}

void enter_pre(json &test) {
  for(auto name : names) {
    if(test["pre"].find(name) != test["pre"].end()) {
      cpu->set(name, get_word_value(test["pre"][name]));
    }
  }
}

void print_word(uint16_t val) {
  std::cout << std::setw(4) << std::setfill('0')
	    << std::uppercase << std::hex
	    << val;
}

bool test_succeeded(json &test) {
  bool success = true;

  for(auto name : names) {
    if(test["post"].find(name) != test["post"].end()) {
      uint16_t expected = get_word_value(test["post"][name]);
      if(expected != cpu->get(name)) {
	std::cout << "Actual " << name << ": ";
	print_word(cpu->get(name));
	std::cout << ", expected " << name << ": ";
	print_word(expected);
	std::cout << std::endl;
	success = false;
      }
    }
  }

  return success;
}

void run_test(std::string file) {
  json test = read_test(file);
  insert_program(test);
  cpu->reset();
  enter_pre(test);
  for(int i = 0;i < test["steps"];i++) {
    cpu->step();
  }
  tests_run++;
  if(!test_succeeded(test)) {
    std::cout << "Failure in file: " << file << std::endl;
    failures++;
  }
}

int main() {
  mem = new Memory();
  cpu = new CPU(mem);

  std::list<std::string> test_files = get_tests();

  for(const std::string& test : test_files) {
    run_test(test);
  }

  std::cout << (tests_run - failures) << "/" << tests_run
	    << " tests passed" << std::endl;
  
  if(failures > 0) {
    return 1;
  }
  
  return 0;
}
