// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright © 2017-2018, Popov Evgeniy Alekseyevich

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
  ORGF_Keys[(lParam >> 16)&0x7f]=ORGFKEY_PRESS;
  break;
  case WM_KEYUP:
  ORGF_Keys[(lParam >> 16)&0x7f]=ORGFKEY_RELEASE;
  break;
 }
 return DefWindowProc(window,Message,wParam,lParam);
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
  puts("Can't create synchronization timer");
  exit(EXIT_FAILURE);
 }

}

void ORGF_Synchronization::set_timer(const unsigned long int interval)
{
 LARGE_INTEGER start;
 start.QuadPart=0;
 if(SetWaitableTimer(timer,&start,interval,NULL,NULL,FALSE)==FALSE)
 {
  puts("Can't set timer");
  exit(EXIT_FAILURE);
 }

}

void ORGF_Synchronization::wait_timer()
{
 WaitForSingleObject(timer,INFINITE);
}

ORGF_Engine::ORGF_Engine()
{
 window_class.lpszClassName=TEXT("ORGF");
 window_class.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
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

void ORGF_Engine::prepare_engine()
{
 window_class.hInstance=GetModuleHandle(NULL);
 if(window_class.hInstance==NULL)
 {
  puts("Can't get the application instance");
  exit(EXIT_FAILURE);
 }
 window_class.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
 if (window_class.hbrBackground==NULL)
 {
  puts("Can't set background color");
  exit(EXIT_FAILURE);
 }
 window_class.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 if (window_class.hIcon==NULL)
 {
  puts("Can't load the standart program icon");
  exit(EXIT_FAILURE);
 }
 window_class.hCursor=LoadCursor(NULL,IDC_ARROW);
 if (window_class.hCursor==NULL)
 {
  puts("Can't load the standart cursor");
  exit(EXIT_FAILURE);
 }
 if (RegisterClass(&window_class)==0)
 {
  puts("Can't register window class");
  exit(EXIT_FAILURE);
 }

}

void ORGF_Engine::create_window()
{
 window=CreateWindow(window_class.lpszClassName,NULL,WS_VISIBLE|WS_POPUP,0,0,width,height,NULL,NULL,window_class.hInstance,NULL);
 if (window==NULL)
 {
  puts("Can't create window");
  exit(EXIT_FAILURE);
 }
 EnableWindow(window,TRUE);
 SetFocus(window);
}

void ORGF_Engine::capture_mouse()
{
 RECT border;
 if(GetClientRect(window,&border)==FALSE)
 {
  puts("Can't capture window");
  exit(EXIT_FAILURE);
 }
 if(ClipCursor(&border)==FALSE)
 {
  puts("Can't capture cursor");
  exit(EXIT_FAILURE);
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

void ORGF_Frame::create_render_buffer()
{
 buffer_length=(size_t)frame_width*(size_t)frame_height;
 buffer=(unsigned long int*)calloc(buffer_length,sizeof(unsigned long int));
 if(buffer==NULL)
 {
  puts("Can't allocate memory for render buffer");
  exit(EXIT_FAILURE);
 }
 else
 {
  buffer_length*=sizeof(unsigned long int);
 }

}

unsigned long int ORGF_Frame::get_rgb(const unsigned long int red,const unsigned long int green,const unsigned long int blue)
{
 return red+(green<<8)+(blue<<16);
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
}

ORGF_Display::~ORGF_Display()
{
 ChangeDisplaySettings(NULL,0);
}

void ORGF_Display::set_video_mode(DEVMODE mode)
{
 if (ChangeDisplaySettings(&mode,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
 {
  puts("Can't change video mode");
  exit(EXIT_FAILURE);
 }

}

DEVMODE ORGF_Display::get_video_mode()
{
 DEVMODE mode;
 memset(&mode,0,sizeof(DEVMODE));
 mode.dmSize=sizeof(DEVMODE);
 if (EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&mode)==FALSE)
 {
  puts("Can't get display setting");
  exit(EXIT_FAILURE);
 }
 else
 {
  width=mode.dmPelsWidth;
  height=mode.dmPelsHeight;
 }
 return mode;
}

void ORGF_Display::check_video_mode()
{
 display=this->get_video_mode();
 if(display.dmBitsPerPel<16) display.dmBitsPerPel=16;
 this->set_video_mode(display);
}

ORGF_Render::ORGF_Render()
{
 memset(&setting,0,sizeof(BITMAPINFO));
 context=NULL;
}

ORGF_Render::~ORGF_Render()
{

}

void ORGF_Render::set_render_setting()
{
 setting.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
 setting.bmiHeader.biWidth=frame_width;
 setting.bmiHeader.biHeight=-1*(long int)frame_height;
 setting.bmiHeader.biPlanes=1;
 setting.bmiHeader.biBitCount=32;
 setting.bmiHeader.biCompression=BI_RGB;
}

void ORGF_Render::refresh()
{
 context=GetDC(window);
 if(context==NULL)
 {
  puts("Can't get window context");
  exit(EXIT_FAILURE);
 }
 else
 {
  StretchDIBits(context,0,0,width,height,0,0,frame_width,frame_height,buffer,&setting,DIB_RGB_COLORS,SRCCOPY);
  ReleaseDC(window,context);
 }

}

void ORGF_Screen::initialize()
{
 this->prepare_engine();
 this->check_video_mode();
 this->create_render_buffer();
 this->create_timer();
 this->create_window();
 this->capture_mouse();
 this->set_render_setting();
 this->create_timer();
 this->set_timer(17);
}

bool ORGF_Screen::sync()
{
 bool quit;
 this->refresh();
 quit=this->process_message();
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
  puts("Can't allocate memory for keyboard state buffer");
  exit(EXIT_FAILURE);
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
  puts("Can't set the mouse cursor position");
  exit(EXIT_FAILURE);
 }

}

unsigned long int ORGF_Mouse::get_x()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
 }
 return position.x;
}

unsigned long int ORGF_Mouse::get_y()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
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
 length=sizeof(XINPUT_STATE);
 XInputEnable(TRUE);
 memset(&current,0,length);
 memset(&preversion,0,length);
 memset(&vibration,0,sizeof(XINPUT_VIBRATION));
 memset(&battery,0,sizeof(XINPUT_BATTERY_INFORMATION));
 active=0;
}

ORGF_Gamepad::~ORGF_Gamepad()
{
 XInputEnable(FALSE);
}

bool ORGF_Gamepad::read_battery_status()
{
 bool result;
 result=false;
 if(XInputGetBatteryInformation(active,BATTERY_DEVTYPE_GAMEPAD,&battery)==ERROR_SUCCESS) return result;
 return result;
}

void ORGF_Gamepad::clear_state()
{
 memset(&current,0,length);
 memset(&preversion,0,length);
}

bool ORGF_Gamepad::read_state()
{
 bool result;
 result=false;
 if(XInputGetState(active,&current)==ERROR_SUCCESS) result=true;
 return result;
}

bool ORGF_Gamepad::write_state()
{
 bool result;
 result=false;
 if(XInputSetState(active,&vibration)==ERROR_SUCCESS) result=true;
 return result;
}

void ORGF_Gamepad::set_motor(const unsigned short int left,const unsigned short int right)
{
 vibration.wLeftMotorSpeed=left;
 vibration.wRightMotorSpeed=right;
}

bool ORGF_Gamepad::check_button(XINPUT_STATE &target,const unsigned short int button)
{
 bool result;
 result=false;
 if(target.Gamepad.wButtons&button) result=true;
 return result;
}

bool ORGF_Gamepad::check_trigger(XINPUT_STATE &target,const unsigned char trigger)
{
 bool result;
 result=false;
 if((trigger==ORGF_GAMEPAD_LEFT_TRIGGER)&&(target.Gamepad.bLeftTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) result=true;
 if((trigger==ORGF_GAMEPAD_RIGHT_TRIGGER)&&(target.Gamepad.bRightTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) result=true;
 return result;
}

void ORGF_Gamepad::set_active(const unsigned long int gamepad)
{
 if(active<ORGF_GAMEPAD_AMOUNT)
 {
  this->clear_state();
  active=gamepad;
 }

}

unsigned long int ORGF_Gamepad::get_active()
{
 return active;
}

unsigned long int ORGF_Gamepad::get_amount()
{
 unsigned long int old,result;
 result=0;
 old=active;
 for(active=0;active<ORGF_GAMEPAD_AMOUNT;++active)
 {
  if(this->read_state()==true)
  {
   this->clear_state();
   result=active+1;
  }

 }
 active=old;
 return result;
}

bool ORGF_Gamepad::check_connection()
{
 return this->read_state();
}

bool ORGF_Gamepad::is_wireless()
{
 bool result;
 result=false;
 if(this->read_battery_status()==true)
 {
  if(battery.BatteryType!=BATTERY_TYPE_DISCONNECTED)
  {
   if(battery.BatteryType!=BATTERY_TYPE_WIRED) result=true;
  }

 }
 return result;
}

unsigned char ORGF_Gamepad::get_battery_type()
{
 unsigned char result;
 result=ORGF_GAMEPAD_BATTERY_ERROR;
 if(this->read_battery_status()==true)
 {
  switch (battery.BatteryType)
  {
   case BATTERY_TYPE_ALKALINE:
   result=ORGF_GAMEPAD_BATTERY_ALKAINE;
   break;
   case BATTERY_TYPE_NIMH:
   result=ORGF_GAMEPAD_BATTERY_NIMH;
   break;
   case BATTERY_TYPE_UNKNOWN:
   result=ORGF_GAMEPAD_BATTERY_UNKNOW;
   break;
  }

 }
 return result;
}

unsigned char ORGF_Gamepad::get_battery_level()
{
 unsigned char result;
 result=ORGF_GAMEPAD_BATTERY_ERROR;
 if(this->read_battery_status()==true)
 {
  switch (battery.BatteryType)
  {
   case BATTERY_LEVEL_EMPTY:
   result=ORGF_GAMEPAD_BATTERY_EMPTY;
   break;
   case BATTERY_LEVEL_LOW:
   result=ORGF_GAMEPAD_BATTERY_LOW;
   break;
   case BATTERY_LEVEL_MEDIUM:
   result=ORGF_GAMEPAD_BATTERY_MEDIUM;
   break;
   case BATTERY_LEVEL_FULL:
   result=ORGF_GAMEPAD_BATTERY_FULL;
   break;
  }
  if((battery.BatteryType==BATTERY_TYPE_WIRED)||(battery.BatteryType==BATTERY_TYPE_DISCONNECTED)) result=ORGF_GAMEPAD_BATTERY_ERROR;
 }
 return result;
}

void ORGF_Gamepad::update()
{
 preversion=current;
 if(this->read_state()==false) this->clear_state();
}

bool ORGF_Gamepad::check_button_hold(const unsigned short int button)
{
 return this->check_button(current,button);
}

bool ORGF_Gamepad::check_button_press(const unsigned short int button)
{
 bool result;
 result=false;
 if(this->check_button(current,button)==true)
 {
  if(this->check_button(preversion,button)==false) result=true;
 }
 return result;
}

bool ORGF_Gamepad::check_button_release(const unsigned short int button)
{
 bool result;
 result=false;
 if(this->check_button(current,button)==false)
 {
  if(this->check_button(preversion,button)==true) result=true;
 }
 return result;
}

bool ORGF_Gamepad::check_trigger_hold(const unsigned char trigger)
{
 return this->check_trigger(current,trigger);
}

bool ORGF_Gamepad::check_trigger_press(const unsigned char trigger)
{
 bool result;
 result=false;
 if(this->check_trigger(current,trigger)==true)
 {
  if(this->check_trigger(preversion,trigger)==false) result=true;
 }
 return result;
}

bool ORGF_Gamepad::check_trigger_release(const unsigned char trigger)
{
 bool result;
 result=false;
 if(this->check_trigger(current,trigger)==false)
 {
  if(this->check_trigger(preversion,trigger)==true) result=true;
 }
 return result;
}

bool ORGF_Gamepad::set_vibration(const unsigned short int left,const unsigned short int right)
{
 this->set_motor(left,right);
 return this->write_state();
}

char ORGF_Gamepad::get_stick_x(const unsigned char stick)
{
 char result;
 short int control;
 result=0;
 if(stick==ORGF_GAMEPAD_LEFT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLX>=control) result=1;
  if(current.Gamepad.sThumbLX<=-1*control) result=-1;
 }
 if(stick==ORGF_GAMEPAD_RIGHT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRX>=control) result=1;
  if(current.Gamepad.sThumbRX<=-1*control) result=-1;
 }
 return result;
}

char ORGF_Gamepad::get_stick_y(const unsigned char stick)
{
 char result;
 short int control;
 result=0;
 if(stick==ORGF_GAMEPAD_LEFT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLY>=control) result=1;
  if(current.Gamepad.sThumbLY<=-1*control) result=-1;
 }
 if(stick==ORGF_GAMEPAD_RIGHT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRY>=control) result=1;
  if(current.Gamepad.sThumbRY<=-1*control) result=-1;
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
 CoUninitialize();
}

wchar_t *ORGF_Multimedia::convert_file_name(const char *target)
{
 wchar_t *name;
 size_t index,length;
 length=strlen(target);
 name=(wchar_t*)calloc(length+1,sizeof(wchar_t));
 if(name==NULL)
 {
  puts("Can't allocate memory");
  exit(EXIT_FAILURE);
 }
 for(index=0;index<length;index++) name[index]=btowc(target[index]);
 return name;
}

void ORGF_Multimedia::open(const wchar_t *target)
{
 player->StopWhenReady();
 if(loader->RenderFile(target,NULL)!=S_OK)
 {
  puts("Can't load a multimedia file");
  exit(EXIT_FAILURE);
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
  puts("Can't get the current and the end position");
  exit(EXIT_FAILURE);
 }
 return result;
}

void ORGF_Multimedia::rewind()
{
 long long position;
 position=0;
 if(controler->SetPositions(&position,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning)!=S_OK)
 {
  puts("Can't set start position");
  exit(EXIT_FAILURE);
 }

}

void ORGF_Multimedia::initialize()
{
 HRESULT status;
 status=CoInitialize(NULL);
 if(status!=S_OK)
 {
  if(status!=S_FALSE)
  {
   puts("Can't initialize COM");
   exit(EXIT_FAILURE);
  }

 }
 if(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&loader)!=S_OK)
 {
  puts("Can't create a multimedia loader");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaControl,(void**)&player)!=S_OK)
 {
  puts("Can't create a multimedia player");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaSeeking,(void**)&controler)!=S_OK)
 {
  puts("Can't create a player controler");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IVideoWindow,(void**)&video)!=S_OK)
 {
  puts("Can't create a video player");
  exit(EXIT_FAILURE);
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
  puts("Can't get the multimedia state");
  exit(EXIT_FAILURE);
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
  puts("Can't get the memory status");
  exit(EXIT_FAILURE);
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
  puts("Can't create log file");
  exit(EXIT_FAILURE);
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
 surface=NULL;
}

ORGF_Primitive::~ORGF_Primitive()
{
 surface=NULL;
}

void ORGF_Primitive::initialize(ORGF_Screen *Screen)
{
 surface=Screen;
}

void ORGF_Primitive::draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2,const unsigned char red,const unsigned char green,const unsigned char blue)
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
  surface->draw_pixel(x,y,red,green,blue);
 }

}

void ORGF_Primitive::draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 this->draw_line(x,y,stop_x,y,red,green,blue);
 this->draw_line(x,stop_y,stop_x,stop_y,red,green,blue);
 this->draw_line(x,y,x,stop_y,red,green,blue);
 this->draw_line(stop_x,y,stop_x,stop_y,red,green,blue);
}

void ORGF_Primitive::draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 unsigned long int step_x,step_y,stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 for(step_x=x;step_x<stop_x;++step_x)
 {
  for(step_y=y;step_y<stop_y;++step_y)
  {
   surface->draw_pixel(step_x,step_y,red,green,blue);
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

unsigned char *ORGF_Image::create_buffer(const unsigned long int length)
{
 unsigned char *result;
 result=(unsigned char*)calloc((size_t)length,sizeof(unsigned char));
 if(result==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
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
  puts("Can't open a image file");
  exit(EXIT_FAILURE);
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
 unsigned long int index,position,amount,compressed_length,uncompressed_length;
 unsigned char *compressed;
 unsigned char *uncompressed;
 TGA_head head;
 TGA_map color_map;
 TGA_image image;
 this->clear_buffer();
 target=this->open_image(name);
 compressed_length=this->get_file_size(target)-18;
 fread(&head,3,1,target);
 fread(&color_map,5,1,target);
 fread(&image,10,1,target);
 if((head.color_map!=0)||(image.color!=24))
 {
  puts("Invalid image format");
  exit(EXIT_FAILURE);
 }
 if(head.type!=2)
 {
  if(head.type!=10)
  {
   puts("Invalid image format");
   exit(EXIT_FAILURE);
  }

 }
 index=0;
 position=0;
 uncompressed_length=3*image.width*image.height;
 uncompressed=this->create_buffer(uncompressed_length);
 if(head.type==2)
 {
  fread(uncompressed,(size_t)uncompressed_length,1,target);
 }
 if(head.type==10)
 {
  compressed=this->create_buffer(compressed_length);
  fread(compressed,(size_t)compressed_length,1,target);
  while(index<uncompressed_length)
  {
   if(compressed[(size_t)position]<128)
   {
    amount=compressed[(size_t)position]+1;
    amount*=3;
    memmove(uncompressed+(size_t)index,compressed+(size_t)(position+1),(size_t)amount);
    index+=amount;
    position+=1+amount;
   }
   else
   {
    for(amount=compressed[(size_t)position]-127;amount>0;--amount)
    {
     memmove(uncompressed+(size_t)index,compressed+(size_t)(position+1),3);
     index+=3;
    }
    position+=4;
   }

  }
  free(compressed);
 }
 fclose(target);
 width=image.width;
 height=image.height;
 data=uncompressed;
}

void ORGF_Image::load_pcx(const char *name)
{
 FILE *target;
 unsigned long int x,y,index,position,line,row,length,uncompressed_length;
 unsigned char repeat;
 unsigned char *original;
 unsigned char *uncompressed;
 PCX_head head;
 this->clear_buffer();
 target=this->open_image(name);
 length=this->get_file_size(target)-128;
 fread(&head,128,1,target);
 if((head.color*head.planes!=24)&&(head.compress!=1))
 {
  puts("Incorrect image format");
  exit(EXIT_FAILURE);
 }
 width=head.max_x-head.min_x+1;
 height=head.max_y-head.min_y+1;
 row=3*width;
 line=head.planes*head.plane_length;
 uncompressed_length=row*height;
 index=0;
 position=0;
 original=this->create_buffer(length);
 uncompressed=this->create_buffer(uncompressed_length);
 fread(original,(size_t)length,1,target);
 fclose(target);
 while (index<length)
 {
  if (original[(size_t)index]<192)
  {
   uncompressed[(size_t)position]=original[(size_t)index];
   position++;
   index++;
  }
  else
  {
   for (repeat=original[(size_t)index]-192;repeat>0;--repeat)
   {
    uncompressed[(size_t)position]=original[(size_t)index+1];
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
   index=x*3+y*row;
   position=x+y*line;
   original[(size_t)index]=uncompressed[(size_t)position+2*(size_t)head.plane_length];
   original[(size_t)index+1]=uncompressed[(size_t)position+(size_t)head.plane_length];
   original[(size_t)index+2]=uncompressed[(size_t)position];
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

unsigned long int ORGF_Image::get_data_length()
{
 return width*height*3;
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

ORGF_Color *ORGF_Canvas::create_buffer(const unsigned long int image_width,const unsigned long int image_height)
{
 ORGF_Color *result;
 size_t length;
 length=(size_t)image_width*(size_t)image_height;
 result=(ORGF_Color*)calloc(length,3);
 if(result==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
 }
 return result;
}

ORGF_Color *ORGF_Canvas::get_image()
{
 return image;
}

unsigned long int ORGF_Canvas::get_width()
{
 return width;
}

unsigned long int ORGF_Canvas::get_height()
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
 unsigned long int length;
 width=buffer.get_width();
 height=buffer.get_height();
 length=buffer.get_data_length();
 if(image!=NULL) free(image);
 image=this->create_buffer(width,height);
 memmove(image,buffer.get_data(),(size_t)length);
 buffer.destroy_image();
}

void ORGF_Canvas::mirror_image(const unsigned char kind)
{
 unsigned long int x,y,index,index2;
 ORGF_Color *mirrored_image;
 mirrored_image=this->create_buffer(width,height);
 if (kind==0)
 {
  for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    index=x+(y*width);
    index2=(width-x-1)+(y*width);
    mirrored_image[(size_t)index]=image[(size_t)index2];
   }

  }

 }
 else
 {
   for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    index=x+(y*width);
    index2=x+(height-y-1)*width;
    mirrored_image[(size_t)index]=image[(size_t)index2];
   }

  }

 }
 free(image);
 image=mirrored_image;
}

void ORGF_Canvas::resize_image(const unsigned long int new_width,const unsigned long int new_height)
{
 float x_ratio,y_ratio;
 unsigned long int x,y,index,index2;
 ORGF_Color *scaled_image;
 scaled_image=this->create_buffer(new_width,new_height);
 x_ratio=(float)width/(float)new_width;
 y_ratio=(float)height/(float)new_height;
 for (x=0;x<new_width;++x)
 {
  for (y=0;y<new_height;++y)
  {
   index=x+(y*new_width);
   index2=(unsigned long int)(x_ratio*(float)x)+width*(unsigned long int)(y_ratio*(float)y);
   scaled_image[(size_t)index]=image[(size_t)index2];
  }

 }
 free(image);
 image=scaled_image;
 width=new_width;
 height=new_height;
}

void ORGF_Background::draw_horizontal_background(const unsigned long int frame)
{
 unsigned long int x,y,start,offset,frame_width;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 for (x=0;x<frame_width;++x)
 {
  for (y=0;y<height;++y)
  {
   offset=(start+x)+(width*y);
   surface->draw_pixel(x,y,image[(size_t)offset].red,image[(size_t)offset].green,image[(size_t)offset].blue);
  }

 }

}

void ORGF_Background::draw_vertical_background(const unsigned long int frame)
{
 unsigned long int x,y,start,offset,frame_height;
 frame_height=height/frames;
 start=(frame-1)*frame_height*width;
 for (x=0;x<width;++x)
 {
  for (y=0;y<frame_height;++y)
  {
   offset=(start+x)+(width*y);
   surface->draw_pixel(x,y,image[(size_t)offset].red,image[(size_t)offset].green,image[(size_t)offset].blue);
  }

 }

}

void ORGF_Background::draw_background()
{
 this->draw_horizontal_background(1);
}

ORGF_Sprite::ORGF_Sprite()
{
 current_x=0;
 current_y=0;
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

void ORGF_Sprite::clone(ORGF_Sprite &target)
{
 unsigned long int length;
 frames=target.get_frames();
 width=target.get_sprite_width();
 height=target.get_sprite_height();
 length=width*height*3;
 image=this->create_buffer(width,height);
 memmove(image,target.get_image(),(size_t)length);
}

void ORGF_Sprite::draw_sprite_frame(const unsigned long int x,const unsigned long int y,const unsigned long int frame)
{
 unsigned long int sprite_x,sprite_y,start,offset,frame_width;
 current_x=x;
 current_y=y;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 for(sprite_x=0;sprite_x<frame_width;++sprite_x)
 {
  for(sprite_y=0;sprite_y<height;++sprite_y)
  {
   offset=(start+sprite_x)+(sprite_y*width);
   if(this->compare_pixels(image[0],image[(size_t)offset])==true) surface->draw_pixel(x+sprite_x,y+sprite_y,image[(size_t)offset].red,image[(size_t)offset].green,image[(size_t)offset].blue);
  }

 }

}

void ORGF_Sprite::draw_sprite(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
 this->draw_sprite_frame(x,y,1);
}

unsigned long int ORGF_Sprite::get_x()
{
 return current_x;
}

unsigned long int ORGF_Sprite::get_y()
{
 return current_y;
}

unsigned long int ORGF_Sprite::get_sprite_width()
{
 return width/frames;
}

unsigned long int ORGF_Sprite::get_sprite_height()
{
 return height;
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
 target.width=width/frames;
 target.height=height;
 return target;
}

ORGF_Text::ORGF_Text()
{
 current_x=0;
 current_y=0;
 sprite=NULL;
}

ORGF_Text::~ORGF_Text()
{
 sprite=NULL;
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
}

void ORGF_Text::draw_text(const char *text)
{
 unsigned long int current,step_x,index;
 step_x=current_x;
 for (index=0;text[(size_t)index]!=0;++index)
 {
  if ((text[(size_t)index]>31)||(text[(size_t)index]<0))
  {
   current=(unsigned long int)text[(size_t)index];
   sprite->draw_sprite_frame(step_x,current_y,current+1);
   step_x+=sprite->get_sprite_width();
  }

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