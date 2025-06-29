// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// SPDX-License-Identifier: BSD-3-Clause


// Declaration of the QWinHost classes

#ifndef QWINHOST_H
#define QWINHOST_H

#include <QtGlobal>
#if QT_VERSION_MAJOR >= 6
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#endif

#if defined(Q_OS_WIN)
#  if !defined(QT_QTWINMIGRATE_EXPORT) && !defined(QT_QTWINMIGRATE_IMPORT)
#    define QT_QTWINMIGRATE_EXPORT
#  elif defined(QT_QTWINMIGRATE_IMPORT)
#    if defined(QT_QTWINMIGRATE_EXPORT)
#      undef QT_QTWINMIGRATE_EXPORT
#    endif
#    define QT_QTWINMIGRATE_EXPORT __declspec(dllimport)
#  elif defined(QT_QTWINMIGRATE_EXPORT)
#    undef QT_QTWINMIGRATE_EXPORT
#    define QT_QTWINMIGRATE_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTWINMIGRATE_EXPORT
#endif

class QT_QTWINMIGRATE_EXPORT QWinHost : public QWidget
{
    Q_OBJECT
public:
    QWinHost(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QWinHost();

    void setWindow(HWND);
    HWND window() const;

protected:
    virtual HWND createWindow(HWND parent, HINSTANCE instance);

    bool event(QEvent *e);
    void showEvent(QShowEvent *);
    void focusInEvent(QFocusEvent*);
    void resizeEvent(QResizeEvent*);

#if QT_VERSION_MAJOR >= 6
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);
#elif QT_VERSION_MAJOR == 5
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#else
    bool winEvent(MSG *msg, long *result);
#endif

private:
    void fixParent();
    friend void* getWindowProc(QWinHost*);

    void *wndproc;
    bool own_hwnd;
    HWND hwnd;
};

#endif // QWINHOST_H
