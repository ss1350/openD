/**
 * @code
 *  ___ _____ _   ___ _  _____ ___  ___  ___ ___
 * / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 * \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 * |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 * embedded.connectivity.solutions.==============
 * @endcode
 *
 * @file       main.cpp
 * @copyright  STACKFORCE GmbH, Heitersheim, Germany, www.stackforce.de
 * @author     Heiko Berger
 * @brief      Client side demo application.
 * @details
 *
 * This work is dual-licensed under Apache 2.0 and GPL 2.0. You can choose between one of them if you use this work.
 * For further details, please refer to the project home: https://github.com/opend-connect/openD
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
 *
 */

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <iomanip>
#include "fp_hanfun_app.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "udp.h"
#include <../../json/single_include/nlohmann/json.hpp>

#ifndef IPC
#define IPC
// #include <boost/thread.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
template <typename T> using Alloc = boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;
template <typename K> using List = boost::interprocess::list<K, Alloc<K>>;
#endif

#ifndef TSN_CUC_DECT
#define TSN_CUC_DECT
#include "tsn-cuc-dect.hpp"
#include "tsn-cuc-dect.cpp"
#endif

using json = nlohmann::json;

/**
 * @brief   Handle confirmation and indication.
 *
 * @details Handle received confirmation and indication data from the udp server.
 *
 * @param   buffer Pointer to the received data.
 */
void handleConfirmationAndIndication(char *buffer);

void handleConfirmationAndIndication(char *buffer)
{
  printf("Received udp data from server: %s\n", buffer);

  json j = json::parse(buffer);

  string version = j.at("version").get<std::string>();
  string module = j.at("module").get<std::string>();
  string primitive = j.at("primitive").get<std::string>();
  string service = j.at("service").get<std::string>();
  string status = j.at("status").get<std::string>();
  string param1 = j.at("param1").get<std::string>();
  string param2 = j.at("param2").get<std::string>();
  string param3 = j.at("param3").get<std::string>();

  if(service.compare("deviceManagementRegisterEnable") == 0)
  {
    if(status.compare("OK") == 0)
    {
      std::cout.clear (); std::cout << "[INFO ] " << "Enable the Registration mode: SUCCESS" <<
      std::endl; std::cout.clear (); std::cerr.clear ();
      std::cout.clear (); std::cout << "[INFO ] " << "Next Registration will have the address : " <<
      param1 << std::endl; std::cout.clear (); std::cerr.clear ();
    }
    if(status.compare("ERR") == 0)
    {
      std::cerr.clear (); std::cerr << "[WARN ] " << "[HANFUN] Enable the Registration mode: FAIL" <<
      std::endl; std::cout.clear (); std::cerr.clear ();
    }
    if(status.compare("INV") == 0)
    {
      std::cerr.clear (); std::cerr << "[ERROR] " << "Registration not possible: address "
      << "not available." << std::endl; std::cout.clear (); std::cerr.clear ();
    }
  }
  if(service.compare("deviceManagementRegisterDisable") == 0)
  {
    if(status.compare("OK") == 0)
    {
      std::cout.clear (); std::cout << "[INFO ] " << "Disable the Registration mode: SUCCESS" <<
      std::endl; std::cout.clear (); std::cerr.clear ();
    }
    if(status.compare("ERR") == 0)
    {
      std::cerr.clear (); std::cerr << "[WARN ] " << "Disable the Registration mode: FAIL" <<
      std::endl; std::cout.clear (); std::cerr.clear ();
    }
  }
  if(service.compare("deviceManagementEntriesRegistration") == 0)
  {
    if(status.compare("OK") == 0)
    {
      std::cout.clear (); std::cout << std::setfill (' ');
      std::cout.clear (); std::cout << "HAN-FUN" << "  Currently registered Devices (" <<
      param1
      << "):" << std::endl; std::cout.clear (); std::cerr.clear ();
      std::cout.clear (); std::cout << std::right << std::setw (5) <<
      param2 << " | ";
      std::cout.clear (); std::cout << "dect uid: " << param3 <<
      std::endl; std::cout.clear (); std::cerr.clear ();
      // write into shm

      // Open shared memory
      boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only,"SYSREPO_SHM");

      //An allocator convertible to any allocator<T, segment_manager_t> type
      void_allocator alloc(segment.get_segment_manager());

      // whole module:
      module_t* module = segment.construct<module_t>("dect_cuc_tsn")(alloc);

      std::pair<module_t*, std::size_t> shmModule = segment.find<module_t>("dect_cuc_tsn");

      if (shmModule.first)
      {
        shmModule.first->addDevice(device_t(param1, param3.c_str(), alloc));
        std::cout << "this should work!\n";
      }
      else
      {
        std::cout << "NO SHM AVAILABLE\n";
      }
      /**
       * International Portable User Identity (IPUI)
       * |  PUT   |                    PUN                    |
       * | 4 bits |                   36 bits                 |
       */
      // typedef uint8_t ipui_t[5];
      // write into shared memory from here!
    }
    if(status.compare("INV") == 0)
    {
      std::cout.clear (); std::cout << std::setfill (' ');
      std::cout.clear (); std::cout << "HAN-FUN" << "  Currently registered Devices (" <<
      0 << ")" << std::endl; std::cout.clear (); std::cerr.clear ();
    }
  }
  if(service.compare("deviceManagementDeregister") == 0)
  {
    if(status.compare("OK") == 0)
    {
      std::cout.clear (); std::cout << "[INFO ] " << "Device " << param1 << " de-registration: "
      << "SUCCESS" << " !" << std::endl; std::cout.clear (); std::cerr.clear ();
    }
    if(status.compare("ERR") == 0)
    {
      std::cout.clear (); std::cout << "[INFO ] " << "Device " << param1 << " de-registration: "
      << "FAIL" << " !" << std::endl; std::cout.clear (); std::cerr.clear ();
    }
  }
  if(service.compare("deviceManagementRegisterDevice") == 0)
  {
    if(status.compare("OK") == 0)
    {
      std::cout.clear (); std::cout << "[INFO ] " << "Device " << param1 << " registration: "
      << "SUCCESS" << " !" << std::endl; std::cout.clear (); std::cerr.clear ();
      
      // client side of successful registration: start list request from here?
      HF::Application::Handle("lr");
    }
    if(status.compare("ERR") == 0)
    {
      std::cout.clear (); std::cout << "[INFO ] " << "Device " << param1 << " registration: "
      << "FAIL" << " !" << std::endl; std::cout.clear (); std::cerr.clear ();
    }
  }
  if(service.compare("IOnOffClientToggle") == 0)
  {
    if(status.compare("OK") == 0)
    {
      std::cout.clear (); std::cout << "[INFO ] " << "Indication 'Toggle LED' received from device: " <<
      param1 << std::endl; std::cout.clear (); std::cerr.clear ();
    }
  }
}

// =============================================================================
// Main
// =============================================================================

int main (void)
{
  char buffer[1024];
  bool udpSendInit = true;

  HF::Application::Initialize(&handleConfirmationAndIndication);

  std::string input = "?";
  while( !HF::Application::Handle(input) )
  {
    std::cin.getline(buffer, sizeof(buffer));
    input = std::string(buffer);
    if(udpSendInit)
    {
      udpSendHello();
      udpSendInit = false;
    }
  }

  exit(0);
}
