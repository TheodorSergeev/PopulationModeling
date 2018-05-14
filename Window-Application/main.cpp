#include <wx/wx.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <wx/string.h>
#include <wx/gdicmn.h> // colours database

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

class CExperiment
{

public:
    void Iteration(){}

};

class CWinApplication: public wxApp
{

private:

    float update_freq; // moves per second
    CExperiment experiment;

public:
    virtual bool OnInit();

	void RunExperiment(float speed, int time_limit)
	{

		for(int t = 0; t <= time_limit; ++t)
		{

            experiment.Iteration();

			// iteration
			// field visualisation
			// data collection
			// data visualisaton
			// pause check
			// save check

		}

	}

};

class CFieldDrawPane;  //for crossreferences between CDraw and CCMainWindow
class CMainWindow;


//colors for drawing cells
const int NUM_COLOURS = 10;
const wxColour COLONY_COLOURS[NUM_COLOURS] = {wxT("GOLDENROD"), wxT("FIREBRICK"), wxT("NAVY"), wxT("FOREST GREEN"), wxT("DARK ORCHID"),
                                              wxT("GOLD"), wxT("INDIAN RED"), wxT("LIGHT BLUE") , wxT("LIGHT GREEN"), wxT("ORCHID")};

//main window size
const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH  = 800;

// field positioning
const double BORDER_X_LEFT  = 50;
const double BORDER_Y_TOP   = 50;
const double FIELD_X_SIZE   = std::min(WINDOW_HEIGHT - 2 * BORDER_Y_TOP, WINDOW_WIDTH - 2 * BORDER_X_LEFT);
const double FIELD_Y_SIZE   = FIELD_X_SIZE;

// number of cells of the envoronment field grid
const int CELLS_NUM_X = 20;
const int CELLS_NUM_Y = 20;


class CMainWindow: public wxFrame
{

private:

    wxMenuBar*  headmenu_bar;
    wxMenu*     headmenu_item;
    wxMenuItem* headmenu_but_quit; // "but" = button

// Process speed controls
    wxStaticText *text_speed;
    wxButton *but_speed_inc;
    wxButton *but_speed_dec;
    wxButton *but_speed_pause;

    CFieldDrawPane* fdraw_pane;    // field draw pane
    wxPanel* base_panel;           // Это в "подвале" окошка

 public:

    CMainWindow(const wxString& title);
    void OnQuit(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

};

enum
{

    BUTTON_SpeedInc = wxID_HIGHEST + 1, // declares an id which will be used to call our button
    BUTTON_SpeedDec = wxID_HIGHEST + 1,
    BUTTON_Pause    = wxID_HIGHEST + 1

};


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

//Идентификаторы - на будещее
const int ID_MENU_LOAD = 1002; // загрузка
const int ID_MENU_EDIT = 1003; // редактирование

// Конструктор фрейма
CMainWindow::CMainWindow(const wxString& title):wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WINDOW_WIDTH, WINDOW_HEIGHT),
                                            (wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER))
{


    headmenu_bar  = new wxMenuBar; // создали полоску для менюшки
    headmenu_item = new wxMenu;    // создали менюшку

    headmenu_item->Append(wxID_ANY, wxT("&todo")); // закинули менюшку на полоску (пока не работает - для "красоты")
    headmenu_item->AppendSeparator();

    // Для всех пунктов меню указывем идентификатор чтобы связать обработчик событие с конкретным элементом
    headmenu_but_quit = new wxMenuItem(headmenu_item, wxID_EXIT, wxT("&Quit"));    // добавили к менюшке раздел quit
    headmenu_item->Append(headmenu_but_quit);
    headmenu_bar->Append(headmenu_item, wxT("&File")); // добавили пункт меню на полоску меню
    SetMenuBar(headmenu_bar);                          // установили полоску в окно
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMainWindow::OnQuit)); // подключили менюшку exit

    base_panel = new wxPanel(this, wxID_ANY);          // создание панельки для текста, кнопок и рисовалки
    fdraw_pane = new CFieldDrawPane(base_panel, this); // тоже панель, но наша, помещаем ее на панель base_panel и задаем указатель на главный фрейм

    text_speed    = new wxStaticText(base_panel, wxID_ANY, wxT("&Speed: "), wxPoint(600, 50), wxSize(100, 50), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
    but_speed_inc = new wxButton(base_panel, BUTTON_SpeedInc, wxT("&>>"), wxPoint(700, 100), wxSize(40, 30), 0); // with the text "hello World"
    but_speed_dec = new wxButton(base_panel, BUTTON_SpeedDec, wxT("&<<"), wxPoint(600, 100), wxSize(40, 30), 0); // with the text "hello World"
    but_speed_dec = new wxButton(base_panel, BUTTON_Pause,    wxT("&■"),  wxPoint(650, 100), wxSize(30, 30), 0); // with the text "hello World"

    //but_speed_dec->SetLabel(wxT("&▶"));
    //text_speed->SetLabel(wxT("&Speed2"));

};

void CMainWindow::OnQuit(wxCommandEvent& event)
{

    Close(true);

};

CFieldDrawPane::CFieldDrawPane(wxPanel *parent, CMainWindow *fr):wxPanel(parent, -1, wxPoint(BORDER_X_LEFT, BORDER_Y_TOP), wxSize(FIELD_X_SIZE, FIELD_Y_SIZE), wxBORDER_NONE)
{

    Connect(wxEVT_PAINT,wxPaintEventHandler(CFieldDrawPane::OnPaint)); // подключили панель к событиям рисования
    mn = fr;

};

void CFieldDrawPane::DrawGrid(wxDC& dc)
{

    for(int x = 0; x <= canvas_size.x; x += cell_x_size)
    {

//         if(x != 0)
            dc.DrawLine(x, 0, x, canvas_size.y);

    }

    for(int y = 0; y <= canvas_size.y; y += cell_y_size)
    {

//        if(y != 0)
         dc.DrawLine(0, y, canvas_size.x, y);

    }

    dc.DrawLine(canvas_size.x - 1, 0, canvas_size.x - 1, canvas_size.y);
    dc.DrawLine(0, canvas_size.y - 1, canvas_size.x, canvas_size.y - 1);

}

void CFieldDrawPane::DrawCell(int x_pos, int y_pos, int col_num, wxDC& dc)
{

    if(col_num > NUM_COLOURS)
        throw "fuckup with colors number";

    dc.SetBrush(wxBrush(COLONY_COLOURS[col_num]));
    dc.SetPen(wxPen(*wxRED, 0, wxTRANSPARENT));
    dc.DrawRectangle(cell_x_size * x_pos + 1, cell_y_size * y_pos + 1, cell_x_size - 1, cell_y_size - 1);

}

void CFieldDrawPane::OnPaint(wxPaintEvent& event)
{

    wxPaintDC dc(this); // положили планшет wxPaintDC на нашу панель

    canvas_size = dc.GetSize();
    cell_x_size = std::floor(canvas_size.x / CELLS_NUM_X);
    cell_y_size = std::floor(canvas_size.y / CELLS_NUM_Y);

    if(cell_x_size <= 0 || cell_y_size <= 0)
        throw "fuckup with cell size";

    DrawGrid(dc);

    /*for(int i = 0; i < CELLS_NUM_X; ++i)
    {

        for(int j = 0; j < CELLS_NUM_Y; ++j)
        {

            if((i + j) % (NUM_COLOURS + 1) != 0)
                DrawCell(i, j, (i + j) % (NUM_COLOURS + 1) - 1, dc);

        }

    }*/

    DrawCell(3, 3, 0, dc);
    DrawCell(3, 4, 1, dc);
    DrawCell(9, 9, 2, dc);
    DrawCell(0, 9, 3, dc);
    DrawCell(0, 0, 4, dc);
    DrawCell(7, 8, 5, dc);

    std::stringstream str;
    str << canvas_size.x << " " << canvas_size.y << "\n";
    str << cell_x_size << " " << cell_y_size << "\n";
    wxFont font(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    dc.SetFont(font);
    dc.SetTextForeground(wxColour(255, 102, 0));
    dc.DrawText(wxString::FromUTF8(str.str().c_str()), 10, 10);

};


IMPLEMENT_APP(CWinApplication)

bool CWinApplication::OnInit()
{

    CMainWindow* wind = new CMainWindow(wxT("Population modeling"));
    wind->Show(true);
    //SetTopWindow(wind);

    return true;

};

BEGIN_EVENT_TABLE( CMainWindow, wxFrame )
    EVT_BUTTON( BUTTON_SpeedInc, CMainWindow::OnQuit ) // Tell the OS to run MainFrame::OnExit when The button is pressed
    EVT_BUTTON( BUTTON_SpeedDec, CMainWindow::OnQuit ) // Tell the OS to run MainFrame::OnExit when
    EVT_BUTTON( BUTTON_Pause,    CMainWindow::OnQuit ) // Tell the OS to run MainFrame::OnExit when
END_EVENT_TABLE()

