#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/pytypes.h>
#include <pybind11/chrono.h>
#include <iostream>

namespace py = pybind11;

class Iterator
{
    std::function<bool()> m_callback;

public:
    explicit Iterator(const std::function<bool()>& m_callback)
        : m_callback(m_callback)
    {
    }

    [[nodiscard]] py::object next() const
    {
        if (m_callback())
        {
            return py::none();
        }

        PyErr_SetString(PyExc_StopIteration, "CounterIter reached the end");
        throw py::error_already_set();
    }
};

Iterator myFunction()
{
    auto i = std::make_shared<int>(0);

    return Iterator([i]
    {
        if (*i < 10)
        {
            std::cout << "Yielding " << *i << std::endl;
            (*i)++;
            return true;
        }
        return false;
    });
}

PYBIND11_MODULE(asyncio_example, m)
{
    m.doc() = "Cheap way to create asyncio compatible C++ iterator (Very dirty solution)";

    py::class_<Iterator, std::shared_ptr<Iterator>>(m, "Iterator")
        .def("__iter__", [](Iterator& iter) -> Iterator& { return iter; })
        .def("__next__", &Iterator::next);

    m.def("myFunction", &myFunction);
}
