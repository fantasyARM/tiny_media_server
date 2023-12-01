#pragma once
#include "common.h"
class MicroThread
{
public:
    constexpr int MICROTHERAD_STACK_SIZE = 1024 * 1024 * 4;
    explicit MicroThread() = default;
    virtual ~MicroThread() = default;
};
