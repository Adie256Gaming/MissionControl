#pragma once
#include <switch.h>

namespace ams::bluetooth {

    enum SubCmdType : u8 {
        SubCmd_GetControllerState   = 0x00,
        SubCmd_ManualPair           = 0x01,
        SubCmd_RequestDeviceInfo 	= 0x02,
        SubCmd_SetInputReportMode	= 0x03,
        SubCmd_TriggersElapsedTime	= 0x04,
        SubCmd_SetHciState          = 0x06,
        SubCmd_ResetPairingInfo     = 0x07,
        SubCmd_SetShipPowerState    = 0x08,
        SubCmd_SpiFlashRead			= 0x10,
        SubCmd_SpiFlashWrite		= 0x11,
        SubCmd_SpiSectorErase       = 0x12,
        SubCmd_ResetMcu             = 0x20,
        SubCmd_SetMcuConfig			= 0x21,
        SubCmd_SetMcuState          = 0x22,
        SubCmd_SetPlayerLeds 		= 0x30,
        SubCmd_GetPlayerLeds        = 0x31,
        SubCmd_SetHomeLed           = 0x38,
        SubCmd_EnableImu			= 0x40,
        SubCmd_SetImuSensitivity    = 0x41,
        SubCmd_WriteImuRegisters    = 0x42,
        SubCmd_ReadImuRegisters     = 0x43,
        SubCmd_EnableVibration		= 0x48,
        SubCmd_GetRegulatedVoltage  = 0x50,
    };

    typedef BluetoothAddress Address;

    struct DeviceSettings : sf::LargeData {
        BluetoothDevicesSettings device;
    };

}
