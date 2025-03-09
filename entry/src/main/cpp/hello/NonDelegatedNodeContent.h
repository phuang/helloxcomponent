//
// Created on 2025-03-08.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HELLOXCOMPONENT_NONDELEGATEDNODECONTENT_H
#define HELLOXCOMPONENT_NONDELEGATEDNODECONTENT_H

#include "hello/NodeContent.h"

namespace hello {

class NonDelegatedNodeContent : public NodeContent {};

} // namespace hello

#endif // HELLOXCOMPONENT_NONDELEGATEDNODECONTENT_H
