#ifndef CAPTURER_DISPATCHER_H
#define CAPTURER_DISPATCHER_H

#include <thread>
#include <future>
#include <tuple>
#include "utils.h"
#include "producer.h"
#include "consumer.h"

extern "C" {
#include <libavutil/time.h>
#include <libavfilter/avfilter.h>
}

class Dispatcher {
public:
    explicit Dispatcher() = default;
    explicit Dispatcher(const std::string_view& video_graph_desc, const std::string_view& audio_graph_desc) 
        : video_graph_desc_(video_graph_desc), audio_graph_desc_(audio_graph_desc) { }

    ~Dispatcher() { reset(); }

    void append(Producer<AVFrame>* decoder)
    {
        if (decoder->has(AVMEDIA_TYPE_AUDIO)) {
            audio_producers_.emplace_back(decoder);
        }

        if (decoder->has(AVMEDIA_TYPE_VIDEO)) {
            video_producers_.emplace_back(decoder);
        }
    }

    void set_encoder(Consumer<AVFrame>* encoder)
    {
        consumer_ = encoder;
    }

    int create_filter_graph(const std::string_view& video_filters, const std::string_view& audio_filters);

    int start();
    void pause();
    void resume();
    int reset();
    void stop() { reset(); }

    [[nodiscard]] bool running() const { return running_; }

    [[nodiscard]] int64_t escaped_us() const;

    [[nodiscard]] int64_t escaped_ms() const { return escaped_us() / 1000; }

    [[nodiscard]] bool has_audio() const { return has_audio_in_; }

    AVFilterContext* find_sink(Consumer<AVFrame>*, enum AVMediaType);

private:
    int create_filter_for_input(const Producer<AVFrame>*, AVFilterContext**, enum AVMediaType);
    int create_filter_for_output(const Consumer<AVFrame>*, AVFilterContext**, enum AVMediaType);

    int create_filter_for_video_input(const Producer<AVFrame>*, AVFilterContext**);
    int create_filter_for_video_output(const Consumer<AVFrame>*, AVFilterContext**);
    
    int create_filter_for_audio_input(const Producer<AVFrame>*, AVFilterContext**);
    int create_filter_for_audio_output(const Consumer<AVFrame>*, AVFilterContext**);
    int dispatch_thread_f();

    int create_filter_graph_for(const std::vector<Producer<AVFrame>*>& producers, const std::string& desc, enum AVMediaType type);

    // clock @{
    int64_t first_pts_{ AV_NOPTS_VALUE };
    int64_t paused_pts_{ AV_NOPTS_VALUE };
    int64_t offset_pts_{ 0 };               // AV_TIME_BASE_Q unit
    //@}

    std::atomic<bool> running_{ false };
    std::atomic<bool> paused_{ false };
    std::atomic<bool> ready_{ false };
    std::atomic<bool> has_audio_in_{ false };

    std::vector<Producer<AVFrame>*> audio_producers_;
    std::vector<Producer<AVFrame>*> video_producers_;
    Consumer<AVFrame>* consumer_;

    std::vector<std::tuple<AVFilterContext*, Producer<AVFrame>*, bool>> i_streams_;
    std::vector<std::tuple<AVFilterContext*, Consumer<AVFrame>*, bool>> o_streams_;

    std::string video_graph_desc_{ };
    std::string audio_graph_desc_{ };

    AVFilterGraph* filter_graph_{ nullptr };

    std::thread dispatch_thread_;
};

#endif // !CAPTURER_DISPATCHER_H