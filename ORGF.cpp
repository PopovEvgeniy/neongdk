// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright (C) 2016-2018 Popov Evgeniy Alekseyevich

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "ORGF.h"

unsigned char ORGF_Keys[ORGF_KEYBOARD];
unsigned char ORGF_Buttons[ORGF_MOUSE];

LRESULT CALLBACK ORGF_Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam)
{
 switch (Message)
 {
  case WM_CLOSE:
  DestroyWindow(window);
  break;
  case WM_DESTROY:
  PostQuitMessage(0);
  break;
  case WM_CREATE:
  memset(ORGF_Keys,ORGFKEY_NONE,ORGF_KEYBOARD);
  memset(ORGF_Buttons,ORGFKEY_NONE,ORGF_MOUSE);
  break;
  case WM_LBUTTONDOWN:
  ORGF_Buttons[ORGF_MOUSE_LEFT]=ORGFKEY_PRESS;
  break;
  case WM_LBUTTONUP:
  ORGF_Buttons[ORGF_MOUSE_LEFT]=ORGFKEY_RELEASE;
  break;
  case WM_RBUTTONDOWN:
  ORGF_Buttons[ORGF_MOUSE_RIGHT]=ORGFKEY_PRESS;
  break;
  case WM_RBUTTONUP:
  ORGF_Buttons[ORGF_MOUSE_RIGHT]=ORGFKEY_RELEASE;
  break;
  case WM_MBUTTONDOWN:
  ORGF_Buttons[ORGF_MOUSE_MIDDLE]=ORGFKEY_PRESS;
  break;
  case WM_MBUTTONUP:
  ORGF_Buttons[ORGF_MOUSE_MIDDLE]=ORGFKEY_RELEASE;
  break;
  case WM_KEYDOWN:
  ORGF_Keys[ORGF_GETSCANCODE(lParam)]=ORGFKEY_PRESS;
  break;
  case WM_KEYUP:
  ORGF_Keys[ORGF_GETSCANCODE(lParam)]=ORGFKEY_RELEASE;
  break;
 }
 return DefWindowProc(window,Message,wParam,lParam);
}

void ORGF_Show_Error(const char *message)
{
 puts(message);
 exit(EXIT_FAILURE);
}

ORGF_Base::ORGF_Base()
{
 HRESULT status;
 status=CoInitialize(NULL);
 if(status!=S_OK)
 {
  if(status!=S_FALSE)
  {
   ORGF_Show_Error("Can't initialize COM");
  }

 }

}

ORGF_Base::~ORGF_Base()
{
 CoUninitialize();
}

ORGF_Synchronization::ORGF_Synchronization()
{
 timer=NULL;
}

ORGF_Synchronization::~ORGF_Synchronization()
{
 if(timer==NULL)
 {
  CancelWaitableTimer(timer);
  CloseHandle(timer);
 }

}

void ORGF_Synchronization::create_timer()
{
 timer=CreateWaitableTimer(NULL,FALSE,NULL);
 if (timer==NULL)
 {
  ORGF_Show_Error("Can't create synchronization timer");
 }

}

void ORGF_Synchronization::set_timer(const unsigned long int interval)
{
 LARGE_INTEGER start;
 start.QuadPart=0;
 if(SetWaitableTimer(timer,&start,interval,NULL,NULL,FALSE)==FALSE)
 {
  ORGF_Show_Error("Can't set timer");
 }

}

void ORGF_Synchronization::wait_timer()
{
 WaitForSingleObject(timer,INFINITE);
}

ORGF_Engine::ORGF_Engine()
{
 window_class.lpszClassName=TEXT("ORGF");
 window_class.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_SAVEBITS;
 window_class.lpfnWndProc=(WNDPROC)ORGF_Process_Message;
 window_class.hInstance=NULL;
 window_class.hbrBackground=NULL;
 window_class.hIcon=NULL;
 window_class.hCursor=NULL;
 window_class.cbClsExtra=0;
 window_class.cbWndExtra=0;
 window=NULL;
 width=0;
 height=0;
}

ORGF_Engine::~ORGF_Engine()
{
 if(window!=NULL) CloseWindow(window);
 UnregisterClass(window_class.lpszClassName,window_class.hInstance);
}

HWND ORGF_Engine::get_window()
{
 return window;
}

void ORGF_Engine::prepare_engine()
{
 window_class.hInstance=GetModuleHandle(NULL);
 if(window_class.hInstance==NULL)
 {
  ORGF_Show_Error("Can't get the application instance");
 }
 window_class.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
 if (window_class.hbrBackground==NULL)
 {
  ORGF_Show_Error("Can't set background color");
 }
 window_class.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 if (window_class.hIcon==NULL)
 {
  ORGF_Show_Error("Can't load the standart program icon");
 }
 window_class.hCursor=LoadCursor(NULL,IDC_ARROW);
 if (window_class.hCursor==NULL)
 {
  ORGF_Show_Error("Can't load the standart cursor");
 }
 if (!RegisterClass(&window_class))
 {
  ORGF_Show_Error("Can't register window class");
 }

}

void ORGF_Engine::create_window()
{
 width=GetSystemMetrics(SM_CXSCREEN);
 height=GetSystemMetrics(SM_CYSCREEN);
 window=CreateWindow(window_class.lpszClassName,NULL,WS_VISIBLE|WS_POPUP,0,0,width,height,NULL,NULL,window_class.hInstance,NULL);
 if (window==NULL)
 {
  ORGF_Show_Error("Can't create window");
 }
 EnableWindow(window,TRUE);
 SetFocus(window);
}

void ORGF_Engine::destroy_window()
{
 if(window!=NULL) CloseWindow(window);
}

void ORGF_Engine::capture_mouse()
{
 RECT border;
 if(GetClientRect(window,&border)==FALSE)
 {
  ORGF_Show_Error("Can't capture window");
 }
 if(ClipCursor(&border)==FALSE)
 {
  ORGF_Show_Error("Can't capture cursor");
 }

}

bool ORGF_Engine::process_message()
{
 bool quit;
 MSG Message;
 quit=false;
 while(PeekMessage(&Message,window,0,0,PM_NOREMOVE)==TRUE)
 {
  if(GetMessage(&Message,window,0,0)==TRUE)
  {
   TranslateMessage(&Message);
   DispatchMessage(&Message);
  }
  else
  {
   quit=true;
   break;
  }

 }
 return quit;
}

unsigned long int ORGF_Engine::get_width()
{
 return width;
}

unsigned long int ORGF_Engine::get_height()
{
 return height;
}

ORGF_Frame::ORGF_Frame()
{
 frame_width=512;
 frame_height=512;
 buffer_length=0;
 buffer=NULL;
}

ORGF_Frame::~ORGF_Frame()
{
 if(buffer!=NULL)
 {
  free(buffer);
  buffer=NULL;
 }

}

unsigned int ORGF_Frame::get_rgb(const unsigned int red,const unsigned int green,const unsigned int blue)
{
 return red+(green<<8)+(blue<<16);
}

void ORGF_Frame::set_size(const unsigned long int surface_width,const unsigned long int surface_height)
{
 frame_width=surface_width;
 frame_height=surface_height;
}

void ORGF_Frame::create_render_buffer()
{
 buffer_length=(size_t)frame_width*(size_t)frame_height;
 buffer=(unsigned int*)calloc(buffer_length,sizeof(unsigned int));
 if(buffer==NULL)
 {
  ORGF_Show_Error("Can't allocate memory for render buffer");
 }
 else
 {
  buffer_length*=sizeof(unsigned int);
 }

}

unsigned int *ORGF_Frame::get_buffer()
{
 return buffer;
}

void ORGF_Frame::draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 if((x<frame_width)&&(y<frame_height))
 {
  buffer[(size_t)x+(size_t)y*(size_t)frame_width]=this->get_rgb(blue,green,red);
 }

}

void ORGF_Frame::clear_screen()
{
 memset(buffer,0,buffer_length);
}

unsigned long int ORGF_Frame::get_frame_width()
{
 return frame_width;
}

unsigned long int ORGF_Frame::get_frame_height()
{
 return frame_height;
}

ORGF_Display::ORGF_Display()
{
 memset(&display,0,sizeof(DEVMODE));
 display.dmSize=sizeof(DEVMODE);
}

ORGF_Display::~ORGF_Display()
{
 ChangeDisplaySettings(NULL,0);
}

void ORGF_Display::set_video_mode()
{
 if (ChangeDisplaySettings(&display,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
 {
  ORGF_Show_Error("Can't change video mode");
 }

}

void ORGF_Display::get_video_mode()
{
 if (EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&display)==FALSE)
 {
  ORGF_Show_Error("Can't get display setting");
 }

}

void ORGF_Display::check_video_mode()
{
 this->get_video_mode();
 if(display.dmBitsPerPel<16) display.dmBitsPerPel=16;
 this->set_video_mode();
}

void ORGF_Display::reset_display()
{
 ChangeDisplaySettings(NULL,0);
}

void ORGF_Display::set_display_mode(const unsigned long int screen_width,const unsigned long int screen_height)
{
 this->get_video_mode();
 display.dmPelsWidth=screen_width;
 display.dmPelsHeight=screen_height;
 if(display.dmBitsPerPel<16) display.dmBitsPerPel=16;
 this->set_video_mode();
}

ORGF_Render::ORGF_Render()
{
 memset(&setting,0,sizeof(BITMAPINFO));
}

ORGF_Render::~ORGF_Render()
{

}

void ORGF_Render::set_render_setting()
{
 setting.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
 setting.bmiHeader.biWidth=this->get_frame_width();
 setting.bmiHeader.biHeight=-1*(long int)this->get_frame_height();
 setting.bmiHeader.biPlanes=1;
 setting.bmiHeader.biBitCount=CHAR_BIT*sizeof(unsigned int);
 setting.bmiHeader.biCompression=BI_RGB;
}

void ORGF_Render::create_render()
{
 this->create_window();
 this->capture_mouse();
 this->set_render_setting();
}

void ORGF_Render::refresh()
{
 HDC context;
 context=GetDC(this->get_window());
 if(context==NULL)
 {
  ORGF_Show_Error("Can't get window context");
 }
 else
 {
  StretchDIBits(context,0,0,this->get_width(),this->get_height(),0,0,this->get_frame_width(),this->get_frame_height(),this->get_buffer(),&setting,DIB_RGB_COLORS,SRCCOPY);
  ReleaseDC(this->get_window(),context);
 }

}

void ORGF_Screen::initialize()
{
 this->check_video_mode();
 this->prepare_engine();
 this->create_render();
 this->create_render_buffer();
 this->create_timer();
 this->set_timer(17);
}

void ORGF_Screen::initialize(const unsigned long int surface_width,const unsigned long int surface_height)
{
 this->set_size(surface_width,surface_height);
 this->initialize();
}

void ORGF_Screen::set_mode(const unsigned long int screen_width,const unsigned long int screen_height)
{
 this->destroy_window();
 this->reset_display();
 this->set_display_mode(screen_width,screen_height);
 this->create_render();
}

bool ORGF_Screen::update()
{
 this->refresh();
 return this->process_message();
}

bool ORGF_Screen::sync()
{
 bool quit;
 quit=this->update();
 this->wait_timer();
 return quit;
}

ORGF_Screen* ORGF_Screen::get_handle()
{
 return this;
}

ORGF_Keyboard::ORGF_Keyboard()
{
 preversion=NULL;
}

ORGF_Keyboard::~ORGF_Keyboard()
{
 if(preversion!=NULL) free(preversion);
}

void ORGF_Keyboard::initialize()
{
 preversion=(unsigned char*)calloc(ORGF_KEYBOARD,1);
 if(preversion==NULL)
 {
  ORGF_Show_Error("Can't allocate memory for keyboard state buffer");
 }

}

bool ORGF_Keyboard::check_hold(const unsigned char code)
{
 bool result;
 result=false;
 if(ORGF_Keys[code]==ORGFKEY_PRESS) result=true;
 return result;
}

bool ORGF_Keyboard::check_press(const unsigned char code)
{
 bool result;
 result=false;
 if(ORGF_Keys[code]==ORGFKEY_PRESS)
 {
  if(preversion[code]!=ORGFKEY_PRESS) result=true;
 }
 preversion[code]=ORGF_Keys[code];
 return result;
}

bool ORGF_Keyboard::check_release(const unsigned char code)
{
 bool result;
 result=false;
 if(ORGF_Keys[code]==ORGFKEY_RELEASE)
 {
  result=true;
  ORGF_Keys[code]=ORGFKEY_NONE;
 }
 return result;
}

ORGF_Mouse::ORGF_Mouse()
{
 memset(preversion,ORGFKEY_NONE,ORGF_MOUSE);
}

ORGF_Mouse::~ORGF_Mouse()
{
 while(ShowCursor(TRUE)<1) ;
}

void ORGF_Mouse::show()
{
 while(ShowCursor(TRUE)<1) ;
}

void ORGF_Mouse::hide()
{
 while(ShowCursor(FALSE)>-2) ;
}

void ORGF_Mouse::set_position(const unsigned long int x,const unsigned long int y)
{
 if(SetCursorPos(x,y)==FALSE)
 {
  ORGF_Show_Error("Can't set the mouse cursor position");
 }

}

unsigned long int ORGF_Mouse::get_x()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  ORGF_Show_Error("Can't get the mouse cursor position");
 }
 return position.x;
}

unsigned long int ORGF_Mouse::get_y()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  ORGF_Show_Error("Can't get the mouse cursor position");
 }
 return position.y;
}

bool ORGF_Mouse::check_hold(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=ORGF_MOUSE_MIDDLE)
 {
  if(ORGF_Buttons[button]==ORGFKEY_PRESS) result=true;
 }
 return result;
}

bool ORGF_Mouse::check_press(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=ORGF_MOUSE_MIDDLE)
 {
  if(ORGF_Buttons[button]==ORGFKEY_PRESS)
  {
   if(preversion[button]!=ORGFKEY_PRESS) result=true;
  }

 }
 preversion[button]=ORGF_Buttons[button];
 return result;
}

bool ORGF_Mouse::check_release(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=ORGF_MOUSE_MIDDLE)
 {
  if(ORGF_Buttons[button]==ORGFKEY_RELEASE)
  {
   result=true;
   ORGF_Buttons[button]=ORGFKEY_NONE;
  }

 }
 return result;
}

ORGF_Gamepad::ORGF_Gamepad()
{
 active=0;
 memset(&configuration,0,sizeof(JOYCAPS));
 memset(&current,0,sizeof(JOYINFOEX));
 memset(&preversion,0,sizeof(JOYINFOEX));
 current.dwSize=sizeof(JOYINFOEX);
 preversion.dwSize=sizeof(JOYINFOEX);
 current.dwFlags=JOY_RETURNALL;
 preversion.dwFlags=JOY_RETURNALL;
 current.dwPOV=JOY_POVCENTERED;
 preversion.dwPOV=JOY_POVCENTERED;
}

ORGF_Gamepad::~ORGF_Gamepad()
{

}

bool ORGF_Gamepad::read_configuration()
{
 bool result;
 result=false;
 if(joyGetDevCaps((UINT_PTR)active,&configuration,sizeof(JOYCAPS))==JOYERR_NOERROR) result=true;
 return result;
}

bool ORGF_Gamepad::read_state()
{
 bool result;
 result=false;
 if(joyGetPosEx(active,&current)==JOYERR_NOERROR) result=true;
 return result;
}

void ORGF_Gamepad::clear_state()
{
 memset(&configuration,0,sizeof(JOYCAPS));
 memset(&current,0,sizeof(JOYINFOEX));
 memset(&preversion,0,sizeof(JOYINFOEX));
 current.dwSize=sizeof(JOYINFOEX);
 preversion.dwSize=sizeof(JOYINFOEX);
 current.dwFlags=JOY_RETURNALL;
 preversion.dwFlags=JOY_RETURNALL;
 current.dwPOV=JOY_POVCENTERED;
 preversion.dwPOV=JOY_POVCENTERED;
}

bool ORGF_Gamepad::check_button(const ORGF_GAMEPAD_BUTTONS button,const JOYINFOEX &target)
{
 bool result;
 result=false;
 if(target.dwButtons&button) result=true;
 return result;
}

unsigned int ORGF_Gamepad::get_amount()
{
 return joyGetNumDevs();
}

unsigned int ORGF_Gamepad::get_button_amount()
{
 unsigned int result;
 result=0;
 if(this->read_configuration()==true) result=configuration.wNumButtons;
 return result;
}

bool ORGF_Gamepad::check_connection()
{
 return this->read_state();
}

void ORGF_Gamepad::update()
{
 preversion=current;
 if(this->read_state()==false) this->clear_state();
}

unsigned long int ORGF_Gamepad::get_sticks_amount()
{
 unsigned long int result;
 result=0;
 if(this->read_configuration()==true)
 {
  switch (configuration.wNumAxes)
  {
   case 2:
   result=1;
   break;
   case 4:
   result=2;
   break;
   default:
   result=0;
   break;
  }

 }
 return result;
}

void ORGF_Gamepad::set_active(const unsigned int gamepad)
{
 if(active<16)
 {
  this->clear_state();
  active=gamepad;
 }

}

unsigned int ORGF_Gamepad::get_active()
{
 return active;
}

ORGF_GAMEPAD_DPAD ORGF_Gamepad::get_dpad()
{
 ORGF_GAMEPAD_DPAD result;
 result=ORGF_GAMEPAD_NONE;
 switch (current.dwPOV)
 {
  case JOY_POVFORWARD:
  result=ORGF_GAMEPAD_UP;
  break;
  case JOY_POVBACKWARD:
  result=ORGF_GAMEPAD_DOWN;
  break;
  case JOY_POVLEFT:
  result=ORGF_GAMEPAD_LEFT;
  break;
  case JOY_POVRIGHT:
  result=ORGF_GAMEPAD_RIGHT;
  break;
  case JOYSTICK_UPLEFT:
  result=ORGF_GAMEPAD_UPLEFT;
  break;
  case JOYSTICK_UPRIGHT:
  result=ORGF_GAMEPAD_UPRIGHT;
  break;
  case JOYSTICK_DOWNLEFT:
  result=ORGF_GAMEPAD_DOWNLEFT;
  break;
  case JOYSTICK_DOWNRIGHT:
  result=ORGF_GAMEPAD_DOWNRIGHT;
  break;
 }
 return result;
}

char ORGF_Gamepad::get_stick_x(const ORGF_GAMEPAD_STICKS stick)
{
 char result;
 unsigned long int control;
 result=0;
 if(stick==ORGF_GAMEPAD_LEFT_STICK)
 {
  if(this->get_sticks_amount()>0)
  {
   control=(configuration.wXmax-configuration.wXmin)/2;
   if(current.dwXpos<control) result=-1;
   if(current.dwXpos>control) result=1;
  }

 }
 if(stick==ORGF_GAMEPAD_RIGHT_STICK)
 {
  if(this->get_sticks_amount()>1)
  {
   control=(configuration.wZmax-configuration.wZmin)/2;
   if(current.dwZpos<control) result=-1;
   if(current.dwZpos>control) result=1;
  }

 }
 return result;
}

char ORGF_Gamepad::get_stick_y(const ORGF_GAMEPAD_STICKS stick)
{
 char result;
 unsigned long int control;
 result=0;
 if(stick==ORGF_GAMEPAD_LEFT_STICK)
 {
  if(this->get_sticks_amount()>0)
  {
   control=(configuration.wYmax-configuration.wYmin)/2;
   if(current.dwYpos<control) result=-1;
   if(current.dwYpos>control) result=1;
  }

 }
 if(stick==ORGF_GAMEPAD_RIGHT_STICK)
 {
  if(this->get_sticks_amount()>1)
  {
   control=(configuration.wRmax-configuration.wRmin)/2;
   if(current.dwRpos<control) result=-1;
   if(current.dwRpos>control) result=1;
  }

 }
 return result;
}

bool ORGF_Gamepad::check_hold(const ORGF_GAMEPAD_BUTTONS button)
{
 return this->check_button(button,current);
}

bool ORGF_Gamepad::check_press(const ORGF_GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if(this->check_button(button,current)==true)
 {
  if(this->check_button(button,preversion)==false) result=true;
 }
 return result;
}

bool ORGF_Gamepad::check_release(const ORGF_GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if(this->check_button(button,current)==false)
 {
  if(this->check_button(button,preversion)==true) result=true;
 }
 return result;
}

ORGF_Multimedia::ORGF_Multimedia()
{
 loader=NULL;
 player=NULL;
 controler=NULL;
 video=NULL;
}

ORGF_Multimedia::~ORGF_Multimedia()
{
 if(player!=NULL) player->StopWhenReady();
 if(video!=NULL) video->Release();
 if(controler!=NULL) controler->Release();
 if(player!=NULL) player->Release();
 if(loader!=NULL) loader->Release();
}

wchar_t *ORGF_Multimedia::convert_file_name(const char *target)
{
 wchar_t *name;
 size_t index,length;
 length=strlen(target);
 name=(wchar_t*)calloc(length+1,sizeof(wchar_t));
 if(name==NULL)
 {
  ORGF_Show_Error("Can't allocate memory");
 }
 for(index=0;index<length;++index) name[index]=btowc(target[index]);
 return name;
}

void ORGF_Multimedia::open(const wchar_t *target)
{
 player->StopWhenReady();
 if(loader->RenderFile(target,NULL)!=S_OK)
 {
  ORGF_Show_Error("Can't load a multimedia file");
 }
 video->put_FullScreenMode(OATRUE);
}

bool ORGF_Multimedia::is_end()
{
 bool result;
 long long current,stop;
 result=false;
 if(controler->GetPositions(&current,&stop)==S_OK)
 {
  if(current>=stop) result=true;
 }
 else
 {
  ORGF_Show_Error("Can't get the current and the end position");
 }
 return result;
}

void ORGF_Multimedia::rewind()
{
 long long position;
 position=0;
 if(controler->SetPositions(&position,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning)!=S_OK)
 {
  ORGF_Show_Error("Can't set start position");
 }

}

void ORGF_Multimedia::initialize()
{
 if(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&loader)!=S_OK)
 {
  ORGF_Show_Error("Can't create a multimedia loader");
  ;
 }
 if(loader->QueryInterface(IID_IMediaControl,(void**)&player)!=S_OK)
 {
  ORGF_Show_Error("Can't create a multimedia player");
  ;
 }
 if(loader->QueryInterface(IID_IMediaSeeking,(void**)&controler)!=S_OK)
 {
  ORGF_Show_Error("Can't create a player controler");
  ;
 }
 if(loader->QueryInterface(IID_IVideoWindow,(void**)&video)!=S_OK)
 {
  ORGF_Show_Error("Can't create a video player");
 }

}

void ORGF_Multimedia::load(const char *target)
{
 wchar_t *name;
 name=this->convert_file_name(target);
 this->open(name);
 free(name);
}

bool ORGF_Multimedia::check_playing()
{
 OAFilterState state;
 bool result;
 result=false;
 if(player->GetState(INFINITE,&state)==E_FAIL)
 {
  ORGF_Show_Error("Can't get the multimedia state");
 }
 else
 {
  if(state==State_Running)
  {
   if(this->is_end()==false) result=true;
  }

 }
 return result;
}

void ORGF_Multimedia::stop()
{
 player->StopWhenReady();
}

void ORGF_Multimedia::play()
{
 this->stop();
 this->rewind();
 player->Run();
}

ORGF_Memory::ORGF_Memory()
{
 memset(&memory,0,sizeof(MEMORYSTATUSEX));
 memory.dwLength=sizeof(MEMORYSTATUSEX);
}

ORGF_Memory::~ORGF_Memory()
{

}

void ORGF_Memory::get_status()
{
 if(GlobalMemoryStatusEx(&memory)==FALSE)
 {
  ORGF_Show_Error("Can't get the memory status");
 }

}

unsigned long long int ORGF_Memory::get_total_memory()
{
 this->get_status();
 return memory.ullTotalPhys;
}

unsigned long long int ORGF_Memory::get_free_memory()
{
 this->get_status();
 return memory.ullAvailPhys;
}

ORGF_System::ORGF_System()
{
 srand(time(NULL));
}

ORGF_System::~ORGF_System()
{

}

unsigned long int ORGF_System::get_random(const unsigned long int number)
{
 return rand()%number;
}

void ORGF_System::quit()
{
 exit(EXIT_SUCCESS);
}

void ORGF_System::run(const char *command)
{
 system(command);
}

char* ORGF_System::read_environment(const char *variable)
{
 return getenv(variable);
}

void ORGF_System::enable_logging(const char *name)
{
 if(freopen(name,"wt",stdout)==NULL)
 {
  ORGF_Show_Error("Can't create log file");
 }

}

ORGF_Timer::ORGF_Timer()
{
 interval=0;
 start=time(NULL);
}

ORGF_Timer::~ORGF_Timer()
{

}

void ORGF_Timer::set_timer(const unsigned long int seconds)
{
 interval=seconds;
 start=time(NULL);
}

bool ORGF_Timer::check_timer()
{
 bool result;
 time_t stop;
 result=false;
 stop=time(NULL);
 if(difftime(stop,start)>=interval)
 {
  result=true;
  start=time(NULL);
 }
 return result;
}

ORGF_Primitive::ORGF_Primitive()
{
 color.red=0;
 color.green=0;
 color.blue=0;
 surface=NULL;
}

ORGF_Primitive::~ORGF_Primitive()
{
 color.red=0;
 color.green=0;
 color.blue=0;
 surface=NULL;
}

void ORGF_Primitive::initialize(ORGF_Screen *Screen)
{
 surface=Screen;
}

void ORGF_Primitive::set_color(const unsigned char red,const unsigned char green,const unsigned char blue)
{
 color.red=red;
 color.green=green;
 color.blue=blue;
}

void ORGF_Primitive::draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2)
{
 unsigned long int delta_x,delta_y,index,steps;
 float x,y,shift_x,shift_y;
 if (x1>x2)
 {
  delta_x=x1-x2;
 }
 else
 {
  delta_x=x2-x1;
 }
 if (y1>y2)
 {
  delta_y=y1-y2;
 }
 else
 {
  delta_y=y2-y1;
 }
 steps=delta_x;
 if (steps<delta_y) steps=delta_y;
 x=x1;
 y=y1;
 shift_x=(float)delta_x/(float)steps;
 shift_y=(float)delta_y/(float)steps;
 for (index=steps;index>0;--index)
 {
  x+=shift_x;
  y+=shift_y;
  surface->draw_pixel(x,y,color.red,color.green,color.blue);
 }

}

void ORGF_Primitive::draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height)
{
 unsigned long int stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 this->draw_line(x,y,stop_x,y);
 this->draw_line(x,stop_y,stop_x,stop_y);
 this->draw_line(x,y,x,stop_y);
 this->draw_line(stop_x,y,stop_x,stop_y);
}

void ORGF_Primitive::draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height)
{
 unsigned long int step_x,step_y,stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 for(step_x=x;step_x<stop_x;++step_x)
 {
  for(step_y=y;step_y<stop_y;++step_y)
  {
   surface->draw_pixel(step_x,step_y,color.red,color.green,color.blue);
  }

 }

}

ORGF_Image::ORGF_Image()
{
 width=0;
 height=0;
 data=NULL;
}

ORGF_Image::~ORGF_Image()
{
 if(data!=NULL) free(data);
}

unsigned char *ORGF_Image::create_buffer(const size_t length)
{
 unsigned char *result;
 result=(unsigned char*)calloc(length,sizeof(unsigned char));
 if(result==NULL)
 {
  ORGF_Show_Error("Can't allocate memory for image buffer");
 }
 return result;
}

void ORGF_Image::clear_buffer()
{
 if(data!=NULL)
 {
  free(data);
  data=NULL;
 }

}

FILE *ORGF_Image::open_image(const char *name)
{
 FILE *target;
 target=fopen(name,"rb");
 if(target==NULL)
 {
  ORGF_Show_Error("Can't open a image file");
 }
 return target;
}

unsigned long int ORGF_Image::get_file_size(FILE *target)
{
 unsigned long int length;
 fseek(target,0,SEEK_END);
 length=ftell(target);
 rewind(target);
 return length;
}

void ORGF_Image::load_tga(const char *name)
{
 FILE *target;
 size_t index,position,amount,compressed_length,uncompressed_length;
 unsigned char *compressed;
 unsigned char *uncompressed;
 TGA_head head;
 TGA_map color_map;
 TGA_image image;
 this->clear_buffer();
 target=this->open_image(name);
 compressed_length=(size_t)this->get_file_size(target)-18;
 fread(&head,3,1,target);
 fread(&color_map,5,1,target);
 fread(&image,10,1,target);
 if((head.color_map!=0)||(image.color!=24))
 {
  ORGF_Show_Error("Invalid image format");
 }
 if(head.type!=2)
 {
  if(head.type!=10)
  {
   ORGF_Show_Error("Invalid image format");
  }

 }
 index=0;
 position=0;
 width=image.width;
 height=image.height;
 uncompressed_length=this->get_data_length();
 uncompressed=this->create_buffer(uncompressed_length);
 if(head.type==2)
 {
  fread(uncompressed,uncompressed_length,1,target);
 }
 if(head.type==10)
 {
  compressed=this->create_buffer(compressed_length);
  fread(compressed,compressed_length,1,target);
  while(index<uncompressed_length)
  {
   if(compressed[position]<128)
   {
    amount=compressed[position]+1;
    amount*=3;
    memmove(uncompressed+index,compressed+(position+1),amount);
    index+=amount;
    position+=1+amount;
   }
   else
   {
    for(amount=compressed[position]-127;amount>0;--amount)
    {
     memmove(uncompressed+index,compressed+(position+1),3);
     index+=3;
    }
    position+=4;
   }

  }
  free(compressed);
 }
 fclose(target);
 data=uncompressed;
}

void ORGF_Image::load_pcx(const char *name)
{
 FILE *target;
 unsigned long int x,y;
 size_t index,position,line,row,length,uncompressed_length;
 unsigned char repeat;
 unsigned char *original;
 unsigned char *uncompressed;
 PCX_head head;
 this->clear_buffer();
 target=this->open_image(name);
 length=(size_t)this->get_file_size(target)-128;
 fread(&head,128,1,target);
 if((head.color*head.planes!=24)&&(head.compress!=1))
 {
  ORGF_Show_Error("Incorrect image format");
 }
 width=head.max_x-head.min_x+1;
 height=head.max_y-head.min_y+1;
 row=3*(size_t)width;
 line=(size_t)head.planes*(size_t)head.plane_length;
 uncompressed_length=row*height;
 index=0;
 position=0;
 original=this->create_buffer(length);
 uncompressed=this->create_buffer(uncompressed_length);
 fread(original,length,1,target);
 fclose(target);
 while (index<length)
 {
  if (original[index]<192)
  {
   uncompressed[position]=original[index];
   position++;
   index++;
  }
  else
  {
   for (repeat=original[index]-192;repeat>0;--repeat)
   {
    uncompressed[position]=original[index+1];
    position++;
   }
   index+=2;
  }

 }
 free(original);
 original=this->create_buffer(uncompressed_length);
 for(x=0;x<width;++x)
 {
  for(y=0;y<height;++y)
  {
   index=(size_t)x*3+(size_t)y*row;
   position=(size_t)x+(size_t)y*line;
   original[index]=uncompressed[position+2*(size_t)head.plane_length];
   original[index+1]=uncompressed[position+(size_t)head.plane_length];
   original[index+2]=uncompressed[position];
  }

 }
 free(uncompressed);
 data=original;
}

unsigned long int ORGF_Image::get_width()
{
 return width;
}

unsigned long int ORGF_Image::get_height()
{
 return height;
}

size_t ORGF_Image::get_data_length()
{
 return (size_t)width*(size_t)height*3;
}

unsigned char *ORGF_Image::get_data()
{
 return data;
}

void ORGF_Image::destroy_image()
{
 width=0;
 height=0;
 this->clear_buffer();
}

ORGF_Canvas::ORGF_Canvas()
{
 image=NULL;
 surface=NULL;
 width=0;
 height=0;
 frames=1;
}

ORGF_Canvas::~ORGF_Canvas()
{
 surface=NULL;
 if(image!=NULL) free(image);
}

void ORGF_Canvas::clear_buffer()
{
 if(image!=NULL) free(image);
}

void ORGF_Canvas::set_width(const unsigned long int image_width)
{
 width=image_width;
}

void ORGF_Canvas::set_height(const unsigned long int image_height)
{
 height=image_height;
}

ORGF_Color *ORGF_Canvas::create_buffer(const unsigned long int image_width,const unsigned long int image_height)
{
 ORGF_Color *result;
 size_t length;
 length=(size_t)image_width*(size_t)image_height;
 result=(ORGF_Color*)calloc(length,3);
 if(result==NULL)
 {
  ORGF_Show_Error("Can't allocate memory for image buffer");
 }
 return result;
}

void ORGF_Canvas::draw_image_pixel(const size_t offset,const unsigned long int x,const unsigned long int y)
{
 surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
}

size_t ORGF_Canvas::get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y)
{
 return (size_t)start+(size_t)x+(size_t)y*(size_t)width;
}

ORGF_Color *ORGF_Canvas::get_image()
{
 return image;
}

size_t ORGF_Canvas::get_length()
{
 return (size_t)width*(size_t)height;
}

unsigned long int ORGF_Canvas::get_image_width()
{
 return width;
}

unsigned long int ORGF_Canvas::get_image_height()
{
 return height;
}

void ORGF_Canvas::set_frames(const unsigned long int amount)
{
 if(amount>1) frames=amount;
}

unsigned long int ORGF_Canvas::get_frames()
{
 return frames;
}

void ORGF_Canvas::initialize(ORGF_Screen *Screen)
{
 surface=Screen;
}

void ORGF_Canvas::load_image(ORGF_Image &buffer)
{
 width=buffer.get_width();
 height=buffer.get_height();
 this->clear_buffer();
 image=this->create_buffer(width,height);
 memmove(image,buffer.get_data(),buffer.get_data_length());
 buffer.destroy_image();
}

void ORGF_Canvas::mirror_image(const ORGF_MIRROR_TYPE kind)
{
 unsigned long int x,y;
 size_t index,index2;
 ORGF_Color *mirrored_image;
 mirrored_image=this->create_buffer(width,height);
 if (kind==ORGF_MIRROR_HORIZONTAL)
 {
  for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    index=this->get_offset(0,x,y);
    index2=this->get_offset(0,(width-x-1),y);
    mirrored_image[index]=image[index2];
   }

  }

 }
 if(kind==ORGF_MIRROR_VERTICAL)
 {
   for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    index=this->get_offset(0,x,y);
    index2=this->get_offset(0,x,(height-y-1));
    mirrored_image[index]=image[index2];
   }

  }

 }
 free(image);
 image=mirrored_image;
}

void ORGF_Canvas::resize_image(const unsigned long int new_width,const unsigned long int new_height)
{
 float x_ratio,y_ratio;
 unsigned long int x,y;
 size_t index,index2;
 ORGF_Color *scaled_image;
 scaled_image=this->create_buffer(new_width,new_height);
 x_ratio=(float)width/(float)new_width;
 y_ratio=(float)height/(float)new_height;
 for (x=0;x<new_width;++x)
 {
  for (y=0;y<new_height;++y)
  {
   index=(size_t)x+(size_t)y*(size_t)new_width;
   index2=(size_t)(x_ratio*(float)x)+(size_t)width*(size_t)(y_ratio*(float)y);
   scaled_image[index]=image[index2];
  }

 }
 free(image);
 image=scaled_image;
 width=new_width;
 height=new_height;
}

ORGF_Background::ORGF_Background()
{
 start=0;
 background_width=0;
 background_height=0;
 frame=1;
 current_kind=ORGF_NORMAL_BACKGROUND;
}

ORGF_Background::~ORGF_Background()
{

}

void ORGF_Background::set_kind(ORGF_BACKGROUND_TYPE kind)
{
 switch(kind)
 {
  case ORGF_NORMAL_BACKGROUND:
  background_width=this->get_image_width();
  background_height=this->get_image_height();
  start=0;
  break;
  case ORGF_HORIZONTAL_BACKGROUND:
  background_width=this->get_image_width()/this->get_frames();
  background_height=this->get_image_height();
  start=(frame-1)*background_width;
  break;
  case ORGF_VERTICAL_BACKGROUND:
  background_width=this->get_image_width();
  background_height=this->get_image_height()/this->get_frames();
  start=(frame-1)*background_width*background_height;
  break;
 }
 current_kind=kind;
}

void ORGF_Background::set_target(const unsigned long int target)
{
 if((target>0)&&(target<=this->get_frames()))
 {
  frame=target;
  this->set_kind(current_kind);
 }

}

void ORGF_Background::draw_background()
{
 unsigned long int x,y;
 size_t offset;
 for(x=0;x<background_width;++x)
 {
  for(y=0;y<background_height;++y)
  {
   offset=this->get_offset(start,x,y);
   this->draw_image_pixel(offset,x,y);
  }

 }

}

ORGF_Sprite::ORGF_Sprite()
{
 current_x=0;
 current_y=0;
 sprite_width=0;
 sprite_height=0;
 frame=0;
 start=0;
 current_kind=ORGF_SINGE_SPRITE;
}

ORGF_Sprite::~ORGF_Sprite()
{

}

bool ORGF_Sprite::compare_pixels(const ORGF_Color &first,const ORGF_Color &second)
{
 bool result;
 result=false;
 if ((first.red!=second.red)||(first.green!=second.green))
 {
  result=true;
 }
 else
 {
  if(first.blue!=second.blue) result=true;
 }
 return result;
}

void ORGF_Sprite::draw_sprite_pixel(const size_t offset,const unsigned long int x,const unsigned long int y)
{
 if(this->compare_pixels(image[0],image[offset])==true) this->draw_image_pixel(offset,x,y);
}

unsigned long int ORGF_Sprite::get_x()
{
 return current_x;
}

unsigned long int ORGF_Sprite::get_y()
{
 return current_y;
}

unsigned long int ORGF_Sprite::get_width()
{
 return sprite_width;
}

unsigned long int ORGF_Sprite::get_height()
{
 return sprite_height;
}

ORGF_Sprite* ORGF_Sprite::get_handle()
{
 return this;
}

ORGF_Box ORGF_Sprite::get_box()
{
 ORGF_Box target;
 target.x=current_x;
 target.y=current_y;
 target.width=sprite_width;
 target.height=sprite_height;
 return target;
}

void ORGF_Sprite::set_kind(const ORGF_SPRITE_TYPE kind)
{
 switch(kind)
 {
  case ORGF_SINGE_SPRITE:
  sprite_width=this->get_image_width();
  sprite_height=this->get_image_height();
  start=0;
  break;
  case ORGF_HORIZONTAL_STRIP:
  sprite_width=this->get_image_width()/this->get_frames();
  sprite_height=this->get_image_height();
  start=(frame-1)*sprite_width;
  break;
  case ORGF_VERTICAL_STRIP:
  sprite_width=this->get_image_width();
  sprite_height=this->get_image_height()/this->get_frames();
  start=(frame-1)*sprite_width*sprite_height;
  break;
 }
 current_kind=kind;
}

ORGF_SPRITE_TYPE ORGF_Sprite::get_kind()
{
 return current_kind;
}

void ORGF_Sprite::set_target(const unsigned long int target)
{
 if((target>0)&&(target<=this->get_frames()))
 {
  frame=target;
  this->set_kind(current_kind);
 }

}

void ORGF_Sprite::clone(ORGF_Sprite &target)
{
 this->set_width(target.get_image_width());
 this->set_height(target.get_image_height());
 this->set_frames(target.get_frames());
 this->set_kind(target.get_kind());
 image=this->create_buffer(target.get_image_width(),target.get_image_width());
 memmove(image,target.get_image(),target.get_length());
}

void ORGF_Sprite::draw_sprite(const unsigned long int x,const unsigned long int y)
{
 size_t offset;
 unsigned long int sprite_x,sprite_y;
 current_x=x;
 current_y=y;
 for(sprite_x=0;sprite_x<sprite_width;++sprite_x)
 {
  for(sprite_y=0;sprite_y<sprite_height;++sprite_y)
  {
   offset=this->get_offset(start,sprite_x,sprite_y);
   this->draw_sprite_pixel(offset,x+sprite_x,y+sprite_y);
  }

 }

}

ORGF_Text::ORGF_Text()
{
 current_x=0;
 current_y=0;
 step_x=0;
 sprite=NULL;
}

ORGF_Text::~ORGF_Text()
{
 sprite=NULL;
}

void ORGF_Text::draw_character(const char target)
{
 if((target>31)||(target<0))
 {
  sprite->set_target((unsigned long int)target+1);
  sprite->draw_sprite(step_x,current_y);
  step_x+=sprite->get_width();
 }

}

void ORGF_Text::set_position(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
}

void ORGF_Text::load_font(ORGF_Sprite *font)
{
 sprite=font;
 sprite->set_frames(256);
 sprite->set_kind(ORGF_HORIZONTAL_STRIP);
}

void ORGF_Text::draw_text(const char *text)
{
 size_t index,length;
 length=strlen(text);
 step_x=current_x;
 for (index=0;index<length;++index)
 {
  this->draw_character(text[index]);
 }

}

bool ORGF_Collision::check_horizontal_collision(const ORGF_Box &first,const ORGF_Box &second)
{
 bool result;
 result=false;
 if((first.x+first.width)>=second.x)
 {
  if(first.x<=(second.x+second.width)) result=true;
 }
 return result;
}

bool ORGF_Collision::check_vertical_collision(const ORGF_Box &first,const ORGF_Box &second)
{
 bool result;
 result=false;
 if((first.y+first.height)>=second.y)
 {
  if(first.y<=(second.y+second.height)) result=true;
 }
 return result;
}

bool ORGF_Collision::check_collision(const ORGF_Box &first,const ORGF_Box &second)
{
 return this->check_horizontal_collision(first,second) || this->check_vertical_collision(first,second);
}