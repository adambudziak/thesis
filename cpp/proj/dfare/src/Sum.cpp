/*
 * author: Adam Budziak
 */

#include <string>
#include <algorithm>
#include <utility>

#include <dfare/regexp/Sum.hpp>
#include <dfare/regexp/Eps.hpp>

namespace dfare::regexp {

Sum::operator std::string() const {
    using namespace std::string_literals;
    return '(' + std::string {*left} + '+' + std::string {*right} + ')';
}

regexp_ptr Sum::derivative(char prefix) const {
    return create(left.derivative(prefix), right.derivative(prefix));
}

bool Sum::operator==(const Sum &other) const {
    return left == other.left && right == other.right;
}

bool Sum::matches_empty() const {
    return left.matches_empty() || right.matches_empty();
}

regexp_ptr Sum::create(regexp_ptr left, regexp_ptr right) {
    static const auto cast = [](const RegExpBase* ptr) { return dynamic_cast<const Sum*>(ptr); };
    if (left == right) return left;
    if (left == Empty::create()) return right;
    if (right == Empty::create()) return left;

    if (auto left_sum = cast(left.get()); left_sum) {
        if (left_sum->left == right || left_sum->right == right) {
            return left;
        }
    }
    if (auto right_sum = cast(right.get()); right_sum) {
        if (right_sum->left == left || right_sum->right == left) {
            return right;
        }
    }

    return RegExpBase_<Sum>::create(std::move(left), std::move(right));
}

bool Sum::equivalent(const Sum &other) const {
    static const auto cast = [](const RegExpBase* ptr) { return dynamic_cast<const Sum*>(ptr); };
    if (*this == other  ||
        (left == other.right && right == other.left)) {
        return true;
    }
    if (auto left_sum = cast(left.get()); left_sum) {
        return Sum::create(left_sum->left, Sum::create(left_sum->right, right)) == other;
    }
    if (auto right_sum = cast(right.get()); right_sum) {
        return Sum::create(Sum::create(left, right_sum->left), right_sum->right) == other;
    }
    return false;
}

}  // namespace dfare::regexp
