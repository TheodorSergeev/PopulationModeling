#include <wx/wx.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <wx/string.h>
#include <sstream>
#include <string>

class WinApplication: public wxApp
{

public:
    virtual bool OnInit();

};

//for crossreferences
class Draw;
class MyWin;

const int WINDOW_HEIGHT = 300;
const int WINDOW_WIDTH  = 400;

class MyWin: public wxFrame
{

    wxMenuBar*  headmenu_bar;      // полоса для меню
    wxMenu*     headmenu_item;     // меню в заголовке окна
    wxMenuItem* headmenu_but_quit; // кнопка в заголовке окна

    Draw*    dp;        // Указатель на объект Draw
    wxPanel* m_pan;     // Это в "подвале" окошка

 public:

    MyWin(const wxString& title);       //  конструктор
    void OnQuit(wxCommandEvent& event); // Функция закрытия окна

};

// класс для рисования
// наследник wxPanel
class Draw: public wxPanel
{

    MyWin *mn; // указатель на окно-родителя

public:

    Draw(wxPanel* parent, MyWin* main);
    void OnPaint(wxPaintEvent & event);

};

//Идентификаторы
const int ID_MENU_LOAD = 1002; // загрузка
const int ID_MENU_EDIT = 1003; // редактирование

// Конструктор фрейма
MyWin::MyWin(const wxString& title):wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WINDOW_WIDTH, WINDOW_HEIGHT),
                                            (wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER))
{

    headmenu_bar  = new wxMenuBar; // создали полоску для менюшки
    headmenu_item = new wxMenu;    // создали менюшку

    // закинули менюшку на полоску
    // пока не работает (для "красоты")
    headmenu_item->Append(wxID_ANY, wxT("&todo"));
    headmenu_item->AppendSeparator();

    // Для всех пунктов меню указывем идентификатор чтобы связать обработчик событие с конкретным элементом
    headmenu_but_quit = new wxMenuItem(headmenu_item, wxID_EXIT, wxT("&Quit"));    // добавили к менюшке раздел quit
    headmenu_item->Append(headmenu_but_quit);
    headmenu_bar->Append(headmenu_item, wxT("&File")); // добавили оба пункта меню на полску меню
    SetMenuBar(headmenu_bar);  // установили полоску в окно
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyWin::OnQuit)); // подключили менюшку exit

    m_pan = new wxPanel(this, wxID_ANY); // создание панельки для текста, кнопок и рисовалки
    dp    = new Draw(m_pan, this);       // тоже панель, но наша, помещаем ее на панель m_pan и задаем указатель на главный фрейм

};

void MyWin::OnQuit(wxCommandEvent& event)
{

    Close(true);

};

Draw::Draw(wxPanel *parent, MyWin *fr):wxPanel(parent, -1, wxPoint(47.5, 47.5), wxSize(WINDOW_WIDTH - 47.5*2, WINDOW_HEIGHT - 47.5*2), wxBORDER_SUNKEN)
{

    Connect(wxEVT_PAINT,wxPaintEventHandler(Draw::OnPaint)); // подключили панель к событиям рисования
    mn = fr;

};

void DrRec(wxPoint a, wxPoint b, wxDC * dc)
{

    dc->DrawRectangle(a, wxSize(abs(a.x - b.x), abs(a.y - b.y)));

};

// Функция OnPaint() сработает при любом рисовании окна:
// первое рисование, сложил-разложили
// или вызов функции Refresh()

void Print(wxPaintDC& dc, std::string str_)
{

    std::stringstream str;
    str << str_;
    wxFont font(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    dc.SetFont(font);
    dc.SetTextForeground(wxColour(255, 102, 0));
    dc.DrawText(wxString::FromUTF8(str.str().c_str()), 20, 20);

}

void Draw::OnPaint(wxPaintEvent& event)
{

    wxPaintDC dc(this);                        // положили планшет wxPaintDC на нашу панель

    wxSize canvas_size = dc.GetSize();
    //canvas_size.x = 300;
    //canvas_size.y = 200;

    //std::string str_;
    //str_ += canvas_size.x + " " + canvas_size.y;
    //Print(dc, str_);



    const int CELLS_X = 10;
    const int CELLS_Y = 10;

    double cell_x_size = canvas_size.x / CELLS_X;
    double cell_y_size = canvas_size.y / CELLS_Y;

    for(double x = 0; x <= canvas_size.x; x += cell_x_size)
    {

         dc.DrawLine(x, 0, x, canvas_size.y);

    }

    for(double y = 0; y <= canvas_size.y; y += cell_y_size)
    {

         dc.DrawLine(0, y, canvas_size.x, y);

    }

    wxPoint a(0, 0);
    wxPoint b(20, 20);

    dc.SetBrush(wxBrush(wxColour(255, 0, 0))); // Установили цвет заливки
    DrRec(a, b, &dc);                  // передали свободной функции координаты и указатель на планшет

    std::stringstream str;
    str << canvas_size.x << " " << canvas_size.y << "\n";
    wxFont font(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    dc.SetFont(font);
    dc.SetTextForeground(wxColour(255, 102, 0));
    dc.DrawText(wxString::FromUTF8(str.str().c_str()), 10, 10);

};


IMPLEMENT_APP(WinApplication)

bool WinApplication::OnInit()
{

    MyWin* wind = new MyWin(wxT("Моделирование популяций"));
    wind->Show(true);
    return true;

};

