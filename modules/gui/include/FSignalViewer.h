// copyright (c) 2017 Richard Guadagno
// contact: rrguadagno@gmail.com
//
// This file is part of French-Roast
//
//    French-Roast is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    French-Roast is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef FSIGVIEWER_H
#define FSIGVIEWER_H

#include <unordered_map>
#include <QWidget>
#include <QSettings>
#include <QTableWidget>
#include "FViewer.h"


namespace frenchroast {
  class FSignalViewer : public FViewer {

    Q_OBJECT

    QTableWidget*                                     _data;
    std::unordered_map<std::string,QTableWidgetItem*> _descriptors;
    static FSignalViewer*                             _instance;
    static const std::string                          FName;
    FSignalViewer(QWidget*);
    ~FSignalViewer();
    void reset_all();
    
  public slots:
      void update_count(const std::string& descriptor, int count);
  signals:
      void view_detail_request(const std::string&);
  public:
    static FSignalViewer* instance(QWidget*);
    static bool restore_is_required();
    static void capture();
    static void reset();
    static bool up();
  };
}
#endif
