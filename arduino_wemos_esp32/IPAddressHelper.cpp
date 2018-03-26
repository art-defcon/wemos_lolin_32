#include "IPAddressHelper.h"

// ctor
IPAddressHelper::IPAddressHelper(IPAddress x)
{
  ip = x;
}
// func
String IPAddressHelper::getString()
{
  return String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
}
