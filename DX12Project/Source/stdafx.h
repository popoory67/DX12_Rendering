#pragma once

#include <assert.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <wrl.h>
#include <optional>
#include <any>
#include <typeindex>
#include <utility>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "D3DUtil.h"
#include "Util.h"