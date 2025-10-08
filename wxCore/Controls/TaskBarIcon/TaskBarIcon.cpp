#include "Gammasoft.xpm"
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/taskbar.h>
#include <wx/timer.h>

// Workaround : with wxWidgets version <= 3.2.0 wxTaskBarIcon mouse double click doesn't work on macOS
#if !defined(__APPLE__)
using TaskBarIcon = wxTaskBarIcon;
#else
class TaskBarIcon : public wxTaskBarIcon {
public:
  TaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE) : wxTaskBarIcon {iconType} {
    timer.Bind(wxEVT_TIMER, [&](wxTimerEvent& event) {
      wxPostEvent(this, wxTaskBarIconEvent(wxEVT_TASKBAR_LEFT_DOWN, this));
      wxPostEvent(this, wxTaskBarIconEvent(wxEVT_TASKBAR_LEFT_UP, this));
    });
  }
  
protected:
  wxMenu* CreatePopupMenu() override {
    if (!timer.IsRunning()) timer.StartOnce(wxSystemSettings::GetMetric(wxSYS_DCLICK_MSEC));
    else wxPostEvent(this, wxTaskBarIconEvent(wxEVT_TASKBAR_LEFT_DCLICK, this));
    return wxTaskBarIcon::CreatePopupMenu();
  }
  
private:
  wxTimer timer;
};
#endif

namespace TaskBarIconExample {
  class Frame : public wxFrame {
  public:
    Frame() : wxFrame {nullptr, wxID_ANY, "TaskBarIcon example"} {
      taskBarIcon.SetIcon(wxBitmapBundle::FromBitmap(wxBitmap {GammasoftIcon}));
      taskBarIcon.Bind(wxEVT_TASKBAR_LEFT_DCLICK, [&](wxTaskBarIconEvent& event) {
        Show(!IsShown());
      });
    }
    
  private:
    wxPanel* panel = new wxPanel {this};
    wxStaticText* staticText1 = new wxStaticText {panel, wxID_ANY, "Double click on Gammasoft tray icon\nto show or hide this frame.", {10, 10}};
    TaskBarIcon taskBarIcon;
  };

  class Application : public wxApp {
    bool OnInit() override {return (new Frame)->Show();}
  };
}

wxIMPLEMENT_APP(TaskBarIconExample::Application);
