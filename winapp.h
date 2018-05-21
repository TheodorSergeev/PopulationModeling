#include <wx/wx.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/aboutdlg.h>
#include <wx/event.h>

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <fstream>

#include "header/classes.h"

// Number of cells of the envoronment field grid
const int CELLS_NUM_X = 20;
const int CELLS_NUM_Y = 20;

// wxWidgets application class
class CWinApplication: public wxApp
{

private:

    float update_freq; // moves per second
    CEnvironment* experiment;

public:

    virtual bool OnInit();
    virtual int  OnExit();

};


// Declaration for crossreferences between CFieldDrawPane and CCMainWindow
class CMainWindow;                     // Window containing menu, buttons, drawing pane etc.
class CFieldDrawPane;                  // Draws grid and cells
CEnvironment* SetInitialConditions();  // Initinal conditions - position of bacteries in the beginning of the experiment


// Colors for drawing cells
const int NUM_COLOURS = 11;
const wxColour COLONY_COLOURS[NUM_COLOURS] = {wxT("GREY"),
                                              wxT("GOLD"), wxT("INDIAN RED"), wxT("LIGHT BLUE") , wxT("LIGHT GREEN"), wxT("ORCHID"),
                                              wxT("GOLDENROD"), wxT("FIREBRICK"), wxT("NAVY"), wxT("FOREST GREEN"), wxT("DARK ORCHID")};

// Main window size
const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH  = 800;

// Drawing field positioning in the main window
const double BORDER_X_LEFT  = 50;
const double BORDER_Y_TOP   = 50;
const double FIELD_X_SIZE   = std::min(WINDOW_HEIGHT - 2 * BORDER_Y_TOP, WINDOW_WIDTH - 2 * BORDER_X_LEFT);
const double FIELD_Y_SIZE   = FIELD_X_SIZE;

// Experiment iteration speed (in millisecond)
const int ITER_SPEED_DEF = 500;
const int ITER_SPEED_DELTA = 250;

// Buttons symbols
const wxString BUT_ITER_SYMB_INC(wxT("&>>"));
const wxString BUT_ITER_SYMB_DEC(wxT("&<<"));
const wxString BUT_ITER_SYMB_PAUSE(wxT("&■"));
const wxString BUT_ITER_SYMB_START(wxT("&▶"));

struct SExperimentStatistics
{

    int total = CELLS_NUM_X * CELLS_NUM_Y; // total number of cells in the grid
    int food;                              // number of food cells
    int poison;                            // number of poison cells
    int cells;                             // number of bacteries cells
    std::map <string, int> cell;           // name of a bacterie type / number of them

};


// Window containing menu, buttons, drawing pane etc
class CMainWindow: public wxFrame
{

private:

// Head menu bar elements
    wxMenuBar*  headmenu_bar;
    wxMenu*     headmenu_item;
    wxMenuItem* headmenu_but_about; // "but" = button
    wxMenuItem* headmenu_but_quit;

// Process speed controls
    wxStaticText *text_speed;
    wxButton *but_speed_inc;
    wxButton *but_speed_dec;
    wxButton *but_speed_pause;

// Drawing pane
    CFieldDrawPane* fdraw_pane;
    wxPanel* base_panel;

// Experiment elements
    int iter_done;  // iterations already done
    int iter_freq;  // time between two experiment iterations in milliseconds
    wxTimer timer;  // times the time between iteration

    SExperimentStatistics stats; // experiment statistics
    std::fstream stat_log;       // statistics is written to this file

    void GetStatistics();        // collect statistics: count number of bacteries, food, poiseon etc
    void WriteStatistics();      // print collect statistics to a stat_log file

    wxString IterFreqStr();      // constructs a string to be printed out as a wxStaticTextBox


 public:

    CEnvironment* experiment;

    CMainWindow(const wxString& title);

    void OnOpen                ();
    void OnAbout               (wxCommandEvent& WXUNUSED(event));
    void OnQuit                (wxCommandEvent& event);
    void OnButtonClick_SpeedInc(wxCommandEvent& event);
    void OnButtonClick_SpeedDec(wxCommandEvent& event);
    void OnButtonClick_Pause   (wxCommandEvent& event);

    void OnTimer(wxTimerEvent& event);

    DECLARE_EVENT_TABLE()

};

// Window events IDs
enum
{

    BUTTON_SpeedInc = wxID_HIGHEST + 1, // declares an id which will be used to call our button
    BUTTON_SpeedDec,
    BUTTON_Pause,
    TIMER,
    WIN_OPEN

};

// Draws grid and cells
class CFieldDrawPane: public wxPanel
{

private:

    int cell_x_size; // in pixels - so using "int"
    int cell_y_size;

    wxSize canvas_size; // size of the draing field

    CMainWindow *mn; // указатель на окно-родителя

public:

    CFieldDrawPane(wxPanel* parent, CMainWindow* main);
    void OnPaint(wxPaintEvent& event);

    void DrawGrid(wxDC& dc);
    void DrawCell(int x, int y, int col_num, wxDC& dc);

};
