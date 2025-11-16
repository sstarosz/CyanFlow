#ifndef CF_CORE_BUILDCONFIG_HPP
#define CF_CORE_BUILDCONFIG_HPP

namespace cf::core {

struct BuildConfig {
private:
    static constexpr bool kIsDebugBuild =
#ifdef NDEBUG
        false;
#else
        true;
#endif

    static constexpr bool kIsMSVCCompiler =
#ifdef _MSC_VER
        true;
#else
        false;
#endif
public:
    static constexpr bool isDebugBuild()
    {
        return kIsDebugBuild;
    }

    static constexpr bool isMSVCCompiler()
    {
        return kIsMSVCCompiler;
    }
};

} // namespace cf::core

#endif // CF_CORE_BUILDCONFIG_HPPq