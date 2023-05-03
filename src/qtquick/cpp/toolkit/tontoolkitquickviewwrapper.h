/*
    Copyright (C) 2017 TonToolkit Team
    http://tontoolkit.co

    TonToolkitQtTools is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TonToolkitQtTools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TONTOOLKITQUICKVIEWWRAPPER_H
#define TONTOOLKITQUICKVIEWWRAPPER_H

#include <QObject>
#include "tontoolkitquickview.h"

class TonToolkitQuickViewWrapper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool reverseScroll READ reverseScroll WRITE setReverseScroll NOTIFY reverseScrollChanged)

    Q_PROPERTY(QObject*    root        READ root        WRITE setRoot        NOTIFY rootChanged)

    Q_PROPERTY(QString offlineStoragePath READ offlineStoragePath WRITE setOfflineStoragePath NOTIFY offlineStoragePathChanged)

    Q_PROPERTY(qreal flickVelocity READ flickVelocity NOTIFY fakeSignal)
    Q_PROPERTY(QWindow* window READ window NOTIFY fakeSignal)

public:
    TonToolkitQuickViewWrapper(TonToolkitQuickView *view, QObject *parent = Q_NULLPTR);
    virtual ~TonToolkitQuickViewWrapper();

    void setReverseScroll(bool stt);
    bool reverseScroll() const;

    void setRoot( QObject *root );
    QObject *root() const;

    void setBackController(bool stt);
    bool backController() const;

    qreal flickVelocity() const;

    QWindow *window() const;

    void setOfflineStoragePath(const QString &path);
    QString offlineStoragePath() const;

    Q_INVOKABLE void registerWindow(QQuickWindow *window);

Q_SIGNALS:
    void fullscreenChanged();
    void rootChanged();
    void backControllerChanged();
    void reverseScrollChanged();
    void fakeSignal();
    void closeRequest();
    void offlineStoragePathChanged();

private Q_SLOTS:
    void viewDestroyed();

private:
    TonToolkitQuickView *mView;
};

#endif // TONTOOLKITQUICKVIEWWRAPPER_H
