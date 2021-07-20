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

#include "analogclock.h"

#include <QPainter>
#include <QTimer>
#include <QStyle>
#include <QApplication>

int AnalogClock::sysUtcOffset = 0;

AnalogClock::AnalogClock(QWidget *parent) : QWidget(parent) {
    m_styleName = qApp->style()->objectName();
    m_time      = QTime::currentTime();

    QDateTime local(QDateTime::currentDateTime());
    sysUtcOffset = local.offsetFromUtc();
    m_utcOffset  = sysUtcOffset;

    QTimer *timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);

    connect(timer, &QTimer::timeout, this,
            QOverload<>::of(&AnalogClock::timerTimeout));

    const int lastSecond  = QTime::currentTime().second();
    QTimer   *offsetTimer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(offsetTimer, &QTimer::timeout,
            [this, timer, offsetTimer, lastSecond]() {
        if (QTime::currentTime().second() != lastSecond) {
            setTime(m_time.addSecs(1));
            timer->start(1000);
            offsetTimer->deleteLater();
        }
    });
    offsetTimer->start(1);

    setWindowTitle(tr("Analog Clock"));
}

QTime AnalogClock::time() const {
    return m_time;
}

void AnalogClock::setTime(const QTime &time) {
    m_time = time;
    update();
}

void AnalogClock::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    const int side = qMin(width(), height());

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    /* Shift the time according to the difference between
     * the system and the local timezones */
    const int actualOffsetDiff = m_utcOffset - sysUtcOffset;
    m_time = m_time.addSecs(actualOffsetDiff);

    if (m_styleName == QStringLiteral("windowsvista"))
        paintWindowsVista(&painter);
    else if (m_styleName == QStringLiteral("windows"))
        paintWindows(&painter);
    else
        paintDefault(&painter);

    /* Revert */
    m_time = m_time.addSecs(-actualOffsetDiff);
}

void AnalogClock::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    if (e->type() == QEvent::StyleChange) {
        m_styleName = qApp->style()->objectName();
        update();
    }
}

void AnalogClock::timerTimeout() {
    m_time = m_time.addSecs(1);
    update();
}

void AnalogClock::paintWindowsVista(QPainter *painter) {
    paintImg(painter, QStringLiteral(":/windowsvista/clock.png"));

    const int offset = 2;

    painter->translate(-offset, -offset);

    static const QPolygon hourHand({
        QPoint(1, 0),
        QPoint(1, -50),
        QPoint(-1, -50),
        QPoint(-1, 0)
    });
    static const QPolygon minuteHand({
        QPoint(1, 0),
        QPoint(1, -73),
        QPoint(-1, -73),
        QPoint(-1, 0)
    });
    static const QPolygon secondsHand(QVector<QPoint>({
        QPoint(0, 20),
        QPoint(0, -73)
    }));

    const QColor hourColor(89, 107, 15);

    if (m_showAmPm)
        paintAmPm(painter, 40);

    paintHourHand(painter, hourHand, hourColor);

    paintMinuteHand(painter, minuteHand, Qt::darkGray);

    if (m_showSecondsHand) {
        paintSecondsHand(painter, secondsHand, Qt::gray);
    }

    painter->translate(offset, offset);

    paintImg(painter, QStringLiteral(":/windowsvista/clock_center.png"));
}

void AnalogClock::paintWindows(QPainter *painter) {
    static const QPolygon hourHand({
        QPoint(7, 0),
        QPoint(0, 15),
        QPoint(-7, 0),
        QPoint(0, -55)
    });
    static const QPolygon minuteHand({
        QPoint(7, 0),
        QPoint(0, 15),
        QPoint(-7, 0),
        QPoint(0, -70)
    });
    static const QPolygon secondsHand(QVector<QPoint>({
        QPoint(0, 25),
        QPoint(0, -70)
    }));

    const QColor color(0, 125, 123);
    const QColor shadowColor(127, 127, 127, 127);

    const int shadowOffset = 3;

    paintImg(painter, QStringLiteral(":/windows/clock.png"));

    if (m_showAmPm)
        paintAmPm(painter, 50);

    painter->translate(shadowOffset, shadowOffset);
    painter->setPen(Qt::NoPen);

    paintHourHand(painter, hourHand, shadowColor);
    paintMinuteHand(painter, minuteHand, shadowColor);
    if (m_showSecondsHand) {
        paintSecondsHand(painter, secondsHand, shadowColor);
    }

    painter->translate(-shadowOffset, -shadowOffset);

    const QPen pen(Qt::white, 0);

    painter->setPen(pen);
    paintMinuteHand(painter, minuteHand, color);
    painter->setPen(pen);
    paintHourHand(painter, hourHand, color);

    if (m_showSecondsHand) {
        paintSecondsHand(painter, secondsHand, Qt::black);
    }
}

void AnalogClock::paintDefault(QPainter *painter) {
    static const QPolygon hourHand({
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -40)
    });
    static const QPolygon minuteHand({
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -70)
    });
    static const QPolygon secondsHand(QVector<QPoint>({
        QPoint(0, 25),
        QPoint(0, -85)
    }));

    static const QLine hourMarker(88, 0, 96, 0);
    static const QLine minuteMarker(92, 0, 96, 0);

    const QColor hourColor(127, 0, 127);
    const QColor minuteColor(0, 127, 127, 191);

    if (m_showAmPm)
        paintAmPm(painter, 50);

    paintHourHand(painter, hourHand, hourColor);
    paintHourMarkers(painter, hourMarker, hourColor);

    paintMinuteHand(painter, minuteHand, minuteColor);
    paintMinuteMarkers(painter, minuteMarker, hourColor);

    if (m_showSecondsHand) {
        paintSecondsHand(painter, secondsHand, Qt::black);
    }
}

void AnalogClock::paintImg(QPainter *painter, const QString &imgpath) {
    painter->drawPixmap(-100, -100, 200, 200, QPixmap(imgpath));
}

void AnalogClock::paintHourHand(QPainter *painter, const QPolygon &poly,
                                const QColor &color) {
    painter->setBrush(color);

    painter->save();
    painter->rotate(30.0 * ((m_time.hour() + m_time.minute() / 60.0)));
    painter->drawConvexPolygon(poly);
    painter->restore();

    painter->setPen(Qt::NoPen);
}

void AnalogClock::paintHourMarkers(QPainter *painter, const QLine &line,
                                   const QColor &color) {
    painter->setPen(color);
    for (int i = 0; i < 12; ++i) {
        painter->drawLine(line);
        painter->rotate(30.0);
    }

    painter->setPen(Qt::NoPen);
}

void AnalogClock::paintMinuteHand(QPainter *painter, const QPolygon &poly,
                                  const QColor &color) {
    painter->setBrush(color);

    painter->save();
    painter->rotate(6.0 * (m_time.minute() + m_time.second() / 60.0));
    painter->drawConvexPolygon(poly);
    painter->restore();

    painter->setPen(Qt::NoPen);
}
void AnalogClock::paintMinuteMarkers(QPainter *painter, const QLine &line,
                                     const QColor &color) {
    painter->setPen(color);

    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0)
            painter->drawLine(line);
        painter->rotate(6.0);
    }
}
void AnalogClock::paintSecondsHand(QPainter *painter, const QPolygon &poly,
                                   const QColor &color) {
    painter->save();
    painter->rotate(6.0 * m_time.second());
    if (poly.size() == 2) {
        painter->setPen(color);
        painter->drawLine(poly[0], poly[1]);
    } else {
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawConvexPolygon(poly);
    }
    painter->restore();

    painter->setPen(Qt::NoPen);
}

void AnalogClock::paintAmPm(QPainter *painter, int offset) {
    painter->setPen(QColor(127, 127, 127, 191));
    QFont font;
    font.setWeight(QFont::Bold);
    font.setPointSize(15);
    painter->setFont(font);
    painter->drawText(
        0, offset, 0, 0, Qt::AlignCenter | Qt::TextDontClip,
        (m_time.hour() < 12) ? locale().amText() : locale().pmText());

    painter->setPen(Qt::NoPen);
}

bool AnalogClock::showSecondsHand() const {
    return m_showSecondsHand;
}

void AnalogClock::setShowSecondsHand(bool showSecondsHand) {
    m_showSecondsHand = showSecondsHand;
    update();
}

int AnalogClock::utcOffset() const {
    return m_utcOffset;
}

bool AnalogClock::showAmPm() const {
    return m_showAmPm;
}

bool AnalogClock::setTimezone(int value) {
    if (value <= 14 && value >= -14) {
        /* 3600 seconds = 1 hour */
        setUtcOffset(value * 3600);
        return true;
    }
    return false;
}

void AnalogClock::setUtcOffset(int offset) {
    m_utcOffset = offset;
    update();
}

void AnalogClock::setShowAmPm(bool value) {
    m_showAmPm = value;
    update();
}
