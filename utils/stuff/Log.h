#pragma once
#include <iostream>

#ifdef DEBUG
#define LOG(s) std::cout << s << std::endl;
#else
#define LOG(s)
#endif
