#pragma once
#ifndef __UNDOABLE_TEMPLATE_HEADER__
#define __UNDOABLE_TEMPLATE_HEADER__

#include <string>

template<typename>
struct default_value;

template<>
struct default_value<bool> {
    static constexpr bool value = false;
};

template<>
struct default_value<int64_t> {
    static constexpr int64_t value = 0ll;
};

template<>
struct default_value<double> {
    static constexpr double value = 0.0;
};

template<>
struct default_value<std::string> {
    static constexpr char value[] = "";
};

template<typename T>
class undoable_t
{
public:
    undoable_t() : current_(default_value<T>::value), previous_(default_value<T>::value), changed_(false), marked_(false) {}
    undoable_t(T initial) : current_(initial), previous_(initial), changed_(false), marked_(false) {}

    void mark() {
        previous_ = current_;
        changed_ = false;
        marked_ = true;
    }
    // void unmark() {
    //     marked_ = false;
    // }
    void restore() {
        if (changed_) {
            current_ = previous_;
            changed_ = false;
            marked_ = false;
        }
    }
    bool hasChanged() const {
        return changed_;
    }
    void touch() {
        marked_ = false;
        changed_ = false;
    }
    T getValue() const {
        return current_;
    }
    void assignValue(T newValue) {
        if (current_ != newValue) {
            current_ = newValue;
        }
    }
    bool operator==(const T& other) const {
        return other == current_;
    }
    bool operator==(const undoable_t& other) const {
        return other.current_ == current_;
    }
    operator T() {
        return current_;
    }
    operator T() const {
        return current_;
    }
    undoable_t& operator=(const undoable_t& other) {
        if (this != &other) {
            if (!marked_) {
                previous_ = current_;
            }
            current_ = other.current_;
            changed_ = current_ != previous_;
        }
        return *this;
    }
    undoable_t& operator=(const T& other) {
        if (!marked_) {
            previous_ = current_;
        }
        current_ = other;
        changed_ = current_ != previous_;
        return *this;
    }

private:
    T current_;
    T previous_;
    bool changed_;
    bool marked_;
};

#endif /// __UNDOABLE_TEMPLATE_HEADER__
