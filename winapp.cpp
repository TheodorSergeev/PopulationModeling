#include "winapp.h"

//================================================
//===========  CMainWindow functions  ============
//================================================

CMainWindow::CMainWindow(const wxString& title):wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WINDOW_WIDTH, WINDOW_HEIGHT),
                                            (wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER)), timer(this, TIMER)
{

    OnOpen();

    iter_done = 0;
    iter_freq = ITER_SPEED_DEF;

    stats.total  = CELLS_NUM_X * CELLS_NUM_Y;
    stats.food   = 0;
    stats.poison = 0;
    stats.cells  = 0;

    headmenu_bar  = new wxMenuBar; // создали полоску для менюшки
    headmenu_item = new wxMenu;    // создали менюшку


    // Для всех пунктов меню указывем идентификатор чтобы связать обработчик событие с конкретным элементом
    headmenu_but_about = new wxMenuItem(headmenu_item, wxID_ABOUT, wxT("&About"));    // добавили к менюшке раздел about
    headmenu_but_quit  = new wxMenuItem(headmenu_item, wxID_EXIT,  wxT("&Quit"));    // добавили к менюшке раздел quit

    headmenu_item->Append(headmenu_but_about);
    headmenu_item->AppendSeparator();
    headmenu_item->Append(headmenu_but_quit);

    headmenu_bar->Append(headmenu_item, wxT("&File")); // добавили пункт меню на полоску меню
    SetMenuBar(headmenu_bar);                          // установили полоску в окно

    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMainWindow::OnAbout)); // подключили менюшку exit
    Connect(wxID_EXIT,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMainWindow::OnQuit)); // подключили менюшку exit

    base_panel = new wxPanel(this, wxID_ANY);          // создание панельки для текста, кнопок и рисовалки
    fdraw_pane = new CFieldDrawPane(base_panel, this); // тоже панель, но наша, помещаем ее на панель base_panel и задаем указатель на главный фрейм

    text_speed      = new wxStaticText(base_panel, wxID_ANY, IterFreqStr(), wxPoint(600, 50), wxSize(170, 50), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
    but_speed_dec   = new wxButton(base_panel, BUTTON_SpeedDec, BUT_ITER_SYMB_DEC,   wxPoint(600, 100), wxSize(30, 30), 0); // with the text "hello World"
    but_speed_pause = new wxButton(base_panel, BUTTON_Pause,    BUT_ITER_SYMB_PAUSE, wxPoint(635, 100), wxSize(30, 30), 0); // with the text "hello World"
    but_speed_inc   = new wxButton(base_panel, BUTTON_SpeedInc, BUT_ITER_SYMB_INC,   wxPoint(670, 100), wxSize(30, 30), 0); // with the text "hello World"

    experiment = SetInitialConditions(); // bacteries and food placement

    timer.Start(iter_freq);

};

void CMainWindow::OnOpen() // it is not an event - just a function for selecting a user's stat_log file
{

    string log_fname = "Plots/statistics_log.txt";

    int answer = wxMessageBox(_("Do you want to save statistics to the default file"), _("Statistics file"),
                              wxICON_QUESTION | wxYES_DEFAULT | wxYES_NO , this);

    if(answer == wxNO) // if the user wants to select a new log file
    {

        // Creates a "open file" dialog with 1 file types
        wxFileDialog* OpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, _("Plots/statistics_log.txt"),
                                                    _("Text files (*.txt)|*.txt"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

        if(OpenDialog->ShowModal() == wxID_OK) // if the user clicks "Save" instead of "Cancel"
        {

            log_fname = string(OpenDialog->GetPath().mb_str());  // Sets our current document to the file the user selected

        }

        OpenDialog->Destroy(); // clean up

    }

    stat_log.open(log_fname.data(), std::fstream::out | std::ofstream::trunc);

    if(!stat_log.is_open())
    {

        wxMessageDialog *error = new wxMessageDialog(NULL, wxT("Couldn't open statistics log file"), wxT("Info"), wxOK);
        error->ShowModal();

    }

};

void CMainWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{

    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName       (wxT("Population modeling"));
    aboutInfo.SetVersion    (wxT("1.0.0 beta 1"));
    aboutInfo.SetDescription(wxT("Application for study and visualization of the model of colonies development."));
    aboutInfo.SetCopyright  (wxT("DCAM MIPT (C) 2018"));
    aboutInfo.SetWebSite    (wxT("https://github.com/TheodorSergeev/PopulationModeling"));
    aboutInfo.AddDeveloper  (wxT("Fedor Sergeev"));
    aboutInfo.AddDeveloper  (wxT("Vitaly Aksenov"));
    wxAboutBox(aboutInfo);

}

void CMainWindow::OnQuit(wxCommandEvent& event)
{

    if(experiment)
        delete experiment;

    stat_log.close();

    Close(true);

};

void CMainWindow::OnButtonClick_SpeedDec(wxCommandEvent& event)
{

    iter_freq += ITER_SPEED_DELTA;
    text_speed->SetLabel(IterFreqStr());
    timer.Start(iter_freq);
    this->Refresh();

};

void CMainWindow::OnButtonClick_SpeedInc(wxCommandEvent& event)
{

    if(iter_freq - ITER_SPEED_DELTA >= 0)
        iter_freq -= ITER_SPEED_DELTA;

    text_speed->SetLabel(IterFreqStr());
    timer.Start(iter_freq);
    this->Refresh();

};

void CMainWindow::OnButtonClick_Pause(wxCommandEvent& event)
{

    if(timer.IsRunning())
    {

        but_speed_pause->SetLabel(BUT_ITER_SYMB_START);
        text_speed->SetLabel(wxT("&Iterations paused"));
        timer.Stop();

    }
    else
    {

        but_speed_pause->SetLabel(BUT_ITER_SYMB_PAUSE);
        text_speed->SetLabel(IterFreqStr());
        timer.Start();

    }

    this->Refresh();

};

void CMainWindow::OnTimer(wxTimerEvent& event)
{

    experiment->Iteration();
    this->Refresh();
    GetStatistics();
    WriteStatistics();
    iter_done++;

};

void CMainWindow::GetStatistics()
{

    stats.food  = 0;
    stats.cells = 0;

    for(int i = 0; i < CELLS_NUM_X; ++i)
    {

        for(int j = 0; j < CELLS_NUM_Y; ++j)
        {

            AREA_TYPE type = experiment->What(i, j);

            switch(type)
			{
				case FOOD:
					//if(((CFood *)(field[i][j]))->isPoison()) poison++; else
					stats.food++;
					break;
				case BIOCELL:
					stats.cells++;
					break;
				default:
					break;

                // + write biocell types numbers in the map

			}

        }

    }

};

void CMainWindow::WriteStatistics()
{

    if(!stat_log.is_open())
        return;

    stat_log << iter_done    << " "
             << stats.total  << " "
             << stats.food   << " "
             << stats.poison << " "
             << stats.cells  << "\n";

};

wxString CMainWindow::IterFreqStr() // constructs a string to be printed out as a wxStaticTextBox
{

    string speed_str = "&Iteration every " + std::to_string(iter_freq) + " ms.";
    wxString speed_wxstr(speed_str.c_str(), wxConvUTF8);
    return speed_wxstr;

};


BEGIN_EVENT_TABLE( CMainWindow, wxFrame )
    EVT_TIMER ( TIMER,           CMainWindow::OnTimer                )
    EVT_BUTTON( BUTTON_SpeedInc, CMainWindow::OnButtonClick_SpeedInc )
    EVT_BUTTON( BUTTON_SpeedDec, CMainWindow::OnButtonClick_SpeedDec )
    EVT_BUTTON( BUTTON_Pause,    CMainWindow::OnButtonClick_Pause    )
END_EVENT_TABLE()



//================================================
//==========  CFieldDrawPane functions  ==========
//================================================

CFieldDrawPane::CFieldDrawPane(wxPanel *parent, CMainWindow *fr):wxPanel(parent, -1, wxPoint(BORDER_X_LEFT, BORDER_Y_TOP), wxSize(FIELD_X_SIZE, FIELD_Y_SIZE), wxBORDER_NONE)
{

    Connect(wxEVT_PAINT,wxPaintEventHandler(CFieldDrawPane::OnPaint)); // connected the panel to paint events
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
        throw "Error with colors number";

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
        throw "Error with cell size";

    DrawGrid(dc);

    for(int i = 0; i < CELLS_NUM_X; ++i)
    {

        for(int j = 0; j < CELLS_NUM_Y; ++j)
        {

            AREA_TYPE env_area_type = mn->experiment->What(j, i);

            DrawCell(i, j, env_area_type % NUM_COLOURS, dc);

        }

    }

    //std::stringstream str;
    //str << canvas_size.x << " " << canvas_size.y << "\n";
    //str << cell_x_size << " " << cell_y_size << "\n";
    //wxFont font(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    //dc.SetFont(font);
    //dc.SetTextForeground(wxColour(255, 102, 0));
    //dc.DrawText(wxString::FromUTF8(str.str().c_str()), 10, 10);

};



//================================================
//=========  CWinApplication functions  ==========
//================================================

bool CWinApplication::OnInit()
{

    CMainWindow* wind = new CMainWindow(wxT("Population modeling")); // note that wxWidgets will free it automatically
    wind->Show(true);
    //SetTopWindow(wind);

    return true;

};

int  CWinApplication::OnExit()
{

    return 0;

};
