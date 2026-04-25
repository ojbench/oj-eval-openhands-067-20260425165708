# Problem 067 - stargazer 的 RefCell

**ACMOJ Problem ID**: 2641

## Table of Contents

- [Problem 067 - stargazer 的 RefCell](#problem-067-stargazer-的-refcell)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
    - [Background](#background)
  - [Assignment Description](#assignment-description)
    - [Grade Composition](#grade-composition)
  - [Assignment Requirements](#assignment-requirements)
    - [Input Format](#input-format)
    - [Output Format](#output-format)
    - [Samples](#samples)
    - [Data Constraints](#data-constraints)
  - [Per-Testcase Resource Limits](#per-testcase-resource-limits)
  - [Test Data](#test-data)
  - [Submission Requirements](#submission-requirements)
    - [OJ Git Repository Compilation Process](#oj-git-repository-compilation-process)
    - [Git Configuration Requirements](#git-configuration-requirements)
    - [Submission Guidelines](#submission-guidelines)
    - [Evaluation Notes](#evaluation-notes)
    - [Academic Integrity](#academic-integrity)

## Introduction

### Background

Implement Rust's RefCell in C++: runtime borrow checking with shared_ptr/weak_ptr.

## Assignment Description

### Grade Composition

| Grading Component | Percentage |
| :--: | :--: |
| Pass **2641. stargazer 的 RefCell** | 80% |
| Code Review | 20% |

Here are several points that need clarification:

- In the Code Review, we will **strictly examine your code style and repository organization structure, etc.**

- This assignment provides some sample data for testing, stored in the `/workspace/data/067/data_test/` directory. Note that these are not the test cases on the Online Judge. Passing all local test cases does not guarantee that you will pass the OJ tests.

- Besides the provided sample data, we also encourage you to design your own test data based on your program logic to assist debugging.

## Assignment Requirements

### Problem Description

**Update : 更新了下发代码模板,删除了`: ref_cell(other.ref_cell)`字段,同时你可以添加任意你想用的成员函数和成员变量**

stargazer 最近正在学习 Rust 语言的所有权机制与内存安全特性。

Rust 的所有权机制要求每个值都有唯一的所有者（通常是变量），并且在同一时间内只能有一个所有者。所有权的转移可以通过赋值、函数参数传递或返回值来实现。相当于 C++ 中的移动语义（`std::move()`）。

在一个值的所有者变量的作用域之外（例如在另外一个函数中）对该值的访问必须通过借用（相当于 C++ 中指向变量的指针）来实现。Rust 中的借用分为两种：
- 不可变借用（Immutable Borrow）：相当于 C++ 中的 `const T*`，允许读取但不允许修改
- 可变借用（Mutable Borrow）：相当于 C++ 中的 `T*`，允许读取和修改

Rust 对变量的借用有着严格的限制：
- 在同一时间内，只能有一个可变借用，或者多个不可变借用
- 不能同时存在可变借用和不可变借用
- 所有借用都必须在拥有变量的生命周期内有效
对以上规则的违反会导致编译错误。

Rust 的借用机制对编译器优化非常有帮助。由于不可变借用不能与可变借用共存，被不可变借用指向的值只需要从堆内存中获取一次，之后可以安全地存储在寄存器或栈上缓存中。相比之下，C++ 中也进行类似的优化，但其他函数修改 `const*` 指针指向的值是未定义行为，可能导致不安全的代码。

Rust 的编译器可以在编译时就能“静态”地检查所有权和借用关系，在运行时无需额外检查。然而，对于堆上对象，在编译期检查所有权和借用关系是非常困难的。因此，Rust 提供了 `RefCell<T>` 类型来在运行时检查所有权和借用关系。它有如下方法：
- `borrow()` 与 `try_borrow()`：获取一个不可变借用，返回 `Ref<T>` 类型。如果当前存在可变借用则失败。`borrow()` 会 panic，相当于 C++ 中的 abort，而 `try_borrow()` 返回一个 `Result<Ref<T>, BorrowError>`，相当于 C++ 中的 `std::optional<Ref<T>>`。
- `borrow_mut()` 与 `try_borrow_mut()`：获取一个可变借用，返回 `RefMut<T>` 类型。如果当前存在任何借用则会失败。
- 返回的 `Ref<T>` 和 `RefMut<T>` 包装器实现了解引用操作符，可以像使用普通引用一样使用
- 当 `Ref<T>` 和 `RefMut<T>` 的生命周期结束时，会自动减少或重置借用计数
- 当 `RefCell<T>` 的生命周期结束时，若仍有借用存在，则会 panic

请借鉴 Rust 中的借用机制，在 C++ 中实现一个 `RefCell` 类。

```cpp
#include <iostream>
#include <optional>
#include <stdexcept>
class RefCellError : public std::runtime_error {
public:
    explicit RefCellError(const std::string& message) : std::runtime_error(message) {}
    virtual ~RefCellError() = default;
};// Abstract class as base class

//invalidly call an immutable borrow
class BorrowError : public RefCellError {
public:
    explicit BorrowError(const std::string& message) : RefCellError(message) {}
};
//invalidly call a mutable borrow
class BorrowMutError : public RefCellError {
public:
    explicit BorrowMutError(const std::string& message) : RefCellError(message) {}
};
//still has refs when destructed
class DestructionError : public RefCellError {
public:
    explicit DestructionError(const std::string& message) : RefCellError(message) {}
};

template <typename T>
class RefCell {
private:
    T value;
    // TODO(student)

public:
    // Forward declarations
    class Ref;
    class RefMut;

    // Constructor
    explicit RefCell(const T& initial_value) {
        // TODO(student)
    }
	explicit RefCell(T && initial_value) {
        // TODO(student)
    }
	
	// Disable copying and moving for simplicity
    RefCell(const RefCell&) = delete;
    RefCell& operator=(const RefCell&) = delete;
    RefCell(RefCell&&) = delete;
    RefCell& operator=(RefCell&&) = delete;

    // Borrow methods
    Ref borrow() const {
        // TODO(student)
    }

    std::optional<Ref> try_borrow() const {
        // TODO(student)
    }

    RefMut borrow_mut() {
        // TODO(student)
    }

    std::optional<RefMut> try_borrow_mut() {
        // TODO(student)
    }

    // Inner classes for borrows
    class Ref {
    private:
        // TODO(student)

    public:
        Ref() {
            // TODO(student)
        }

        ~Ref() {
            // TODO(student)
        }

        const T& operator*() const {
            // TODO(student)
        }

        const T* operator->() const {
            // TODO(student)
        }

        // Allow copying
        Ref(const Ref&) {
            // TODO(student)
        }
        Ref& operator=(const Ref&) {
            // TODO(student)
        }

        // Allow moving
        Ref(Ref&& other) noexcept {
            // TODO(student)
        }

        Ref& operator=(Ref&& other) noexcept {
            // TODO(student)
        }
    };

    class RefMut {
    private:
        // TODO(student)

    public:
        RefMut() {
        // TODO(student)
        }

        ~RefMut() {
        // TODO(student)
        }

        T& operator*() {
        // TODO(student)
        }

        T* operator->() {
        // TODO(student)
        }

        // Disable copying to ensure correct borrow rules
        RefMut(const RefMut&) = delete;
        RefMut& operator=(const RefMut&) = delete;

        // Allow moving
        RefMut(RefMut&& other) noexcept  {
        // TODO(student)
        }

        RefMut& operator=(RefMut&& other) noexcept {
        // TODO(student)
        }
    };

    // Destructor
    ~RefCell() {
        // TODO(student)
    }
};
```
须知：
- 出现panic状况需要throw相应的Error类，我们不检查message内容，但会检查你的程序是否正确的抛出正确的异常类别。

- 保证评测程序中所有 `Ref<T>` 和 `RefMut<T>` 类型的对象不会通过调用 `Ref()` 或 `RefMut()` 生成。

- 保证 `RefCell<T>` 中的类型 `T` 有拷贝构造函数**或**移动构造函数。

- 保证被 `std::move`的对象不会再被使用。

- 保证引用 `RefCell<T> cell` 的 `RefCell<T>::Ref ref` 和 `RefCell<T>::RefMut ref_mut` 一定比 `cell` 先析构。

- 为简单起见，在评测程序中，如果一个对象抛出了异常，它不会再被使用。你也只需确保在无任何异常抛出时不发生内存泄露。

你可以访问以下网页：
- https://en.cppreference.com/w/cpp/utility/optional
- https://en.cppreference.com/w/cpp/utility/move

### Input Format

See the problem description above.

### Output Format

See the problem description above.

### Samples

#### Sample 1

### Data Constraints

| Subtask  |  Testcase No. | 考察内容  |  分值 |
| ------------ | ------------ | ------------ | ------------ |
| basic-1  | 1, 12(memcheck)  | 考察 `borrow` 和 `borrow_mut` 的简单使用，保证不出现异常  | 5  |
| basic-2  | 2, 13(memcheck)  | 考察 `borrow` 和 `borrow_mut` 的简单使用，保证不出现异常  | 5  |
| basic-3  | 3, 14(memcheck)  | 考察 `borrow` 和 `borrow_mut` 的简单使用，保证不出现异常  | 5  |
| panic-1  | 4  | 考察会出现异常的基础情况  | 5  |
| panic-2  | 5  | 考察会出现异常的基础情况  | 5  |
| panic-3  | 6  | 考察会出现异常的基础情况  | 5  |
| panic-4  | 7  | 考察会出现异常的基础情况  | 5  |
| panic-5  | 8, 15(memcheck)  | 考察 `try_borrow` 和 `try_borrow_mut` 的简单使用 | 5  |
| hard-1  | 9, 16(memcheck)  | 综合测试  | 20  |
| hard-2  | 10, 17(memcheck)  | 综合测试  | 20  |
| move  | 11, 18(memcheck)  | 考查 `Ref` 和 `RefMut` 的移动构造函数和移动赋值运算符  | 20  |

## Per-Testcase Resource Limits

- **Time Limit (per test case)**: 1000 ms
- **Memory Limit (per test case)**: 256 MiB
- **Disk Usage**: No disk usage is permitted.

## Test Data

The test data for this problem is located at `/workspace/data/067/data_test/`.

## Submission Requirements

### OJ Git Repository Compilation Process

For Git compilation, we will first clone the repository using a command similar to:
```bash
git clone <repo_url> . --depth 1 --recurse-submodules --shallow-submodules --no-local
```

Then we check if there is a `CMakeLists.txt` file. If it exists, we run (if not, a warning message will be displayed):
```bash
cmake .
```

Finally, we check if there is any of `GNUmakefile`/`makefile`/`Makefile` (if cmake was run previously, this will be the generated Makefile). If it exists, we run (if not, a warning message will be displayed):
```bash
make
```

After this process is complete, we will use the `code` file in the project root directory as the compilation result.

A `CMakeLists.txt` file is provided in the project. You can use or modify it as needed. The local environment has gcc-13 and g++-13 available.

### Git Configuration Requirements

**IMPORTANT**: You must create a `.gitignore` file in your project root directory to avoid OJ evaluation conflicts.

The `.gitignore` file should include at least the following entries:

```gitignore
CMakeFiles/
CMakeCache.txt
```

### Submission Guidelines

- The submitted code must be able to compile successfully through the above compilation process
- The compiled executable file name must be `code`
- The program needs to be able to read data from standard input and write results to standard output
- Please ensure the code runs correctly within the given time and space limits
- **You must use C++ or C language** to implement this assignment

### Evaluation Notes

- The evaluation system will test your program using the provided test data
- The program output must exactly match the expected output (including format)
- Exceeding time or memory limits will be judged as the corresponding error type
- Please pay attention to the overall time performance of your code and the time complexity of each part of your algorithm.

### Academic Integrity

If any violations are found during evaluation or code review (including but not limited to using unconventional methods to pass test cases), your final score may be significantly reduced or become **0 points**.
