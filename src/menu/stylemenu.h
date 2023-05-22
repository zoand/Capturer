#ifndef CAPTURER_STYLE_MENU_H
#define CAPTURER_STYLE_MENU_H

#include "buttongroup.h"
#include "colorpanel.h"
#include "combobox.h"
#include "editmenu.h"
#include "widthbutton.h"

#include <QCheckBox>

class StyleMenu : public EditMenu
{
    Q_OBJECT
public:
    enum
    {
        WIDTH_BTN   = 0x01,
        FILL_BTN    = 0x02,
        COLOR_PENAL = 0x04,
        FONT_BTNS   = 0x08
    };

public:
    explicit StyleMenu(int buttons, QWidget *parent = nullptr);

    void color(const QColor& c) override
    {
        color_ = c;
        if (color_panel_) color_panel_->setColor(color_);
    }

    void lineWidth(int w) override
    {
        width_ = std::max<int>(1, w);
        if (width_btn_) width_btn_->setValue(width_);
    }

    void fill(bool fill) override
    {
        fill_ = fill;

        if (fill_btn_) {
            fill_btn_->setChecked(fill);
        }
        if (width_btn_) {
            width_btn_->setChecked(!fill);
        }
    }

    QFont font() override
    {
        QFont font;
        if (font_family_) font.setFamily(font_family_->currentText());
        if (font_size_) font.setPointSizeF(font_size_->currentText().toFloat());
        if (font_style_) font.setStyleName(font_style_->currentText());
        return font;
    }

    void font(const QFont& font) override
    {
        font_ = font;
        if (font_family_) font_family_->setCurrentText(font.family());
        if (font_size_) font_size_->setCurrentText(QString::number(font.pointSizeF(), 'f', 2));
        if (font_style_) font_style_->setCurrentText(font.styleName());
    }

private:
    WidthButton *width_btn_{ nullptr };
    QCheckBox *fill_btn_{ nullptr };
    ComboBox *font_family_{ nullptr };
    ComboBox *font_size_{ nullptr };
    ComboBox *font_style_{ nullptr };
    ColorPanel *color_panel_{ nullptr };

    const int HEIGHT = 35;
    const int ICON_W = 21;
};

#endif // !CAPTURER_STYLE_MENU_H
