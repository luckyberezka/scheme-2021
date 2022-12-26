#pragma once

#include "tokenizer.h"

#include <memory>
#include <map>
#include <vector>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;

    virtual std::string TakeStringValue(){};

    virtual std::string Inside(){};

    virtual std::shared_ptr<Object> Calculate() {
        throw RuntimeError("");
    }
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (dynamic_cast<T*>(obj.get())) {
        return true;
    }
    return false;
}

class Number : public Object {
public:
    Number(ConstantToken now) : mean_(now.value) {
    }

    Number(int now) : mean_(now) {
    }

    int GetValue() const {
        return mean_;
    }
    std::string TakeStringValue() override {
        return std::to_string(mean_);
    }

    std::string Inside() override {
        return std::to_string(mean_);
    }
    std::shared_ptr<Object> Calculate() override {
        return shared_from_this();
    }

private:
    ConstantToken value_;
    int mean_;
};

class Symbol : public Object {
public:
    Symbol(Token now) {
        if (const SymbolToken* btw1 = std::get_if<SymbolToken>(&now)) {
            str_ = btw1->name;
            value_ = *btw1;
        } else if (const DotToken* btw3 = std::get_if<DotToken>(&now)) {
            str_ = ".";
            value_ = *btw3;
        }
    }

    Symbol(std::string str) : str_(str){};

    const std::string& GetName() const {
        return str_;
    }

    std::string TakeStringValue() override {
        return str_;
    }

    std::string Inside() override {
        return str_;
    }

    std::shared_ptr<Object> Calculate() override {
        return shared_from_this();
    }

private:
    std::variant<SymbolToken, QuoteToken, DotToken> value_;
    std::string str_;
};

class SymbolDot : public Object {
public:
    SymbolDot(DotToken btw) {
        str_ = ".";
        value_ = btw;
    }

    std::string TakeStringValue() override {
        return ".";
    }

    std::string Inside() override {
        return str_;
    }

private:
    DotToken value_;
    std::string str_;
};

class SymbolQuote : public Object {
public:
    SymbolQuote(QuoteToken btw) {
        str_ = "\'";
        value_ = btw;
    }

    std::string TakeStringValue() override {
        return str_;
    }

    std::string Inside() override {
        return str_;
    }

private:
    QuoteToken value_;
    std::string str_;
};

class SymbolBracket : public Object {
public:
    SymbolBracket(BracketToken btw) {
        if (btw == BracketToken::OPEN) {
            str_ = "(";
            value_ = btw;
        } else if (btw == BracketToken::CLOSE) {
            str_ = ")";
            value_ = btw;
        }
    }

    std::string TakeStringValue() override {
        return str_;
    }

    std::string Inside() override {
        return str_;
    }

private:
    BracketToken value_;
    std::string str_;
};

std::vector<std::shared_ptr<Object>> TakeElem(std::shared_ptr<Object> args_head);

template <class T>
void TypeChecker(std::vector<std::shared_ptr<Object>> now_list);

class Function : public Object {
public:
    virtual ~Function() = default;
    virtual std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) = 0;
};

class IsNumber : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Equality : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class SignMore : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class SignLess : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class SignME : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class SignLE : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Plus : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Minus : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Multiplication : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Devided : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Maximum : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Modul : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Minimum : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Quote : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class IsBool : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Not : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class And : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Or : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class IsNull : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Liist : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class ListRef : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class ListTail : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Car : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Cdr : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Cons : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Papair : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class IsList : public Function {
public:
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object>& args_head) override;
};

class Cell : public Object {
public:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
    std::map<std::string, std::shared_ptr<Function>> list_of_func;

    Cell() {
        first_ = nullptr;
        second_ = nullptr;
        list_of_func["number?"] = std::shared_ptr<Function>(new IsNumber());
        list_of_func["="] = std::shared_ptr<Function>(new Equality());
        list_of_func[">"] = std::shared_ptr<Function>(new SignMore());
        list_of_func["<"] = std::shared_ptr<Function>(new SignLess());
        list_of_func[">="] = std::shared_ptr<Function>(new SignME());
        list_of_func["<="] = std::shared_ptr<Function>(new SignLE());
        list_of_func["+"] = std::shared_ptr<Function>(new Plus());
        list_of_func["-"] = std::shared_ptr<Function>(new Minus());
        list_of_func["*"] = std::shared_ptr<Function>(new Multiplication());
        list_of_func["/"] = std::shared_ptr<Function>(new Devided());
        list_of_func["max"] = std::shared_ptr<Function>(new Maximum());
        list_of_func["min"] = std::shared_ptr<Function>(new Minimum());
        list_of_func["abs"] = std::shared_ptr<Function>(new Modul());
        list_of_func["'"] = std::shared_ptr<Function>(new Quote());
        list_of_func["quote"] = std::shared_ptr<Function>(new Quote());
        list_of_func["boolean?"] = std::shared_ptr<Function>(new IsBool());
        list_of_func["not"] = std::shared_ptr<Function>(new Not());
        list_of_func["and"] = std::shared_ptr<Function>(new And());
        list_of_func["or"] = std::shared_ptr<Function>(new Or());
        list_of_func["null?"] = std::shared_ptr<Function>(new IsNull());
        list_of_func["list"] = std::shared_ptr<Function>(new Liist());
        list_of_func["list-ref"] = std::shared_ptr<Function>(new ListRef());
        list_of_func["list-tail"] = std::shared_ptr<Function>(new ListTail());
        list_of_func["car"] = std::shared_ptr<Function>(new Car());
        list_of_func["cdr"] = std::shared_ptr<Function>(new Cdr());
        list_of_func["cons"] = std::shared_ptr<Function>(new Cons());
        list_of_func["pair?"] = std::shared_ptr<Function>(new Papair());
        list_of_func["list?"] = std::shared_ptr<Function>(new IsList());
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    std::string Inside() override {
        std::string str;
        if (first_ == nullptr && second_ == nullptr) {
            str = "()";
            return str;
        }
        if (Is<Cell>(first_)) {
            str += first_->Inside();
        } else {
            if (first_ == nullptr) {
                str += "()";
            } else {
                str += first_->TakeStringValue();
            }
        }
        if (second_ != nullptr) {
            if (Is<Cell>(second_)) {
                if (!(As<Cell>(second_)->first_ == nullptr &&
                      As<Cell>(second_)->second_ == nullptr)) {
                    str += " ";
                    str += second_->Inside();
                }
            } else {
                str += " . ";
                str += second_->TakeStringValue();
            }
        }
        return str;
    }

    std::string TakeStringValue() override {
        std::string res;
        res.push_back('(');
        res += Inside();
        res.push_back(')');
        return res;
    }

    std::shared_ptr<Object> Calculate() override {
        if (first_ == nullptr) {
            throw RuntimeError("");
        }
        if (list_of_func.find(first_->TakeStringValue()) == list_of_func.end()) {
            throw RuntimeError("");
        }
        return list_of_func[first_->TakeStringValue()]->Apply(second_);  // ...
    }
};
