/*
 * author: Adam Budziak
 */

#pragma once

#include <memory>
#include <string>
#include <utility>

namespace dfare::regexp {

class RegExpBase;

/**
 * An almost transparent wrapper for the RegExpBase class that operates using pointers.
 *
 * It's purpose is to make the inheritance/polymorphism natural and easy.
 */
class regexp_ptr {
    using ptr_t = std::shared_ptr<const RegExpBase>;
    ptr_t ptr;

 public:
    /*
     * The following definitions of ctors and assignment operators mean that the RegExp objects
     * will be shared. It's not a problem since every object is constant and any modification
     * creates a new object.
     */
    regexp_ptr() = default;
    regexp_ptr(const regexp_ptr&) = default;
    regexp_ptr(regexp_ptr&&) = default;
    regexp_ptr& operator=(const regexp_ptr&) = default;
    regexp_ptr& operator=(regexp_ptr&&) = default;

    template <typename T, std::enable_if_t<!std::is_same_v<regexp_ptr, std::decay_t<T>>>* = nullptr>
    explicit regexp_ptr(T&& ptr) : ptr {std::forward<T>(ptr)} {  } // NOLINT

    decltype(ptr.operator->()) operator->() const {
        return ptr.operator->();
    }

    decltype(ptr.operator*()) operator*() const {
        return ptr.operator*();
    }

    decltype(ptr.get()) get() const {
        return ptr.get();
    }

    regexp_ptr derivative(char prefix) const;
    bool equivalent(const regexp_ptr& other) const;
    bool equivalent(const RegExpBase& other) const;

    bool operator==(const regexp_ptr& other) const;
    bool operator==(const RegExpBase& other) const;

    bool matches_empty() const;

    friend std::ostream& operator<<(std::ostream& os, const regexp_ptr& p);

    operator std::string();  //NOLINT
};

std::ostream& operator<<(std::ostream& os, const regexp_ptr& p);


class RegExpBase {
 public:
    virtual ~RegExpBase() = default;
    virtual explicit operator std::string() const = 0;

    virtual regexp_ptr derivative(char prefix) const = 0;
    virtual bool matches_empty() const = 0;

    bool operator==(const RegExpBase& other) const {
        return equals(other);
    }
    bool equivalent(const RegExpBase& other) const {
        return equivalent_(other);
    }

 protected:
    virtual bool equals(const RegExpBase& other) const = 0;
    virtual bool equivalent_(const RegExpBase& other) const = 0;
};


template<typename Derived>
class RegExpBase_ : public RegExpBase {
 public:
    template<typename ...Args>
    static regexp_ptr create(Args... args) {
        return regexp_ptr{std::make_shared<Derived>(std::forward<Args>(args)...)};
    }

 protected:
    bool equals(const RegExpBase& o) const override {
        auto other = dynamic_cast<const Derived*>(&o);
        return other != nullptr && static_cast<const Derived&>(*this) == *other;
    }

    bool equivalent_(const RegExpBase& o) const override {
        auto other = dynamic_cast<const Derived*>(&o);
        return other && static_cast<const Derived&>(*this).equivalent(*other);
    }

 private:
    constexpr bool equivalent(const Derived& o) const {
        return static_cast<const Derived&>(*this) == o;
    }

    constexpr bool operator==(const RegExpBase_&) const {
        return false;
    }
    RegExpBase_() = default;
    friend Derived;
};

}  // namespace dfare::regexp
