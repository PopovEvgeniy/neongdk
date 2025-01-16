#include "neongdk.h"

int main()
{
 int row,column;
 NEONGDK::Graphics::Screen screen;
 NEONGDK::Graphics::Scene sky;
 NEONGDK::Graphics::Cartoon ground;
 NEONGDK::Graphics::Text text;
 NEONGDK::Transformation::Coordinates cartesian;
 NEONGDK::Transformation::Level level;
 NEONGDK::Input::Keyboard keyboard;
 keyboard.initialize();
 screen.initialize();
 sky.load("sky.tga");
 sky.prepare(screen);
 ground.load("ground.tga");
 text.load_font("font.tga");
 text.set_position(text.get_font_width(),text.get_font_height());
 cartesian.initialize(screen.get_width(),screen.get_height());
 level.initialize(ground.get_width(),ground.get_height());
 while (screen.sync())
 {
  if (keyboard.check_hold(1)==true)
  {
   break;
  }
  sky.draw();
  for (row=0;row<10;++row)
  {
   for (column=0;column<10;++column)
   {
    ground.draw(cartesian.get_screen_x(level.get_x(row,column)),cartesian.get_screen_y(level.get_y(row,column)));
   }

  }
  text.print("Press Esc to exit");
 }
 return 0;
}