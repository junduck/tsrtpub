#pragma once

#include <simdjson.h>
#include <string>
#include <string_view>
#include <array>

struct tsrt_quote
{

  tsrt_quote(const std::string &json_quote, simdjson::dom::parser &p)
  {
    simdjson::dom::array data;
    auto err = p.parse(json_quote).get(data);
    if (!err)
    {
      std::string_view tmp_view;
      std::size_t i = 0;
      for (auto elem : data)
      {
        switch (i)
        {
        case 0:
          err = elem.get(tmp_view);
          Code = std::string(tmp_view);
          break;
        case 1:
          err = elem.get(tmp_view);
          Name = std::string(tmp_view);
          break;
        case 2:
          err = elem.get(tmp_view);
          Time = std::string(tmp_view);
          break;
        case 3:
          err = elem.get(Price);
          break;
        case 4:
          err = elem.get(PreClose);
          break;
        case 5:
          err = elem.get(Open);
          break;
        case 6:
          err = elem.get(High);
          break;
        case 7:
          err = elem.get(Low);
          break;
        case 8:
          // err = elem.get(Close);
          Close = 0.0;
          break;
        case 9:
          err = elem.get(Vol);
          break;
        case 10:
          err = elem.get(Tnvr);
          break;
        case 11:
          err = elem.get(Ask[0]);
          break;
        case 12:
          err = elem.get(AskVol[0]);
          break;
        case 13:
          err = elem.get(Bid[0]);
          break;
        case 14:
          err = elem.get(BidVol[0]);
          break;
        case 15:
          err = elem.get(Ask[1]);
          break;
        case 16:
          err = elem.get(AskVol[1]);
          break;
        case 17:
          err = elem.get(Bid[1]);
          break;
        case 18:
          err = elem.get(BidVol[1]);
          break;
        case 19:
          err = elem.get(Ask[2]);
          break;
        case 20:
          err = elem.get(AskVol[2]);
          break;
        case 21:
          err = elem.get(Bid[2]);
          break;
        case 22:
          err = elem.get(BidVol[2]);
          break;
        case 23:
          err = elem.get(Ask[3]);
          break;
        case 24:
          err = elem.get(AskVol[3]);
          break;
        case 25:
          err = elem.get(Bid[3]);
          break;
        case 26:
          err = elem.get(BidVol[3]);
          break;
        case 27:
          err = elem.get(Ask[4]);
          break;
        case 28:
          err = elem.get(AskVol[4]);
          break;
        case 29:
          err = elem.get(Bid[4]);
          break;
        case 30:
          err = elem.get(BidVol[4]);
          break;
        case 31:
          err = elem.get(OpenInterest);
          break;
        case 32:
          err = elem.get(Order);
          break;
        default:
          break;
        }
        ++i;
      }
      const auto td = Time.data();
      ITime = static_cast<int>(td[0] - '0') * 10 * 3600
            + static_cast<int>(td[1] - '0') * 3600
            + static_cast<int>(td[3] - '0') * 10 * 60 
            + static_cast<int>(td[4] - '0') * 60
            + static_cast<int>(td[6] - '0') * 10 
            + static_cast<int>(td[7] - '0');
    }
  }

  std::string Code;
  std::string Name;
  std::string Time;
  // data.table compatible ITime
  int ITime;
  double Price;
  double PreClose;
  double Open;
  double High;
  double Low;
  // Close is not parsed
  double Close;
  double Vol;
  double Tnvr;
  std::array<double, 5> Ask, AskVol, Bid, BidVol;
  long OpenInterest;
  long Order;
};