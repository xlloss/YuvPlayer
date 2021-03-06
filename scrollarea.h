#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QObject>
#include <QScrollArea>
#include <QLabel>

#include <QPoint>
#include <QRect>

class ScaledImage
{
public:
    ScaledImage(QImage *img, int scale = 0);
    ~ScaledImage();

    int scale() const;
    void setScale(int scale);

    QPixmap get(QRect area);

    int width() const;
    int height() const;
private:
    QImage *m_img0;
    QImage *m_img;
    int m_scale;
    QRect m_area;
};

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ScrollArea(QWidget *parent = nullptr);
    ~ScrollArea();
    void init();
    void setImg(QImage *img);
    void display();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QLabel *m_imageLabel;

    QImage *m_originImg;
    ScaledImage *m_img;

    QPoint m_startPos;
    QPoint m_lastPos;

    int m_scale;
    QRect m_viewport;

    int m_lcu_size;
    QPoint m_focusPos;
};

#endif // SCROLLAREA_H
