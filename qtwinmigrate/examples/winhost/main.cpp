// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// SPDX-License-Identifier: BSD-3-Clause

#include <QtGlobal>
#include <windows.h>

#if QT_VERSION_MAJOR >= 6
#include <QtGui/QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#else
#include <QApplication>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <qwinhost.h>
#endif


#if QT_VERSION_MAJOR >= 6
class HostWindow : public QWidget {
    Q_OBJECT
public:
    HostWindow(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QWidget(parent, f) {
        setFocusPolicy(Qt::StrongFocus);
        initWin32Window();
    }

signals:
    void message(const QString &msg, int timeout);

public slots:
    void returnPressed() {
        QMessageBox::information(topLevelWidget(), "Message from Qt", "Return pressed in QLineEdit!");
    }

private:
    HWND nativeHwnd = nullptr;
    QWindow *nativeWindow = nullptr;
    QWidget *container = nullptr;

    void initWin32Window()
    {
        static ATOM windowClass = 0;
        HINSTANCE instance = GetModuleHandle(nullptr);

        if (!windowClass) {
            WNDCLASSEX wcex;
            wcex.cbSize         = sizeof(WNDCLASSEX);
            wcex.style          = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc    = (WNDPROC)WndProc;
            wcex.cbClsExtra     = 0;
            wcex.cbWndExtra     = 0;
            wcex.hInstance      = instance;
            wcex.hIcon          = NULL;
            wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
            wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
            wcex.lpszMenuName   = NULL;
            wcex.lpszClassName  = L"qtest";
            wcex.hIconSm        = NULL;

            windowClass = RegisterClassEx(&wcex);
        }

        nativeHwnd = CreateWindowEx(0, L"qtest", L"",
                                    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP,
                                    0, 0, 200, 100,
                                    (HWND)winId(), NULL, instance, NULL);

        nativeWindow = QWindow::fromWinId((WId)nativeHwnd);
        container = QWidget::createWindowContainer(nativeWindow, this);
        container->setFocusPolicy(Qt::StrongFocus);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(container);
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        QWidget *widget = QWidget::find((WId)GetParent(hWnd));
        HostWindow *window = qobject_cast<HostWindow*>(widget);

        if (window) switch (message) {
            case WM_SETFOCUS:
                window->message("SetFocus for Win32 window!", 1000);
                break;
            case WM_KILLFOCUS:
                window->message("KillFocus for Win32 window!", 1000);
                break;
            case WM_MOUSEMOVE:
                window->message("Moving the mouse, aren't we?", 200);
                break;
            case WM_KEYDOWN:
                if (wParam != VK_TAB)
                    window->message("Key Pressed!", 500);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        return 0;
    }
};
#else
class HostWindow : public QWinHost {
    Q_OBJECT
public:
    HostWindow(QWidget *parent = 0, Qt::WindowFlags f = 0) : QWinHost(parent, f) {
        setFocusPolicy(Qt::StrongFocus);
    }

    HWND createWindow(HWND parent, HINSTANCE instance) {
        static ATOM windowClass = 0;
        if (!windowClass) {
            WNDCLASSEX wcex;
            wcex.cbSize		= sizeof(WNDCLASSEX);
            wcex.style		= CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc	= (WNDPROC)WndProc;
            wcex.cbClsExtra	= 0;
            wcex.cbWndExtra	= 0;
            wcex.hInstance	= instance;
            wcex.hIcon		= NULL;
            wcex.hCursor	= LoadCursor(NULL, IDC_ARROW);
            wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
            wcex.lpszMenuName	= NULL;
            wcex.lpszClassName	= L"qtest";
            wcex.hIconSm	= NULL;

            windowClass = RegisterClassEx(&wcex);
        }

        HWND hwnd = CreateWindow((TCHAR*)windowClass, 0, WS_CHILD|WS_CLIPSIBLINGS|WS_TABSTOP,
                                 CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, parent, NULL, instance, NULL);

        return hwnd;
    }

signals:
    void message(const QString &msg, int timeout);

public slots:
    void returnPressed()
    {
        QMessageBox::information(topLevelWidget(), "Message from Qt", "Return pressed in QLineEdit!");
    }

protected:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        QWidget *widget = QWidget::find((WId)GetParent(hWnd));
        HostWindow *window = qobject_cast<HostWindow*>(widget);

        if (window) switch (message) {
            case WM_SETFOCUS:
                window->message("SetFocus for Win32 window!", 1000);
                break;
            case WM_KILLFOCUS:
                window->message("KillFocus for Win32 window!", 1000);
                break;
            case WM_MOUSEMOVE:
                window->message("Moving the mouse, aren't we?", 200);
                break;
            case WM_KEYDOWN:
                if (wParam != VK_TAB)
                    window->message("Key Pressed!", 500);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        return 0;
    }
};
#endif


#include "main.moc"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    QMainWindow mw;
    mw.menuBar()->addMenu("&File")->addAction("&Exit", &a, SLOT(quit()));

    QWidget central(&mw);

    QLineEdit edit(&central);
    HostWindow host(&central);
    QObject::connect(&host, SIGNAL(message(const QString&,int)), mw.statusBar(), SLOT(showMessage(const QString&,int)));
    QObject::connect(&edit, SIGNAL(returnPressed()), &host, SLOT(returnPressed()));

    QVBoxLayout vbox(&central);
    vbox.addWidget(&edit);
    vbox.addWidget(&host);

    mw.setCentralWidget(&central);
    mw.show();
    return a.exec();
}
