
#include "common/device_id.h"
#include "log/log.h"

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace gomros {
namespace common {

bool DeviceID::Get(std::string& device_id, const std::string& type) {
  if (type == "HardDriveSerial") {
    device_id = GetHardDriveSerialNumber();
    if (device_id == "") return false;

  } else {
    return GetMac(device_id);
  }

  LOG_INFO("device_id: %s", device_id.c_str());

  return true;
}

bool DeviceID::GetMac(std::string& mac) {
  std::vector<std::string> macAddresses;
  if (!GetALLMac(macAddresses)) {
    return false;
  }

  if (macAddresses.size() == 0) {
    return false;
  }

  mac = macAddresses[0];

  LOG_INFO("mac: %s", mac.c_str());
  return true;
}

bool DeviceID::GetALLMac(std::vector<std::string>& macAddresses) {
  macAddresses.clear();

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    LOG_ERROR("无法创建套接字");
    return false;
  }

  struct ifreq ifr;
  struct ifconf ifc;
  char buf[1024];

  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;

  if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
    LOG_ERROR("无法获取接口配置信息。");
    close(sock);
    return false;
  }

  struct ifreq* it = ifc.ifc_req;
  const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

  for (; it != end; ++it) {
    std::string interfaceName = it->ifr_name;
    if (ioctl(sock, SIOCGIFFLAGS, it) == 0) {
      if (!(it->ifr_flags & IFF_LOOPBACK)) {  // 排除回环接口
        std::string macAddress = GetMACAddress(sock, interfaceName);
        if (!macAddress.empty()) {
          macAddresses.push_back(macAddress);
        }
      }
    } else {
      LOG_WARN("无法获取接口标志。");
    }
  }

  close(sock);

  return true;
}

// 获取指定网络接口的MAC地址
std::string DeviceID::GetMACAddress(int sock,
                                    const std::string& interfaceName) {
  struct ifreq ifr;
  std::memset(&ifr, 0, sizeof(ifr));
  std::strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

  if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {
    LOG_WARN("无法获取MAC地址。");
    return "";
  }

  unsigned char macAddress[6];
  std::memcpy(macAddress, ifr.ifr_hwaddr.sa_data, 6);

  std::stringstream stream;
  stream << std::hex << std::uppercase << std::setfill('0');
  ;
  for (int i = 0; i < 6; ++i) {
    if (i > 0) {
      stream << ":";
    }
    stream << std::setw(2) << static_cast<int>(macAddress[i]);
  }

  return stream.str();
}

std::string DeviceID::GetHardDriveSerialNumber() {
  std::string serialNumber;

  std::string device_name = GetHardDriveName();
  if (device_name == "") {
    return serialNumber;
  }

  // /sys/class/block/<device>/serial文件以获取序列号信息
  std::string sysPath = "/sys/class/block/" + device_name + "/serial";
  std::ifstream sysfsFile(sysPath);
  if (sysfsFile) {
    std::getline(sysfsFile, serialNumber);
    sysfsFile.close();
    if (!serialNumber.empty()) {
      return serialNumber;
    }
  }
  return serialNumber;
}

std::string DeviceID::GetHardDriveName() {
  std::string mountPoint = "/";
  std::string deviceName;
  std::ifstream mountsFile("/proc/mounts");

  if (mountsFile) {
    std::string line;
    while (std::getline(mountsFile, line)) {
      std::istringstream iss(line);
      std::string device, mountPath, fsType, options;
      iss >> device >> mountPath >> fsType >> options;

      if (mountPath == mountPoint) {
        // 解析设备路径，通常在/dev目录下
        size_t lastSlash = device.find_last_of('/');
        if (lastSlash != std::string::npos) {
          deviceName = device.substr(lastSlash + 1);
        }
        break;
      }
    }
    mountsFile.close();
  }

    LOG_INFO("deviceName=%s",deviceName.c_str());
  return deviceName;
}

}  // namespace common
}  // namespace gomros
