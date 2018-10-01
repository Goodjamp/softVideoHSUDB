#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include <locale>
#include <QDebug>

#include <windows.h>

#include "hidinterface.h"

extern "C"
{
#include "Hidsdi.h"
}
#include "Setupapi.h"


hidInterface::hidInterface()
{
    currentHID = NULL;
}


void hidInterface::initUSB(void)
{
        GUID hidGUID;
        hidWPathList.clear();
        //Get the HID GUID value - used as mask to get list of devices
        HidD_GetHidGuid(&hidGUID);
        //Get a list of devices matching the criteria (hid interface, present)
        HDEVINFO hwDevInf = SetupDiGetClassDevs(&hidGUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

        SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
        deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        //Go through the list and get the interface data
        for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hwDevInf, NULL, &hidGUID, i, &deviceInterfaceData); i++) {
            ULONG reqSize;
            //Get the details with null values to get the required size of the buffer
            SetupDiGetDeviceInterfaceDetail(hwDevInf, &deviceInterfaceData, NULL, 0, &reqSize, 0);

            //Allocate the buffer
            PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(new uint8_t[reqSize]);
            deviceDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

            //Fill the buffer with the device details
            if (SetupDiGetDeviceInterfaceDetail(hwDevInf, &deviceInterfaceData, deviceDetail, reqSize, &reqSize, NULL)) {
                  hidWPathList.push_back(deviceDetail->DevicePath);
            }

            delete deviceDetail;
        }
        SetupDiDestroyDeviceInfoList(hwDevInf);
}



bool hidInterface::getInterfaceVidPid(uint index ,uint &VID, uint &PID)
{
    HIDD_ATTRIBUTES tempHidAtributes;
    if(index > hidWPathList.size())
    {
        return false;
    }
    std::wstring pathString = hidWPathList[index];
    HANDLE deviceFile = CreateFile(pathString.c_str(),
                                   GENERIC_READ|GENERIC_WRITE,
                                   FILE_SHARE_READ|FILE_SHARE_WRITE,
                                   (LPSECURITY_ATTRIBUTES)NULL,
                                   OPEN_EXISTING,
                                   0,
                                   NULL);
    if(deviceFile == NULL)
    {
        return false;
    }
    // Get a VID and PID information
    tempHidAtributes.Size = sizeof(tempHidAtributes);
    HidD_GetAttributes( deviceFile,&tempHidAtributes);
    VID = tempHidAtributes.VendorID;
    PID = tempHidAtributes.ProductID;

    CloseHandle(deviceFile);

    return true;
}



bool hidInterface::getDInterfaceInfo(uint index ,std::wstring &VID, std::wstring&PID , std::wstring &Manufacturer, std::wstring &Product )
{
    HIDD_ATTRIBUTES tempHidAtributes;
    if(index > hidWPathList.size())
    {
        return false;
    }
    std::wstring pathString = hidWPathList[index];
    HANDLE deviceFile = CreateFile(pathString.c_str(),
                                   GENERIC_READ|GENERIC_WRITE,
                                   FILE_SHARE_READ|FILE_SHARE_WRITE,
                                   (LPSECURITY_ATTRIBUTES)NULL,
                                   OPEN_EXISTING,
                                   0,
                                   NULL);
    if(deviceFile == NULL)
    {
        return false;
    }
    // Get a VID and PID information
    tempHidAtributes.Size = sizeof(tempHidAtributes);
    HidD_GetAttributes( deviceFile,&tempHidAtributes);
    VID = std::to_wstring(tempHidAtributes.VendorID);
    PID = std::to_wstring(tempHidAtributes.ProductID);

    wchar_t tempStringBuff[200];

    if( HidD_GetManufacturerString(deviceFile, tempStringBuff, sizeof(tempStringBuff)) )
    {
        Manufacturer = tempStringBuff;
    }
    else
    {
        Manufacturer = L"No Manufacturer str";
    }
    //Get Productr information
    if( HidD_GetProductString(deviceFile, tempStringBuff, sizeof(tempStringBuff)) )
    {
        Product = tempStringBuff;
    }
    else
    {
        Product = L"No Product str";
    }

    CloseHandle(deviceFile);
    return true;
}


bool hidInterface::openInterface(uint index)
{
    //HIDD_ATTRIBUTES tempHidAtributes;
    if(index > hidWPathList.size())
    {
        return false;
    }
    if(currentHID != 0 ) //class interface should be free
    {
        return false;
    }
    std::wstring pathString = hidWPathList[index];
    currentHID = CreateFile(pathString.c_str(),
                                   GENERIC_READ|GENERIC_WRITE,
                                   FILE_SHARE_READ|FILE_SHARE_WRITE,
                                   (LPSECURITY_ATTRIBUTES)NULL,
                                   OPEN_EXISTING,
                                   0,
                                   NULL);
    if(currentHID == NULL)
    {
        return false;
    }
    return true;
}


bool hidInterface::openInterface(uint VID, uint PID)
{
    HIDD_ATTRIBUTES tempHidAtributes;
    uint16_t k;
    std::wstring pathString;
    if(currentHID != 0 ) //class interface should be free
    {
        return false;
    }
    for(k = 0; k < hidWPathList.size(); k++)
    {
        pathString = hidWPathList[k];
        currentHID = CreateFile(pathString.c_str(),
                                       GENERIC_READ|GENERIC_WRITE,
                                       FILE_SHARE_READ|FILE_SHARE_WRITE,
                                       (LPSECURITY_ATTRIBUTES)NULL,
                                       OPEN_EXISTING,
                                       0,
                                       NULL);
        if(currentHID == NULL)
        {
            return false;
        }
        // Get a VID and PID information
        tempHidAtributes.Size = sizeof(tempHidAtributes);
        HidD_GetAttributes( currentHID,&tempHidAtributes);
        if(        VID == tempHidAtributes.VendorID &&
                   PID == tempHidAtributes.ProductID)

        {
            qDebug()<<"Device path: "<<pathString.c_str()<<"\n";
            qDebug()<<"Open succsesfuly \n";
            return true;
        }
        CloseHandle(currentHID);
    }
    return false;
}


bool hidInterface::closeInterface(void)
{
    if(currentHID == NULL)
    {
        return false;
    }
    qDebug()<<"HID handler before"<<currentHID<<"\n";
    if( 0 == CloseHandle(currentHID))
    {
        return false;
        qDebug()<<"Сant close HID device \n";
    }
    currentHID = NULL;
    qDebug()<<"Сlose HID device OK \n";
    return true;
}


bool hidInterface::isHIDOpen(void)
{
    if( NULL == currentHID )
    {
        return false;
    }
    return true;
}


uint32_t hidInterface::read(uint8_t *buff, uint32_t numToRead, uint32_t timeout)
{
     if(currentHID == NULL)
     {
         return 0;
     }
     long unsigned int numBytesOfRead;
     uint8_t *buffCommand = (uint8_t*)malloc(numToRead + 1);
     if( ReadFile(currentHID, buffCommand, numToRead + 1, &numBytesOfRead, NULL) )
     {
         memcpy(buff, buffCommand + 1, numToRead);
         free(buffCommand);
         return numBytesOfRead;
     }
     free(buffCommand);
     return 0;
}

//hidInterface
//userHIDInterfaces
uint32_t hidInterface::write(uint8_t *buff, uint32_t numToWrite, uint32_t timeout)
{
     if(currentHID == NULL)
     {
         return 0;
     }
     DWORD numBytesOfWrite;

     uint8_t *buffCommand = (uint8_t*)malloc(numToWrite + 1);
     buffCommand[0] = 0;
     memcpy( &buffCommand[1], buff, numToWrite);
     qDebug()<<"numToWrite = "<< numToWrite;;
     if( WriteFile(currentHID, buffCommand, numToWrite + 1, &numBytesOfWrite, NULL) > 0)
     {
         free(buffCommand);
         qDebug()<<"write Ok";
         return numBytesOfWrite;
     }
     qDebug()<<"write Error";
     DWORD lastError = GetLastError();
     free(buffCommand);
     return 0;
}


/*
HidDevice::HidDevice(std::string path)
{
 this->path = path;
 isValid = false;
 handle = CreateFile(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

 if (handle == INVALID_HANDLE_VALUE) {
  std::cout << "Error: Can't open device." << std::endl;
  return;
 }

 if (HidD_GetAttributes(handle, &this->attributes) == FALSE) {
  std::cout << "Error: Can't read hid device attributes." << std::endl;
  return;
 }

 PHIDP_PREPARSED_DATA preparsedData = NULL;

 if (HidD_GetPreparsedData(handle, &preparsedData) == FALSE) {
  std::cout << "Error: Can't get top-level collection's preparsed data." << std::endl;
  return;
 }

 if (HidP_GetCaps(preparsedData, &this->capabilities) != HIDP_STATUS_SUCCESS) {
  std::cout << "Error: Can't get hid device capabilities." << std::endl;
  return;
 }

 isValid = true;
}
*/
