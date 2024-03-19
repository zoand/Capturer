#ifndef CAPTURER_RECORD_MENU_H
#define CAPTURER_RECORD_MENU_H

#include "framelesswindow.h"

#include <QCheckBox>
#include <QLabel>

class RecordingMenu final : public FramelessWindow
{
    Q_OBJECT

public:
    enum : uint8_t
    {
        DEFAULT = 0x00,
        AUDIO   = 0x01,
        ALL     = 0xff
    };

    explicit RecordingMenu(bool, bool, uint8_t = ALL, QWidget *parent = nullptr);

signals:
    void started();
    void paused();
    void resumed();
    void muted(int, bool);
    void stopped();

public slots:
    void start();
    void time(const std::chrono::seconds&);
    void mute(int, bool);

    void disable_mic(bool);
    void disable_speaker(bool);

protected:
    void showEvent(QShowEvent *event) override;

private:
    QCheckBox *mic_btn_{};
    QCheckBox *speaker_btn_{};
    QCheckBox *pause_btn_{};
    QCheckBox *close_btn_{};

    QLabel *time_label_{};
};

#endif //! CAPTURER_RECORD_MENU_H
