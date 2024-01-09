#include "NEONGDK.h"

int main()
{
 char perfomance[8];
 NEONGDK::Common::Timer timer;
 NEONGDK::Input::Keyboard keyboard;
 NEONGDK::Input::Gamepad gamepad;
 NEONGDK::Input::Mouse mouse;
 NEONGDK::Misc::Audio media;
 NEONGDK::Graphics::Screen screen;
 NEONGDK::Graphics::Background space;
 NEONGDK::Graphics::Sprite ship;
 NEONGDK::Graphics::Text text;
 NEONGDK::Tools::enable_logging("log.txt");
 keyboard.initialize();
 screen.initialize();
 space.load("space.tga");
 space.prepare(screen);
 ship.load("ship.tga",NEONGDK::HORIZONTAL_ANIMATED,2);
 ship.set_start(screen.get_width()/2,screen.get_height()/2);
 ship.go_start();
 text.load_font("font.tga");
 text.set_position(text.get_font_width(),text.get_font_height());
 mouse.hide();
 timer.set_timer(1);
 media.load("space.mp3");
 memset(perfomance,0,8);
 while(screen.sync())
 {
  gamepad.update();
  media.play_loop();
  if (mouse.check_press(NEONGDK::MOUSE_LEFT)==true)
  {
   break;
  }
  if (keyboard.check_hold(59)==true)
  {
   space.disable_mirror();
  }
  if (keyboard.check_hold(60)==true)
  {
   space.horizontal_mirror();
  }
  if (keyboard.check_hold(61)==true)
  {
   space.vertical_mirror();
  }
  if (keyboard.check_hold(62)==true)
  {
   space.complex_mirror();
  }
  if (keyboard.check_hold(57)==true)
  {
   break;
  }
  if (keyboard.check_hold(72)==true)
  {
   ship.decrease_y(2);
  }
  if (keyboard.check_hold(80)==true)
  {
   ship.increase_y(2);
  }
  if (keyboard.check_hold(75)==true)
  {
   ship.decrease_x(2);
  }
  if (keyboard.check_hold(77)==true)
  {
   ship.increase_x(2);
  }
  if (gamepad.check_hold(NEONGDK::GAMEPAD_BUTTON2)==true)
  {
   break;
  }
  switch (gamepad.get_dpad())
  {
   case NEONGDK::GAMEPAD_UP:
   ship.decrease_y();
   break;
   case NEONGDK::GAMEPAD_DOWN:
   ship.increase_y();
   break;
   case NEONGDK::GAMEPAD_UPLEFT:
   ship.decrease_y();
   ship.decrease_x();
   break;
   case NEONGDK::GAMEPAD_UPRIGHT:
   ship.decrease_y();
   ship.increase_x();
   break;
   case NEONGDK::GAMEPAD_DOWNLEFT:
   ship.increase_y();
   ship.decrease_x();
   break;
   case NEONGDK::GAMEPAD_DOWNRIGHT:
   ship.increase_y();
   ship.increase_x();
   break;
   case NEONGDK::GAMEPAD_LEFT:
   ship.decrease_x();
   break;
   case NEONGDK::GAMEPAD_RIGHT:
   ship.increase_x();
   break;
   default:
   ;
   break;
  }
  if (gamepad.get_stick_x(NEONGDK::GAMEPAD_LEFT_STICK)==NEONGDK::GAMEPAD_NEGATIVE_DIRECTION)
  {
   ship.decrease_x();
  }
  if (gamepad.get_stick_x(NEONGDK::GAMEPAD_LEFT_STICK)==NEONGDK::GAMEPAD_POSITIVE_DIRECTION)
  {
   ship.increase_x();
  }
  if (gamepad.get_stick_y(NEONGDK::GAMEPAD_LEFT_STICK)==NEONGDK::GAMEPAD_NEGATIVE_DIRECTION)
  {
   ship.decrease_y();
  }
  if (gamepad.get_stick_y(NEONGDK::GAMEPAD_LEFT_STICK)==NEONGDK::GAMEPAD_POSITIVE_DIRECTION)
  {
   ship.increase_y();
  }
  if (screen.check_x(ship.get_x())==false)
  {
   ship.set_x(ship.get_start_x());
  }
  if (screen.check_y(ship.get_y())==false)
  {
   ship.set_y(ship.get_start_y());
  }
  itoa(screen.get_fps(),perfomance,10);
  space.draw();
  text.print(perfomance);
  ship.draw();
  if (timer.check_timer()==true)
  {
   ship.step();
  }

 }
 return 0;
}