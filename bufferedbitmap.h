/*  The current file is a modified version  of bufferedbitmap.h from the
    wx_dialogs_tutorial by lszl84, with the following license:

    MIT License

Copyright (c) 2022 lszl84

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

This code is part of NerdyNotes, a software for personal knowledge 
    management and spaced repetition.
    Copyright (C) 2024  Meike Wittmann (meike.wittmann@uni-bielefeld.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. 
    */

#ifndef BUFFEREDBITMAP_HH
#define BUFFEREDBITMAP_HH

#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

class BufferedBitmap : public wxWindow
{
public:
    BufferedBitmap(wxWindow *parent, wxWindowID id, const wxBitmap &b, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0)
        : wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE)
    {
        this->SetBackgroundStyle(wxBG_STYLE_PAINT); // needed for windows

        this->Bind(wxEVT_PAINT, &BufferedBitmap::OnPaint, this);
        this->SetBitmap(b);
    }

    void OnPaint(wxPaintEvent &evt)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();

        wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

        if (gc)
        {
            // scaling consistent with wxStaticBitmap
            const wxSize drawSize = ToDIP(GetClientSize());

            const wxSize bmpSize = bitmap.GetSize();

            //changed the scaling here to make the bitmap fit the desired size
            double wreal = bmpSize.GetWidth();
            double w = drawSize.GetWidth();
            double h = w*bmpSize.GetHeight()/wreal;

            double x = (drawSize.GetWidth() - w) / 2;
            double y = (drawSize.GetHeight() - h) / 2;

            gc->DrawBitmap(bitmap, gc->FromDIP(x), gc->FromDIP(y), gc->FromDIP(w), gc->FromDIP(h));

            delete gc;
        }
    }

    void SetBitmap(const wxBitmap &bitmap)
    {
        this->bitmap = bitmap;
        this->Refresh();
    }

    const wxBitmap &GetBitmap()
    {
        return bitmap;
    }

private:
    wxBitmap bitmap;
};


#endif