/*
 * author: Adam Budziak
 */

#include <string>

#include <dfare/regexp/Concat.hpp>
#include <dfare/regexp/Sum.hpp>
#include <dfare/regexp/Eps.hpp>

namespace dfare::regexp {

Concat::operator std::string() const {
    return std::string{*left} + std::string{*right};
}

regexp_ptr Concat::derivative(char prefix) const {
    return Sum::create(
        Concat::create(left.derivative(prefix), right),
        Concat::create(left.matches_empty() ? Eps::create() : Empty::create(), right.derivative(prefix)));
}

bool Concat::matches_empty() const {
    return left.matches_empty() && right.matches_empty();
}

bool Concat::operator==(const Concat &other) const {
    return left == other.left && right == other.right;
}

regexp_ptr Concat::create(regexp_ptr left, regexp_ptr right) {
    if (dynamic_cast<const Empty*>(left.get())) return left;
    if (dynamic_cast<const Empty*>(right.get())) return right;
    if (dynamic_cast<const Eps*>(left.get())) return right;
    if (dynamic_cast<const Eps*>(right.get())) return left;
    if (auto l = dynamic_cast<const Concat*>(left.get()); l) {
        return RegExpBase_<Concat>::create(l->left, Concat::create(l->right, right));
    }

    return RegExpBase_<Concat>::create(left, right);
}

bool Concat::equivalent(const Concat &other) const {
    auto cast = [](const RegExpBase* ptr) { return dynamic_cast<const Concat*>(ptr); };
    if (*this == other) {
        return true;
    }
    if (auto left_concat = cast(left.get()); left_concat) {
        return Concat::create(left_concat->left, Concat::create(left_concat->right, right)) == other;
    }
    if (auto right_concat = cast(right.get()); right_concat) {
        return Concat::create(Concat::create(left, right_concat->left), right_concat->right) == other;
    }
    return false;
}
}  // namespace dfare::regexp
