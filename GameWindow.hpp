#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <QWidget>

#include <QPixmap>

//////////////Embedded function emulation
static uint8_t KEY_PIO_BASE=0;
static uint8_t IORD_ALTERA_AVALON_PIO_EDGE_CAP_KEY_PIO_BASE = 0;
static uint8_t IOWR_ALTERA_AVALON_PIO_EDGE_CAP(uint8_t qwertyu){
    return IORD_ALTERA_AVALON_PIO_EDGE_CAP_KEY_PIO_BASE;
}
static uint8_t IOWR_ALTERA_AVALON_PIO_EDGE_CAP(uint8_t qwertyu,int asdfg){
    return IORD_ALTERA_AVALON_PIO_EDGE_CAP_KEY_PIO_BASE;
}
////////////////
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
