/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QWidget>
#include <QTime>

class AnalogClock : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QTime time READ time WRITE setTime)
    Q_PROPERTY(
        bool showSecondsHand READ showSecondsHand WRITE setShowSecondsHand)
    Q_PROPERTY(
        int utcOffset READ utcOffset WRITE setUtcOffset)
    Q_PROPERTY(bool showAmPm READ showAmPm WRITE setShowAmPm)

public:
    AnalogClock(QWidget *parent = nullptr);

    QTime time() const;
    bool showSecondsHand() const;
    int utcOffset() const;
    bool showAmPm() const;

    QSize sizeHint() const override {
        return QSize(200, 200);
    }

    QSize minimumSizeHint() const override {
        return QSize(109, 109);
    }

public slots:
    void setTime(const QTime &time);
    void setShowSecondsHand(bool showSecondsHand);
    bool setTimezone(int value);
    void setUtcOffset(int offset);
    void setShowAmPm(bool value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *) override;

private slots:
    void timerTimeout();

private:
    void paintWindowsVista(QPainter *painter);
    void paintWindows(QPainter *painter);
    void paintDefault(QPainter *painter);

    void paintImg(QPainter *painter, const QString &imgpath);
    void paintHourHand(QPainter *painter, const QPolygon &poly,
                       const QColor &color);
    void paintHourMarkers(QPainter *painter, const QLine &line,
                          const QColor &color);
    void paintMinuteHand(QPainter *painter, const QPolygon &poly,
                         const QColor &color);
    void paintMinuteMarkers(QPainter *painter, const QLine &line,
                            const QColor &color);
    void paintSecondsHand(QPainter *painter, const QPolygon &poly,
                          const QColor &color);
    void paintAmPm(QPainter *painter, int offset);

    QTime m_time;
    QString m_styleName;
    int m_utcOffset        = 0;
    bool m_showSecondsHand = true;
    bool m_showAmPm        = false;

    static int sysUtcOffset;
};

#endif
