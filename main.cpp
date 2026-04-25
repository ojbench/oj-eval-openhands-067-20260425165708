
#include <iostream>
#include <optional>
#include <stdexcept>
#include <memory>
#include <atomic>

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
    
    // Shared borrow state
    struct BorrowState {
        std::atomic<int> immutable_count{0};
        std::atomic<bool> mutable_borrowed{false};
        std::weak_ptr<BorrowState> self_weak;
    };
    
    std::shared_ptr<BorrowState> borrow_state;

public:
    // Forward declarations
    class Ref;
    class RefMut;

    // Constructor
    explicit RefCell(const T& initial_value) : value(initial_value) {
        borrow_state = std::make_shared<BorrowState>();
        borrow_state->self_weak = borrow_state;
    }
    
    explicit RefCell(T&& initial_value) : value(std::move(initial_value)) {
        borrow_state = std::make_shared<BorrowState>();
        borrow_state->self_weak = borrow_state;
    }
    
    // Disable copying and moving for simplicity
    RefCell(const RefCell&) = delete;
    RefCell& operator=(const RefCell&) = delete;
    RefCell(RefCell&&) = delete;
    RefCell& operator=(RefCell&&) = delete;

    // Borrow methods
    Ref borrow() const {
        auto state = borrow_state;
        if (state->mutable_borrowed.load()) {
            throw BorrowError("Cannot borrow immutably while mutably borrowed");
        }
        state->immutable_count.fetch_add(1);
        return Ref(state, &value);
    }

    std::optional<Ref> try_borrow() const {
        auto state = borrow_state;
        if (state->mutable_borrowed.load()) {
            return std::nullopt;
        }
        state->immutable_count.fetch_add(1);
        return Ref(state, &value);
    }

    RefMut borrow_mut() {
        auto state = borrow_state;
        if (state->mutable_borrowed.load()) {
            throw BorrowMutError("Already mutably borrowed");
        }
        if (state->immutable_count.load() > 0) {
            throw BorrowMutError("Cannot borrow mutably while immutably borrowed");
        }
        state->mutable_borrowed.store(true);
        return RefMut(state, &value);
    }

    std::optional<RefMut> try_borrow_mut() {
        auto state = borrow_state;
        if (state->mutable_borrowed.load() || state->immutable_count.load() > 0) {
            return std::nullopt;
        }
        state->mutable_borrowed.store(true);
        return RefMut(state, &value);
    }

    // Inner classes for borrows
    class Ref {
    private:
        std::shared_ptr<BorrowState> state;
        const T* ptr;

    public:
        Ref() : ptr(nullptr) {}

        Ref(std::shared_ptr<BorrowState> s, const T* p) : state(s), ptr(p) {}

        ~Ref() {
            if (state && ptr) {
                state->immutable_count.fetch_sub(1);
            }
        }

        const T& operator*() const {
            return *ptr;
        }

        const T* operator->() const {
            return ptr;
        }

        // Allow copying
        Ref(const Ref& other) : state(other.state), ptr(other.ptr) {
            if (state && ptr) {
                state->immutable_count.fetch_add(1);
            }
        }
        
        Ref& operator=(const Ref& other) {
            if (this != &other) {
                if (state && ptr) {
                    state->immutable_count.fetch_sub(1);
                }
                state = other.state;
                ptr = other.ptr;
                if (state && ptr) {
                    state->immutable_count.fetch_add(1);
                }
            }
            return *this;
        }

        // Allow moving
        Ref(Ref&& other) noexcept : state(std::move(other.state)), ptr(other.ptr) {
            other.ptr = nullptr;
        }

        Ref& operator=(Ref&& other) noexcept {
            if (this != &other) {
                if (state && ptr) {
                    state->immutable_count.fetch_sub(1);
                }
                state = std::move(other.state);
                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }
    };

    class RefMut {
    private:
        std::shared_ptr<BorrowState> state;
        T* ptr;

    public:
        RefMut() : ptr(nullptr) {}

        RefMut(std::shared_ptr<BorrowState> s, T* p) : state(s), ptr(p) {}

        ~RefMut() {
            if (state && ptr) {
                state->mutable_borrowed.store(false);
            }
        }

        T& operator*() {
            return *ptr;
        }

        T* operator->() {
            return ptr;
        }

        // Disable copying to ensure correct borrow rules
        RefMut(const RefMut&) = delete;
        RefMut& operator=(const RefMut&) = delete;

        // Allow moving
        RefMut(RefMut&& other) noexcept : state(std::move(other.state)), ptr(other.ptr) {
            other.ptr = nullptr;
        }

        RefMut& operator=(RefMut&& other) noexcept {
            if (this != &other) {
                if (state && ptr) {
                    state->mutable_borrowed.store(false);
                }
                state = std::move(other.state);
                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }
    };

    // Destructor
    ~RefCell() noexcept(false) {
        if (borrow_state->immutable_count.load() > 0 || borrow_state->mutable_borrowed.load()) {
            throw DestructionError("RefCell destroyed while still borrowed");
        }
    }
};

// Dummy main function for compilation - will be overridden by test harness
int main() {
    return 0;
}

