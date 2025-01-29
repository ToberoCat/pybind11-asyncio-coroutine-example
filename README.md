# 🚀 Asyncio Coroutine Integration with Pybind11

[![Language](https://img.shields.io/badge/language-C%2B%2B%2F%20Python-blue)](https://github.com/)

## 📝 Overview

This repository provides a minimal yet functional demonstration of integrating **C++20 coroutines** with Python's *
*asyncio** using **Pybind11**. The goal is to allow calling C++ coroutine functions directly and using them with `await`
in Python.

**Why this project?**  
Despite extensive research, no clean and simple way to achieve this was found. This repository serves as a starting
point for those looking to bridge C++ coroutines with Python’s asyncio.

## 📂 Project Structure

```
📦 asyncio-coroutine-pybind11
├── src/
│   └── main.cpp       # C++20 coroutine logic with Pybind11 bindings
├── example/
│   └── main.py        # Python example showcasing async iterator usage
├── install.sh         # Script to install dependencies and build extension
└── README.md          # Documentation
```

## 🔑 Features

✅ **Bridges C++ coroutines with Python asyncio**  
✅ **No threading—pure coroutine-based execution**  
✅ **Provides an async iterator directly callable in Python**  
✅ **Simple, minimal, and easily extendable**

## 🛠️ Prerequisites

Ensure you have the following installed:

- Python 3.7+ (with `venv` support)
- A **C++17+ compiler** (e.g., `g++`, `clang`)
- **CMake** (for building C++ extension)
- **Ninja** (optional, for faster builds)
- **Pybind11** (installed automatically)

## ⚡ Installation & Usage

### 🔹 Step 1: Set up a Virtual Environment

```bash
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
```

### 🔹 Step 2: Install & Build the Extension

Run the provided install script:

```bash
./install.sh
```

### 🔹 Step 3: Run the Example

```bash
python example/main.py
```

## 📌 Expected Output

Running `main.py` will create two concurrent counter coroutines. The expected output should resemble:

```
Yielding 0
Yielding 1
Yielding 2
...
Yielding 10
Yielding 15
```

It demonstrates **asynchronous iteration** where Python's `asyncio` can await a C++ coroutine without blocking
execution.

## ⚠️ Limitations

🚧 **This is a Proof-of-Concept (PoC).**

- Not optimized for production.
- Error handling and resource management can be improved.
- Further refinements and enhancements are encouraged.

## 🤝 Contributing

Contributions are welcome! If you find a bug, have suggestions, or want to enhance functionality, feel free to:

1. Fork this repository
2. Create a feature branch (`git checkout -b feature-name`)
3. Commit your changes (`git commit -m "Add new feature"`)
4. Push to your branch (`git push origin feature-name`)
5. Open a **Pull Request**

## 🔗 Resources

- [Pybind11 Documentation](https://pybind11.readthedocs.io/en/stable/)
- [C++ Coroutines](https://en.cppreference.com/w/cpp/coroutine)
- [Asyncio in Python](https://docs.python.org/3/library/asyncio.html)

This repository serves as a **starting point** for integrating **C++20 coroutines** with **Python asyncio** using *
*Pybind11**. Feel free to explore, modify, and improve it as needed! 🚀  

