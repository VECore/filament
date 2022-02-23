/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "details/DebugRegistry.h"

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>

#ifndef NDEBUG
#   define DEBUG_PROPERTIES_WRITABLE true
#else
#   define DEBUG_PROPERTIES_WRITABLE false
#endif

using namespace filament::math;
using namespace utils;

namespace filament {

FDebugRegistry::FDebugRegistry() noexcept = default;

UTILS_NOINLINE
void *FDebugRegistry::getPropertyAddress(const char *name) noexcept {
    StaticString key = StaticString::make(name, strlen(name));
    auto &propertyMap = mPropertyMap;
    if (propertyMap.find(key) == propertyMap.end()) {
        return nullptr;
    }
    return propertyMap[key];
}

void FDebugRegistry::registerProperty(utils::StaticString name, void *p, Type type) noexcept {
    auto& propertyMap = mPropertyMap;
    if (propertyMap.find(name) == propertyMap.end()) {
        propertyMap[name] = p;
    }
}

UTILS_NOINLINE
inline bool FDebugRegistry::hasProperty(const char *name) const noexcept {
    return const_cast<FDebugRegistry *>(this)->getPropertyAddress(name) != nullptr;
}

template<typename T>
inline bool FDebugRegistry::setProperty(const char *name, T v) noexcept {
    if constexpr (DEBUG_PROPERTIES_WRITABLE) {
        T * const addr = static_cast<T *>(getPropertyAddress(name));
        if (addr) {
            *addr = v;
            return true;
        }
    }
    return false;
}

template <typename T>
inline bool FDebugRegistry::getProperty(const char* name, T* UTILS_RESTRICT p) const noexcept {
    T const * const addr = static_cast<T *>(const_cast<FDebugRegistry *>(this)->getPropertyAddress(name));
    if (addr) {
        *p = *addr;
        return true;
    }
    return false;
}

void FDebugRegistry::registerDataSource(StaticString name, void const* data, size_t count) noexcept {
    auto& dataSourceMap = mDataSourceMap;
    if (dataSourceMap.find(name) == dataSourceMap.end()) {
        dataSourceMap[name] = { data, count };
    }
}

UTILS_NOINLINE
DebugRegistry::DataSource FDebugRegistry::getDataSource(const char* name) const noexcept {
    StaticString key = StaticString::make(name, strlen(name));
    auto &dataSourceMap = mDataSourceMap;
    auto const& it = dataSourceMap.find(key);
    if (it == dataSourceMap.end()) {
        return { nullptr, 0u };
    }
    return it->second;
}

} // namespace filament

