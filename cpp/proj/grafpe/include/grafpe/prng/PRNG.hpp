/*
 * author: Adam Budziak
 */

#pragma once

#include <climits>
#include <utility>
#include <queue>
#include <memory>
#include <bitset>
#include <array>
#include <atomic>
#include <future>
#include <iostream>
#include <functional>

#include <grafpe/common/type_utils.hpp>

namespace grafpe {

/**
 * Class adding some common functionality to any prng_engine_t
 * passed as a class template parameter.
 *
 * It implements idiomatic C++ CRTP for efficient static polymorphism.
 *
 * This class cannot be instantiated directly, only through a deriving class.
 *
 * @tparam Derived - class deriving from PRNG_, implementing
 *          fill_block
 *          fill_bytes
 */
template<typename Derived, typename ctx_type>
class PRNG_ {
 public:
    static constexpr size_t   BIT_COUNT = 16 * CHAR_BIT;
    using block_type = typename ctx_type::block_type;

 private:
    size_t bit_index = BIT_COUNT;
    block_type buffer;

 public:
    /**
     * Needed for compatibility with PRNG_MT_
     */
    void initialize() {}

    bool fetch_bit() {
        if (bit_index == BIT_COUNT) {
            static_cast<Derived&>(*this).fill_block(buffer);
            bit_index = 0;
        }
        auto res = buffer[bit_index / CHAR_BIT] & (1u << bit_index % CHAR_BIT);
        ++bit_index;
        return static_cast<bool>(res);
    }

 private:
    PRNG_() = default;
    friend Derived;
};


/**
 * Another PRNG engine that uses a separate thread for filling the internal data
 * from the sources.
 *
 * It's purpose is to optimize the process and make it faster.
 *
 * On the contrary, it's usually equal in performance or sometimes even slower.
 *
 * It is equivalent to the single threaded PRNG engine as long as fetch_bit method is
 * considered. The fill array method is not equivalent.
 * It can be used inside the GraphBuilder class instead of the typical single-threaded PRNG.
 * Cannot be used inside Crypter (and shouldn't since it builds many PRNGs all the time
 * and creating threads is costly.
 *
 * This class cannot be directly instantiated, only through a deriving class.
 *
 * @tparam Derived - class that should derive from PRNG_MT_ and implement functions:
 *      fill_block
 *      fill_bytes
 *
 */
template <typename Derived, typename ctx_type>
class PRNG_MT_ {
    static constexpr size_t BUFFERS_CNT = 1024;
    static constexpr size_t BUFFER_SIZE = 16;
    static constexpr size_t   BIT_COUNT = BUFFER_SIZE * 8;

    Derived& derived_this = static_cast<Derived&>(*this);

    using buffer_t       = std::array<uint8_t, BUFFER_SIZE>;
    bool initialized     = false;

    enum class ORDER_TYPE { KILL_ORDER = -1, NO_ORDERS, REFRESH_ORDER };

 public:
    ~PRNG_MT_() {
        if (refresher_.joinable()) {
            std::unique_lock<std::mutex> lck{queue_mtx};
            current_order = ORDER_TYPE::KILL_ORDER;
            lck.unlock();
            queue_wait.notify_one();
            refresher_.join();
        }
    }

    PRNG_MT_(PRNG_MT_<Derived, ctx_type>&& oth) noexcept {
        swap(*this, oth);
    }

    PRNG_MT_& operator=(PRNG_MT_ oth) {
        swap(*this, oth);
        return *this;
    }

    void initialize() {
        if (!initialized) {
            initialized = true;
            refresher_ = std::thread{&PRNG_MT_<Derived, ctx_type>::initialize_refresher_, this};
            refresher_.join();
            refresher_ = std::thread{&PRNG_MT_<Derived, ctx_type>::run_refresher_, this};
        }
    }

    bool fetch_bit() {
        if (bit_index * 2 == BIT_COUNT * BUFFERS_CNT || bit_index == BIT_COUNT * BUFFERS_CNT) {
            order_task_();
        }
        auto res = buffers[bit_index / BIT_COUNT][(bit_index % BIT_COUNT) / 8] & (1u << bit_index % 8);
        bit_index = (bit_index + 1) % (BIT_COUNT * BUFFERS_CNT);
        return static_cast<bool>(res);
    }

    template<typename DerivedT, typename ctx_T>
    friend void swap(PRNG_MT_<DerivedT, ctx_T>& lhs, PRNG_MT_<DerivedT, ctx_T>& rhs);

 private:
    size_t  bit_index  = 0;
    std::array<buffer_t, BUFFERS_CNT> buffers;
    std::mutex                        queue_mtx;
    std::thread                       refresher_;
    ORDER_TYPE current_order = ORDER_TYPE ::NO_ORDERS;
    std::condition_variable queue_wait;

    PRNG_MT_() = default;
    friend Derived;

    void order_task_() {
        auto wait_cond = [this]() { return current_order == ORDER_TYPE ::NO_ORDERS; };
        std::unique_lock<std::mutex> lck{queue_mtx};
        queue_wait.wait(lck, wait_cond);
        current_order = ORDER_TYPE ::REFRESH_ORDER;
        lck.unlock();
        queue_wait.notify_one();
    }

    void initialize_refresher_() {
        std::unique_lock<std::mutex> lck{queue_mtx};
        for (auto& buffer : buffers) {
            derived_this.fill_block(buffer);
        }
    }

    void run_refresher_() {
        size_t  batch_to_refresh = 0;
        bool will_exit = false;
        do {
            {
                std::unique_lock<std::mutex> lck {queue_mtx};
                queue_wait.wait(lck, [this] { return current_order != ORDER_TYPE ::NO_ORDERS; });
                if (current_order == ORDER_TYPE::KILL_ORDER) { will_exit = true; }
                current_order = ORDER_TYPE::NO_ORDERS;
            }
            queue_wait.notify_one();
            if (will_exit) {
                return;
            }

            for (size_t i = 0; i < BUFFERS_CNT / 2; ++i) {
                derived_this.fill_block(buffers[batch_to_refresh + i]);
            }
            batch_to_refresh = batch_to_refresh ? 0 : BUFFERS_CNT / 2;
        } while (true);
    }
};

template<typename Derived, typename ctx_t>
void swap(PRNG_MT_<Derived, ctx_t>& lhs, PRNG_MT_<Derived, ctx_t>& rhs) {
    // TODO(abudziak) assert they're not running
    using std::swap;
    swap(lhs.bit_index, rhs.bit_index);
    swap(lhs.buffers, rhs.buffers);
}

}  // namespace grafpe
