#pragma once
#include "IRenderer.h"
#include <memory>

std::unique_ptr<IRenderer> CreateRenderer(API api);

