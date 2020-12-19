//
// Created by lukaszk on 19.12.2020.
//

#ifndef MASTERTHESISAPP_SINGLETON_H
#define MASTERTHESISAPP_SINGLETON_H

#include <iostream>

template<typename T>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    static T& getInstance() {
        static T instance{};
        return instance;
    }

protected:
    Singleton() = default;
};

#endif // MASTERTHESISAPP_SINGLETON_H
