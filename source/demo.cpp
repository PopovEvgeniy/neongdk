#include "NEONGDK.h"

int main()
{
 char perfomance[8];
 NEONGDK::Common::Timer timer;
 NEONGDK::Input::Keyboard keyboard;
 NEONGDK::Input::Gamepad gamepad;
 NEONGDK::Input::Mouse mouse;
 NEONGDK::Graphics::Screen screen;
 NEONGDK::Graphics::Image image;
 NEONGDK::Graphics::Background space;
 NEONGDK::Graphics::Sprite ship;
 NEONGDK::Graphics::Text text;
 NEONGDK::Tools::enable_logging("log.txt");
 keyboard.initialize();
 screen.initialize();
 image.load_tga("space.tga");
 space.load_background(image);
 space.prepare(screen);
 image.load_tga("ship.tga");
 ship.load_sprite(image,NEONGDK::HORIZONTAL_ANIMATED,2);
 ship.set_position(screen.get_width()/2,screen.get_height()/2);
 image.load_tga("font.tga");
 text.load_font(image);
 text.set_position(text.get_font_width(),text.get_font_height());
 mouse.hide();
 timer.set_timer(1);
 memset(perfomance,0,8);
 MUSIC_PLAY("space.wav");
 while(screen.sync())
 {
  gamepad.update();
  if (mouse.check_press(NEONGDK::MOUSE_LEFT)==true)
  {
   break;
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
  if (ship.get_x()>screen.get_width())
  {
   ship.set_x(screen.get_width()/2);
  }
  if (ship.get_y()>screen.get_height())
  {
   ship.set_y(screen.get_height()/2);
  }
  itoa(screen.get_fps(),perfomance,10);
  space.draw_background();
  text.draw_text(perfomance);
  ship.draw_sprite();
  if (timer.check_timer()==true)
  {
   ship.step();
  }

 }
 SOUND_STOP;
 return 0;
}