#pragma once

#include <string>

#include "world/anatomy/types.hpp"

namespace robot::world::anatomy {

class SkeletonBuilder;

class Part {
   public:
    enum class AttachState { Detached, Attached, Failed };

    virtual ~Part() = default;

    Part(const Part&) = delete;
    Part& operator=(const Part&) = delete;
    Part(Part&&) = delete;
    Part& operator=(Part&&) = delete;

    AttachState state() const { return _state; }

    void attachTo(SkeletonBuilder& builder, Socket socket);
    const std::string& name() const { return _name; }

   protected:
    Part(const std::string& name) : _name(name), _state(AttachState::Detached) {}

    virtual void onAttach(SkeletonBuilder& builder, Socket socket) = 0;

    void requireAttached() const;

   private:
    std::string _name;
    AttachState _state;
};

}  // namespace robot::world::anatomy
