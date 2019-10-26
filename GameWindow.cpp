
#include "GameWindow.hpp"

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <qapplication.h>

#include <iostream>
#include <cstring>

#define NORMAL_SNAKE



constexpr int WIDTH = 16;
constexpr int HEIGHT = 16;

struct Point {
    int xpos;
    int ypos;
    int x(){
        return xpos;
    }
    int y(){
      return ypos;
    }
    void setX(int x){
        xpos=x;
    }
    void setY(int y){
        ypos=y;
    }

};

struct SnakeNode {
    Point pos;
    SnakeNode *next = nullptr;
};

static volatile struct GameData{
    int running=0;
    int direction = 0, newDirection = 0;
    uint8_t * bitmap;
    SnakeNode *current, *previous;
} gameData;

void spawnFood();

void resetSnake();

void setPosition(Point pos, int sprite);
int getPosition(Point pos);


static void handle_key_interrupts(void* context)
{
    volatile GameData* data = (volatile GameData *)context;

    // Get the pin that has triggered the edge interrupt
    int edge_pin = IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_PIO_BASE);

    // To prevent the interrupt from happening again without it being triggered, we need to reset it
    // To do that, we'll write the pin bit to the edge capture register. It will then be reset.
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_PIO_BASE, edge_pin);

    if(!data->running){
        data->running=2;
        resetSnake();
        spawnFood();
        return;
    }
  switch (edge_pin) {
  case 8://2^3
    if (data->direction != 1)
      data->newDirection = 3;
    break;
  case 1://2^0
    if (data->direction != 2)
      data->newDirection = 0;
    break;
  case 4://2^2
    if (data->direction != 0)
      data->newDirection = 2;
    break;
  case 2://2^1
    if (data->direction != 3)
      data->newDirection = 1;
    break;
  }

}

/*
static void init_key_interrupt(volatile int *reg)
{
    // We first need to enable the interrupts. We have two buttons which are mapped to bit 0 and 1.
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_PIO_BASE, 0x03);

    // To make sure that the interrupts don't trigger immediately due to randomness,
    // it's better to reset them just to be sure.
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_PIO_BASE, 0x03);

    // We can also provide it with a variable, the context.
    // In this example we want it to point to a volatile int in which the triggered pin bit will be saved.
    // The function only takes a void pointer, so we first need to take the address and then cast it to a void pointer.
    void* reg_ptr = (void*)reg;

    // Now we let the system know what to do when the interrupt happens.
    // It should call the function handle_key_interrupts.
    // Flags don't do anything, so let's just give NULL as value.
    alt_ic_isr_register(KEY_PIO_IRQ_INTERRUPT_CONTROLLER_ID, KEY_PIO_IRQ, handle_key_interrupts, reg_ptr, NULL);
}*/

void setPosition(Point pos, int sprite) {
  gameData.bitmap[(pos.y() * WIDTH + pos.x())] = (size_t(sprite) & 0xFFUL) | 0x80UL;
}

int getPosition(Point pos) {
  return gameData.bitmap[(pos.y() * WIDTH + pos.x())] & 0x7F;
}

void spawnFood() {
#if 1
  Point validPoints[HEIGHT*WIDTH];
  uint16_t size = 0;
  for (int wid =0;wid<WIDTH;wid++) {
      for (int hei=0;hei<HEIGHT;hei++){
          Point pos = Point{wid, hei};
          if(getPosition(pos)==0){
            validPoints[size]=pos;
            size++;
          }
      }
  }

  int chosen=rand() % size;
  setPosition(validPoints[chosen], 1);
#else
  QPoint pos{qrand() % WIDTH, qrand() % HEIGHT};

  while (getPosition(pos) != 0)
    pos = QPoint{qrand() % WIDTH, qrand() % HEIGHT};

  setPosition(pos, 1);
#endif
}



void resetSnake() {
memset(gameData.bitmap,0x80,WIDTH*HEIGHT);
#ifdef NORMAL_SNAKE
    gameData.direction = gameData.newDirection = 0;
  while (gameData.current->next != gameData.current) {
    SnakeNode *del = gameData.current->next;
    gameData.current->next = gameData.current->next->next;
    setPosition(del->pos, 0);
    delete del;
  }

  for (int wid =0;wid<WIDTH;wid++) {
      for (int hei=0;hei<HEIGHT;hei++){
          Point pos = Point{wid, hei};
          if(getPosition(pos)==1){
              setPosition(pos,0);
          }
      }

  }

  gameData.current->pos = Point{WIDTH / 2, HEIGHT / 2};
  setPosition(gameData.current->pos, 0x02);
  gameData.previous = gameData.current;


  for (int I = 0; I < 4; I++) {
    gameData.current = (gameData.current->next = new SnakeNode);
    gameData.current->pos = Point{WIDTH / 2 - 4 + I, HEIGHT/2};
    setPosition(gameData.current->pos, 0x12 - (I==0?12:0));
  }
  gameData.current->next = gameData.previous;


  gameData.previous = gameData.current->next;

#else
  while (current->next->pos != current->pos && current->next->next != current) {
    SnakeNode *del = current->next;
    current->next = del->next;
    delete del;
  }

  while (current->next->next != current) {
    SnakeNode *del = current->next;
    current->next = del->next;
    setPosition(del->pos, 0);
    delete del;
  }
#endif
}


Point operator+( Point const& lhs, Point const& rhs ){
    return Point{lhs.xpos+rhs.xpos,lhs.ypos+rhs.ypos};
}


inline size_t posToBitmapIndex(Point pt) {
  return (pt.y() * WIDTH + pt.x()) / 2;
}






/*

  [1] [2] [3] [4]



*/

void timer_interrupt() {
  if(!gameData.running)return;
    // Change direction on new tick
  gameData.direction = gameData.newDirection;
  // Remember the current position as previous position
  gameData.previous = gameData.current;
  // Move to the next position node which is now the end of the snake
  gameData.current = gameData.current->next;
  // Clear the snake sprite
  setPosition(gameData.current->pos, 0);
  // Determine the new position of the snake
  Point dir{};
  Point future;
  switch (gameData.direction) {
  case 0:
    dir = Point{1, 0};
    break;
  case 1:
    dir = Point{0, 1};
    break;
  case 2:
    dir = Point{-1, 0};
    break;
  case 3:
    dir = Point{0, -1};
    break;
  }
  gameData.current->pos = gameData.previous->pos + dir;
  future = gameData.current->pos + dir;
  gameData.current->pos.setX((gameData.current->pos.x() + WIDTH) % WIDTH);
  gameData.current->pos.setY((gameData.current->pos.y() + HEIGHT) % HEIGHT);
  future.setX((future.x() + WIDTH) % WIDTH);
  future.setY((future.y() + HEIGHT) % HEIGHT);

  if (getPosition(gameData.current->pos) == 1) {
    setPosition(gameData.current->pos, 2 + gameData.direction + 0x20);
    spawnFood();
    SnakeNode *ins = new SnakeNode;
    ins->next = gameData.current->next->next;
    ins->pos = gameData.current->next->pos;
    gameData.current->next->next = ins;
  } else if (getPosition(gameData.current->pos) != 0) {
    gameData.running=1;
    std::strcpy((char*)gameData.bitmap, "    GAME OVER");
  } else {
    setPosition(gameData.current->pos, 2 + gameData.direction + (getPosition(future) != 0?0x24:0));
  }

  int head = (getPosition(gameData.previous->pos));
  if(head >= 0x26 && head <= 0x29) {
      head -= 0x24;
  }

  int thickness = (head & 0x20);

  if ((head & 0x1F) == gameData.direction + 2 ) {
    setPosition(gameData.previous->pos, gameData.direction + 18 + thickness);
  } else
      if ((head & 0x1F) == gameData.direction + 1 ||
          (head & 0x1F) == gameData.direction + 5) {
    setPosition(gameData.previous->pos, 10 + gameData.direction + thickness);
  } else
      if ((head & 0x1F) == gameData.direction - 1 ||
          (head & 0x1F) == gameData.direction + 3) {
    setPosition(gameData.previous->pos, 14 + gameData.direction + thickness);
  }

//  auto a = current->next->pos;
//  auto b = current->next->next->pos;

  int piece = (getPosition(gameData.current->next->pos) - 2) % 4;
  if(piece < 0) {
      int orient = ((getPosition(gameData.current->next->next->pos) & 0x1F) - 2);
      int add;
      if(orient < 12)
          add = 3;
          else
      if(orient < 16)
          add = 5;
      else
          add = 0;
      piece = (getPosition(gameData.current->next->next->pos) - 2 + add) % 4;
  }
  setPosition(gameData.current->next->pos, piece + 6);

//  if (a.x() < b.x()) {
//    setPosition(current->next->pos, 6);
//  } else if (a.x() > b.x()) {
//    setPosition(current->next->pos, 8);
//  } else if (a.y() < b.y()) {
//    setPosition(current->next->pos, 7);
//  } else if (a.y() > b.y()) {
//    setPosition(current->next->pos, 9);
//  }

  qDebug() << "Prev: " << getPosition(gameData.previous->pos) << " -> " << gameData.direction;

  SnakeNode *ptr = gameData.current->next;
  qDebug() << "Positions";
  do {
    qDebug() << QPoint{ptr->pos.x(),ptr->pos.y()};

    ptr = ptr->next;
  } while (ptr != gameData.current->next);
  if(gameData.running==1){
      gameData.running=0;

      setPosition(gameData.current->pos,0x22+gameData.direction);
  }
}



int mmain(void){
    for (int I = 0; I < WIDTH * HEIGHT; I++) {
      gameData.bitmap[I] = 0x80;
    }
    std::strcpy((char*)gameData.bitmap, "Press any key tobegin");

    gameData.current = new SnakeNode;
    gameData.current->next = gameData.current;

    //setupinterrupts
}


/////////////////////////////////////////////////////////////////////////////////////////
GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent), sprites(":/sprites.png") {
    gameData.bitmap = bitmap.data();
  setMinimumSize(WIDTH * sprites.width() / 32, HEIGHT * sprites.height() / 32);
  startTimer(400);

  mmain();

}

GameWindow::~GameWindow() {}

void GameWindow::paintEvent(QPaintEvent *event) {
  QPainter paint(this);
  paint.setPen(Qt::cyan);

  int scale = qMin(width() / WIDTH, height() / HEIGHT);

  for (int X = 0; X < WIDTH; X++) {
    for (int Y = 0; Y < HEIGHT; Y++) {
      QRect tile(QPoint{X, Y} * scale, QSize(scale, scale));
      int idx = bitmap[X+Y*WIDTH];
      paint.drawPixmap(tile, sprites,
                       QRect(QPoint{idx % 16, idx / 16} * sprites.width() / 16,
                             sprites.size()/16));
//      paint.drawText(tile, Qt::AlignCenter, QString("%1").arg(idx, 1, 16));
//      paint.drawRect(tile);
    }
  }
}

void GameWindow::timerEvent(QTimerEvent *event) {
    timer_interrupt();
    update();
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Up:
      IORD_ALTERA_AVALON_PIO_EDGE_CAP_KEY_PIO_BASE = 8;//2^3;
    break;
  case Qt::Key_Right:
      IORD_ALTERA_AVALON_PIO_EDGE_CAP_KEY_PIO_BASE = 1;//2^0;
    break;
  case Qt::Key_Left:
      IORD_ALTERA_AVALON_PIO_EDGE_CAP_KEY_PIO_BASE = 4;//2^2;
    break;
  case Qt::Key_Down:
      IORD_ALTERA_AVALON_PIO_EDGE_CAP_KEY_PIO_BASE = 2;//2^1;
    break;
  }
  handle_key_interrupts((void*)&gameData);
}
