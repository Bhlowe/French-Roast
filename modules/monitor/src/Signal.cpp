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


#include <iostream>
#include "Signal.h"
#include "Util.h"

namespace frenchroast { namespace monitor {


    Signal::Signal(const StackReport& sreport, const SignalParams& sparams, const SignalMarkers smarkers) :
      _report(sreport), _params(sparams), _markers(smarkers)
    {
    }
    
    const std::string Signal::thread_name() const
    {
      return  _report.trace().thread_name();
    }

    const std::string Signal::key() const
    {
      return thread_name() + descriptor();
    }
    
    const std::string Signal::descriptor() const
    {
      return _report.trace().descriptor_frames()[0].get_name();
    }

    const SignalParams& Signal::params() const
    {
      return _params;
    }
    
    const SignalMarkers& Signal::markers() const
    {
      return _markers;
    }

    const StackReport& Signal::report() const
    {
      return _report;
    }

    bool Signal::operator==(const Signal& ref) const
    {
      return _report == ref._report && _params == ref._params && _markers == ref._markers;
    }

    Signal& operator>>(const std::string& rep, Signal&& ref)
    {
      return rep >> ref;
    }
    
    Signal& operator>>(const std::string& rep, Signal& ref)
    {
      auto parts = frenchroast::split(rep.substr(1), "<end-report>");
      parts[0] >> ref._report;
      auto parts2 = frenchroast::split(parts[1], "<end-params>");
      parts2[0] >> ref._params;
      parts2[1] >> ref._markers;
      return ref;
    }

    std::vector<Signal>& operator>>(const std::string& rep, std::vector<Signal>& ref)
    {

      for(auto& sig : frenchroast::split(rep, "<end-signal>")) {
        if(sig == "") continue;
        ref.push_back( sig >> Signal{});
      }
      return ref;
    }
    

  }
}
