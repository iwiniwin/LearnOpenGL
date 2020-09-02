#pragma once
#include "shader.h"

unsigned int  glCheckError_(const char* file, int line);

// __FILE__, __LINE__预处理指令，会在编译时被替换为对应的文件与行号
#define glCheckError() glCheckError_(__FILE__, __LINE__)