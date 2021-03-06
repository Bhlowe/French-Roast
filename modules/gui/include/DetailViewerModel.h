// copyright (c) 2018 Richard Guadagno
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

#ifndef DETAILVIEWERMODEL_H
#define DETAILVIEWERMODEL_H

#include <unordered_map>
#include "qtablewidget.h"
#include "StackReport.h"
#include "MarkerField.h"

namespace frenchroast {
  class DetailViewerModel  {
  QTableWidget*                                        _stackData;
  QTableWidget*                                        _argData;
  std::unordered_map<std::string, QTableWidgetItem*>&  _items;
  std::map<std::string, int>                           _detailItems;
  public:
  DetailViewerModel(QTableWidget* stackData, QTableWidget* argData, std::unordered_map<std::string, QTableWidgetItem*>&  items);
    void update_stack_view(const std::unordered_map<std::string,frenchroast::monitor::StackReport>& stacks);
    void init_arg_instance_headers(const std::vector<std::string>& argHeades, const std::vector<std::string>& instanceHeaders );
    void update_args_markers(const std::unordered_map<std::string,frenchroast::monitor::MarkerField>&);
  };
}
#endif
