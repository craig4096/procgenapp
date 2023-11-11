
#ifndef ARRAY2D_H
#define ARRAY2D_H


template <class T> class Array2D
{
    T* array;
    int width, height;
    int size;
public:
    Array2D()
    {
        this->width = 0;
        this->height = 0;
        size = 0;
        array = 0;
    }

    Array2D(int width, int height)
    {
        this->width = width;
        this->height = height;
        size = width * height;
        array = new T[size];
    }

    Array2D(const Array2D& other)
    {
        this->width = other.width;
        this->height = other.height;
        this->size = other.size;
        this->array = new T[size];
        // copy all values of array
        for (int i = 0; i < other.GetSize(); ++i)
        {
            this->array[i] = other.array[i];
        }
    }

    ~Array2D()
    {
        delete[] array;
    }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetSize() const { return size; }

    void SetValue(int x, int y, T value)
    {
        array[y * width + x] = value;
    }

    T GetValue(int x, int y) const
    {
        return array[y * width + x];
    }

    void SetValue(int index, T value)
    {
        array[index] = value;
    }

    T GetValue(int index) const
    {
        return array[index];
    }

    int GetIndex(int x, int y) const
    {
        return y * width + x;
    }

    bool IndexValid(int x, int y) const
    {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    void ClearAll(T value)
    {
        for (int i = 0; i < GetSize(); ++i) 
        {
            array[i] = value;
        }
    }

    T& operator()(int x, int y) { return array[y * width + x]; }

    const T& operator()(int x, int y) const { return array[y * width + x]; }

    T& operator[](int index) { return array[index]; }
    const T& operator[](int index) const { return array[index]; }

    void Normalize(float minValue, float maxValue)
    {
        float max = -10000000.0f;
        float min = 10000000.0f;
        for (int i = 0; i < GetSize(); ++i)
        {
            float v = (*this)[i];
            if (v > max) max = v;
            if (v < min) min = v;
        }

        // convert range(min, max) to range(0,1)
        float size = max - min;

        for (int i = 0; i < GetSize(); ++i)
        {
            (*this)[i] = ((((*this)[i] - min) / size) * (maxValue - minValue)) + minValue;
        }
    }
};

#endif
