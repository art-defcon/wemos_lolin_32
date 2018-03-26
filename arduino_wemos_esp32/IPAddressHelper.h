#include "WiFi.h"

class IPAddressHelper
{
  private:
  IPAddress ip;

  public:
  IPAddressHelper(IPAddress x);
  String getString();
};
