#include "fakeswitchcontroller.hpp"
#include "../bluetooth/bluetooth_hid_report.hpp"

namespace ams::controller {

    Result FakeSwitchController::setVibration(void) {

        return ams::ResultSuccess();
    }

    Result FakeSwitchController::setPlayerLed(u8 led_mask) {

        return ams::ResultSuccess();
    }

    const bluetooth::HidReport * FakeSwitchController::handleIncomingReport(const bluetooth::HidReport *report) {
        m_inputReport.size = 0;

        u8 cmdId = report->data[0];
        switch (cmdId) {

            default:
                break;
        }

        return &m_inputReport;;
    }

    const bluetooth::HidReport * FakeSwitchController::handleOutgoingReport(const bluetooth::HidReport *report) {
        m_outputReport.size = 0;

        u8 cmdId = report->data[0];
        switch (cmdId) {
            case 0x01:  // Subcmd
                this->handleSubCmdReport(report);
                break;

            case 0x10:  // Rumble
            default:
                break;
        }

        return &m_outputReport;
    }

    Result FakeSwitchController::handleSubCmdReport(const bluetooth::HidReport *report) {
        const u8 *subCmd = &report->data[10];
        auto subCmdId = static_cast<bluetooth::SubCmdType>(subCmd[0]);
        switch (subCmdId) {
            case bluetooth::SubCmd_RequestDeviceInfo:
                R_TRY(this->subCmdRequestDeviceInfo(report));
                break;
            case bluetooth::SubCmd_SpiFlashRead:
                R_TRY(this->subCmdSpiFlashRead(report));
                break;
            case bluetooth::SubCmd_SpiFlashWrite:
                R_TRY(this->subCmdSpiFlashWrite(report));
                break;
            case bluetooth::SubCmd_SpiSectorErase:
                R_TRY(this->subCmdSpiSectorErase(report));
                break;
            case bluetooth::SubCmd_SetInputReportMode:
                R_TRY(this->subCmdSetInputReportMode(report));
                break;
            case bluetooth::SubCmd_TriggersElapsedTime:
                R_TRY(this->subCmdTriggersElapsedTime(report));
                break;
            case bluetooth::SubCmd_SetShipPowerState:
                R_TRY(this->subCmdSetShipPowerState(report));
                break;
            case bluetooth::SubCmd_SetMcuConfig:
                R_TRY(this->subCmdSetMcuConfig(report));
                break;
            case bluetooth::SubCmd_SetMcuState:
                R_TRY(this->subCmdSetMcuState(report));
                break;
            case bluetooth::SubCmd_SetPlayerLeds:
                R_TRY(this->subCmdSetPlayerLeds(report));
                break;
            case bluetooth::SubCmd_EnableImu:
                R_TRY(this->subCmdEnableImu(report));
                break;
            case bluetooth::SubCmd_EnableVibration:
                R_TRY(this->subCmdEnableVibration(report));
                break;
            default:
                break;
        }

        return ams::ResultSuccess();
    }

    Result FakeSwitchController::subCmdRequestDeviceInfo(const bluetooth::HidReport *report) {
        const u8 response[] = {0x82, 0x02, 0x03, 0x48, 0x03, 0x02, m_address.address[0], m_address.address[1], m_address.address[2], m_address.address[3], m_address.address[4], m_address.address[5], 0x01, 0x02};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdSpiFlashRead(const bluetooth::HidReport *report) {
        // Official Pro Controller reads these
        // @ 0x00006000: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff                            <= Serial 
        // @ 0x00006050: 32 32 32 ff ff ff ff ff ff ff ff ff                                        <= RGB colours (body, buttons, left grip, right grip)
        // @ 0x00006080: 50 fd 00 00 c6 0f 0f 30 61 ae 90 d9 d4 14 54 41 15 54 c7 79 9c 33 36 63    <= Factory Sensor and Stick device parameters
        // @ 0x00006098: 0f 30 61 ae 90 d9 d4 14 54 41 15 54 c7 79 9c 33 36 63                      <= Stick device parameters 2. Normally the same with 1, even in Pro Contr.
        // @ 0x00008010: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    <= User Analog sticks calibration
        // @ 0x0000603d: e6 a5 67 1a 58 78 50 56 60 1a f8 7f 20 c6 63 d5 15 5e ff 32 32 32 ff ff ff <= Left analog stick calibration
        // @ 0x00006020: 64 ff 33 00 b8 01 00 40 00 40 00 40 17 00 d7 ff bd ff 3b 34 3b 34 3b 34    <= 6-Axis motion sensor Factory calibration
        
        u32 read_addr = *(u32 *)(&report->data[11]);
        u8  read_size = report->data[15];

        const u8 prefix[] = {0x90, bluetooth::SubCmd_SpiFlashRead, report->data[11], report->data[12], report->data[13], report->data[14], report->data[15]};

        int response_size = read_size + sizeof(prefix);
        auto response = std::make_unique<u8[]>(response_size);
        std::memcpy(response.get(), prefix, sizeof(prefix));
        std::memset(response.get() + sizeof(prefix), 0xff, read_size); // Console doesn't seem to mind if response is uninitialised data (0xff)

        // Set default controller body colour
        if (read_addr == 0x6050) {
            std::memset(response.get() + sizeof(prefix), 0x32, 3);
        }

        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response.get(), response_size);    
    }

    Result FakeSwitchController::subCmdSpiFlashWrite(const bluetooth::HidReport *report) {
        const u8 response[] = {0x80, bluetooth::SubCmd_SpiFlashWrite, 0x01};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdSpiSectorErase(const bluetooth::HidReport *report) {
        const u8 response[] = {0x80, bluetooth::SubCmd_SpiSectorErase, 0x01};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdSetInputReportMode(const bluetooth::HidReport *report) {
        const u8 response[] = {0x80, bluetooth::SubCmd_SetInputReportMode};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdTriggersElapsedTime(const bluetooth::HidReport *report) {
        const u8 response[] = {0x83, bluetooth::SubCmd_TriggersElapsedTime};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdSetShipPowerState(const bluetooth::HidReport *report) {
        const u8 response[] = {0x80, bluetooth::SubCmd_SetShipPowerState, 0x00};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdSetMcuConfig(const bluetooth::HidReport *report) {
        const u8 response[] = {0xa0, bluetooth::SubCmd_SetMcuConfig, 0x01, 0x00, 0xff, 0x00, 0x03, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdSetMcuState(const bluetooth::HidReport *report) {
        const u8 response[] = {0x80, bluetooth::SubCmd_SetMcuState};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdSetPlayerLeds(const bluetooth::HidReport *report) {
        const u8 *subCmd = &report->data[10];
        u8 led_mask = subCmd[1];
        R_TRY(this->setPlayerLed(led_mask));

        const u8 response[] = {0x80, bluetooth::SubCmd_SetPlayerLeds};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdEnableImu(const bluetooth::HidReport *report) {
        const u8 response[] = {0x80, bluetooth::SubCmd_EnableImu};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

    Result FakeSwitchController::subCmdEnableVibration(const bluetooth::HidReport *report) {
        const u8 response[] = {0x80, bluetooth::SubCmd_EnableVibration};
        return bluetooth::hid::report::FakeSubCmdResponse(&m_address, response, sizeof(response));
    }

}
