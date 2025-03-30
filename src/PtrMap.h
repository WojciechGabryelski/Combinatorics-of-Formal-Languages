#ifndef PTR_MAP_H
#define PTR_MAP_H

#include <map>

template<typename T1, typename T2>
class PtrMap {
private:
    std::map<T1, T2*> map;
public:
    T2*& operator[](const T1& key) {
        auto it = map.find(key);
        if (it == map.end()) {
            T2* newObj = new T2();
            map[key] = newObj;
            return map[key];
        }
        return it->second;
    }
};

#endif //PTR_MAP_H
