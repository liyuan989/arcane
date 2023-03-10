#ifndef ARCANE_THREAD_UTILS_H
#define ARCANE_THREAD_UTILS_H

#include <stdint.h>
#include <string>

namespace arcane {

int64_t GetTid();

std::string GetTidString();

} // namespace arcane

#endif

