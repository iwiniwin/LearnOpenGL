#pragma once
#include "shader.h"

unsigned int  glCheckError_(const char* file, int line);

// __FILE__, __LINE__Ԥ����ָ����ڱ���ʱ���滻Ϊ��Ӧ���ļ����к�
#define glCheckError() glCheckError_(__FILE__, __LINE__)