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

class CWinApplication: public wxApp
{

public:
    virtual bool OnInit();

};

class CFieldDrawPane;  //for crossreferences between CDraw and CCMainWindow
class CMainWindow;


const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH  = 800;

// field positioning
const double BORDER_X_LEFT  = 50;
const double BORDER_Y_TOP   = 50;
const double FIELD_X_SIZE   = std::min(WINDOW_HEIGHT - 2 * BORDER_Y_TOP, WINDOW_WIDTH - 2 * BORDER_X_LEFT);
const double FIELD_Y_SIZE   = FIELD_X_SIZE;

class CMainWindow: public wxFrame
{

private:

    wxMenuBar*  headmenu_bar;      // полоса для меню
    wxMenu*     headmenu_item;     // меню в заголовке окна
    wxMenuItem* headmenu_but_quit; // кнопка в заголовке окна

    CFieldDrawPane* fdraw_pane;    // Указатель на объект CFieldDrawPane
    wxPanel* base_panel;           // Это в "подвале" окошка

 public:

    CMainWindow(const wxString& title);
    void OnQuit(wxCommandEvent& event);

};

class CFieldDrawPane: public wxPanel
{

private:

    int cells_num_x;
    int cells_num_y;
    int cell_x_size;
    int cell_y_size;

    wxSize canvas_size;

    CMainWindow *mn; // указатель на окно-родителя

public:

    CFieldDrawPane(wxPanel* parent, CMainWindow* main);
    void OnPaint(wxPaintEvent & event);

    void DrawGrid(wxDC& dc);
    void DrawCell(int x, int y, int col_num, wxDC& dc);

};

//Идентификаторы
const int ID_MENU_LOAD = 1002; // загрузка
const int ID_MENU_EDIT = 1003; // редактирование

std::vector <wxColour> cell_colors;

// Конструктор фрейма
CMainWindow::CMainWindow(const wxString& title):wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WINDOW_WIDTH, WINDOW_HEIGHT),
                                            (wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER))
{

    headmenu_bar  = new wxMenuBar; // создали полоску для менюшки
    headmenu_item = new wxMenu;    // создали менюшку

    headmenu_item->Append(wxID_ANY, wxT("&todo")); // закинули менюшку на полоску пока не работает (для "красоты")
    headmenu_item->AppendSeparator();

    // Для всех пунктов меню указывем идентификатор чтобы связать обработчик событие с конкретным элементом
    headmenu_but_quit = new wxMenuItem(headmenu_item, wxID_EXIT, wxT("&Quit"));    // добавили к менюшке раздел quit
    headmenu_item->Append(headmenu_but_quit);
    headmenu_bar->Append(headmenu_item, wxT("&File")); // добавили оба пункта меню на полску меню
    SetMenuBar(headmenu_bar);  // установили полоску в окно
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMainWindow::OnQuit)); // подключили менюшку exit

    base_panel = new wxPanel(this, wxID_ANY); // создание панельки для текста, кнопок и рисовалки
    fdraw_pane = new CFieldDrawPane(base_panel, this);       // тоже панель, но наша, помещаем ее на панель base_panel и задаем указатель на главный фрейм

};

void CMainWindow::OnQuit(wxCommandEvent& event)
{

    Close(true);

};

CFieldDrawPane::CFieldDrawPane(wxPanel *parent, CMainWindow *fr):wxPanel(parent, -1, wxPoint(BORDER_X_LEFT, BORDER_Y_TOP), wxSize(FIELD_X_SIZE, FIELD_Y_SIZE), wxBORDER_NONE)
{

    Connect(wxEVT_PAINT,wxPaintEventHandler(CFieldDrawPane::OnPaint)); // подключили панель к событиям рисования
    mn = fr;

    // constant
    cells_num_x = 20;
    cells_num_y = 20;

    cell_colors.push_back(wxT("GOLD"));
    cell_colors.push_back(wxT("FIREBRICK"));
    cell_colors.push_back(wxT("NAVY"));
    cell_colors.push_back(wxT("FOREST GREEN"));
    cell_colors.push_back(wxT("ORCHID"));
    cell_colors.push_back(wxT("CYAN"));

};


void Print(wxPaintDC& dc, std::string str_)
{

    std::stringstream str;
    str << str_;
    wxFont font(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    dc.SetFont(font);
    dc.SetTextForeground(wxColour(255, 102, 0));
    dc.DrawText(wxString::FromUTF8(str.str().c_str()), 20, 20);

}

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

    dc.SetBrush(wxBrush(cell_colors[col_num]));
    dc.SetPen(wxPen(*wxRED, 0, wxTRANSPARENT));
    dc.DrawRectangle(cell_x_size * x_pos + 1, cell_y_size * y_pos + 1, cell_x_size - 1, cell_y_size - 1);

}

void CFieldDrawPane::OnPaint(wxPaintEvent& event)
{


    wxPaintDC dc(this); // положили планшет wxPaintDC на нашу панель

    canvas_size = dc.GetSize();
    cell_x_size = std::floor(canvas_size.x / cells_num_x);
    cell_y_size = std::floor(canvas_size.y / cells_num_y);

    DrawGrid(dc);

    wxPoint a(0, 0);
    wxPoint b(20, 20);

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
    return true;

};

