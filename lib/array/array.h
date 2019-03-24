#ifndef __ARRAY_H__
#define __ARRAY_H__

template<class T>
class array {

protected:
    T* data;
    size_t length;

public:
    array() : data(NULL), length(0) { }

    array(const T& v, size_t t) : data(NULL), length(t)
    {
        data = new T[length];
        for(size_t i = 0; i < length; ++i){
            data[i] = v;
        }
    }

    array(const T* p, size_t t) : data(NULL), length(t)
    {
        data = new T[length];
        for(size_t i = 0; i < length; ++i){
            data[i] = p[i];
        }
    }

    size_t size()const{
        return length;
    }

    T operator[](size_t i) const{
        return data[i];
    }

    T& operator[](size_t i){
        return data[i];
    }
}


#endif // __ARRAY_H__
