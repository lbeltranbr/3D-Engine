#pragma once
#include <queue>
#include <vector>
#include <string>
#include <bitset>
#include <memory>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <unordered_map>

// include glew
#define GLEW_STATIC
#include <GL/glew.h>

// include entt
#include <../entt/entt.hpp>

// include magic enum
#include <../magic_enum/magic_enum.hpp>

// include glm
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/matrix_decompose.hpp>
#include <ext/matrix_transform.hpp>
#include <ext/matrix_clip_space.hpp>

// include spdlog (only on debug mode)
#ifdef  _DEBUG
    #define ENABLE_SPDLOG
    #define FMT_HEADER_ONLY 
    #define SPDLOG_FMT_EXTERNAL
    #include <spdlog/spdlog.h>
    #include <spdlog/sinks/stdout_color_sinks.h>
#endif

// import, export
#ifdef SP_EXPORT
#ifdef _MSC_VER
#define SP_API __declspec(dllexport)
#else
#define SP_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define SP_API __declspec(dllimport)
#else
#define SP_API
#endif
#endif

// runtime assertion
#define SP_ASSERT assert

// compile assertion
#if defined(__clang__) || defined(__gcc__)
#define SP_STATIC_ASSERT _Static_assert
#else
#define SP_STATIC_ASSERT static_assert
#endif

// function inlining
#if defined(__clang__) || defined(__gcc__)
#define SP_INLINE __attribute__((always_inline)) inline
#define SP_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define SP_INLINE __forceinline
#define SP_NOINLINE __declspec(noinline)
#else
#define SP_INLINE inline
#define SP_NOINLINE
#endif

//configuration macros
#ifdef  ENABLE_SPDLOG
    #define ENABLE_COMMENTS 1
#endif

// core features
namespace SP
{
    // entity identifier
    using EntityID = entt::entity;
    // null entity
    constexpr EntityID NENTT = entt::null;
    // entity registry
    using EntityRegistry = entt::registry;

    // runtime type
    template <typename T>
    SP_INLINE constexpr uint32_t TypeID()
    {
        return static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&typeid(T)));
    }
#ifdef ENABLE_SPDLOG
    // console logging
    struct SP_API Logger
    {
        using SPDLog = std::shared_ptr<spdlog::logger>;

        SP_INLINE Logger()
        {
            m_SPD = spdlog::stdout_color_mt("stdout");
            spdlog::set_level(spdlog::level::trace);
            spdlog::set_pattern("%^[%T]: [#%t] %v%$");
        }

        SP_INLINE static SPDLog& Ref()
        {
            static Logger logger;
            return logger.m_SPD;
        }

    private:
        SPDLog m_SPD;
    };
#endif
}

// free allocated memory
#define SP_DELETE(ptr) if (ptr != nullptr) { delete (ptr); ptr = nullptr; }