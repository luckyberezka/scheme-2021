#pragma once

#include "error.h"

#include <variant>
#include <optional>
#include <istream>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) {
        in_ = in;
        Next();

    }

    bool IsEnd() {
        return flag_;
    }

    void Next() {
        char now_symbol = in_->peek();
        if (now_symbol == EOF) {
            flag_ = true;
            return;
        }
        if (now_symbol == ' ' || now_symbol == '\t' || now_symbol == '\n') {
            in_->get();
            Next();
            return;
        }
        if (now_symbol == '\'') {
            in_->get();
            QuoteToken now_token;
            tkn_ = now_token;
        } else if (now_symbol == '.') {
            in_->get();
            DotToken now_token;
            tkn_ = now_token;
        } else if (now_symbol == ')') {
            in_->get();
            BracketToken now_token = BracketToken::CLOSE;
            tkn_ = now_token;
        } else if (now_symbol == '(') {
            in_->get();
            BracketToken now_token = BracketToken::OPEN;
            tkn_ = now_token;
        } else if ((now_symbol >= 'A' && now_symbol <= 'z') || (now_symbol == '#') ||
                   (now_symbol == '*') || (now_symbol >= '<' && now_symbol <= '>')) {
            std::string str_fot_tkn;
            str_fot_tkn.push_back(in_->get());
            now_symbol = in_->peek();
            while ((now_symbol >= 'A' && now_symbol <= 'z') || (now_symbol == '#') ||
                   (now_symbol == '*') || (now_symbol >= '<' && now_symbol <= '>') ||
                   (now_symbol >= '0' && now_symbol <= '9') || (now_symbol == '!') ||
                   (now_symbol == '?') || (now_symbol == '-') || (now_symbol == '+')) {
                if (now_symbol == EOF) {
                    break;
                }
                str_fot_tkn.push_back(in_->get());
                now_symbol = in_->peek();
            }
            SymbolToken now_token;
            now_token.name = str_fot_tkn;
            tkn_ = now_token;
        } else if (now_symbol == '+' || now_symbol == '-') {
            now_symbol = in_->get();
            char next_symbol = in_->peek();
            if (!(next_symbol >= '0' && next_symbol <= '9')) {
                SymbolToken now_token;
                now_token.name = now_symbol;
                tkn_ = now_token;
            } else {
                std::string str_for_tkn;
                str_for_tkn.push_back(now_symbol);
                now_symbol = in_->peek();
                while (now_symbol >= '0' && now_symbol <= '9') {
                    if (now_symbol == EOF) {
                        break;
                    }
                    str_for_tkn.push_back(in_->get());
                    now_symbol = in_->peek();
                }
                bool flag_for_znak = false;
                if (str_for_tkn[0] == '-') {
                    flag_for_znak = true;
                }
                int res = str_for_tkn[1] - '0';
                for (size_t i = 2; i < str_for_tkn.size(); ++i) {
                    res = res * 10 + (str_for_tkn[i] - '0');
                }
                if (flag_for_znak) {
                    res *= -1;
                }
                ConstantToken now_token;
                now_token.value = res;
                tkn_ = now_token;
            }
        } else if (now_symbol >= '0' && now_symbol <= '9') {
            std::string str_for_tkn;
            while (now_symbol >= '0' && now_symbol <= '9') {
                if (now_symbol == EOF) {
                    break;
                }
                str_for_tkn.push_back(in_->get());
                now_symbol = in_->peek();
            }
            int res = str_for_tkn[0] - '0';
            for (size_t i = 1; i < str_for_tkn.size(); ++i) {
                res = res * 10 + (str_for_tkn[i] - '0');
            }
            ConstantToken now_token;
            now_token.value = res;
            tkn_ = now_token;
        } else if (now_symbol == '/') {
            now_symbol = in_->get();
            SymbolToken now_token;
            now_token.name = now_symbol;
            tkn_ = now_token;

        } else {
            throw SyntaxError("");
        }
    }

    Token GetToken() {
        return tkn_;
    }

private:
    Token tkn_;
    std::istream* in_;
    bool flag_ = false;
};