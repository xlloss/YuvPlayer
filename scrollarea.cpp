#include "scrollarea.h"

#include <QtDebug>

#include <QPixmap>
#include <QMouseEvent>
#include <QGridLayout>
#include <QScrollBar>
#include <QRgb>

ScrollArea::ScrollArea(QWidget *parent):
    QScrollArea(parent)
{
    setBackgroundRole(QPalette::Dark);

    m_scale = 0;
    m_originImg = NULL;
    m_img = NULL;

    m_viewport = QRect(0, 0, 0, 0);
}

ScrollArea::~ScrollArea()
{
    delete m_imageLabel;
    if (m_img) {
        delete m_img;
    }
}

void ScrollArea::init()
{
    // qt designer add a scrollAreaWidgetContents
    m_imageLabel = new QLabel(this);
    setWidget(m_imageLabel);
}

void ScrollArea::setImg(QImage *img)
{
    m_originImg = img;
    if (m_img) {
        delete m_img;
    }
    m_img = new ScaledImage(img, m_scale);
    m_viewport.setSize(viewport()->rect().size());
    display();
}

void ScrollArea::imgExtendHor(int left, int right)
{
//    if (m_logScale > 0) {
//        int scale = m_logScale;
//        for (int i = m_scaledArea.top(); i < m_scaledArea.bottom(); i++) {
//            for (int j = left; j < right; j++) {
//                m_scaledImg->setPixel(j, i, m_img->pixel(j >> scale, i >> scale));
//            }
//        }
//        if (m_scaledArea.left() == left) {
//            m_scaledArea.setRight(right);
//        } else {
//            m_scaledArea.setLeft(left);
//        }
//        qDebug() << "scaled area extend to" << m_scaledArea << endl;
//    }
}

void ScrollArea::imgExtendVer(int top, int bottom)
{

}

void ScrollArea::display()
{
    qDebug() << "display" << m_viewport << endl;
    m_imageLabel->setPixmap(QPixmap::fromImage(m_img->get(m_viewport)));
//    } else if (m_logScale < 0) {
//        // downsample
//        int scale = -m_logScale;
//        for (int i = 0; i < m_scaledImg->height(); i++) {
//            for (int j = 0; j < m_scaledImg->width(); j++) {
//                m_scaledImg->setPixel(j, i, m_img->pixel(j << scale, i << scale));
//            }
//        }
//        m_scaledArea = m_scaledImg->rect();
//        m_imageLabel->setPixmap(QPixmap::fromImage(m_scaledImg->copy(m_viewport)));
//    } else {
//        m_imageLabel->setPixmap(QPixmap::fromImage(m_img->copy(m_viewport)));
//    }
}

void ScrollArea::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        m_startPos = event->pos();
        m_lastPos = event->pos();

        // TODO move to other posiztion
        //m_viewport.setSize(viewport()->rect().size());
    }
}

void ScrollArea::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->pos() - m_lastPos;
        QPoint pos = QPoint(m_viewport.x(), m_viewport.y()) - delta;
        pos.setX(std::min(std::max(pos.x(), 0), std::max(m_img->width() - m_viewport.width(), 0)));
        pos.setY(std::min(std::max(pos.y(), 0), std::max(m_img->height() - m_viewport.height(), 0)));
        m_viewport.moveTo(pos);
        m_lastPos = event->pos();

        qDebug() << delta << m_viewport;

        display();
    }
}

void ScrollArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        QPoint delta = event->pos() - m_startPos;
        qDebug() << "move end, distance" << delta << endl;
    }
}

void ScrollArea::wheelEvent(QWheelEvent *event) {
    if (event->delta() > 0) {
        m_scale += 1;
        m_img->setScale(m_scale);
        qDebug() << "zoom in to level" << m_scale << "x" << exp2(m_scale) << endl;
    } else if (event->delta() < 0){
        m_scale -= 1;
        m_img->setScale(m_scale);
        qDebug() << "zoom out to level" << m_scale << "x" << exp2(m_scale) << endl;
    }

    QPoint pos;
    pos.setX(std::min(m_viewport.left(), std::max(m_img->width() - m_viewport.width(), 0)));
    pos.setY(std::min(m_viewport.top(), std::max(m_img->height() - m_viewport.height(), 0)));
    m_viewport.moveTo(pos);

    display();
}

ScaledImage::ScaledImage(QImage *img, int scale)
{
    m_img0 = img;
    m_img = NULL;

    setScale(scale);
}

ScaledImage::~ScaledImage()
{
    if (m_img) {
        delete m_img;
    }
}

void ScaledImage::setScale(int scale)
{
    m_scale = scale;

    QSize size;
    if (scale > 0) {
        size.setWidth(m_img0->width() << scale);
        size.setHeight(m_img0->height() << scale);
    } else if (scale < 0) {
        size.setWidth(m_img0->width() >> -scale);
        size.setHeight(m_img0->height() >> -scale);
    } else {
        size = m_img0->size();
    }

    m_img = new QImage(size, m_img0->format());
    m_area = QRect(0, 0, 0, 0);

    qDebug() << "scale to" << m_img->size();
}

int ScaledImage::scale() const
{
    return m_scale;
}

QImage ScaledImage::get(QRect area)
{
    qDebug() << "get sub area" << area << "in" << m_img->rect();
    QRect rect = area.intersected(m_img->rect());
    qDebug() << "get sub area" << rect << "in" << m_img->rect();

    if (m_scale == 0) {
        return m_img0->copy(rect);
    } else if (m_area.contains(rect)) {
        return m_img->copy(rect);
    } else {
        if (m_scale > 0) {
            int scale = m_scale;
            for (int i = rect.top(); i < rect.bottom(); i++) {
                for (int j = rect.left(); j < rect.right(); j++) {
                    m_img->setPixel(j, i, m_img0->pixel(j >> scale, i >> scale));
                }
            }
            return m_img->copy(rect);
        } else {
            int scale = -m_scale;
            for (int i = rect.top(); i < rect.bottom(); i++) {
                for (int j = rect.left(); j < rect.right(); j++) {
                    m_img->setPixel(j, i, m_img0->pixel(j << scale, i << scale));
                }
            }
            return m_img->copy(rect);
        }
    }
}

int ScaledImage::width() const
{
    return m_img->width();
}

int ScaledImage::height() const
{
    return m_img->height();
}
