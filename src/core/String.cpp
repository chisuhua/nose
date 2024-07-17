#include "String.h"

//template<typename T, typename>
//StringRef String::intern(T&& arg) {
    //std::string str = args;
    //return StringPool::getInstance()->intern(str.c_str(), CONSTEXPR_HASH(str.c_str()));
//};

StringRef StringPool::intern(const char* str, std::uint32_t hash) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = pool_.find(hash);
    if (it != pool_.end()) {
        return it->second.lock();
    }
    auto deleter = [this, hash](const String* p) {
        std::lock_guard<std::mutex> lock(this->mutex_);
        //this->remove(hash);
        this->pool_.erase(hash);
        delete p;
    };
    auto string_ref = StringRef(new String(str), deleter);
    pool_.emplace(hash, std::weak_ptr<const String>(string_ref));
    return string_ref;
};
