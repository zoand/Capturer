#ifndef CAPTURER_IMAGE_WINDOW_H
#define CAPTURER_IMAGE_WINDOW_H

#include "framelesswindow.h"
#include "menu.h"
#include "menu/editing-menu.h"
#include "texture-widget.h"

#include <any>
#include <memory>
#include <optional>
#include <QStandardPaths>

class ImageWindow final : public FramelessWindow
{
    Q_OBJECT

public:
    explicit ImageWindow(const std::shared_ptr<QMimeData>& data, QWidget *parent = nullptr);

    void preview(const std::shared_ptr<QMimeData>& mimedata);

    std::optional<QPixmap> render(const std::shared_ptr<QMimeData>& mimedata);

    void present(const QPixmap& pixmap);

    static bool is_supported(const QMimeData *);

public slots:
    void paste();
    void open();
    void saveAs();

private:
    void mouseDoubleClickEvent(QMouseEvent *) override;

    void wheelEvent(QWheelEvent *) override;
    void contextMenuEvent(QContextMenuEvent *) override;
    void dropEvent(QDropEvent *) override;
    void dragEnterEvent(QDragEnterEvent *) override;

    void closeEvent(QCloseEvent *) override;

    void registerShortcuts();
    void initContextMenu();

    bool  thumbnail_{ false };
    qreal scale_{ 1.0 };
    qreal opacity_{ 1.0 };

    QSize THUMBNAIL_SIZE_{ 125, 125 };

    QMenu   *context_menu_{ nullptr };
    QAction *zoom_action_{ nullptr };
    QAction *opacity_action_{ nullptr };

    // data
    std::shared_ptr<QMimeData> data_{};
    QPixmap                    pixmap_{};
    TextureWidget             *texture_{};
};

#endif //! CAPTURER_IMAGE_WINDOW_H
