#ifndef CAPTURER_VIDEO_PLAYER_H
#define CAPTURER_VIDEO_PLAYER_H

#include <QWidget>
#include "decoder.h"
#include "consumer.h"
#include "dispatcher.h"

class VideoPlayer : public QWidget, public Consumer<AVFrame> {
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget* parent = nullptr);
    ~VideoPlayer() override;

    int run() override { return 0; };
    bool ready() const override { return true; }
    void reset() override { }

    int consume(AVFrame* frame, int type) override;

    bool full(int) const override { return false; }
    void enable(int, bool) override { }
    bool accepts(int type) const override { return type == AVMEDIA_TYPE_VIDEO; }
    int format(int) const override { return AV_PIX_FMT_RGB24; }

    bool play(const std::string& name, const std::string& fmt = "", const std::string& filters = "");

signals:
    void closed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    Decoder* decoder_{ nullptr };
    Dispatcher* dispatcher_{ nullptr };

    std::mutex mtx_;
    AVFrame* frame_{ nullptr };
};

#endif // !CAPTURER_VIDEO_PLAYER_H