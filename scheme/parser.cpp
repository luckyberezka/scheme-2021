#include "parser.h"
#include "object.h"

#include <vector>

std::shared_ptr<Object> ReadClone(Tokenizer* tokenizer) {
    Token now_token = tokenizer->GetToken();
    tokenizer->Next();
    Token next_token = tokenizer->GetToken();
    if (std::get_if<BracketToken>(&now_token)) {
        if (*std::get_if<BracketToken>(&now_token) == BracketToken::OPEN) {
            return ReadList(tokenizer);
        } else {
            return std::shared_ptr<Object>(
                new SymbolBracket(*std::get_if<BracketToken>(&now_token)));
        }
    } else if (std::get_if<ConstantToken>(&now_token)) {
        return std::shared_ptr<Object>(new Number(*std::get_if<ConstantToken>(&now_token)));
    } else if (std::get_if<QuoteToken>(&now_token)) {
        if (tokenizer->IsEnd()) {
            return std::shared_ptr<Object>(new SymbolQuote(*std::get_if<QuoteToken>(&now_token)));
        } else {
            std::shared_ptr<Cell> raduga(new Cell);
            raduga->first_ =
                std::shared_ptr<Object>(new SymbolQuote(*std::get_if<QuoteToken>(&now_token)));
            raduga->second_ = ReadClone(tokenizer);
            return raduga;
        }
    } else if (std::get_if<DotToken>(&now_token)) {
        if (std::get_if<BracketToken>(&next_token)) {
            if (*std::get_if<BracketToken>(&next_token) == BracketToken::OPEN) {
                return ReadClone(tokenizer);
            }
        }
        return std::shared_ptr<Object>(new SymbolDot(*std::get_if<DotToken>(&now_token)));
    } else {
        return std::shared_ptr<Object>(new Symbol(now_token));
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    std::vector<std::shared_ptr<Object>> v;
    std::shared_ptr<Object> now_object = nullptr;
    while (true) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError("");
        }
        now_object = ReadClone(tokenizer);
        if (Is<SymbolBracket>(now_object)) {
            break;
        } else {
            v.push_back(now_object);
        }
    }
    if (v.empty()) {
        return nullptr;
    }
    if (v.size() == 1) {
        if (!Is<SymbolDot>(v[0])) {
            std::shared_ptr<Cell> res(new Cell);
            res->first_ = v[0];
            res->second_ = nullptr;
            return res;
        } else {
            throw SyntaxError("");
        }
    }
    if (v.size() == 2) {
        if (!Is<SymbolDot>(v[0]) && !Is<SymbolDot>(v[1])) {
            std::shared_ptr<Cell> res_btw(new Cell);
            res_btw->first_ = v[1];
            res_btw->second_ = nullptr;
            std::shared_ptr<Cell> res(new Cell);
            res->first_ = v[0];
            res->second_ = res_btw;
            return res;
        } else {
            throw SyntaxError("");
        }
    }
    int sz = v.size();
    for (int i = 0; i < sz; ++i) {
        if (Is<SymbolDot>(v[i]) && i != sz - 2) {
            throw SyntaxError("");
        }
    }
    int index = sz - 1;
    std::shared_ptr<Cell> last(new Cell);
    if (Is<SymbolDot>(v[sz - 2])) {
        last->second_ = v[sz - 1];
        last->first_ = v[sz - 3];
        index = sz - 4;
    } else {
        last = nullptr;
    }
    for (int i = index; i >= 0; --i) {
        std::shared_ptr<Cell> new_last(new Cell);
        new_last->second_ = last;
        new_last->first_ = v[i];
        last = new_last;
    }
    return last;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("");
    }
    std::shared_ptr<Object> res = ReadClone(tokenizer);
    if (!tokenizer->IsEnd()) {
        throw SyntaxError("");
    }
    if (Is<SymbolQuote>(res)) {
        throw SyntaxError("");
    }
    return res;
}

// KOMMEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEENT

template <class T>
void TypeChecker(std::vector<std::shared_ptr<Object>> now_list) {
    for (size_t i = 0; i < now_list.size(); ++i) {
        if (!Is<T>(now_list[i])) {
            throw RuntimeError("");
        }
    }
}

std::vector<std::shared_ptr<Object>> TakeElem(std::shared_ptr<Object> args_head) {
    std::vector<std::shared_ptr<Object>> res;
    if (args_head == nullptr) {
        return res;
    }
    if (Is<Cell>(args_head)) {
        std::shared_ptr<Cell> now_cell = As<Cell>(args_head);
        if (now_cell->first_ == nullptr && now_cell->second_ == nullptr) {
            return res;
        }
        if (now_cell->first_ != nullptr) {
            res.push_back(now_cell->first_->Calculate());
        }
        if (now_cell->second_ == nullptr) {
            return res;
        }

        std::vector<std::shared_ptr<Object>> dop_res = TakeElem(now_cell->second_);
        for (auto i : dop_res) {
            res.push_back(i);
        }
    } else {
        res.push_back(args_head);
    }
    return res;
}

std::shared_ptr<Object> IsNumber::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() != 1) {
        throw RuntimeError("");
    }
    if (Is<Number>(elems[0])) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Equality::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() == 0) {
        return std::make_shared<Symbol>("#t");
    }
    TypeChecker<Number>(elems);
    int size_of_elems = elems.size();
    for (int i = 0; i < size_of_elems - 1; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        std::shared_ptr<Number> second_number = As<Number>(elems[i + 1]);
        if (first_number->GetValue() != second_number->GetValue()) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> SignMore::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() == 0) {
        return std::make_shared<Symbol>("#t");
    }
    TypeChecker<Number>(elems);
    int size_of_elems = elems.size();
    for (int i = 0; i < size_of_elems - 1; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        std::shared_ptr<Number> second_number = As<Number>(elems[i + 1]);
        if (first_number->GetValue() <= second_number->GetValue()) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> SignLess::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() == 0) {
        return std::make_shared<Symbol>("#t");
    }
    TypeChecker<Number>(elems);
    int size_of_elems = elems.size();
    for (int i = 0; i < size_of_elems - 1; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        std::shared_ptr<Number> second_number = As<Number>(elems[i + 1]);
        if (first_number->GetValue() >= second_number->GetValue()) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> SignME::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() == 0) {
        return std::make_shared<Symbol>("#t");
    }
    TypeChecker<Number>(elems);
    int size_of_elems = elems.size();
    for (int i = 0; i < size_of_elems - 1; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        std::shared_ptr<Number> second_number = As<Number>(elems[i + 1]);
        if (first_number->GetValue() < second_number->GetValue()) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> SignLE::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() == 0) {
        return std::make_shared<Symbol>("#t");
    }
    TypeChecker<Number>(elems);
    int size_of_elems = elems.size();
    for (int i = 0; i < size_of_elems - 1; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        std::shared_ptr<Number> second_number = As<Number>(elems[i + 1]);
        if (first_number->GetValue() > second_number->GetValue()) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Plus::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    TypeChecker<Number>(elems);
    if (Is<Cell>(args_head)) {
        std::shared_ptr<Cell> burunduk = As<Cell>(args_head);
        if (burunduk->first_ == nullptr && burunduk->second_ == nullptr) {
            throw RuntimeError("");
        }
    }
    int summa = 0;
    int size_of_elems = elems.size();
    for (int i = 0; i < size_of_elems; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        summa += first_number->GetValue();
    }
    return std::make_shared<Number>(summa);
}

std::shared_ptr<Object> Minus::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() == 0) {
        throw RuntimeError("");
    }
    TypeChecker<Number>(elems);
    int summa = As<Number>(elems[0])->GetValue();
    int size_of_elems = elems.size();
    for (int i = 1; i < size_of_elems; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        summa -= first_number->GetValue();
    }
    return std::make_shared<Number>(summa);
}

std::shared_ptr<Object> Multiplication::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    TypeChecker<Number>(elems);
    int summa = 1;
    int size_of_elems = elems.size();
    for (int i = 0; i < size_of_elems; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        summa *= first_number->GetValue();
    }
    return std::make_shared<Number>(summa);
}

std::shared_ptr<Object> Devided::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    TypeChecker<Number>(elems);
    if (elems.size() == 0) {
        throw RuntimeError("");
    }
    int summa = As<Number>(elems[0])->GetValue();
    int size_of_elems = elems.size();
    for (int i = 1; i < size_of_elems; ++i) {
        std::shared_ptr<Number> first_number = As<Number>(elems[i]);
        summa /= first_number->GetValue();
    }
    return std::make_shared<Number>(summa);
}

std::shared_ptr<Object> Maximum::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() < 1) {
        throw RuntimeError("");
    }
    TypeChecker<Number>(elems);
    std::shared_ptr<Number> first_number = As<Number>(elems[0]);
    int maxim = first_number->GetValue();
    int size_of_elems = elems.size();
    for (int i = 1; i < size_of_elems; ++i) {
        maxim = std::max(As<Number>(elems[i])->GetValue(), maxim);
    }
    return std::make_shared<Number>(maxim);
}

std::shared_ptr<Object> Minimum::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() < 1) {
        throw RuntimeError("");
    }
    TypeChecker<Number>(elems);
    std::shared_ptr<Number> first_number = As<Number>(elems[0]);
    int maxim = first_number->GetValue();
    int size_of_elems = elems.size();
    for (int i = 1; i < size_of_elems; ++i) {
        maxim = std::min(As<Number>(elems[i])->GetValue(), maxim);
    }
    return std::make_shared<Number>(maxim);
}

std::shared_ptr<Object> Modul::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    TypeChecker<Number>(elems);
    if (elems.size() != 1) {
        throw RuntimeError("");
    }
    return std::make_shared<Number>(abs(As<Number>(elems[0])->GetValue()));
}

std::shared_ptr<Object> Quote::Apply(const std::shared_ptr<Object>& args_head) {
    if (args_head == nullptr) {
        return std::make_shared<Symbol>("()");
    }
    return args_head;
}

std::shared_ptr<Object> IsBool::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() != 1) {
        throw RuntimeError("");
    }
    if (Is<Symbol>(elems[0])) {
        if (elems[0]->TakeStringValue() == "#t" || elems[0]->TakeStringValue() == "#f") {
            return std::make_shared<Symbol>("#t");
        } else {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Not::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() != 1) {
        throw RuntimeError("");
    }
    if (Is<Symbol>(elems[0])) {
        if (elems[0]->TakeStringValue() == "#t") {
            return std::make_shared<Symbol>("#f");
        } else if (elems[0]->TakeStringValue() == "#f") {
            return std::make_shared<Symbol>("#t");
        } else {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> And::Apply(const std::shared_ptr<Object>& args_head) {
    if (args_head == nullptr) {
        return std::make_shared<Symbol>("#t");
    }
    if (Is<Symbol>(args_head) && args_head->TakeStringValue() == "#f") {
        return std::make_shared<Symbol>("#f");
    }
    if (As<Symbol>(args_head) || As<Number>(args_head)) {
        return args_head;
    }
    if (Is<Cell>(args_head)) {
        std::shared_ptr<Cell> now_cell = As<Cell>(args_head);
        if (now_cell->second_ == nullptr) {
            return now_cell->first_->Calculate();
        }
        if (now_cell->first_->Calculate()->TakeStringValue() == "#f") {
            return std::make_shared<Symbol>("#f");
        } else {
            return And::Apply(now_cell->second_);
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Or::Apply(const std::shared_ptr<Object>& args_head) {
    if (args_head == nullptr) {
        return std::make_shared<Symbol>("#f");
    }
    if (Is<Symbol>(args_head) && args_head->TakeStringValue() == "#t") {
        return std::make_shared<Symbol>("#t");
    }
    if (As<Symbol>(args_head) || As<Number>(args_head)) {
        return args_head;
    }
    if (Is<Cell>(args_head)) {
        std::shared_ptr<Cell> now_cell = As<Cell>(args_head);
        if (now_cell->second_ == nullptr) {
            return now_cell->first_->Calculate();
        }
        if (now_cell->first_->Calculate()->TakeStringValue() == "#t") {
            return std::make_shared<Symbol>("#t");
        } else {
            return Or::Apply(now_cell->second_);
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> IsNull::Apply(const std::shared_ptr<Object>& args_head) {
    if (!Is<Cell>(args_head)) {
        throw RuntimeError("");
    }
    std::shared_ptr<Object> karakatica = As<Cell>(args_head)->first_->Calculate();
    if (karakatica == nullptr ||
        (Is<Cell>(karakatica) && As<Cell>(karakatica)->first_ == nullptr &&
         As<Cell>(karakatica)->second_ == nullptr) ||
        (Is<Symbol>(karakatica) && As<Symbol>(karakatica)->TakeStringValue() == "()")) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Liist::Apply(const std::shared_ptr<Object>& args_head) {
    if (args_head == nullptr) {
        return std::make_shared<Symbol>("()");
    }
    return args_head;
}

std::shared_ptr<Object> ListRef::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    std::vector<std::shared_ptr<Object>> in_elems = TakeElem(elems[0]);
    size_t n = As<Number>(elems[1])->GetValue();
    if (n >= in_elems.size()) {
        throw RuntimeError("");
    }
    return in_elems[n];
}

std::shared_ptr<Object> ListTail::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    std::vector<std::shared_ptr<Object>> in_elems = TakeElem(elems[0]);
    size_t n = As<Number>(elems[1])->GetValue();
    if (n > in_elems.size()) {
        throw RuntimeError("");
    }
    size_t counter = 0;
    std::shared_ptr<Object> result = elems[0];
    while (counter != n) {
        result = As<Cell>(result)->second_;
        ++counter;
    }
    if (result == nullptr) {
        return std::make_shared<Symbol>("()");  // СОМНИТЕЛЬНЫЙ КОСТЫЛЬ!
    }
    return result;
}

std::shared_ptr<Object> Car::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems[0] == nullptr || Is<Symbol>(elems[0])) {
        throw RuntimeError("");
    }
    if (Is<Cell>(elems[0])) {
        return As<Cell>(elems[0])->first_;
    } else {
        return elems[0];
    }
}

std::shared_ptr<Object> Cdr::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (args_head == nullptr || Is<Symbol>(elems[0])) {
        throw RuntimeError("");
    }
    if (Is<Cell>(elems[0])) {
        if (As<Cell>(elems[0])->second_ != nullptr) {
            return As<Cell>(elems[0])->second_;
        } else {
            return std::make_shared<Symbol>("()");
        }
    }
    return std::make_shared<Symbol>("()");
}

std::shared_ptr<Object> Cons::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems.size() != 2) {
        throw RuntimeError("");
    }
    std::shared_ptr<Cell> result = std::make_shared<Cell>(Cell());
    result->first_ = elems[0];
    result->second_ = elems[1];
    return result;
}

std::shared_ptr<Object> Papair::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    elems = TakeElem(elems[0]);
    if (elems.size() != 2) {
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> IsList::Apply(const std::shared_ptr<Object>& args_head) {
    std::vector<std::shared_ptr<Object>> elems = TakeElem(args_head);
    if (elems[0] == nullptr ||
        (Is<Symbol>(elems[0]) && As<Symbol>(elems[0])->TakeStringValue() == "()")) {
        return std::make_shared<Symbol>("#t");
    }
    std::shared_ptr<Object> now_ob = elems[0];
    while (Is<Cell>(now_ob)) {
        now_ob = As<Cell>(now_ob)->second_;
    }
    if (now_ob != nullptr) {
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#t");
}