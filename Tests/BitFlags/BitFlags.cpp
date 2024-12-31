/**============================================================================
Name        : BitFlags.cpp
Created on  : 27.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BitFlags.cpp
============================================================================**/

#include "BitFlags.h"

#include <iostream>
#include <bitset>

namespace BitFlags
{
    template <typename T>
    class BitFlags
    {
        using UnderlyingT = std::underlying_type_t<T>;

    public:

        constexpr BitFlags() : flags(static_cast<UnderlyingT>(0)) {
        }

        constexpr explicit BitFlags(T v) : flags(ToUnderlying(v)) {
        }

        constexpr BitFlags(std::initializer_list<T> vs) : BitFlags()
        {
            for (T v : vs) {
                flags |= ToUnderlying(v);
            }
        }

        [[nodiscard]]
        constexpr bool IsSet(T v) const
        {
            return (flags & ToUnderlying(v)) == ToUnderlying(v);
        }

        constexpr void Set(T v)
        {
            flags |= ToUnderlying(v);
        }

        constexpr void Unset(T v)
        {
            flags &= ~ToUnderlying(v);
        }

        constexpr void Clear()
        {
            flags = static_cast<UnderlyingT>(0);
        }

        constexpr explicit operator bool() const
        {
            return flags != static_cast<UnderlyingT>(0);
        }

        friend constexpr BitFlags operator|(BitFlags lhs, T rhs)
        {
            return BitFlags(lhs.flags | ToUnderlying(rhs));
        }

        friend constexpr BitFlags operator|(BitFlags lhs, BitFlags rhs)
        {
            return BitFlags(lhs.flags | rhs.flags);
        }

        friend constexpr BitFlags operator&(BitFlags lhs, T rhs)
        {
            return BitFlags(lhs.flags & ToUnderlying(rhs));
        }

        friend constexpr BitFlags operator&(BitFlags lhs, BitFlags rhs)
        {
            return BitFlags(lhs.flags & rhs.flags);
        }

        friend constexpr BitFlags operator^(BitFlags lhs, T rhs)
        {
            return BitFlags(lhs.flags ^ ToUnderlying(rhs));
        }

        friend constexpr BitFlags operator^(BitFlags lhs, BitFlags rhs)
        {
            return BitFlags(lhs.flags ^ rhs.flags);
        }

        friend constexpr BitFlags& operator|=(BitFlags& lhs, T rhs)
        {
            lhs.flags |= ToUnderlying(rhs);
            return lhs;
        }

        friend constexpr BitFlags& operator|=(BitFlags& lhs, BitFlags rhs)
        {
            lhs.flags |= rhs.flags;
            return lhs;
        }

        friend constexpr BitFlags& operator&=(BitFlags& lhs, T rhs)
        {
            lhs.flags &= ToUnderlying(rhs);
            return lhs;
        }

        friend constexpr BitFlags& operator&=(BitFlags& lhs, BitFlags rhs)
        {
            lhs.flags &= rhs.flags;
            return lhs;
        }

        friend constexpr BitFlags& operator^=(BitFlags& lhs, T rhs)
        {
            lhs.flags ^= ToUnderlying(rhs);
            return lhs;
        }

        friend constexpr BitFlags& operator^=(BitFlags& lhs, BitFlags rhs)
        {
            lhs.flags ^= rhs.flags;
            return lhs;
        }

        friend constexpr BitFlags operator~(const BitFlags& bf)
        {
            return BitFlags(~bf.flags);
        }

        friend constexpr bool operator==(const BitFlags& lhs, const BitFlags& rhs)
        {
            return lhs.flags == rhs.flags;
        }

        friend constexpr bool operator!=(const BitFlags& lhs, const BitFlags& rhs)
        {
            return lhs.flags != rhs.flags;
        }

        friend std::ostream& operator<<(std::ostream& os, const BitFlags& bf)
        {
            // Write out a bitset representation.
            os << std::bitset<sizeof(UnderlyingT) * 8>(bf.flags);
            return os;
        }

        // Construct BitFlags from raw values.
        static constexpr BitFlags FromRaw(UnderlyingT flags)
        {
            return BitFlags(flags);
        }

        // Retrieve the raw underlying flags.
        [[nodiscard]]
        constexpr UnderlyingT ToRaw() const
        {
            return flags;
        }

    private:

        constexpr explicit BitFlags(UnderlyingT flags) : flags(flags) {
        }

        static constexpr UnderlyingT ToUnderlying(T v)
        {
            return static_cast<UnderlyingT>(v);
        }

        UnderlyingT flags;
    };
}

namespace BitFlags::Tests
{
#define LINE std::cout << std::string(180, '-') << std::endl;

    enum class RenderPass : uint8_t
    {
        None = 0,
        Geometry = 1 << 0,
        Lighting = 1 << 1,
        Particles = 1 << 2,
        PostProcess = 1 << 3,
    };

    std::string toString(const RenderPass state)
    {
        switch (state) {
            case RenderPass::None: return "None";
            case RenderPass::Geometry: return "Geometry";
            case RenderPass::Lighting: return "Lighting";
            case RenderPass::Particles: return "Particles";
            case RenderPass::PostProcess: return "PostProcess";
        }
    }

    void checkFlags(const BitFlags<RenderPass>& flags)
    {
        std::cout << "Flags: [ ";
        for (const RenderPass state: { RenderPass::None, RenderPass::Geometry, RenderPass::Lighting,
                                       RenderPass::Particles, RenderPass::PostProcess })
        {
            if (flags.IsSet(state)) {
                std::cout << toString(state) << ' ';
            }
        }
        std::cout << "]\n";
    }

    void test()
    {
        {
            BitFlags<RenderPass> flags;
            checkFlags(flags);
        }
        LINE

        {
            BitFlags<RenderPass> flags(RenderPass::Lighting);
            checkFlags(flags);
        }
        LINE

        {
            BitFlags<RenderPass> flags = { RenderPass::Particles,
                                           RenderPass::PostProcess };
            checkFlags(flags);
        }
        LINE

        {
            constexpr BitFlags<RenderPass> flags = { RenderPass::Geometry,
                                                     RenderPass::Lighting};
            checkFlags(flags);
        }
        LINE

        // Combine with OR.
        {
            BitFlags<RenderPass> lightingSet (RenderPass::Lighting);
            BitFlags<RenderPass> flags = lightingSet | RenderPass::Particles;
            checkFlags(flags);
        }
        LINE

        // Toggle with XOR.
        {
            BitFlags<RenderPass> flags = { RenderPass::Lighting,
                                           RenderPass::Particles};
            checkFlags(flags);

            flags ^= RenderPass::Lighting;
            checkFlags(flags);
        }
        LINE

        // Clear a set of flags with AND NOT.
        {
            constexpr BitFlags<RenderPass> lighting_and_particles = { RenderPass::Lighting,
                                                                      RenderPass::Particles };
            BitFlags<RenderPass> flags = { RenderPass::Geometry,
                                           RenderPass::Lighting };
            checkFlags(lighting_and_particles);
            checkFlags(flags);

            flags &= ~lighting_and_particles;
            checkFlags(flags);
        }
    }
}

void BitFlags::TestAll()
{
    Tests::test();

}