#include <iostream>
#include <string>
#include <stdexcept>


class StringPool;
class String;
using StringPtr = std::shared_ptr<const String>;

template<typename ...>
using void_t = void;

template <typename, typename = void_t<>>
struct is_valid_type : std::false_type {};

template <typename T>
struct is_valid_type<T, void_t<
        std::enable_if_t<
            std::is_same_v<std::remove_reference<T>, std::string> ||
            std::is_same_v<std::remove_reference<T>, const std::string&> ||
            std::is_same_v<std::remove_reference<T>, const char*> ||
            std::is_same_v<std::remove_reference<T>, char*>
        >
>> : std::true_type {};

// 定义宏以前向声明任意参数数目的成员函数
#define FORWARD_METHOD(FuncName, ...) \
    template <typename... Args> \
    auto FuncName(Args&&... args) const -> decltype(data.FuncName(std::forward<Args>(args)...)) { \
        return data.FuncName(std::forward<Args>(args)...); \
    }

// 定义宏以前向声明 const 成员函数操作符重载
#define FORWARD_OPERATOR(Op) \
    bool operator Op(const String& other) const { \
        return data Op other.data; \
    }

// 定义宏以前向声明含参数的成员函数操作符重载
#define FORWARD_OPERATOR_WITH_ARGS(Op, ...) \
    template <typename... Args> \
    auto operator Op (Args&& ... args) const -> decltype(data.operator Op(std::forward<Args>(args)...)) { \
        return data.operator Op(std::forward<Args>(args)...); \
    } \
    template <typename... Args> \
    auto operator Op (Args&& ... args) -> decltype(data.operator Op(std::forward<Args>(args)...)) { \
        return data.operator Op(std::forward<Args>(args)...); \
    }

class String {
public:
    template<typename T, typename = std::enable_if_t<is_valid_type<T>::value>>
    static StringPtr intern(T&& arg);

    std::string data;

    FORWARD_METHOD(length)
    FORWARD_METHOD(size)
    FORWARD_METHOD(empty)

    FORWARD_OPERATOR_WITH_ARGS([], size_t)
    FORWARD_METHOD(at, size_t )
    FORWARD_METHOD(find, const std::string&, size_t)
    // FORWARD_METHOD(find, const char*, size_t)
    // FORWARD_METHOD(find, char c, size_t)

    FORWARD_METHOD(rfind, const std::string& str, size_t pos = std::string::npos) 
    FORWARD_METHOD(substr, size_t pos = 0, size_t len = std::string::npos)
    FORWARD_METHOD(view)
    FORWARD_OPERATOR(std::string)

    FORWARD_OPERATOR(==)

    // FORWARD_OPERATOR_WITH_ARGS(==, std::string)
    // FORWARD_OPERATOR_WITH_ARGS(!=, std::string)

    // FORWARD_OPERATOR_WITH_ARGS(<<, std::ostream& os, std::string)

private:
    // no copy and assign constructor
    String(const String&) = delete;
    String& operator=(const String&) = delete;

    String(const char* str) : data(str) {}
    String(const std::string& str) : data(str) {}
friend:
    StringPool;
}

class StringPool {
public:
    static std::shared_ptr<StringPool> getInstance() {
        static std::shared_ptr<StringPool> instance(new StringPool());
        return instance;
    }
    StringPtr intern(const char* str, std::uint32_t hash);

    ~StringPool() {
        clearPool();
    }
    bool isStringIntern(const std::string& str) {
        if (findStringByHash(CONSTEXPR_HASH(str))) { return true; }
        else return false;
    }
    StringPtr getStringByHash(std::uint32_t hash) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = pool_.find(hash);
        if (it != pool_.end()) {
            return it->second.lock();
        }
        return nullptr;
    }
private:
    StringPool() = default;

    void clearPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            auto it = pool_.begin();
            pool_.erase(it); // Erase from the pool map
        }
    }

    std::mutex mutex_;
    std::unordered_map<std::uint32_t, std::weak_ptr<const String>> pool_;
};

constexpr uint32_t fnv1a_32_constexpr(const char *s, std::size_t count) {
    return ((count ? fnv1a_32_constexpr(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

#define CONSTEXPR_HASH(str) (fnv1a_32_constexpr((str), sizeof(str) - 1))

constexpr StringPtr operator "" _hs(const char *s, std::size_t count) {
    constexpr uint32_t hash = fnv1a_32_constexpr(s, count);
    return StringPool::getInstance()->intern(s, hash);
}