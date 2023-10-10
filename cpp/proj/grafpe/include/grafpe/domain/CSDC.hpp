/*
 * author: Adam Budziak
 */

#pragma once

#include <functional>

#include <grafpe/domain/CycleSlicer.hpp>

namespace grafpe::domain::extension {

template<typename Crypter_T, typename PRNG_T>
class CSDC : public grafpe::domain::targeting::MultiRoundsDT<Crypter_T> {
public:
    using value_type = typename Crypter_T::value_type;
    using incl_fn = std::function<bool(const value_type &)>;
    using map_fn = std::function<value_type(const value_type &, const tweak_t &)>;

    using key_type = typename PRNG_T::key_type;
    using iv_type = typename PRNG_T::iv_type;

private:
    incl_fn source_incl_fn;         // T in the source material
    incl_fn target_incl_fn;         // U in the source material

    map_fn preserve_forward_fn;     // G in the source material
    map_fn preserve_bckward_fn;     // G^-1

    using CycleSlicer_T = grafpe::domain::targeting::CycleSlicer<Crypter_T, PRNG_T>;
    CycleSlicer_T cycle_slicer;

    value_type first(value_type x, const tweak_t& tweak) const {
        while (target_incl_fn(x)) {
            x = preserve_bckward_fn(x, tweak);
        }
        return x;
    }

    value_type last(value_type x, const tweak_t& tweak) const {
        while (source_incl_fn(x)) {
            x = preserve_forward_fn(x, tweak);
        }
        return x;
    }

public:

    CSDC(map_fn preserve_forward_fn, map_fn preserve_bckward_fn,
         incl_fn source_incl_fn, incl_fn target_incl_fn,
         CycleSlicer_T cycle_slicer) :
            targeting::MultiRoundsDT<Crypter_T>(cycle_slicer.default_rounds_cnt),
            source_incl_fn{source_incl_fn},
            target_incl_fn{target_incl_fn},
            preserve_forward_fn{preserve_forward_fn},
            preserve_bckward_fn{preserve_bckward_fn},
            cycle_slicer{cycle_slicer} {}

    CSDC(map_fn preserve_forward_fn, map_fn preserve_bckward_fn,
         incl_fn source_incl_fn, incl_fn target_incl_fn,
         targeting::CS_Seed seed, uint64_t default_rounds_cnt) :
            targeting::MultiRoundsDT<Crypter_T>(default_rounds_cnt),
            source_incl_fn{source_incl_fn},
            target_incl_fn{target_incl_fn},
            preserve_forward_fn{preserve_forward_fn},
            preserve_bckward_fn{preserve_bckward_fn},
            cycle_slicer{std::move(seed),
                         get_cs_incl_fn(), default_rounds_cnt} {}

private:

    typename CycleSlicer_T::incl_fn_type get_cs_incl_fn() const {
        return [this](const value_type &a, const value_type &b) {
            return !target_incl_fn(a) && !target_incl_fn(b);
        };
    }

    value_type encrypt_impl(const Crypter_T &crypter, value_type x,
                            const tweak_t &tweak, std::size_t rounds) const override {
        if (source_incl_fn(x)) {
            return preserve_forward_fn(x, tweak);
        } else if (target_incl_fn(x)) {
            x = first(std::move(x), tweak);
        }
        return cycle_slicer.encrypt(crypter, std::move(x), tweak, rounds);
    }

    value_type decrypt_impl(const Crypter_T &crypter, value_type x,
                            const tweak_t &tweak, std::size_t rounds) const override {
        if (target_incl_fn(x)) {
            return preserve_bckward_fn(x, tweak);
        } else {
            x = cycle_slicer.decrypt(crypter, std::move(x), rounds);
            if (source_incl_fn(x)) {
                return last(std::move(x), tweak);
            }
        }
        return x;
    }

};

}