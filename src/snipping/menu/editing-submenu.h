#ifndef CAPTURER_EDITING_SUBMENU_H
#define CAPTURER_EDITING_SUBMENU_H

#include "colorpanel.h"
#include "combobox.h"
#include "widthbutton.h"

#include <QCheckBox>
#include <QFont>
#include <QPen>
#include <QWidget>

class EditingSubmenu final : public QWidget
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
    explicit EditingSubmenu(int buttons, QWidget *parent = nullptr);

    //
    void setPen(const QPen& pen, bool silence = true);

    [[nodiscard]] QPen pen() const { return pen_; }

    //
    void setFont(const QFont& font);

    [[nodiscard]] QFont font() const { return font_; }

    //
    [[nodiscard]] bool filled() const;

    void fill(bool);

signals:
    void penChanged(const QPen&);
    void fontChanged(const QFont&);
    void fillChanged(bool);

    void moved();

private:
    WidthButton *width_btn_{};
    QCheckBox *fill_btn_{};
    ComboBox *font_family_{};
    ComboBox *font_size_{};
    ComboBox *font_style_{};
    ColorPanel *color_panel_{};

    QPen pen_{ Qt::red, 3 };
    QFont font_{};
    bool fill_{};
};

#endif //! CAPTURER_EDITING_SUBMENU_H
