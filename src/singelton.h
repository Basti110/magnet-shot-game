#pragma once
#include <stdio.h>

template <typename T>
class Singleton
{
public:
    Singleton()
    {
        if (instance != nullptr)
            std::cerr << "SINGELTON: More than one Singelton instances";
        instance = static_cast<T*>(this);
    }

    ~Singleton()
    {
        if (instance != nullptr)
        {
            delete instance;
        }
        instance = nullptr;
    }

    static T* getInstance()
    {
        if (instance == nullptr)
        {
            instance = new T();
        }
        return instance;
    }

    Singleton(Singleton const&) = delete;
    void operator=(Singleton const&) = delete;

private:
    static T* instance;
};

template <typename T>
T* Singleton<T>::instance = nullptr;
