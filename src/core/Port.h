#pragma once
#include <tuple>
#include <string>
#include <memory>
#include <numeric> 
#include <typeindex>
#include <array>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <cassert>
#include <refl.hpp>
#include "IPort.h"


//// 辅助类用于生成反射信息
//template <typename T>
//struct ReflTypeGenerator;

//template <typename... Ts>
//struct ReflTypeGenerator<MergeTypes<Ts...>> {
    //ReflTypeGenerator() {
        //generate_refl_for_type<MergeTypes<Ts...>>();
    //}

//private:
    //template<typename U, typename = void>
    //struct is_reflective : std::false_type {};

    //template<typename U>
    //struct is_reflective<U, std::void_t<decltype(reflect<U>())>> : std::true_type {};

    //template<typename U>
    //void generate_refl_for_type() {
        //using namespace refl;
        //const auto type_info = reflect<U>();
        //REFL_AUTO(template((typename... Ts), (U)), expand_type_info(type_info));
    //}

    //template<typename TypeInfo>
    //static auto expand_type_info(const TypeInfo& type_info) {
        //return expand_members<TypeInfo, typename TypeInfo::members>();
    //}

    //template<typename TypeInfo, typename Members, size_t... Is>
    //static auto expand_members(const TypeInfo& type_info, std::index_sequence<Is...>) {
        //return std::make_tuple(field(type_info.members[Is].name.str(), type_info.members[Is].pointer)...);
    //}

    //template<typename TypeInfo>
    //static auto expand_members(const TypeInfo& type_info) {
        //return expand_members(type_info, std::make_index_sequence<type_info.members.size()>{});
    //}
//};

//// 辅助模板类用于生成反射信息
//template <typename T>
//struct ReflTypeGenerator {
    //static void generate() {
        //// 在全局上下文中生成反射信息
        //static const bool initialized = []() {
            //REFL_AUTO(template((typename... Ts), (T)), field(io_));
            //return true;
        //}();
        //(void)initialized; // 避免编译器警告
    //}
//};

//// IOType 模板类
//template<typename... Ts>
//struct IOType {
    //using sorted_tuple = typename SortedTypes<std::tuple<Ts...>>::Sorted;
    //using io_type = typename ExtractTypesFromTuple<sorted_tuple>::Type;

    //io_type io_;

    //IOType() {
        //(initialize<Ts>(), ...);
    //}

//private:
    //template<typename T>
    //void initialize() {
        //using namespace refl;
        //const auto type_info = reflect<T>();
        //for_each(type_info.members, [this](auto member) {
            //const auto& io_prop = refl::descriptor::get_attribute<IoProperty<>>(member);
            //set_attribute(member.name.c_str(), member(this->io_), io_prop.role);
        //});
    //}

    //template<typename MemberType>
    //void set_attribute(const std::string& name, MemberType& member_ref, Role role) {
        //using namespace refl;
        //const auto io_type_info = reflect<decltype(io_)>();

        //for_each(io_type_info.members, [this, &name, &member_ref, &role](auto member) {
            //if (std::string(member.name.c_str()) == name) {
                //// 设置属性时使用refl元编程方法
                ////set_member_attribute<decltype(io_)>(member.pointer, member_ref, IoProperty<>(role));
                //descriptor::set_attribute<decltype(io_)>(member.member_pointer, member_ref, IoProperty<>(role));
            //}
        //});
    //}

    //template<typename ClassType, typename MemberType>
    //void set_member_attribute(MemberType ClassType::*member_pointer, MemberType& member_ref, IoProperty<> io_property) {
        ////TODO: 实现refl属性设置逻辑
    //}
//};

//// io_trait 结构，帮助提取 IOType 信息
//template<typename T>
//struct io_trait {
    //static_assert(sizeof(T) == 0, "io_trait must be specialized");
//};

//// io_trait 特化版本，用于提取 IOType 的 io_ 成员类型
//template<typename... Ts>
//struct io_trait<IOType<Ts...>> {
    //using type = typename IOType<Ts...>::io_type;
//};


template<typename Bundle>
class Port : public IPort, public std::enable_shared_from_this<Port<Bundle>>
{
    using Self = Port<Bundle>;
    using SelfPtr = std::shared_ptr<Self>;
public:
    Port() :peer_(nullptr) {};

    //const Bundle& getIO() const {
        //return io_;
    //}

    //std::any getIO() override {
        //return io_;
    //}

    void bind(std::shared_ptr<IPort> other) override {
        auto derived_other = std::dynamic_pointer_cast<Port<Bundle>>(other);
        if (derived_other) {
            peer_ = derived_other;
        } else {
            throw std::invalid_argument("Incompatible port types for binding");
        }
    }

    std::shared_ptr<IPort> clone() const override {
        auto clonedPort = std::make_shared<Port<Bundle>>();
        clonedPort->data_ = this->data_;
        clonedPort->role_ = this->role_;
        return clonedPort;
    }

    void set_data(const Bundle data) { data_ = data; }
    const Bundle& get_data() const { return data_; }

    template<typename T>
    Port<Bundle>& operator<<(const T& field_value) {
        auto& peer_data = peer_->data_;
        if ((getPortRole() == Role::Master)) {
            assert(peer_data.template getRole<T>() == Role::Slave);
        } else {
            assert(peer_data.template getRole<T>() == Role::Master);
        }
        peer_data.template set(field_value);
        this->data_.template set(field_value);
        return *this;
    }

    template<typename T>
    Port<Bundle>& operator >> (T& field_value) {
        auto& peer_data = peer_->data_;
        if ((getPortRole() == Role::Master)) {
            assert(peer_data.template getRole<T>() == Role::Slave);
        } else {
            assert(peer_data.template getRole<T>() == Role::Master);
        }
        field_value = this->data_.template get<T>();
        return *this;
    }


    // 添加 << 操作符用于send data to port
    //Port<Bundle>& operator<<(const Bundle& data) {
        //assert(peer_);
        //using namespace refl;
        //const auto type_info = reflect<Bundle>();
        //for_each(type_info.members, [this, &data](auto member) {
            //if (peer) {
                //auto& peer_data = peer->data_;
                //const auto& field_prop = refl::descriptor::get_attribute<IoProperty>(member);

                //// 如果自己是 Master，写入对方是 Slave 的成员
                //if (role == Role::Master && field_prop.role == Role::Slave) {
                    //member(peer_data) = member(data);
                    //// always 写入自己的成员
                    //member(this->data_) = member(data);
                //}

                //// 如果自己是 Slave，写入对方是 Master 的成员
                //if (role == Role::Slave && field_prop.role == Role::Master) {
                    //member(peer_data) = member(data);
                    //// alway 写入自己的成员
                    //member(this->data_) = member(data);
                //}
            //}
            //// 无论何种角色，总是写入自己的成员
            ////member(this->io_.io_) = member(data);
        //});
        //return *this;
    //}

    // 添加 >> 操作符用于从port读取数据
    //Port<Bundle>& operator>>(Bundle& data)
        //assert(peer);
        //using namespace refl;
        //const auto type_info = reflect<typename Bundle>();
        //for_each(type_info.members, [this, &data](auto member) {
            //auto& peer_data = peer->data_;
            //const auto& field_prop = refl::descriptor::get_attribute<IoProperty>(member);

            //// 如果自己是 Master，从自身的 Slave 成员读取数据
            //if (role == Role::Master && field_prop.role == Role::Slave) {
                //member(data) = member(this->data_);
            //}

            //// 如果自己是 Slave，从对方的 Master 成员读取数据
            //if (role == Role::Slave && field_prop.role == Role::Master) {
                //member(data) = member(peer_data);
            //}
        //});
        //return *this;
    //}

    SelfPtr getPeer() { return peer_; }
private:
    Bundle data_;
    SelfPtr peer_;
};

REFL_AUTO(
        template((typename T), (Port<T>), bases<IPort>)
        );
