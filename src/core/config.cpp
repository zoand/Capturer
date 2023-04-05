#include "config.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include "utils.h"
#include "platform.h"
#include "devices.h"
#include "logging.h"

Config::Config()
{
    auto config_dir_path_ = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir config_dir(config_dir_path_);
    if(!config_dir.exists()) {
        config_dir.mkpath(config_dir_path_);
    }
    filepath_ = config_dir_path_ + "/config.json";
    LOG(INFO) << "config file path: " << filepath_;

    QString text;
    QFile config_file(filepath_);
    if(config_file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&config_file);
        text = in.readAll();
    }

    try {
        settings_ = json::parse(text.toStdString());
    }
    catch (json::parse_error&) {
        settings_ = json::parse("{}");
    }

    // default
    IF_NULL_SET(settings_["autorun"],                               true);
    IF_NULL_SET(settings_["language"],                              "zh_CN");
    IF_NULL_SET(settings_["detectwindow"],                          true);
    IF_NULL_SET(settings_["theme"],                                 "auto");

    IF_NULL_SET(settings_["snip"]["selector"]["border"]["width"],   2);
    IF_NULL_SET(settings_["snip"]["selector"]["border"]["color"],   "#409EFF");
    IF_NULL_SET(settings_["snip"]["selector"]["border"]["style"],   Qt::DashDotLine);
    IF_NULL_SET(settings_["snip"]["selector"]["mask"]["color"],     "#88000000");

    IF_NULL_SET(settings_["record"]["selector"]["border"]["width"], 2);
    IF_NULL_SET(settings_["record"]["selector"]["border"]["color"], "#ffff5500");
    IF_NULL_SET(settings_["record"]["selector"]["border"]["style"], Qt::DashDotLine);
    IF_NULL_SET(settings_["record"]["selector"]["mask"]["color"],   "#88000000");
    IF_NULL_SET(settings_["record"]["encoder"],                     "libx264");
    IF_NULL_SET(settings_["record"]["quality"],                     "medium");
    IF_NULL_SET(settings_["record"]["box"],                         false);

    IF_NULL_SET(settings_["gif"]["selector"]["border"]["width"],    2);
    IF_NULL_SET(settings_["gif"]["selector"]["border"]["color"],    "#ffff00ff");
    IF_NULL_SET(settings_["gif"]["selector"]["border"]["style"],    Qt::DashDotLine);
    IF_NULL_SET(settings_["gif"]["selector"]["mask"]["color"],      "#88000000");
    IF_NULL_SET(settings_["gif"]["quality"],                        "medium");
    IF_NULL_SET(settings_["gif"]["box"],                            false);

    IF_NULL_SET(settings_["snip"]["hotkey"],                        "F1");
    IF_NULL_SET(settings_["pin"]["hotkey"],                         "F3");
    IF_NULL_SET(settings_["pin"]["visiable"]["hotkey"],             "Shift+F3");
    IF_NULL_SET(settings_["record"]["hotkey"],                      "Ctrl+Alt+V");
    IF_NULL_SET(settings_["gif"]["hotkey"],                         "Ctrl+Alt+G");

    IF_NULL_SET(settings_["record"]["framerate"],                   30);
    IF_NULL_SET(settings_["gif"]["framerate"],                      6);

    if(Devices::cameras().size() > 0)
        settings_["devices"]["cameras"] = Devices::cameras()[0];

    if (Devices::microphones().size() > 0)
        settings_["devices"]["microphones"] = Devices::microphones()[0];

    if (Devices::speakers().size() > 0)
        settings_["devices"]["speakers"] = Devices::speakers()[0];

    connect(this, &Config::changed, this, &Config::save);
    connect(this, &Config::SYSTEM_THEME_CHANGED, this, [this](int theme) { 
        load_theme(platform::system::theme_name(static_cast<platform::system::theme_t>(theme)));
    });

    monitor_system_theme(settings_["theme"].get<std::string>() == "auto");
}

void Config::save()
{
    QFile file(filepath_);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << settings_.dump(4).c_str();

    file.close();
}

std::string Config::theme()
{
    auto theme = Config::instance()["theme"].get<std::string>();
    if (theme == "auto") {
        return platform::system::theme_name(platform::system::theme());
    }

    return (theme == "dark") ? "dark" : "light";
}

void Config::monitor_system_theme(bool m)
{
#ifdef _WIN32
    if (m && win_theme_monitor_ == nullptr) {
        win_theme_monitor_ = platform::windows::monitor_regkey(
            HKEY_CURRENT_USER,
            R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
            [this](auto) {
                emit SYSTEM_THEME_CHANGED(static_cast<int>(platform::system::theme()));
            }
        );
    }
    
    if(!m) {
        win_theme_monitor_ = nullptr;
    }
#endif
}

void Config::set_theme(const std::string& theme)
{
    if (settings_["theme"].get<std::string>() == theme)
        return;

    set(settings_["theme"], theme);

    monitor_system_theme(theme == "auto");

    load_theme(Config::theme());
}

void Config::load_theme(const std::string& theme)
{
    static std::string _theme = "unknown";
    if (_theme != theme) {
        _theme = theme;

        LOAD_QSS(qApp,
            {
                ":/qss/capturer.qss",
                ":/qss/capturer-" + QString::fromStdString(theme) + ".qss",
                ":/qss/menu/menu.qss",
                ":/qss/menu/menu-" + QString::fromStdString(theme) + ".qss",
                ":/qss/setting/settingswindow.qss",
                ":/qss/setting/settingswindow-" + QString::fromStdString(theme) + ".qss"
            }
        );
    }
}