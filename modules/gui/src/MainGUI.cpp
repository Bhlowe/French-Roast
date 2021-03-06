// copyright (c) 2016-2017 Richard Guadagno
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

#include <string>
#include <unordered_map>
#include <QFileDialog>
#include "fr.h"
#include "FRMain.h"
#include "MonitorUtil.h"
#include "FSignalViewer.h"
#include "FTimerViewer.h"
#include "FClassViewer.h"
#include "Editor.h"
#include "TrafficViewer.h"
#include "JammedViewer.h"
#include "HeapViewer.h"
#include "AboutHelpViewer.h"
#include "SignalReport.h"
#include "TimerReport.h"
#include "DetailViewer.h"
#include "HeapDetailViewer.h"

using namespace frenchroast;


QSettings* FViewer::_settings = nullptr;



FRMain::FRMain( QSettings& settings, const std::string& path_to_hooks, Session& session, const std::string& session_dir) : _settings(settings), _hooksfile(path_to_hooks), _session(session), _session_default_dir(session_dir)
{
  FViewer::setSettings(&settings);
  restore();
}


void FRMain::validate_signals()
{
  if(Editor::up()) {
    Editor::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(Editor::instance(this), &frenchroast::Editor::validated_signals,  this,    &FRMain::validated_signals);
  }
  Editor::instance(this)->validate_signals();
}


void FRMain::handshake(const std::string& host, const std::string& ip)
{
  AboutHelpViewer::instance(this)->remote_ready(host, ip);
  if(_watchTraffic) {
    start_traffic(TrafficViewer::instance(this)->traffic_rate());
  }
  if(_watchLoading) {
    start_loading();
  }

}

void FRMain::remote_connected(const std::string& host, const std::string& pid)
{
  AboutHelpViewer::instance(this)->remote_connected(host,pid);
}

void FRMain::remote_disconnected(const std::string& host, const std::string& pid)
{
  AboutHelpViewer::instance(this)->remote_disconnected(host,pid);
}

void FRMain::remote_ack_off(const std::string& host, const std::string& pid)
{
  AboutHelpViewer::instance(this)->remote_ack_off(host,pid);
}

void FRMain::remote_ack_on(const std::string& host, const std::string& pid)
{
  AboutHelpViewer::instance(this)->remote_ack_on(host,pid);
}


void FRMain::view_traffic()
{
  if(TrafficViewer::up()) {
    TrafficViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::start_watching, this,  &FRMain::start_watching_traffic);
    QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::stop_watching,  this,  &FRMain::stop_watching_traffic);
    QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::add_signal,     this,  &FRMain::add_hook);
    connect_common_listeners(TrafficViewer::instance(this));
  }
}

void FRMain::view_classviewer()
{
  if(FClassViewer::up()) {
    FClassViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::start_watching, this,  &FRMain::start_watch_loading);
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::stop_watching, this,   &FRMain::stop_watch_loading);
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::add_signal, this,      &FRMain::add_hook);
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::closed, this,          &FRMain::stop_watch_loading);
    connect_common_listeners(FClassViewer::instance(this));
  }
}

void FRMain::view_jammed()
{
  if(JammedViewer::up()) {
    JammedViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(JammedViewer::instance(this), &JammedViewer::add_signal, this,  &FRMain::add_hook);
    connect_common_listeners(JammedViewer::instance(this));
  }
}

void FRMain::view_heap()
{
  if(HeapViewer::up()) {
    HeapViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(HeapViewer::instance(this),  &frenchroast::HeapViewer::view_detail_request, this, &FRMain::show_heap_detail);
    connect_common_listeners(HeapViewer::instance(this));
  }
}

void FRMain::start_watch_loading()
{
  _watchLoading = true;
  start_loading();
}

void FRMain::stop_watch_loading()
{
   stop_loading();
   _watchLoading = false;
}


void FRMain::connect_common_listeners(FViewer* instance)
{
  QObject::connect(instance, &frenchroast::FViewer::signal_viewer,    this,  &FRMain::view_signals);
  QObject::connect(instance, &frenchroast::FViewer::timer_viewer,     this,  &FRMain::view_timers);
  QObject::connect(instance, &frenchroast::FViewer::editor_viewer,    this,  &FRMain::view_hooks_editor);
  QObject::connect(instance, &frenchroast::FViewer::traffic_viewer,   this,  &FRMain::view_traffic);
  QObject::connect(instance, &frenchroast::FViewer::jammed_viewer,    this,  &FRMain::view_jammed);
  QObject::connect(instance, &frenchroast::FViewer::heap_viewer,      this,  &FRMain::view_heap);
  QObject::connect(instance, &frenchroast::FViewer::classload_viewer, this,  &FRMain::view_classviewer);
  QObject::connect(instance, &frenchroast::FViewer::about_viewer,     this,  &FRMain::view_about);
  QObject::connect(instance, &frenchroast::FViewer::reset,            this,  &FRMain::reset_viewers);
  QObject::connect(instance, &frenchroast::FViewer::session_save,     this,  &FRMain::session_save);
  QObject::connect(instance, &frenchroast::FViewer::session_save_as,  this,  &FRMain::session_save_as);
  QObject::connect(instance, &frenchroast::FViewer::session_load,     this,  &FRMain::session_load);
  QObject::connect(instance, &frenchroast::FViewer::exit_fr,          this,  &FRMain::exit_fr);
}

void FRMain::session_save()
{
  if(_session.has_descriptor()) {
    _session.store();
  }
  else {
    session_save_as();
  }
}

void FRMain::session_save_as()
{
  QString fileName = QFileDialog::getSaveFileName(this, "Choose sesion file", QString{_session_default_dir.c_str()});
  if(fileName == "") return;
  _session.store(fileName.toStdString());
}

void FRMain::session_load()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Choose sesion file", QString{_session_default_dir.c_str()});
  if(fileName == "") return;
  _session.load(fileName.toStdString());

  reset();
  FClassViewer::instance(this)->update(_session.get_loaded_classes());
  for(auto& rpt : _session.get_jammed_reports()) {
    JammedViewer::instance(this)->update(rpt.second);
  }
  TrafficViewer::instance(this)->update_traffic(_session.get_traffic());
  TrafficViewer::instance(this)->update_ranking(_session.get_rankings());
  for(auto& sig_rpts : _session.get_signal_reports()) {
    for(auto& sig : sig_rpts.second.xsignals()) {
      update_list(sig);
    }
  }
  for(auto& timer : _session.get_timer_reports()) {
    update_timed_list(timer.second);
  }

}


void FRMain::view_about()
{
  if(AboutHelpViewer::up()) {
    AboutHelpViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(AboutHelpViewer::instance(this),  &frenchroast::AboutHelpViewer::turn_on_profiler, this, &FRMain::turn_on_profiler);
    QObject::connect(AboutHelpViewer::instance(this),  &frenchroast::AboutHelpViewer::turn_off_profiler, this, &FRMain::turn_off_profiler);
    connect_common_listeners(AboutHelpViewer::instance(this));
  }
}

void FRMain::view_signals()
{
  if(FSignalViewer::up()) {
    FSignalViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(FSignalViewer::instance(this),  &frenchroast::FSignalViewer::view_detail_request, this, &FRMain::show_detail);
    connect_common_listeners(FSignalViewer::instance(this));
  }
}


void FRMain::view_timers()
{
  if(FTimerViewer::up()) {
    FTimerViewer::instance(this)->bring_to_top();
  }
  else {
    connect_common_listeners(FTimerViewer::instance(this));
  }
}

void FRMain::view_hooks_editor()
{
  if(Editor::up()) {
    Editor::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(Editor::instance(this), &frenchroast::Editor::validated_signals,  this,    &FRMain::validated_signals);
    connect_common_listeners(Editor::instance(this));
  }
}

void FRMain::add_hook(QString txt)
{
  Editor::instance(this)->add_hook(txt);
}

void FRMain::show_detail(const std::string& descriptor)
{
  DetailViewer* dv = DetailViewer::instance(this, descriptor);
  connect_common_listeners(dv);
  QObject::connect(this, &FRMain::update_detail_list, dv, &DetailViewer::update);
  QObject::connect(dv, &frenchroast::DetailViewer::add_signal,     this,  &FRMain::add_hook);
  QDockWidget* dock = *frenchroast::FSignalViewer::instance(this);
  dv->move(dock->x() + 50, dock->y() + 50 ); 
  update_detail_list(descriptor, &_detailDescriptors[descriptor]);
}

void FRMain::show_heap_detail(const std::string& class_name)
{
    HeapDetailViewer* dv = HeapDetailViewer::instance(this, class_name);
  connect_common_listeners(dv);
  QObject::connect(this, &FRMain::update_heap_detail, dv, &HeapDetailViewer::update);
  QObject::connect(dv, &frenchroast::HeapDetailViewer::add_signal,     this,  &FRMain::add_hook);
  QDockWidget* dock = *frenchroast::HeapViewer::instance(this);
  dv->move(dock->x() + 50, dock->y() + 50 );
  update_heap_detail(class_name, &_heapReports[class_name]);
}



void FRMain::update_class_viewer(const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  FClassViewer::instance(this)->update(details);
  _session.update(details);
}

void FRMain::update_jammed(const frenchroast::monitor::JammedReport& rpt)
{
  JammedViewer::instance(this)->update(rpt);
  _session.update(rpt);
}

void FRMain::update_heap(const frenchroast::monitor::HeapReport& rpt)
{
  HeapViewer::instance(this)->update(rpt);
  _heapReports[rpt.classname()] = rpt;
  update_heap_detail(rpt.classname(), &_heapReports[rpt.classname()]);
  //_session.update(rpt);
}

void FRMain::update_instrumentation_status(const frenchroast::monitor::InstrumentationReport& rpt)
{
  Editor::instance(this)->update(rpt);
}

void FRMain::start_watching_traffic(int rate)
{
  _watchTraffic = true;
  _trafficRate = rate;
  start_traffic(rate);
}

void FRMain::stop_watching_traffic()
{
  _watchTraffic = false;
  stop_traffic();
}


void FRMain::method_ranking(std::vector<frenchroast::monitor::MethodStats> ranks)
{
  TrafficViewer::instance(this)->update_ranking(ranks);
  _session.update(ranks);
}

void FRMain::update_list(const frenchroast::monitor::Signal& rpt)
{
  if(_exit) return;
  _signalReports[rpt.key()] += rpt;
  SignalReport& srpt = _signalReports[rpt.key()];
  _session.update(rpt);
  std::string tname = "[ " + srpt.thread_name() + " ] ";
  std::string descriptor = srpt.descriptor_name();
  frenchroast::monitor::pad(descriptor, 50);
  frenchroast::monitor::pad(tname, 10);

  descriptor = tname + descriptor;
  _detailDescriptors[descriptor] = DetailHolder{srpt.count(), srpt.arg_headers(), srpt.instance_headers(),
                                              srpt.markers(), srpt.stacks()};

  frenchroast::FSignalViewer::instance(this)->update_count(descriptor, srpt.count());
  update_detail_list(descriptor, &_detailDescriptors[descriptor]);
}

void FRMain::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
{
  if(!_exit) {
    TrafficViewer::instance(this)->update_traffic(stacks);
    _session.update(stacks);
  }
}

void FRMain::update_timed_list(const frenchroast::monitor::TimerReport& rpt)
{
  _session.update(rpt);
  std::string descriptor = rpt.descriptor_name();
  std::string tname = "[ " + rpt.thread_name() + " ]";
  frenchroast::monitor::pad(descriptor, 50);
  frenchroast::monitor::pad(tname, 10);
  descriptor = tname + descriptor;
  FTimerViewer::instance(this)->update_time(descriptor, rpt.elapsed());
}

void FRMain::exit_fr()
{
  QCoreApplication::quit();
}

void FRMain::handle_exit()
{
  _exit = true;

  FSignalViewer::capture();
  FTimerViewer::capture();
  Editor::capture();
  FClassViewer::capture();
  TrafficViewer::capture();
  JammedViewer::capture();
  HeapViewer::capture();
  AboutHelpViewer::capture();
}


void FRMain::restore_if_required(const bool required, void (FRMain::* func)(), bool& result)
{
  if(required) {
    (this->*func)();
    result = true;
  }
}

void FRMain::restore()
{
  bool winup = false;
  restore_if_required(FSignalViewer::restore_is_required(),   &FRMain::view_signals, winup);
  restore_if_required(FTimerViewer::restore_is_required(),    &FRMain::view_timers, winup);
  restore_if_required(FClassViewer::restore_is_required(),    &FRMain::view_classviewer, winup);
  restore_if_required(Editor::restore_is_required(),          &FRMain::view_hooks_editor, winup);
  restore_if_required(TrafficViewer::restore_is_required(),   &FRMain::view_traffic, winup);
  restore_if_required(JammedViewer::restore_is_required(),    &FRMain::view_jammed, winup);
  restore_if_required(HeapViewer::restore_is_required(),      &FRMain::view_heap, winup);
  restore_if_required(AboutHelpViewer::restore_is_required(), &FRMain::view_about, winup);

  if(!winup) {
    view_about();
  }
}

void FRMain::reset_viewers()
{
  FSignalViewer::reset();
  FTimerViewer::reset();
  FClassViewer::reset();
  TrafficViewer::reset();
  JammedViewer::reset();
  HeapViewer::reset();
  reset();
  _signalReports.clear();
}

