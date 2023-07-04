#ifdef _WIN32

#include "framelesswindow.h"
#include "logging.h"

#include <dwmapi.h>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <windowsx.h>

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::FramelessWindowHint)
{
    auto hwnd = reinterpret_cast<HWND>(winId());

    // shadow
    DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
    ::DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(DWMNCRENDERINGPOLICY));
    MARGINS margins = { -1, -1, -1, -1 };
    ::DwmExtendFrameIntoClientArea(hwnd, &margins);

    // window animation
    auto style = ::GetWindowLong(hwnd, GWL_STYLE);
    ::SetWindowLong(hwnd, GWL_STYLE,
                    style | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | CS_DBLCLKS);
}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    if (ReleaseCapture())
        SendMessage(reinterpret_cast<HWND>(winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
    event->ignore();
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event) { return QWidget::mouseReleaseEvent(event); }

void FramelessWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    return QWidget::mouseDoubleClickEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event) { return QWidget::mouseMoveEvent(event); }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool FramelessWindow::nativeEvent(const QByteArray& eventType, void *message, qintptr *result)
#else
bool FramelessWindow::nativeEvent(const QByteArray& eventType, void *message, long *result)
#endif
{
    if (!message || !result || !reinterpret_cast<MSG *>(message)->hwnd) return false;

    auto wmsg = reinterpret_cast<MSG *>(message);
    auto hwnd = wmsg->hwnd;

    switch (wmsg->message) {
    case WM_NCCALCSIZE:
        *result = 0;
        return true;
        // resize
    case WM_NCHITTEST: {
        RECT rect{};
        if (!::GetWindowRect(hwnd, &rect)) return false;

        auto x = GET_X_LPARAM(wmsg->lParam);
        auto y = GET_Y_LPARAM(wmsg->lParam);

        const auto xthickness = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
        const auto ythickness = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

        auto hl = x >= rect.left && x < rect.left + xthickness;
        auto ht = y >= rect.top && y < rect.top + ythickness;
        auto hr = x >= rect.right - xthickness && x < rect.right;
        auto hb = y >= rect.bottom - ythickness && y < rect.bottom;

        if (hl && ht) {
            *result = HTTOPLEFT;
            return true;
        }

        if (hr && ht) {
            *result = HTTOPRIGHT;
            return true;
        }

        if (hl && hb) {
            *result = HTBOTTOMLEFT;
            return true;
        }

        if (hr && hb) {
            *result = HTBOTTOMRIGHT;
            return true;
        }

        if (hl) {
            *result = HTLEFT;
            return true;
        }

        if (hr) {
            *result = HTRIGHT;
            return true;
        }

        if (ht) {
            *result = HTTOP;
            return true;
        }
        if (hb) {
            *result = HTBOTTOM;
            return true;
        }

        break;
    }

    default: break;
    }

    return QWidget::nativeEvent(eventType, message, result);
}

#endif