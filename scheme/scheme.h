#pragma once

#include "tokenizer.h"
#include "object.h"
#include "parser.h"

#include <istream>
#include <string>
#include <memory>
#include <sstream>

class Interpreter {
public:
    Interpreter() = default;

    std::string Run(const std::string& now) {
        std::stringstream ss{now};
        Tokenizer tknzr{&ss};
        std::shared_ptr<Object> lst = Read(&tknzr);
        if (lst == nullptr) {
            throw RuntimeError("");
        }
        std::shared_ptr<Object> res = lst->Calculate();
        return res->TakeStringValue();
    }
};