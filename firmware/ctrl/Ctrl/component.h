#include <stdint.h>
#include <variant>
#include <optional>
class ComPonentBase
{
public:
    virtual void update(uint32_t timestamp) = 0;
};


template<typename T>
class InputPort;

template<typename T>
class OutputPort
{
public:
    OutputPort(T val) : content_(val) {}

    void operator=(T value)
    {
        content_ = value;
        age_ = 0;
    }


    void reset()
    {
        age_++;
    }


    std::optional<T> present()
    {
        if (age_ == 0)
        {
            return content_;
        } else
        {
            return std::nullopt;
        }
    }

    std::optional<T> previous()
    {
        if (age_ == 1)
        {
            return content_;
        } else
        {
            return std::nullopt;
        }
    }

    std::optional<T> any()
    {
        return content_;
    }

private:
    uint32_t age_ = 2; // Age in number of control loop iterations
    T content_;
};


template<typename T>
class InputPort {
public:
    void connect_to(OutputPort<T>* input_port)
    {
        content_ = input_port;
    }

    void connect_to(T* input_ptr)
    {
        content_ = input_ptr;
    }

    void disconnect()
    {
        content_ = (OutputPort<T>*)nullptr;
    }

    std::optional<T> present()
    {
        if (content_.index() == 2)
        {
            OutputPort<T>* ptr = std::get<2>(content_);
            return ptr ? ptr->present() : std::nullopt;
        } else if (content_.index() == 1)
        {
            T* ptr = std::get<1>(content_);
            return ptr ? std::make_optional(*ptr) : std::nullopt;
        } else
        {
            return std::get<0>(content_);
        }
    }

    std::optional<T> any()
    {
        if (content_.index() == 2) {
            OutputPort<T>* ptr = std::get<2>(content_);
            return ptr ? ptr->any() : std::nullopt;
        } else if (content_.index() == 1) {
            T* ptr = std::get<1>(content_);
            return ptr ? std::make_optional(*ptr) : std::nullopt;
        } else {
            return std::get<0>(content_);
        }
    }

private:
    std::variant<T, T*, OutputPort<T>*> content_;
};
