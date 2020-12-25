#ifndef CAPTURER_MAINWINDOW_H
#define CAPTURER_MAINWINDOW_H

#include <vector>
#include <queue>
#include <memory>
#include <QPixmap>
#include <QSystemTrayIcon>
#include <QMimeData>
#include <QMenu>
#include "screenshoter.h"
#include "imagewindow.h"
#include "screenrecorder.h"
#include "qxtglobalshortcut.h"
#include "screenshoter.h"
#include "gifcapturer.h"
#include "settingdialog.h"
#include "json.hpp"
#include "clipboardhistorywindow.h"

template <typename T, int MAX_SIZE = 100>
class LimitSizeVector : public std::vector<T> {
public:
    void push(const T& value)
    {
        push_back(value);

        if(this->size() > MAX_SIZE) {
            this->erase(this->begin());
        }
    }
};

class Capturer : public QWidget
{
    Q_OBJECT

public:
    explicit Capturer(QWidget *parent = nullptr);
    virtual ~Capturer() override = default;

private slots:
    void pinLastImage();
    void showImages();

    void updateConfig();

    void showMessage(const QString &title, const QString &msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                     int msecs = 10000);

    void clipboardChanged();

private:
    void setupSystemTrayIcon();

    ScreenShoter * sniper_ = nullptr;
    ScreenRecorder * recorder_ = nullptr;
    GifCapturer * gifcptr_ = nullptr;

    LimitSizeVector<std::shared_ptr<ImageWindow>> clipboard_history_;

    QSystemTrayIcon *sys_tray_icon_ = nullptr;
    QMenu * sys_tray_icon_menu_ = nullptr;

    SettingWindow * setting_dialog_ = nullptr;

    // hotkey
    QxtGlobalShortcut *snip_sc_ = nullptr;
    QxtGlobalShortcut *show_pin_sc_ = nullptr;
    QxtGlobalShortcut *pin_sc_ = nullptr;
    QxtGlobalShortcut *gif_sc_ = nullptr;
    QxtGlobalShortcut *video_sc_ = nullptr;

    bool images_visible_ = true;
};

#endif // MAINWINDOW_H
