#pragma once
#include <srl.hpp>

namespace Atlantis
{
    // tracks the pool of a specific object, kind of like a particle generator
    template<typename T, size_t CAPACITY>
    struct ObjectPool
    {
        T items[CAPACITY]{};
        size_t activeCount = 0;

        T* Spawn()
        {
            for (size_t i = 0; i < CAPACITY; ++i)
            {
                if (!items[i].active)
                {
                    items[i] = T{};
                    items[i].active = true;
                    activeCount++;
                    return &items[i];
                }
            }
            return nullptr; // pool is full
        }

        void Despawn(T* item) // die
        {
            if (item && item->active)
            {
                item->active = false;
                if (activeCount > 0)
                {
                    activeCount--;
                }
            }
        }

        void Clear()
        {
            for (size_t i = 0; i < CAPACITY; ++i)
            {
                items[i].active = false;
            }
            activeCount = 0;
        }

        size_t Count() const
        {
            return activeCount;
        }
    };
}