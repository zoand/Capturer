#ifndef CAPTURER_CONSUMER_H
#define CAPTURER_CONSUMER_H

#include <thread>
#include <mutex>
#include <atomic>
extern "C" {
#include <libavutil/rational.h>
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
}
#include "clock.h"
#include "media.h"

template<class T>
class Consumer {
public:
    Consumer() = default;
    Consumer(const Consumer&) = delete;
    Consumer& operator=(const Consumer&) = delete;
    virtual ~Consumer() {
        std::lock_guard lock(mtx_);

        running_ = false;
        eof_ = 0x00;
        ready_ = false;

        if (thread_.joinable()) {
            thread_.join();
        }
    }

    virtual int run() = 0;
    virtual int consume(T*, int) = 0;

    virtual void stop() { running_ = false; reset(); }
    virtual void reset() = 0;
    virtual int wait()
    {
        if (thread_.joinable()) {
            thread_.join();
        }
        return 0;
    }

    [[nodiscard]] virtual bool full(int) const = 0;
    [[nodiscard]] virtual bool accepts(int) const = 0;
    virtual void enable(int, bool = true) = 0;

    [[nodiscard]] virtual bool ready() const { return ready_; }
    [[nodiscard]] bool running() const { return running_; }
    [[nodiscard]] virtual bool eof() const { return eof_; }

    vformat_t vfmt;
    aformat_t afmt;

protected:
    std::atomic<bool> running_{ false };
    std::atomic<uint8_t> eof_{ 0x00 };
    std::atomic<bool> ready_{ false };
    std::thread thread_;
    std::mutex mtx_;

    bool is_cfr_{ false };
    enum AVHWDeviceType hwaccel_ { AV_HWDEVICE_TYPE_NONE };
};

#endif // !CAPTURER_CONSUMER_H
