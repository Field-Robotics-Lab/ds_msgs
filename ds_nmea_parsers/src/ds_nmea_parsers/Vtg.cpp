#include "ds_nmea_parsers/Vtg.h"
#include "ds_nmea_parsers/util.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace ds_nmea_msgs
{

bool from_string(Vtg& output, const std::string &nmea_string)
{
  // Set defaults
  output.talker.clear();
  output.track_degrees_true = Vtg::VTG_NO_DATA;
  output.track_degrees_magnetic = Vtg::VTG_NO_DATA;
  output.speed_knots = Vtg::VTG_NO_DATA;
  output.speed_km_per_hour = Vtg::VTG_NO_DATA;
  output.checksum = 0;

  // The GLL NMEA string can have lots of empty fields.  So we can't rely
  // on a standard sscanf with the whole message definition.
  //
  // Instead we'll split the message into substrings and use sscanf on each
  // part.  This is certainly not the most efficient way to do things BUT
  // we know that sscanf never generates exceptions (unlike boost::lexical_cast,
  // or some of the newer std::strtoX methods).  At worst we get bad data,
  // but at lest we don't have a lot of try/catch blocks.

  auto fields = std::vector<std::string>{};
  boost::split(fields, nmea_string, boost::is_any_of(",*"));

  // Expect at LEAST 10 fields.
  if (fields.size() < 10) {
    return false;
  }

  // VTG Track Made Good and Ground Speed
  //        1   2 3   4 5   6 7   8 9
  //        |   | |   | |   | |   | |
  // $--VTG,x.x,T,x.x,M,x.x,N,x.x,K*hh
  // 1) Track Degrees
  // 2) T = True
  // 3) Track Degrees
  // 4) M = Magnetic
  // 5) Speed Knots
  // 6) N = Knots
  // 7) Speed Kph
  // 8) K = Kilometres Per Hour
  // 9) Checksum

  auto i = 0;
  char talker[2];
  if (!sscanf(fields.at(i++).c_str(), "$%2cVTG", talker)) {
    return false;
  }
  output.talker = std::string{std::begin(talker), std::end(talker)};

  sscanf(fields.at(i++).c_str(), "%lf", &output.track_degrees_true);
  if(fields.at(i++).compare("T")) {
    return false;
  }
  sscanf(fields.at(i++).c_str(), "%lf", &output.track_degrees_magnetic);
  if(fields.at(i++).compare("M")) {
    return false;
  }
  sscanf(fields.at(i++).c_str(), "%lf", &output.speed_knots);
  if(fields.at(i++).compare("N")) {
    return false;
  }
  sscanf(fields.at(i++).c_str(), "%lf", &output.speed_km_per_hour);
  if(fields.at(i++).compare("K")) {
    return false;
  }

  if (fields.size() > 10) {
    sscanf(fields.at(i++).c_str(), "%hhx", &output.checksum);
  }

  return true;
}

}