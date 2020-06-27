/*  JoystickConfiguration.cpp
 *  PCSX2 Dev Team
 *  Copyright (C) 2015
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "JoystickConfiguration.h"

// Constructor of JoystickConfiguration
JoystickConfiguration::JoystickConfiguration(int pad, bool left, wxWindow *parent)
    : wxDialog(parent, wxID_ANY, _T("Joystick configuration"), wxDefaultPosition, wxDefaultSize,
               wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
{
    m_init_reverse_Lx = m_init_reverse_Ly =
        m_init_reverse_Rx = m_init_reverse_Ry =
            m_init_mouse_Ljoy = m_init_mouse_Rjoy = false;

    m_pad_id = pad;
    m_isForLeftJoystick = left;

    wxBoxSizer *joy_conf_box = new wxBoxSizer(wxVERTICAL);

    if (m_isForLeftJoystick) {
        m_cb_reverse_Lx = new wxCheckBox(this, wxID_ANY, _T("Reverse Lx"));
        m_cb_reverse_Ly = new wxCheckBox(this, wxID_ANY, _T("Reverse Ly"));
        m_cb_mouse_Ljoy = new wxCheckBox(this, wxID_ANY, _T("Use mouse for left analog joystick"));

        joy_conf_box->Add(m_cb_reverse_Lx, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 5));
        joy_conf_box->Add(m_cb_reverse_Ly, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 5));
        joy_conf_box->Add(m_cb_mouse_Ljoy, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 5));

        m_cb_reverse_Rx = m_cb_reverse_Ry = m_cb_mouse_Rjoy = nullptr;

    } else {
        m_cb_reverse_Rx = new wxCheckBox(this, wxID_ANY, _T("Reverse Rx"));
        m_cb_reverse_Ry = new wxCheckBox(this, wxID_ANY, _T("Reverse Ry"));
        m_cb_mouse_Rjoy = new wxCheckBox(this, wxID_ANY, _T("Use mouse for right analog joystick"));

        joy_conf_box->Add(m_cb_reverse_Rx, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 5));
        joy_conf_box->Add(m_cb_reverse_Ry, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 5));
        joy_conf_box->Add(m_cb_mouse_Rjoy, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 5));

        m_cb_reverse_Lx = m_cb_reverse_Ly = m_cb_mouse_Ljoy = nullptr;
    }

    joy_conf_box->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Border(wxALL, 5).Right());

    Bind(wxEVT_BUTTON, &JoystickConfiguration::OnOk, this, wxID_OK);
    Bind(wxEVT_BUTTON, &JoystickConfiguration::OnCancel, this, wxID_CANCEL);
    Bind(wxEVT_CHECKBOX, &JoystickConfiguration::OnCheckboxChange, this);

    SetSizerAndFit(joy_conf_box);
}

/**
    Initialize the frame
    Check if a gamepad is detected
*/
void JoystickConfiguration::InitJoystickConfiguration()
{
    repopulate(); // Set label and fit simulated key array
    /*
     * Check if there exist at least one pad available
     * if the pad id is 0, you need at least 1 gamepad connected,
     * if the pad id is 1, you need at least 2 gamepads connected,
     * Prevent to use a none initialized value on s_vgamePad (core dump)
    */
    if (s_vgamePad.size() < m_pad_id + 1) {
        if (s_vgamePad.empty())
            wxMessageBox(L"No gamepad detected.");
        else
            wxMessageBox(L"No second gamepad detected.");

        // disable all checkbox
        if (m_isForLeftJoystick) {
            m_cb_reverse_Lx->Disable();
            m_cb_reverse_Ly->Disable();
        } else {
            m_cb_reverse_Rx->Disable();
            m_cb_reverse_Ry->Disable();
        }
    }
}

void JoystickConfiguration::OnOk(wxCommandEvent &event)
{
    printf("Okay!\n");
    Destroy();
}

void JoystickConfiguration::OnCancel(wxCommandEvent &event)
{
    reset();
    Destroy();
}

/**
 * Checkbox event, called when the value of the checkbox change
*/
void JoystickConfiguration::OnCheckboxChange(wxCommandEvent &event)
{
    wxCheckBox *cb_tmp = (wxCheckBox *)event.GetEventObject(); // get the slider object
    int cb_id = cb_tmp->GetId();
    bool val;

    if (m_isForLeftJoystick) {
        if (cb_id == m_cb_reverse_Ly->GetId()) {
            val = m_cb_reverse_Ly->GetValue();
            g_conf.pad_options[m_pad_id].reverse_ly = val;
        } else if (cb_id == m_cb_reverse_Lx->GetId()) {
            val = m_cb_reverse_Lx->GetValue();
            g_conf.pad_options[m_pad_id].reverse_lx = val;
        } else if (cb_id == m_cb_mouse_Ljoy->GetId()) {
            val = m_cb_mouse_Ljoy->GetValue();
            g_conf.pad_options[m_pad_id].mouse_l = val;
        }
    } else {
        if (cb_id == m_cb_reverse_Ry->GetId()) {
            val = m_cb_reverse_Ry->GetValue();
            g_conf.pad_options[m_pad_id].reverse_ry = val;
        } else if (cb_id == m_cb_reverse_Rx->GetId()) {
            val = m_cb_reverse_Rx->GetValue();
            g_conf.pad_options[m_pad_id].reverse_rx = val;
        } else if (cb_id == m_cb_mouse_Rjoy->GetId()) {
            val = m_cb_mouse_Rjoy->GetValue();
            g_conf.pad_options[m_pad_id].mouse_r = val;
        }
    }
}

/****************************************/
/*********** Methods functions **********/
/****************************************/

// Reset checkbox and slider values
void JoystickConfiguration::reset()
{
    if (m_isForLeftJoystick) {
        m_cb_reverse_Lx->SetValue(m_init_reverse_Lx);
        m_cb_reverse_Ly->SetValue(m_init_reverse_Ly);
        m_cb_mouse_Ljoy->SetValue(m_init_mouse_Ljoy);
    } else {
        m_cb_reverse_Rx->SetValue(m_init_reverse_Rx);
        m_cb_reverse_Ry->SetValue(m_init_reverse_Ry);
        m_cb_mouse_Rjoy->SetValue(m_init_mouse_Rjoy);
    }
}

// Set button values
void JoystickConfiguration::repopulate()
{
    bool val;
    if (m_isForLeftJoystick) {
        val = g_conf.pad_options[m_pad_id].reverse_lx;
        m_init_reverse_Lx = val;
        m_cb_reverse_Lx->SetValue(val);

        val = g_conf.pad_options[m_pad_id].reverse_ly;
        m_init_reverse_Ly = val;
        m_cb_reverse_Ly->SetValue(val);

        val = g_conf.pad_options[m_pad_id].mouse_l;
        m_init_mouse_Ljoy = val;
        m_cb_mouse_Ljoy->SetValue(val);
    } else {
        val = g_conf.pad_options[m_pad_id].reverse_rx;
        m_init_reverse_Rx = val;
        m_cb_reverse_Rx->SetValue(val);

        val = g_conf.pad_options[m_pad_id].reverse_ry;
        m_init_reverse_Ry = val;
        m_cb_reverse_Ry->SetValue(val);

        val = g_conf.pad_options[m_pad_id].mouse_r;
        m_init_mouse_Rjoy = val;
        m_cb_mouse_Rjoy->SetValue(val);
    }
}
