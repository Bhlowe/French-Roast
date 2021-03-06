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


#include <iostream>
#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"
#include "KeyListener.h"
#include "Util.h"
#include "FClassViewer.h"

namespace frenchroast {
  
  FClassViewer::FClassViewer(QWidget* parent) : FViewer( parent)
  {
    _data = new QTableWidget;
    _data->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _data->verticalHeader()->hide();
    _data->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QObject::connect(_actionBar->add(new ActionButton("Start|Stop")), &ActionButton::request, this, &FClassViewer::start_stop);
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(_data );
    vlayout->setContentsMargins(0,0,0,0);
    setLayout(vlayout);
    setStyleSheet(_settings->value("zero_border_style").toString());
    
    _data->insertColumn(0);
    _data->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _data->setHorizontalHeaderItem(0, createItem("Class"));
    _data->setItemDelegateForColumn(0, new SignalDelegate(_data)); 

    setMinimumSize(700,350);
    KeyListener* keyListener = new KeyListener;
    _data->installEventFilter(keyListener);
    setup_dockwin("Loaded Classes", _data, false);
    QObject::connect(keyListener,   &KeyListener::enterkey,           this, [&](){ methods_for_class(_data->currentItem());});
    QObject::connect(keyListener  , &KeyListener::signalkey,          this, [&](){handle_add_signal(_data->currentRow(), _data->currentItem()->text());});
    QObject::connect(_data,         &QTableWidget::itemDoubleClicked, this, &FClassViewer::methods_for_class);
}

void FClassViewer::update(const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  for(auto& citem : details) {
    ++_loadCount[citem.name()];
    if(_classes.count(citem.name()) == 1) {

      _classes[citem.name()]->setText(QString::fromStdString(citem.name() + " [ " + std::to_string(_loadCount[citem.name()]) + " ]"));
    }
    else {
      _classes[citem.name()] = createItem(citem.name(), Qt::AlignLeft|Qt::AlignVCenter);
      addRow(_data, _classes[citem.name()]);
      _methods[citem.name()] = citem.methods();
    }
  }
}

  void FClassViewer::start_stop(const std::string& text)
  {
    if(text == "Start") {
      start_watching();
    }
    else {
      stop_watching();
    }
  }
  
  std::string clean_name(const std::string& name) {
    size_t idx = name.find(" [");
    if(idx == std::string::npos) return name;
    return name.substr(0, idx);
  }
  
  void FClassViewer::expand_methods(const std::string& name, int row)
  {
    _ind[name] = 1;
    for(auto& mitem : _methods[name]) {
      _data->insertRow(++row);
      _row_class[row] = name;
      _data->setItem(row, 0, createItem("    " + mitem.method_name(), Qt::AlignLeft|Qt::AlignVCenter));
    }
  }

void FClassViewer::collapse_methods(const std::string& name, int row)
{
  _ind[name] = 0;
  ++row;
  for(auto& mitem : _methods[name]) {
    _row_class.erase(row);
    _data->removeRow(row);
  }
}

void FClassViewer::methods_for_class(QTableWidgetItem* item)
{
  std::string name = clean_name(item->text().toStdString());
  if(_methods.count(name) == 0) return;

  if(_ind[name] == 0) {
    expand_methods(name, item->row());
  }
  else {
    collapse_methods(name, item->row());
  }
}

void FClassViewer::handle_add_signal(int row, QString text)
{
  std::string name = clean_name(text.toStdString());
  frenchroast::remove_blanks(name);
  if(_methods.count(name) == 0) {
    add_signal(QString::fromStdString(_row_class[row] + "::" + name + " "));
  }
  else {
    add_signal(QString::fromStdString(name + "::* "));
  }
}

  FClassViewer* FClassViewer::_instance{nullptr};
  const std::string FClassViewer::FName{"classviewer"};

  
  FClassViewer::~FClassViewer()
  {
    _instance = nullptr;
  }

  FClassViewer* FClassViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new FClassViewer(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  void FClassViewer::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  bool FClassViewer::restore_is_required()
  {
    return restore_required(FName, _settings);
  }


  void FClassViewer::reset()
  {
    if(_instance == nullptr) return;
    _instance->reset_all();
  }


  void FClassViewer::reset_all()
  {
    clearTable(_data);
    _methods.clear();
    _loadCount.clear();
    _classes.clear();
    _ind.clear();
  }
  
  bool FClassViewer::up()
  {
      return _instance != nullptr;
  }
  
}
