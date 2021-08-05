#pragma once

#include <variant>

#include "../../param/Param.hpp"
#include "ParamEntity.hpp"

namespace JSettings {
    using ParamVariant_t = std::variant<Param<int>, Param<std::string>>;
    using ParamsMap_t = std::unordered_map<std::string, ParamVariant_t>;
    /**
     * Provides friendly interface for storing/retrieving
     * parameters from storages that keeps parameters
     * between launches.
     */
    class PersistentTable {
    public:
        virtual ~PersistentTable() {};
        virtual ParamVariant_t read(const std::string_view name) = 0;
        virtual ParamsMap_t readAll() = 0;
        virtual void write(const ParamEntity& param) = 0;
    };
};