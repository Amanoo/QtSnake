#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <QWidget>

#include <QPixmap>



constexpr int QWIDTH = 16;
constexpr int QHEIGHT = 16;


class GameWindow : public QWidget
{


    Q_OBJECT

    QPixmap sprites;



    std::array<quint8, QWIDTH*QHEIGHT> bitmap;


public:
    GameWindow(QWidget *parent = nullptr);
    virtual ~GameWindow() override;

    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // GAMEWINDOW_HPP
