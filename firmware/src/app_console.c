/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_console.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"
#include "app_console.h"

#include "./mplabml/inc/kb.h"
#include "./mplabml/inc/testdata.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/* Structure containing data to be stored in non volatile memory */
typedef struct
{
    /* Meter ID */
    char meterID[7];

} APP_CONSOLE_STORAGE_DATA;

#define CONSOLE_TASK_DELAY_MS_UNTIL_DATALOG_READY      100
#define CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES   10
#define CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT       30


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_CONSOLE_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_CONSOLE_DATA CACHE_ALIGN app_consoleData;

/* Local variable to hold a duplicate of storage data */
APP_CONSOLE_STORAGE_DATA CACHE_ALIGN app_consoleStorageData;
/* Constant conaining default value for storage data */
const APP_CONSOLE_STORAGE_DATA CACHE_ALIGN app_defaultConsoleStorageData = {{'1', '2', '3', '4', '\0', '\0', '\0'}};

/* Local storage objects */
static APP_ENERGY_ACCUMULATORS energyData;
static APP_ENERGY_MAX_DEMAND maxDemandLocalObject;
static DRV_METROLOGY_HARMONICS_RMS harmonicAnalysisRMSData[DRV_METROLOGY_HARMONICS_MAX_ORDER];
static HARMONICS_REG_ID harRegsDisplayOrder[HARMONICS_REG_NUM] = {
    HARMONICS_I_A_m_R_ID,
    HARMONICS_I_A_m_I_ID,
    HARMONICS_V_A_m_R_ID,
    HARMONICS_V_A_m_I_ID,
    HARMONICS_I_B_m_R_ID,
    HARMONICS_I_B_m_I_ID,
    HARMONICS_V_B_m_R_ID,
    HARMONICS_V_B_m_I_ID,
    HARMONICS_I_C_m_R_ID,
    HARMONICS_I_C_m_I_ID,
    HARMONICS_V_C_m_R_ID,
    HARMONICS_V_C_m_I_ID,
    HARMONICS_I_N_m_R_ID,
    HARMONICS_I_N_m_I_ID
}; 

/* Local Queue element to request Datalog operations */
APP_DATALOG_QUEUE_DATA datalogQueueElement;

/* Reference to datalog queue */
extern APP_DATALOG_QUEUE appDatalogQueue;

/* Local array to hold password for Commands */
#define APP_CONSOLE_MET_PWD_SIZE             6
static char metPwd[APP_CONSOLE_MET_PWD_SIZE] = APP_CONSOLE_DEFAULT_PWD;

static char sign[2] = {' ', '-'};

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
static void _commandBUF (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandCALA(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandCALB(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandCALC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandCALT(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandCNF (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDAR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCB (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCD (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCM (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCS (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDCW (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandDSR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandENC (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandENR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandEVEC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandEVER(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandHAR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandHRR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandIDR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandIDW (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandMDC (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandMDR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandPAR (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRTCR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRTCW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandTOUR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandTOUW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRST (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandRLD (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandHELP(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandLOADDATA(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _commandLOADID(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

#define APP_CONSOLE_HAR_DESC_SIZE    7
const char * _console_har_desc[APP_CONSOLE_HAR_DESC_SIZE] =
{
  "Irms_Har_A(A)",
  "Irms_Har_B(A)",
  "Irms_Har_C(A)",
  "Irms_Har_N(A)",
  "Vrms_Har_A(V)",
  "Vrms_Har_B(V)",
  "Vrms_Har_C(V)"
};

static const SYS_CMD_DESCRIPTOR appCmdTbl[]=
{
    {"BUF", _commandBUF, ": Read waveform capture data (if a parameter is used, only a 512 samples sector is returned)"},
    {"CAL_A", _commandCALA, ": Automatic calibration phase A"},
    {"CAL_B", _commandCALB, ": Automatic calibration phase B"},
    {"CAL_C", _commandCALC, ": Automatic calibration phase C"},
    {"CAL_T", _commandCALT, ": Automatic calibration three phases A,B,C"},
    {"CNF", _commandCNF, ": Automatic configuration"},
    {"DAR", _commandDAR, ": Read DSP_ACC register"},
    {"DCB", _commandDCB, ": Go to low power mode"},
    {"DCD", _commandDCD, ": Load default metrology control register values"},
    {"DCM", _commandDCM, ": Write DSP_CONTROL several registers"},
    {"DCR", _commandDCR, ": Read DSP_CONTROL registers"},
    {"DCS", _commandDCS, ": Save metrology constants to non volatile memory"},
    {"DCW", _commandDCW, ": Write DSP_CONTROL register"},
    {"DSR", _commandDSR, ": Read DSP_ST register"},
    {"ENC", _commandENC, ": Clear all energy"},
    {"ENR", _commandENR, ": Read energy"},
    {"EVEC",_commandEVEC, ": Clear all event record"},
    {"EVER",_commandEVER, ": Read single event record"},
    {"HAR", _commandHAR, ": Read harmonic register"},
    {"HRR", _commandHRR, ": Read harmonic Irms/Vrms"},
    {"IDR", _commandIDR, ": Read meter id"},
    {"IDW", _commandIDW, ": Write meter id (id length limited to 6 characters)"},
    {"MDC", _commandMDC, ": Clear all maxim demand and happen time"},
    {"MDR", _commandMDR, ": Read maxim demand"},
    {"PAR", _commandPAR, ": Read measure parameter"},
    {"RTCR",_commandRTCR, ": Read meter RTC"},
    {"RTCW",_commandRTCW, ": Write meter RTC"},
    {"TOUR",_commandTOUR, ": Read meter TOU"},
    {"TOUW",_commandTOUW, ": Write meter TOU"},
    {"RST", _commandRST, ": System reset"},
    {"RLD", _commandRLD, ": Reload Metrology Coprocessor"},
    {"HELP",_commandHELP, ": Help on commands"},
    {"LOADDATA",_commandLOADDATA, ": Capture Data forLoad disaggregation"},
    {"LOADID",_commandLOADID, ": Identification of Load with ML"}
};



/*******************************************************************************
  Function:
    Timer in milliseconds for creating and waiting the delay.
 */

static bool APP_CONSOLE_TaskDelay(uint32_t ms, SYS_TIME_HANDLE* handle)
{
    // Check if there is the timer has been created and running
    if (*handle == SYS_TIME_HANDLE_INVALID)
    {
        // Create timer
        if (SYS_TIME_DelayMS(ms, handle) != SYS_TIME_SUCCESS)
        {
            return false;
        }
    }
    else
    {
        // Check timer
        if (SYS_TIME_DelayIsComplete(*handle) == true)
        {
            *handle = SYS_TIME_HANDLE_INVALID;
            return true;
        }
    }

    return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void _getSysTimeFromMonthIndex(struct tm *time, uint8_t index)
{
    struct tm sysTime;

    RTC_TimeGet(&sysTime);

    if (sysTime.tm_mon < index)
    {
        sysTime.tm_mon += 12;
        sysTime.tm_year--;
    }
    sysTime.tm_mon -= index;

    *time = sysTime;
}

uint8_t _getMonthIndexFromSysTime(struct tm *time)
{
    struct tm sysTime;
    int8_t index;

    RTC_TimeGet(&sysTime);

    index = sysTime.tm_mon - time->tm_mon;
    if (index < 0)
    {
        index += 12;
    }

    return (uint8_t)index;
}

bool _preprocessorCallback ( const SYS_CMD_DESCRIPTOR* pCmdTbl,
        SYS_CMD_DEVICE_NODE* pCmdIO, char* cmdBuff, size_t bufSize, void* hParam)
{
    char *pBuff = cmdBuff;
    char cmdChar;
    size_t idx;

    for(idx = 0; idx < bufSize; idx++, pBuff++)
    {
        cmdChar = *pBuff;
        if ((cmdChar >= 'a') && (cmdChar <= 'z'))
        {
            // Convert upper case
            *pBuff -= 32;
        }
        else if ((cmdChar == '\t') || (cmdChar == ',') || (cmdChar == ';') || (cmdChar == '[') ||
                (cmdChar == ']') || (cmdChar == '(') || (cmdChar == ')'))
        {
            // Replace command separator
            *pBuff = ' ';
        }
    }

    return false;
}

static void _consoleReadStorage(APP_DATALOG_RESULT result)
{
    // Check result and go to corresponding state
    if (result == APP_DATALOG_RESULT_SUCCESS)
    {
        app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
    }
    else
    {
        app_consoleData.state = APP_CONSOLE_STATE_READ_STORAGE_ERROR;
    }
}

static void _monthlyEnergyCallback(struct tm * time, bool dataValid)
{
    if (!dataValid)
    {
        memset(&energyData, 0, sizeof(energyData));
    }

    app_consoleData.timeRequest = *time;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY;
}

static void _maxDemandCallback(struct tm * time, bool dataValid)
{
    if (!dataValid)
    {
        memset(&maxDemandLocalObject, 0, sizeof(maxDemandLocalObject));
    }

    app_consoleData.timeRequest = *time;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_MAX_DEMAND;
}

static void _harmonicAnalysisCallback(uint8_t harmonicNum)
{
    // Stop harmonic analysis
    DRV_METROLOGY_StopHarmonicAnalysis();
    
    app_consoleData.harmonicNumRequest = harmonicNum;
    app_consoleData.harmonicNumPrint = 1;
    if (harmonicNum == 0)
    {
        app_consoleData.numRegsPending = APP_CONSOLE_HAR_DESC_SIZE;
        app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
        app_consoleData.state = APP_CONSOLE_STATE_PRINT_ALL_HARMONIC_ANALYSIS;
    }
    else
    {
        app_consoleData.state = APP_CONSOLE_STATE_PRINT_HARMONIC_ANALYSIS;
    }
}

static void _calibrationCallback(bool result)
{
    app_consoleData.calibrationResult = result;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_CALIBRATION_RESULT;
}

// *****************************************************************************
// COMMANDS
// *****************************************************************************
static inline void _removePrompt(void)
{
    SYS_CMD_MESSAGE("\b");
}

static void _commandHELP(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        SYS_CMD_DESCRIPTOR *pCmd;
        uint8_t idx;
        size_t len;

        // Show help for a single command
        pCmd = (SYS_CMD_DESCRIPTOR *)appCmdTbl;
        for (idx = 0; idx < app_consoleData.numCommands; idx++, pCmd++)
        {
            len = strlen(argv[1]);

            if (strncmp(pCmd->cmdStr, argv[1], len) == 0)
            {
                SYS_CMD_PRINT("%s\t%s\r\n", pCmd->cmdStr, pCmd->cmdDescr);
                break;
            }
        }

        if (idx == app_consoleData.numCommands)
        {
            SYS_CMD_MESSAGE("Command is not found.\r\n");
        }
    }
    else
    {
        app_consoleData.state = APP_CONSOLE_STATE_PRINT_HELP;
        app_consoleData.cmdNumToShowHelp = app_consoleData.numCommands;
        app_consoleData.pCmdDescToShowHelp = (SYS_CMD_DESCRIPTOR *)appCmdTbl;

        // Remove Prompt symbol
        _removePrompt();
    }

    /* Show console communication icon */
    APP_DISPLAY_SetSerialCommunication();
}

static void _commandBUF(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint32_t captureAddress;
    size_t captureSize;
    uint8_t idxMax;
    uint8_t idx = 0xFF;

    if (argc > 2)
    {
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
        return;
    }

    if (argc == 2)
    {
        // Extract index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
    }

    // Store parameters in local variables and go to corresponding state
    captureSize = APP_METROLOGY_GetWaveformCaptureData(&captureAddress);

    if ((captureSize == 0) || (captureAddress == 0))
    {
        SYS_CMD_MESSAGE("Waveform data is disabled.\r\n");
        return;
    }

    app_consoleData.rawDataLen = captureSize;
    app_consoleData.rawData = (uint32_t *)captureAddress;

    idxMax = (captureSize - 1) >> 9;

    if (idx != 0xFF)
    {
        if (idx > idxMax)
        {
            SYS_CMD_MESSAGE("Parameter is out of range.\r\n");
            return;
        }

        if (captureSize > 512)
        {
            /* Check if it is the last fragment */
            if (idx == idxMax)
            {
                app_consoleData.rawDataLen = captureSize - (idx << 9);
            }
            else
            {
                app_consoleData.rawDataLen = 512;
            }
        }

        app_consoleData.rawData += (idx << 9);
    }

    app_consoleData.rawDataFlag = true;
    app_consoleData.state = APP_CONSOLE_STATE_PRINT_WAVEFORM_DATA;

    /* Show console communication icon */
    APP_DISPLAY_SetSerialCommunication();

}

static bool _getCalibrationValue(char * argv, char * substring, double * value)
{
    char *p;

    if (strncmp(argv, substring, 2) == 0) {
        // Get substring after '=' char
        p = strstr(argv, "=");
        if (p != NULL) {
            // Advance ptr to ignore '=' and extract value
            p++;
            *value = strtod(p, NULL);
            return true;
        }
    }

    return false;
}

static void _commandCALA(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    APP_METROLOGY_CALIBRATION newCalibration = {0};
    bool parseError = false;

    if (argc != 4) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        return;
    }

    newCalibration.lineId = PHASE_A;

    if(_getCalibrationValue(argv[1], "UA", &newCalibration.aimVA) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[2], "IA", &newCalibration.aimIA) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[3], "AA", &newCalibration.angleA) == false)
    {
        parseError = true;
    }

    if (parseError) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        SYS_CMD_MESSAGE("Calibrating...\r\n");
        APP_METROLOGY_StartCalibration(&newCalibration);
    }
}

static void _commandCALB(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    APP_METROLOGY_CALIBRATION newCalibration = {0};
    bool parseError = false;

    if (argc != 4) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        return;
    }

    newCalibration.lineId = PHASE_B;

    if(_getCalibrationValue(argv[1], "UB", &newCalibration.aimVB) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[2], "IB", &newCalibration.aimIB) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[3], "AB", &newCalibration.angleB) == false)
    {
        parseError = true;
    }

    if (parseError) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        SYS_CMD_MESSAGE("Calibrating...\r\n");
        APP_METROLOGY_StartCalibration(&newCalibration);
    }
}

static void _commandCALC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    APP_METROLOGY_CALIBRATION newCalibration = {0};
    bool parseError = false;

    if (argc != 4) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        return;
    }

    newCalibration.lineId = PHASE_C;

    if(_getCalibrationValue(argv[1], "UC", &newCalibration.aimVC) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[2], "IC", &newCalibration.aimIC) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[3], "AC", &newCalibration.angleC) == false)
    {
        parseError = true;
    }

    if (parseError) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        SYS_CMD_MESSAGE("Calibrating...\r\n");
        APP_METROLOGY_StartCalibration(&newCalibration);
    }
}

static void _commandCALT(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    APP_METROLOGY_CALIBRATION newCalibration = {0};
    bool parseError = false;

    if (argc != 10) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        return;
    }

    newCalibration.lineId = PHASE_T;

    if(_getCalibrationValue(argv[1], "UA", &newCalibration.aimVA) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[2], "IA", &newCalibration.aimIA) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[3], "AA", &newCalibration.angleA) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[4], "UB", &newCalibration.aimVB) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[5], "IB", &newCalibration.aimIB) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[6], "AB", &newCalibration.angleB) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[7], "UC", &newCalibration.aimVC) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[8], "IC", &newCalibration.aimIC) == false)
    {
        parseError = true;
    }
    else if(_getCalibrationValue(argv[9], "AC", &newCalibration.angleC) == false)
    {
        parseError = true;
    }

    if (parseError) {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        SYS_CMD_MESSAGE("Calibrating...\r\n");
        APP_METROLOGY_StartCalibration(&newCalibration);
    }
}

static void _commandCNF(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    DRV_METROLOGY_CONFIGURATION newConf;
    uint8_t idx, value;
    char *p;
    bool parseError = false;

    if (argc == 8)
    {
        // Parse arguments
        for (idx = 1; idx < argc; idx++)
        {
            if (strncmp(argv[idx], "MC", 2) == 0)
            {
                // Get substring after '=' char
                p = strstr(argv[idx], "=");
                if (p != NULL)
                {
                    // Advance ptr to ignore '=' and extract value
                    p++;
                    newConf.mc = strtol(p, NULL, 10);
                }
                else
                {
                    parseError = true;
                    break;
                }
            }
            else if (strncmp(argv[idx], "ST", 2) == 0)
            {
                // Get substring after '=' char
                p = strstr(argv[idx], "=");
                if (p != NULL)
                {
                    // Advance ptr to ignore '=' and extract value
                    p++;
                    value = strtol(p, NULL, 10);
                    if (value < SENSOR_NUM_TYPE)
                    {
                        newConf.st = (DRV_METROLOGY_SENSOR_TYPE)value;
                    }
                    else
                    {
                        parseError = true;
                    }
                }
                else
                {
                    parseError = true;
                    break;
                }
            }
            else if (strncmp(argv[idx], "F", 1) == 0)
            {
                // Get substring after '=' char
                p = strstr(argv[idx], "=");
                if (p != NULL)
                {
                    // Advance ptr to ignore '=' and extract value
                    p++;
                    newConf.freq = strtod(p, NULL);
                }
                else
                {
                    parseError = true;
                    break;
                }
            }
            else if (strncmp(argv[idx], "G", 1) == 0)
            {
                // Get substring after '=' char
                p = strstr(argv[idx], "=");
                if (p != NULL)
                {
                    // Advance ptr to ignore '=' and extract value
                    p++;
                    value = strtol(p, NULL, 10);
                    switch(value)
                    {
                        case 1:
                            newConf.gain = (DRV_METROLOGY_GAIN_TYPE)0;
                            break;
                        case 2:
                            newConf.gain = (DRV_METROLOGY_GAIN_TYPE)1;
                            break;
                        case 4:
                            newConf.gain = (DRV_METROLOGY_GAIN_TYPE)2;
                            break;
                        case 8:
                            newConf.gain = (DRV_METROLOGY_GAIN_TYPE)3;
                            break;
                        default:
                            parseError = true;
                    }
                }
                else
                {
                    parseError = true;
                    break;
                }
            }
            else if (strncmp(argv[idx], "TR", 2) == 0)
            {
                // Get substring after '=' char
                p = strstr(argv[idx], "=");
                if (p != NULL)
                {
                    // Advance ptr to ignore '=' and extract value
                    p++;
                    newConf.tr = strtod(p, NULL);
                }
                else
                {
                    parseError = true;
                    break;
                }
            }
            else if (strncmp(argv[idx], "RL", 2) == 0)
            {
                // Get substring after '=' char
                p = strstr(argv[idx], "=");
                if (p != NULL)
                {
                    // Advance ptr to ignore '=' and extract value
                    p++;
                    newConf.rl = strtod(p, NULL);
                }
                else
                {
                    parseError = true;
                    break;
                }
            }
            else if (strncmp(argv[idx], "KU", 2) == 0)
            {
                // Get substring after '=' char
                p = strstr(argv[idx], "=");
                if (p != NULL)
                {
                    // Advance ptr to ignore '=' and extract value
                    p++;
                    newConf.ku = strtol(p, NULL, 10);
                }
                else
                {
                    parseError = true;
                    break;
                }
            }
            else
            {
                parseError = true;
                break;
            }
        }
    }
    else
    {
        // Incorrect parameter number
        parseError = true;
    }

    if (parseError)
    {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        APP_METROLOGY_SetConfiguration(&newConf);
        SYS_CMD_MESSAGE("Configure Meter is Ok !\r\n");

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
}

static void _commandDAR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        // Read all metrology accumulator registers
        app_consoleData.accumRegToRead = 0;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_ACCUM_REGS;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < ACCUMULATOR_REG_NUM)
        {
            // Read register value
            app_consoleData.accumRegToRead = idx;
            app_consoleData.state = APP_CONSOLE_STATE_READ_ACCUM_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCB(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        app_consoleData.state = APP_CONSOLE_STATE_LOW_POWER_MODE;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCD(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        SYS_CMD_MESSAGE("Load Default Is Ok !\r\n");
        // Set default control register values
        APP_METROLOGY_SetControlByDefault();

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCM(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t i;
    uint8_t numRegsToWrite;
    char *p;
    bool parseError = false;

    if (argc > 1)
    {
        // Each argument includes reg index and value
        numRegsToWrite = (argc - 1);
        for (i = 0; i < numRegsToWrite; i++)
        {
            // Extract register index and value from argument
            p = argv[i + 1];
            app_consoleData.regsToModify[i].index = (uint8_t)strtol(p, NULL, 10);
            // Look for ":" char and advance to next char
            p = strstr(p, ":");
            if (p != NULL)
            {
                p++;
                app_consoleData.regsToModify[i].value = (uint32_t)strtoul(p, NULL, 16);
            }
            else
            {
                SYS_CMD_MESSAGE("Unsupported Command !\r\n");
                parseError = true;
                break;
            }
        }

        if (!parseError)
        {
            // Write invalid values after last, to later detect it
            app_consoleData.regsToModify[i].index = 0xFF;
            app_consoleData.regsToModify[i].value = 0xFFFFFFFF;
            // Go to corresponding state
            app_consoleData.state = APP_CONSOLE_STATE_WRITE_CONTROL_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        // Read all metrology control registers
        app_consoleData.ctrlRegToRead = 0;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_CONTROL_REGS;
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < CONTROL_REG_NUM)
        {
            // Read register value
            app_consoleData.ctrlRegToRead = idx;
            app_consoleData.state = APP_CONSOLE_STATE_READ_CONTROL_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCS(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        SYS_CMD_MESSAGE("Save Data Is Ok !\r\n");
        // Save Metrology Constants and configuration settings to NVM
        APP_METROLOGY_StoreMetrologyData();

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDCW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;
    uint32_t regValue;

    if (argc == 3)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < CONTROL_REG_NUM)
        {
            // Extract register value
            regValue = (uint32_t)strtoul(argv[2], NULL, 16);
            // Write register value
            if (APP_METROLOGY_SetControlRegister((CONTROL_REG_ID)idx, regValue))
            {
                // Show response on console
                SYS_CMD_MESSAGE("Set Is Ok !\r\n");

                /* Show console communication icon */
                APP_DISPLAY_SetSerialCommunication();
            }
            else
            {
                // Cannot write register
                SYS_CMD_PRINT("Could not write register %02d\r\n", idx);
            }
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandDSR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        // Read all metrology status registers
        app_consoleData.statusRegToRead = 0;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_STATUS_REGS;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < STATUS_REG_NUM)
        {
            // Read register value
            app_consoleData.statusRegToRead = idx;
            app_consoleData.state = APP_CONSOLE_STATE_READ_STATUS_REG;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid register index\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandENC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Clear Energy records
            APP_ENERGY_ClearEnergy(true);
            // Show response on console
            SYS_CMD_MESSAGE("Clear Energy is ok !\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandENR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t monthIndex;

    if (argc > 2)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }

    if (argc == 2) {
        // Extract month index from parameters
        monthIndex = (uint8_t)strtol(argv[1], NULL, 10);
        monthIndex %= 12;
        app_consoleData.requestCounter = 1;
    }
    else
    {
        monthIndex = 0;
        // Start process to get full Monthly energy data
        app_consoleData.requestCounter = 12;
    }

    // Get SysTime
    _getSysTimeFromMonthIndex(&app_consoleData.sysTime, monthIndex);

    // Get monthly energy from energy app
    if (APP_ENERGY_GetMonthEnergy(&app_consoleData.sysTime) == false)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param\r\n");
    }
    else
    {
        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    // Response will be provided on _monthlyEnergyCallback function
}

static void _commandEVEC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            APP_EVENTS_ClearEvents();
            // Show response on console
            SYS_CMD_MESSAGE("Clear All Event is ok !\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandEVER(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    app_consoleData.eventIdRequest = EVENT_INVALID_ID;

    if (argc == 3)
    {
        // Extract event id from parameters
        if (strcmp(argv[1], "UA") == 0)
        {
            app_consoleData.eventIdRequest = SAG_UA_EVENT_ID;
        }
        else if (strcmp(argv[1], "UB") == 0)
        {
            app_consoleData.eventIdRequest = SAG_UB_EVENT_ID;
        }
        else if (strcmp(argv[1], "UC") == 0)
        {
            app_consoleData.eventIdRequest = SAG_UC_EVENT_ID;
        }
        else if (strcmp(argv[1], "PA") == 0)
        {
            app_consoleData.eventIdRequest = POW_UA_EVENT_ID;
        }
        else if (strcmp(argv[1], "PB") == 0)
        {
            app_consoleData.eventIdRequest = POW_UB_EVENT_ID;
        }
        else if (strcmp(argv[1], "PC") == 0)
        {
            app_consoleData.eventIdRequest = POW_UC_EVENT_ID;
        }

        if (app_consoleData.eventIdRequest != EVENT_INVALID_ID)
        {
            // Extract last times from parameters
            app_consoleData.eventLastTimeRequest = (uint8_t)strtol(argv[2], NULL, 10);

            app_consoleData.state = APP_CONSOLE_STATE_PRINT_EVENT;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid Command
            SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandHAR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t idx;

    if (argc == 1)
    {
        APP_METROLOGY_CaptureHarmonicData();
        
        // Read all metrology harmonics registers
        app_consoleData.harNumToRead = 0;
        app_consoleData.numRegsPending = HARMONICS_REG_NUM;
        app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
        app_consoleData.state = APP_CONSOLE_STATE_READ_ALL_HARMONICS_REGS;
    }
    else if (argc == 2)
    {
        // Extract register index from parameters
        idx = (uint8_t)strtol(argv[1], NULL, 10);
        if (idx < DRV_METROLOGY_HARMONICS_MAX_ORDER)
        {
            APP_METROLOGY_CaptureHarmonicData();
            
            // Read register value
            app_consoleData.harNumToRead = idx;
            app_consoleData.numRegsPending = HARMONICS_REG_NUM;
            app_consoleData.state = APP_CONSOLE_STATE_READ_HARMONIC_REGS;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid index
            SYS_CMD_MESSAGE("Invalid harmonic order\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandHRR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t harmonicNum = 0xFF;

    if (argc == 1)
    {
        harmonicNum = 0;
    }
    else if (argc == 2)
    {
        // Extract harmonic number from parameters
        harmonicNum = (uint8_t)strtol(argv[1], NULL, 10);
    }
    
    if (harmonicNum != 0xFF)
    {
        // Set harmonics calculation mode on metrology driver
        if (APP_METROLOGY_StartHarmonicAnalysis(harmonicNum) == false)
        {
            // Incorrect parameter number
            SYS_CMD_MESSAGE("Previous harmonic analysis is running\r\n");
        }
        else
        {
            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        // Response will be provided on _harmonicAnalysisCallback function
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandIDR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        // Read Meter ID
        app_consoleData.state = APP_CONSOLE_STATE_READ_METER_ID;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandIDW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 3)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, write Meter ID
            memcpy(&app_consoleStorageData.meterID, argv[2], sizeof(app_consoleStorageData.meterID));
            // Build queue element to write it to storage
            datalogQueueElement.userId = APP_DATALOG_USER_CONSOLE;
            datalogQueueElement.operation = APP_DATALOG_WRITE;
            datalogQueueElement.endCallback = NULL;
            datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
            datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
            datalogQueueElement.dataLen = sizeof(app_consoleStorageData);
            datalogQueueElement.pData = (uint8_t*)&app_consoleStorageData;
            // Put it in queue
            APP_DATALOG_SendDatalogData(&datalogQueueElement);
            SYS_CMD_MESSAGE("Set Meter ID is Ok\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandMDC(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Clear Max Demand records
            APP_ENERGY_ClearMaxDemand(true);
            // Show response on console
            SYS_CMD_MESSAGE("Clear MaxDemand is ok !\r\n");

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandMDR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t monthIndex;

    if (argc >2)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }

    if (argc == 2) {
        // Extract month index from parameters
        monthIndex = (uint8_t)strtol(argv[1], NULL, 10);
        monthIndex %= 12;
        app_consoleData.requestCounter = 1;
    }
    else
    {
        monthIndex = 0;
        // Start process to get full Monthly energy data
        app_consoleData.requestCounter = 12;
    }

    // Get SysTime
    _getSysTimeFromMonthIndex(&app_consoleData.sysTime, monthIndex);

    // Get monthly energy from energy app
    if (APP_ENERGY_GetMonthMaxDemand(&app_consoleData.sysTime) == false)
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param\r\n");
    }
    else
    {
        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    // Response will be provided on _maxDemandCallback function
}

static void _commandPAR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool wakeup = false;

    if (argc == 2)
    {
        // Extract data to retrieve from parameters
        if (strcmp(argv[1], "U") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_VOLTAGE;
            wakeup = true;
        }
        else if (strcmp(argv[1], "I") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_CURRENT;
            wakeup = true;
        }
        else if (strcmp(argv[1], "P") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_ACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "Q") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_REACTIVE_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "S") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_APARENT_POWER;
            wakeup = true;
        }
        else if (strcmp(argv[1], "F") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_FREQUENCY;
            wakeup = true;
        }
        else if (strcmp(argv[1], "A") == 0)
        {
            app_consoleData.state = APP_CONSOLE_STATE_PRINT_ANGLE;
            wakeup = true;
        }
        else
        {
            // Invalid Command
            SYS_CMD_MESSAGE("Unsupported Command !\r\n");
        }

        if (wakeup)
        {
            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandRTCR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        // Read RTC
        app_consoleData.state = APP_CONSOLE_STATE_READ_RTC;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandRTCW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 5)
    {
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            char *p;

            // Get Date
            p = argv[2];
            // Year
            app_consoleData.sysTime.tm_year = (uint8_t)strtol(p, NULL, 10) + 2000 - 1900;
            // Look for "-" char and advance to next char
            p = strstr(p, "-");
            if (p != NULL)
            {
                p++;
                // Month
                app_consoleData.sysTime.tm_mon = (uint8_t)strtol(p, NULL, 10) - 1;
                // Look for "-" char and advance to next char
                p = strstr(p, "-");
                if (p != NULL)
                {
                    p++;
                    // Day
                    app_consoleData.sysTime.tm_mday = (uint8_t)strtol(p, NULL, 10);
                }
            }

            // Get Week Day
            p = argv[3];
            app_consoleData.sysTime.tm_wday = (uint8_t)strtol(p, NULL, 10) - 1;

            // Get Time
            p = argv[4];
            // Hour
            app_consoleData.sysTime.tm_hour = (uint8_t)strtol(p, NULL, 10);
            // Look for ":" char and advance to next char
            p = strstr(p, ":");
            if (p != NULL)
            {
                p++;
                // Minute
                app_consoleData.sysTime.tm_min = (uint8_t)strtol(p, NULL, 10);
                // Look for ":" char and advance to next char
                p = strstr(p, ":");
                if (p != NULL)
                {
                    p++;
                    // Second
                    app_consoleData.sysTime.tm_sec = (uint8_t)strtol(p, NULL, 10);
                }
            }

            if (RTC_TimeSet(&app_consoleData.sysTime))
            {
                // Build queue element to write it to storage
                datalogQueueElement.userId = APP_DATALOG_USER_RTC;
                datalogQueueElement.operation = APP_DATALOG_WRITE;
                datalogQueueElement.endCallback = NULL;
                datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
                datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
                datalogQueueElement.dataLen = sizeof(struct tm);
                datalogQueueElement.pData = (uint8_t*)&app_consoleData.sysTime;
                // Put it in queue
                APP_DATALOG_SendDatalogData(&datalogQueueElement);

                // Clear No-persistent energy/demand data
                APP_ENERGY_ClearEnergy(false);
                APP_ENERGY_ClearMaxDemand(false);

                SYS_CMD_MESSAGE("Set RTC is ok!\r\n");

                /* Show console communication icon */
                APP_DISPLAY_SetSerialCommunication();
            }
            else
            {
                SYS_CMD_MESSAGE("Unsupported Command !\r\n");
            }
        }
        else
        {
            SYS_CMD_MESSAGE("Password Error !\r\n");
        }
    }
    else
    {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
}

static void _commandTOUR(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 1)
    {
        // Go to state to read TOU
        app_consoleData.state = APP_CONSOLE_STATE_READ_TOU;

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
    else
    {
        // Incorrect param number
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
}

static void _commandTOUW(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    char *p;
    APP_ENERGY_TOU_TIME_ZONE timeZone[APP_ENERGY_TOU_MAX_ZONES];
    uint8_t idx, argIdx;
    bool parseError = false;

    if ((argc > 3) && ((argc - 2) % 2 == 0))
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
             // Correct password, write TOW
             for (idx = 0; idx < APP_ENERGY_TOU_MAX_ZONES; idx++)
             {
                // Check whether there are arguments left to write
                argIdx = ((idx << 1) + 2);
                if (argc > argIdx)
                {
                    // Extract hour, minute and rate
                    p = argv[argIdx];
                    // Extract hour from argument
                    timeZone[idx].hour = (uint8_t)strtol(p, NULL, 10);
                    // Look for ":" char and advance to next char
                    p = strstr(p, ":");
                    if (p != NULL)
                    {
                        p++;
                        // Extract minute from argument
                        timeZone[idx].minute = (uint8_t)strtol(p, NULL, 10);
                        // Extract rate from next argument
                        timeZone[idx].tariff = (uint8_t)strtol(argv[argIdx + 1], NULL, 10);
                    }
                    else
                    {
                        parseError = true;
                        break;
                    }

                    if ((timeZone[idx].hour > 23) ||
                        (timeZone[idx].minute > 59) ||
                        (timeZone[idx].tariff > TARIFF_4))
                    {
                            parseError = true;
                            break;
                    }
                }
                else
                {
                    // No more arguments, fill TOU index with invalid data
                    timeZone[idx].hour = 0;
                    timeZone[idx].minute = 0;
                    timeZone[idx].tariff = TARIFF_INVALID;
                }
             }
        }
        else
        {
            // Invalid password
            parseError = true;
        }
    }
    else
    {
        // Incorrect parameter number
        parseError = true;
    }

    if (parseError)
    {
        SYS_CMD_MESSAGE("Unsupported Command !\r\n");
    }
    else
    {
        APP_ENERGY_SetTOUTimeZone(timeZone);

        // Clear No-persistent energy/demand data
        APP_ENERGY_ClearEnergy(false);
        APP_ENERGY_ClearMaxDemand(false);

        SYS_CMD_MESSAGE("Set TOU is Ok !\r\n");

        /* Show console communication icon */
        APP_DISPLAY_SetSerialCommunication();
    }
}

static void _commandRST(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Reset System
            SYS_CMD_MESSAGE("Reset Command is Ok !\r\n");
            // Go to state to reset system
            app_consoleData.state = APP_CONSOLE_STATE_SW_RESET;

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _commandRLD(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc == 2)
    {
        // Check password from parameters
        if (strncmp(argv[1], metPwd, APP_CONSOLE_MET_PWD_SIZE) == 0)
        {
            // Correct password, Reset System
            SYS_CMD_MESSAGE("Reloading Metrology...\r\n\r\n");
            // Reload Metrology coprocessor
            APP_METROLOGY_Restart();

            /* Show console communication icon */
            APP_DISPLAY_SetSerialCommunication();
        }
        else
        {
            // Invalid password
            SYS_CMD_MESSAGE("Invalid password\r\n");
        }
    }
    else
    {
        // Incorrect parameter number
        SYS_CMD_MESSAGE("Incorrect param number\r\n");
    }
}

static void _harmonicAnalysisCaptureLoadDataCallback(uint8_t harmonicNum)
{
    // Collected metrology data
    typedef struct
    {
        uint16_t voltage;
        uint16_t current;
        uint16_t active_power;
        uint16_t reactive_power;
        uint16_t apparent_power;
        uint16_t phase;
        uint16_t harmonic_1;
        uint16_t harmonic_2;
        uint16_t harmonic_3;
        uint16_t harmonic_4;
        uint16_t harmonic_5;
        uint16_t harmonic_6;
        uint16_t harmonic_7;
        uint16_t harmonic_8;
        uint16_t harmonic_9;
    } SENSOR_DATA_SET;
    
    SENSOR_DATA_SET sensor_data;
    double harmonic_factor = 1000000;
        
    // Data trunked the most significant bits to use 16-bits format for samples
    sensor_data.voltage         = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_UA)>>6);         // Voltage
    sensor_data.current         = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_IA)>>4);         // Current
    sensor_data.active_power    = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_PA));            // Active Power
    sensor_data.reactive_power  = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_QA));            // Reactive Power
    sensor_data.apparent_power  = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_SA));            // Apparent Power
    sensor_data.phase           = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_ANGLEA)>>9);     // Phase angle
    sensor_data.harmonic_1 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[0].Irms_A_m * harmonic_factor)));   // Current Harmonic 1
    sensor_data.harmonic_2 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[1].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_3 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[2].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_4 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[3].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_5 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[4].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_6 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[5].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_7 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[6].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_8 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[7].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_9 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[8].Irms_A_m * harmonic_factor)));      // Current Harmonic 1

    // Data sent using MPLAB Data Visualizer format
    uint8_t headerFrame = 0x03;
    uint8_t endFrame = 0xFC;
    FLEXCOM0_USART_Write(&headerFrame, sizeof(headerFrame));
    FLEXCOM0_USART_Write(&sensor_data, sizeof(sensor_data));
    FLEXCOM0_USART_Write(&endFrame, sizeof(endFrame));
}

static void _commandLOADDATA(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    APP_METROLOGY_SetHarmonicAnalysisCallback(_harmonicAnalysisCaptureLoadDataCallback, &harmonicAnalysisRMSData[0]);
    APP_METROLOGY_StartHarmonicAnalysis(0);
}

//#define KB_DATA_TEST
#ifdef KB_DATA_TEST
uint8_t test_idx = 0;
#endif
static void _harmonicAnalysisLoadIdentificationCallback(uint8_t harmonicNum)
{
    int32_t ret = 0;
    uint32_t num_sensors = 6;
    uint32_t kb_model_index = 0;
    
    // Stop harmonic analysis for load disaggregation
    DRV_METROLOGY_StopHarmonicAnalysis();
    APP_METROLOGY_SetHarmonicAnalysisCallback(_harmonicAnalysisCallback, &harmonicAnalysisRMSData[0]);

    // Metrology data required by ML model (defined in model.json)
    typedef struct
    {
//        uint16_t voltage;
        uint16_t phase;
        uint16_t current;
        uint16_t active_power;
        uint16_t reactive_power;
//        uint16_t apparent_power;
        uint16_t harmonic_1;
//        uint16_t harmonic_2;
        uint16_t harmonic_3;
//        uint16_t harmonic_4;
//        uint16_t harmonic_5;
//        uint16_t harmonic_6;
//        uint16_t harmonic_7;
//        uint16_t harmonic_8;
//        uint16_t harmonic_9;
    } SENSOR_DATA_SET;
    
    SENSOR_DATA_SET sensor_data;
    // Loads included in the ML model (defined in model.json)
    char* loads[6] = {"Unknown", "Fan", "Heater_R1", "Heater_R2", "Lightbulb", "No load"};
    
#ifdef KB_DATA_TEST    
    // Loading test data values (in testdata.h)
    sensor_data.current =           testdata[test_idx][CURRENT_S_PIPELINE_LOAD_2_RANK_0];  // Current
    sensor_data.active_power =      testdata[test_idx][ACTIVEPOWER_S_PIPELINE_LOAD_2_RANK_0]; // Active Power
    sensor_data.reactive_power =    testdata[test_idx][REACTIVEPOWER_S_PIPELINE_LOAD_2_RANK_0]; // Reactive Power
//    sensor_data.apparent_power =    testdata[test_idx][APPARENTPOWER_S_PIPELINE_LOAD_RANK_0]; // Apparent Power    
    sensor_data.phase =             testdata[test_idx][PHASE_S_PIPELINE_LOAD_2_RANK_0];  // Phase angle
    sensor_data.harmonic_1 =        testdata[test_idx][HARMONIC1_S_PIPELINE_LOAD_2_RANK_0];  // Current Harmonic 1
//    sensor_data.harmonic_2 =        testdata[test_idx][HARMONIC2_S_PIPELINE_LOAD_RANK_0];  // Current Harmonic 2
    sensor_data.harmonic_3 =        testdata[test_idx][HARMONIC3_S_PIPELINE_LOAD_2_RANK_0];  // Current Harmonic 3
//    sensor_data.harmonic_4 =        testdata[test_idx][HARMONIC4_S_PIPELINE_LOAD_RANK_0];  // Current Harmonic 4
//    sensor_data.harmonic_5 =        testdata[test_idx][HARMONIC5_S_PIPELINE_LOAD_RANK_0];  // Current Harmonic 5
//    sensor_data.harmonic_6 =        testdata[test_idx][HARMONIC6_S_PIPELINE_LOAD_RANK_0];  // Current Harmonic 6
//    sensor_data.harmonic_7 =        testdata[test_idx][HARMONIC7_S_PIPELINE_LOAD_RANK_0];  // Current Harmonic 7
//    sensor_data.harmonic_8 =        testdata[test_idx][HARMONIC8_S_PIPELINE_LOAD_RANK_0]; // Current Harmonic 8
//    sensor_data.harmonic_9 =        testdata[test_idx][HARMONIC9_S_PIPELINE_LOAD_RANK_0]; // Current Harmonic 9
    
    if(test_idx < TD_NUMROWS) 
        test_idx++;
    else
        test_idx=0;
#else
    double harmonic_factor = 1000000;        
    // Data trunked the most significant bits to use 16-bits format for samples
    sensor_data.current         = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_IA)>>4);         // Current
    sensor_data.active_power    = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_PA));            // Active Power
    sensor_data.reactive_power  = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_QA));            // Reactive Power
//    sensor_data.apparent_power  = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_SA));            // Apparent Power
    sensor_data.phase           = (uint16_t)(DRV_METROLOGY_GetRMSValue(RMS_ANGLEA)>>9);     // Phase angle
    sensor_data.harmonic_1 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[0].Irms_A_m * harmonic_factor)));   // Current Harmonic 1
//    sensor_data.harmonic_2 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[1].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
    sensor_data.harmonic_3 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[2].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
//    sensor_data.harmonic_4 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[3].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
//    sensor_data.harmonic_5 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[4].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
//    sensor_data.harmonic_6 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[5].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
//    sensor_data.harmonic_7 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[6].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
//    sensor_data.harmonic_8 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[7].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
//    sensor_data.harmonic_9 = (uint16_t)(((uint32_t) (harmonicAnalysisRMSData[8].Irms_A_m * harmonic_factor)));      // Current Harmonic 1
#endif    
    
    ret = kb_run_model((int16_t *) &sensor_data, num_sensors, kb_model_index);
    if (ret >= 0)
    {
        //Output model results
        _removePrompt();
//        SYS_CMD_PRINT("Voltage:        %u\r\n", sensor_data.voltage);
        SYS_CMD_PRINT("Current:        %u\r\n", sensor_data.current);
        SYS_CMD_PRINT("Active Power:   %u\r\n", sensor_data.active_power);
        SYS_CMD_PRINT("Reactive Power: %u\r\n", sensor_data.reactive_power);
//        SYS_CMD_PRINT("Apparent Power: %u\r\n", sensor_data.apparent_power);
        SYS_CMD_PRINT("Phase:          %u\r\n", sensor_data.phase);
        SYS_CMD_PRINT("Harmonic_Ia_1:  %u\r\n", sensor_data.harmonic_1);
//        SYS_CMD_PRINT("Harmonic_Ia_2:  %u\r\n", sensor_data.harmonic_2);
        SYS_CMD_PRINT("Harmonic_Ia_3:  %u\r\n", sensor_data.harmonic_3);
//        SYS_CMD_PRINT("Harmonic_Ia_4:  %u\r\n", sensor_data.harmonic_4);
//        SYS_CMD_PRINT("Harmonic_Ia_5:  %u\r\n", sensor_data.harmonic_5);
//        SYS_CMD_PRINT("Harmonic_Ia_6:  %u\r\n", sensor_data.harmonic_6);
//        SYS_CMD_PRINT("Harmonic_Ia_7:  %u\r\n", sensor_data.harmonic_7);
//        SYS_CMD_PRINT("Harmonic_Ia_8:  %u\r\n", sensor_data.harmonic_8);
//        SYS_CMD_PRINT("Harmonic_Ia_9:  %u\r\n", sensor_data.harmonic_9);
        SYS_CMD_PRINT("Load identified is: %s\r\n", loads[ret]);

        //Reset running model to initial state.
        kb_reset_model(kb_model_index);
        kb_flush_model_buffer(kb_model_index);

    }
    else
    {
        SYS_CMD_PRINT("Error in load identification: %02d\r\n", ret);        
    }

    /* Wait time to show message through the Console */
    app_consoleData.state = APP_CONSOLE_STATE_PROMPT;    
}

    
static void _commandLOADID(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    APP_METROLOGY_SetHarmonicAnalysisCallback(_harmonicAnalysisLoadIdentificationCallback, &harmonicAnalysisRMSData[0]);
    APP_METROLOGY_StartHarmonicAnalysis(0);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CONSOLE_Initialize ( void )

  Remarks:
    See prototype in app_console.h.
 */

void APP_CONSOLE_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_consoleData.state = APP_CONSOLE_STATE_INIT;
    app_consoleData.numCommands = sizeof(appCmdTbl)/sizeof(SYS_CMD_DESCRIPTOR);

    /* Init timer */
    app_consoleData.timer = SYS_TIME_HANDLE_INVALID;

    if (!SYS_CMD_ADDGRP(appCmdTbl, app_consoleData.numCommands, "App Console", ": Metering console commands"))
    {
        SYS_CONSOLE_Print(SYS_CONSOLE_INDEX_0, "Failed to create APP Console Commands\r\n");
        app_consoleData.numCommands = 0;
    }
    else
    {
        SYS_CMD_CallbackRegister(appCmdTbl, _preprocessorCallback, 0);
    }
    
    /* Init ML model */
    kb_model_init();    
//    SYS_CMD_PRINT("%u\r\n", kb_get_model_uuid_ptr(0));
//    SYS_CMD_PRINT("Number of expected columns in the input sensor: %i\r\n", kb_get_num_sensor_buffers(0));
    
}


/******************************************************************************
  Function:
    void APP_CONSOLE_Tasks ( void )

  Remarks:
    See prototype in app_console.h.
 */

void APP_CONSOLE_Tasks ( void )
{
    char regName[4][18];
    uint32_t regValue32[4];
    uint64_t regValue64[4];
    uint8_t numRegsPending;

    /* Check the application's current state. */
    switch ( app_consoleData.state )
    {
        /* Application's initial state. */
        case APP_CONSOLE_STATE_IDLE:
        case APP_CONSOLE_STATE_WAIT_DATA:
        {
            break;
        }

        case APP_CONSOLE_STATE_PROMPT:
        {
            SYS_CMD_MESSAGE(">");
            app_consoleData.state = APP_CONSOLE_STATE_IDLE;
            break;
        }

        case APP_CONSOLE_STATE_INIT:
        {
            if (SYS_CMD_READY_TO_READ())
            {
                /* Initialize Energy App callbacks */
                APP_ENERGY_SetMonthEnergyCallback(_monthlyEnergyCallback, &energyData);
                APP_ENERGY_SetMaxDemandCallback(_maxDemandCallback, &maxDemandLocalObject);

                /* Initialize Metrology App callbacks */
                APP_METROLOGY_SetHarmonicAnalysisCallback(_harmonicAnalysisCallback, &harmonicAnalysisRMSData[0]);
                APP_METROLOGY_SetCalibrationCallback(_calibrationCallback);

                app_consoleData.currentWaitForDatalogReady = 0;
                app_consoleData.state = APP_CONSOLE_STATE_WAIT_STORAGE_READY;

                // Set default console storage data just in case it cannot be read later
                app_consoleStorageData = app_defaultConsoleStorageData;
            }
            break;
        }

        case APP_CONSOLE_STATE_WAIT_STORAGE_READY:
        {
            if (APP_DATALOG_GetStatus() == APP_DATALOG_STATE_READY)
            {
               app_consoleData.state = APP_CONSOLE_STATE_READ_STORAGE;
            }
            break;
        }

        case APP_CONSOLE_STATE_READ_STORAGE:
        {
            // Build queue element
            datalogQueueElement.userId = APP_DATALOG_USER_CONSOLE;
            datalogQueueElement.operation = APP_DATALOG_READ;
            datalogQueueElement.endCallback = _consoleReadStorage;
            datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
            datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
            datalogQueueElement.dataLen = sizeof(app_consoleStorageData);
            datalogQueueElement.pData = (uint8_t*)&app_consoleStorageData;
            // Put it in queue
            APP_DATALOG_SendDatalogData(&datalogQueueElement);

            // Wait for data to be read (semaphore is released in callback)
            app_consoleData.state = APP_CONSOLE_STATE_WAIT_DATA;
            // Data read, depending on read result, state has changed to READ_OK or READ_ERROR
            break;
        }

        case APP_CONSOLE_STATE_READ_STORAGE_ERROR:
        {
            // Build queue element to write it to storage
            datalogQueueElement.userId = APP_DATALOG_USER_CONSOLE;
            datalogQueueElement.operation = APP_DATALOG_WRITE;
            datalogQueueElement.endCallback = NULL;
            datalogQueueElement.date.year = APP_DATALOG_INVALID_YEAR; /* Not used */
            datalogQueueElement.date.month = APP_DATALOG_INVALID_MONTH; /* Not used */
            datalogQueueElement.dataLen = sizeof(app_consoleStorageData);
            datalogQueueElement.pData = (uint8_t*)&app_consoleStorageData;
            // Put it in queue
            APP_DATALOG_SendDatalogData(&datalogQueueElement);
            // Go to Idle state
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_DATALOG_NOT_READY:
        {
            SYS_CMD_MESSAGE("Datalog Service not ready!\r\nApplication will run without storage capabilities\r\n");
            // Go to Idle state
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_CONTROL_REG:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read register value
            if (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0]))
            {
                SYS_CMD_PRINT("%s\r\n%X\r\n", regName[0], regValue32[0]);
            }
            else
            {
                // Cannot read register
                SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
            }
            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_WRITE_CONTROL_REG:
        {
            uint8_t idx;

            // Remove Prompt symbol
            _removePrompt();

            for (idx = 0; idx < APP_CONSOLE_MAX_REGS; idx++)
            {
                if (app_consoleData.regsToModify[idx].index != 0xFF)
                {
                    if (app_consoleData.regsToModify[idx].index < CONTROL_REG_NUM)
                    {
                        // Write register value
                        if (APP_METROLOGY_SetControlRegister((CONTROL_REG_ID)app_consoleData.regsToModify[idx].index,
                                app_consoleData.regsToModify[idx].value))
                        {
                            // Show response on console
                            SYS_CMD_PRINT("Set %02d Is Ok !\r\n", app_consoleData.regsToModify[idx].index);
                        }
                        else
                        {
                            // Cannot write register
                            SYS_CMD_PRINT("Could not write register %02d\r\n", app_consoleData.regsToModify[idx].index);
                        }
                    }
                    else
                    {
                        // Invalid index
                        SYS_CMD_PRINT("Invalid register index %02d\r\n", app_consoleData.regsToModify[idx].index);
                    }
                }
                else
                {
                    // All registers have been written
                    app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
                    break;
                }

                if ((idx % 10) == 0)
                {
                    app_consoleData.nextState = app_consoleData.state;
                    app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                    app_consoleData.delayMs = CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES;
                }
            }
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_CONTROL_REGS:
        {
            if (app_consoleData.ctrlRegToRead < CONTROL_REG_NUM)
            {
                if (app_consoleData.ctrlRegToRead == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Check how many registers are pending to print, to format line
                numRegsPending = CONTROL_REG_NUM - app_consoleData.ctrlRegToRead;
                // Read and print register values
                if (numRegsPending >= 4)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 2, &regValue32[2], regName[2])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 3, &regValue32[3], regName[3])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2], regName[3]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 4;
                }
                else if (numRegsPending == 3)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 2, &regValue32[2], regName[2])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 3;
                }
                else if (numRegsPending == 2)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead + 1, &regValue32[1], regName[1])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s\r\n", regName[0], regName[1]);
                        SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 2;
                }
                else if (numRegsPending == 1)
                {
                    if ((APP_METROLOGY_GetControlRegister((CONTROL_REG_ID)app_consoleData.ctrlRegToRead, &regValue32[0], regName[0])))
                    {
                        SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                        SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.ctrlRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.ctrlRegToRead += 1;
                }
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            break;
        }

        case APP_CONSOLE_STATE_READ_ACCUM_REG:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read register value
            if (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead, &regValue64[0], regName[0]))
            {
                SYS_CMD_PRINT("%s\r\n%llX\r\n", regName[0], regValue64[0]);
            }
            else
            {
                // Cannot read register
                SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.accumRegToRead);
            }
            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_ACCUM_REGS:
        {
            if (app_consoleData.accumRegToRead < ACCUMULATOR_REG_NUM)
            {
                if (app_consoleData.accumRegToRead == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Check how many registers are pending to print, to format line
                numRegsPending = ACCUMULATOR_REG_NUM - app_consoleData.accumRegToRead;
                // Read and print register values
                if (numRegsPending >= 4)
                {
                    if ((APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead, &regValue64[0], regName[0])) &&
                        (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead + 1, &regValue64[1], regName[1])) &&
                        (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead + 2, &regValue64[2], regName[2])) &&
                        (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead + 3, &regValue64[3], regName[3])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2], regName[3]);
                        SYS_CMD_PRINT("%-19llX%-19llX%-19llX%-19llX\r\n", regValue64[0], regValue64[1], regValue64[2], regValue64[3]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.accumRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 4;
                }
                else if (numRegsPending == 3)
                {
                    if ((APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead, &regValue64[0], regName[0])) &&
                        (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead + 1, &regValue64[1], regName[1])) &&
                        (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead + 2, &regValue64[2], regName[2])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2]);
                        SYS_CMD_PRINT("%-19llX%-19llX%-19llX\r\n", regValue64[0], regValue64[1], regValue64[2]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.accumRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 3;
                }
                else if (numRegsPending == 2)
                {
                    if ((APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead, &regValue64[0], regName[0])) &&
                        (APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead + 1, &regValue64[1], regName[1])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s\r\n", regName[0], regName[1]);
                        SYS_CMD_PRINT("%-19llX%-19llX\r\n", regValue64[0], regValue64[1]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.accumRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 2;
                }
                else if (numRegsPending == 1)
                {
                    if ((APP_METROLOGY_GetAccumulatorRegister((ACCUMULATOR_REG_ID)app_consoleData.accumRegToRead, &regValue64[0], regName[0]))) {
                        SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                        SYS_CMD_PRINT("%-19llX\r\n", regValue64[0]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.accumRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.accumRegToRead += 1;
                }
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            break;
        }

        case APP_CONSOLE_STATE_READ_STATUS_REG:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read register value
            if (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0]))
            {
                SYS_CMD_PRINT("%s\r\n%X\r\n", regName[0], regValue32[0]);
            }
            else
            {
                // Cannot read register
                SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
            }
            // Go back to Idle
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_STATUS_REGS:
        {
            if (app_consoleData.statusRegToRead < STATUS_REG_NUM)
            {
                if (app_consoleData.statusRegToRead == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Check how many registers are pending to print, to format line
                numRegsPending = STATUS_REG_NUM - app_consoleData.statusRegToRead;
                // Read and print register values
                if (numRegsPending >= 4)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 2, &regValue32[2], regName[2])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 3, &regValue32[3], regName[3])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2], regName[3]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);
                    }
                    else {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 4;
                }
                else if (numRegsPending == 3)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 1, &regValue32[1], regName[1])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 2, &regValue32[2], regName[2])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2]);
                        SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 3;
                }
                else if (numRegsPending == 2)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])) &&
                        (APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead + 1, &regValue32[1], regName[1])))
                    {
                        SYS_CMD_PRINT("%-19s%-19s\r\n", regName[0], regName[1]);
                        SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 2;
                }
                else if (numRegsPending == 1)
                {
                    if ((APP_METROLOGY_GetStatusRegister((STATUS_REG_ID)app_consoleData.statusRegToRead, &regValue32[0], regName[0])))
                    {
                        SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                        SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);
                    }
                    else
                    {
                        // Cannot read register
                        SYS_CMD_PRINT("Could not read register %02d\r\n", app_consoleData.statusRegToRead);
                    }
                    // Advance to next register group
                    app_consoleData.statusRegToRead += 1;
                }
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            break;
        }

        case APP_CONSOLE_STATE_READ_HARMONIC_REGS:
        {
            uint8_t numRegId;
            uint8_t harmonicNum;
            
            if (app_consoleData.numRegsPending == HARMONICS_REG_NUM)
            {
                // Remove Prompt symbol
                _removePrompt();
            }
            
            numRegId = HARMONICS_REG_NUM - app_consoleData.numRegsPending;
            harmonicNum = app_consoleData.harNumToRead;
            
            // Read and print register values
            if (app_consoleData.numRegsPending >= 4)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 1, harmonicNum, &regValue32[1], regName[1]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 2, harmonicNum, &regValue32[2], regName[2]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 3, harmonicNum, &regValue32[3], regName[3]);
                SYS_CMD_PRINT("%-19s%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2], regName[3]);
                SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);
                
                // Advance to next register group
                app_consoleData.numRegsPending -= 4;
            }
            else if (app_consoleData.numRegsPending == 3)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 1, harmonicNum, &regValue32[1], regName[1]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 2, harmonicNum, &regValue32[2], regName[2]);
                SYS_CMD_PRINT("%-19s%-19s%-19s\r\n", regName[0], regName[1], regName[2]);
                SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);
                
                // Advance to next register group
                app_consoleData.numRegsPending -= 3;
            }
            else if (app_consoleData.numRegsPending == 2)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                APP_METROLOGY_GetHarmonicRegister(numRegId + 1, harmonicNum, &regValue32[1], regName[1]);
                SYS_CMD_PRINT("%-19s%-19s\r\n", regName[0], regName[1]);
                SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);
                
                // Advance to next register group
                app_consoleData.numRegsPending -= 2;
            }
            else if (app_consoleData.numRegsPending == 1)
            {
                APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);
                
                // Advance to next register group
                app_consoleData.numRegsPending -= 1;
            }
            else
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }

            if (app_consoleData.state != APP_CONSOLE_STATE_PROMPT)
            {
                app_consoleData.nextState = app_consoleData.state;
                app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            }
            break;
        }

        case APP_CONSOLE_STATE_READ_ALL_HARMONICS_REGS:
        {
            if (app_consoleData.numRegsPending == HARMONICS_REG_NUM)
            {
                // Remove Prompt symbol
                _removePrompt();
            }
            
            // Read and print register values
            if (app_consoleData.numRegsPending == 0)
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }
            else
            {
                uint8_t numRegId;
                uint8_t harmonicNum;

                numRegId = HARMONICS_REG_NUM - app_consoleData.numRegsPending;
                numRegId = harRegsDisplayOrder[numRegId];
                harmonicNum = DRV_METROLOGY_HARMONICS_MAX_ORDER - app_consoleData.numHarmsPending;

                // Read and print harmonic values
                if (app_consoleData.numHarmsPending >= 4)
                {
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 1, &regValue32[1], regName[1]);
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 2, &regValue32[2], regName[2]);
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 3, &regValue32[3], regName[3]);
                    
                    if (app_consoleData.numHarmsPending == DRV_METROLOGY_HARMONICS_MAX_ORDER)
                    {
                        SYS_CMD_PRINT("%-19s\r\n", regName[0]);
                    }
                    
                    SYS_CMD_PRINT("%-19X%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2], regValue32[3]);

                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending -= 4;
                }
                else if (app_consoleData.numHarmsPending == 3)
                {
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 1, &regValue32[1], regName[1]);
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 2, &regValue32[2], regName[2]);
                    
                    SYS_CMD_PRINT("%-19X%-19X%-19X\r\n", regValue32[0], regValue32[1], regValue32[2]);

                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending -= 3;
                }
                else if (app_consoleData.numHarmsPending == 2)
                {
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum + 1, &regValue32[1], regName[1]);
                    
                    SYS_CMD_PRINT("%-19X%-19X\r\n", regValue32[0], regValue32[1]);

                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending -= 2;
                }
                else if (app_consoleData.numHarmsPending == 1)
                {
                    APP_METROLOGY_GetHarmonicRegister(numRegId, harmonicNum, &regValue32[0], regName[0]);
                    
                    SYS_CMD_PRINT("%-19X\r\n", regValue32[0]);

                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending--;
                }
                else
                {
                    // Advance to next register group
                    app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
                    app_consoleData.numRegsPending--;
                }

                app_consoleData.nextState = app_consoleData.state;
                app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            }

            break;
        }

        case APP_CONSOLE_STATE_READ_METER_ID:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Show response on console
            SYS_CMD_MESSAGE("Meter ID is:\r\n");
            SYS_CMD_PRINT("%s\r\n", app_consoleStorageData.meterID);
            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_RTC:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Read and print RTC
            RTC_TimeGet(&app_consoleData.sysTime);
            SYS_CMD_MESSAGE("Present RTC is(yy-mm-dd w hh:mm:ss):\r\n");
            SYS_CMD_PRINT("%02u-%02u-%02u %u %02u:%02u:%02u\r\n",
                    app_consoleData.sysTime.tm_year + 1900 - 2000, app_consoleData.sysTime.tm_mon + 1, app_consoleData.sysTime.tm_mday,
                    app_consoleData.sysTime.tm_wday + 1, app_consoleData.sysTime.tm_hour, app_consoleData.sysTime.tm_min, app_consoleData.sysTime.tm_sec);
            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_READ_TOU:
        {
            APP_ENERGY_TOU_TIME_ZONE * timeZone;
            uint8_t idx;

            timeZone = APP_ENERGY_GetTOUTimeZone();

            // Remove Prompt symbol
            _removePrompt();

            SYS_CMD_MESSAGE("TOU table is:\r\n");
            for (idx = 0; idx < APP_ENERGY_TOU_MAX_ZONES; idx++, timeZone++)
            {
                if (timeZone->tariff != TARIFF_INVALID)
                {
                    SYS_CMD_PRINT("TOU%d=%02d:%02d T%d ",
                        (idx + 1), timeZone->hour, timeZone->minute, timeZone->tariff);
                }
            }
            SYS_CMD_MESSAGE("\r\n");

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_HARMONIC_ANALYSIS:
        {
            DRV_METROLOGY_HARMONICS_RMS *pHarmonicRMSData;
            uint8_t harmonicNum;
            
            harmonicNum = app_consoleData.harmonicNumRequest;
            pHarmonicRMSData = &harmonicAnalysisRMSData[harmonicNum - 1];
            
            if (app_consoleData.harmonicNumPrint == 1)
            {
                if (app_consoleData.harmonicNumRequest == 0)
                {
                    // Remove Prompt symbol
                    _removePrompt();
                }

                // Show received data on console
                SYS_CMD_PRINT("The calculated harmonic %u Irms/Vrms:\r\n", harmonicNum);

                SYS_CMD_MESSAGE("Irms_Har_A(A)      Irms_Har_B(A)      Irms_Har_C(A)\r\n");
                SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f\r\n", pHarmonicRMSData->Irms_A_m, 
                        pHarmonicRMSData->Irms_B_m, pHarmonicRMSData->Irms_C_m);
            }
            else if (app_consoleData.harmonicNumPrint == 2)
            {
                SYS_CMD_MESSAGE("Irms_Har_N(A)      Vrms_Har_A(V)      Vrms_Har_B(V)\r\n");
                SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f\r\n", pHarmonicRMSData->Irms_N_m,
                        pHarmonicRMSData->Vrms_A_m, pHarmonicRMSData->Vrms_B_m);
            }
            else if (app_consoleData.harmonicNumPrint == 3)
            {
                SYS_CMD_MESSAGE("Vrms_Har_C(V)\r\n");
                SYS_CMD_PRINT("%-19.3f\r\n", pHarmonicRMSData->Vrms_C_m);

                // Go back to IDLE
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }
            
            if (app_consoleData.state != APP_CONSOLE_STATE_PROMPT)
            {
                app_consoleData.harmonicNumPrint++;
                app_consoleData.nextState = app_consoleData.state;
                app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            }
            break;
        }

        case APP_CONSOLE_STATE_PRINT_ALL_HARMONIC_ANALYSIS:
        {
            if (app_consoleData.numRegsPending == APP_CONSOLE_HAR_DESC_SIZE)
            {
                // Remove Prompt symbol
                _removePrompt();
            }
            
            // Read and print register values
            if (app_consoleData.numRegsPending == 0)
            {
                // All registers have been read
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            }
            else
            {
                double rmsValues[4];
                uint8_t numRegId;
                uint8_t harmonicNum;

                numRegId = APP_CONSOLE_HAR_DESC_SIZE - app_consoleData.numRegsPending;
                harmonicNum = DRV_METROLOGY_HARMONICS_MAX_ORDER - app_consoleData.numHarmsPending;
                
                // Read and print harmonic values
                if (app_consoleData.numHarmsPending >= 4)
                {
                    if (app_consoleData.numHarmsPending == DRV_METROLOGY_HARMONICS_MAX_ORDER)
                    {
                        SYS_CMD_PRINT("%s\r\n", _console_har_desc[numRegId]);
                    }
                    
                    rmsValues[0] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    rmsValues[1] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    rmsValues[2] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    rmsValues[3] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    
                    SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f%-19.3f\r\n", 
                            rmsValues[0], rmsValues[1], rmsValues[2], rmsValues[3]); 
                    
                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending -= 4;
                }
                else if (app_consoleData.numHarmsPending == 3)
                {
                    rmsValues[0] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    rmsValues[1] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    rmsValues[2] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    
                    SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f\r\n", 
                            rmsValues[0], rmsValues[1], rmsValues[2]); 
                    
                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending -= 3;
                }
                else if (app_consoleData.numHarmsPending == 2)
                {
                    rmsValues[0] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    rmsValues[1] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    
                    SYS_CMD_PRINT("%-19.3f%-19.3f\r\n", rmsValues[0], rmsValues[1]); 
                    
                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending -= 2;
                }
                else if (app_consoleData.numHarmsPending == 1)
                {
                    rmsValues[0] = *((double *)&harmonicAnalysisRMSData[harmonicNum++] + numRegId);
                    
                    SYS_CMD_PRINT("%-19.3f\r\n", rmsValues[0]); 
                    
                    // Advance to next harmonic group
                    app_consoleData.numHarmsPending -= 1;
                }
                else
                {
                    // Advance to next register group
                    app_consoleData.numHarmsPending = DRV_METROLOGY_HARMONICS_MAX_ORDER;
                    app_consoleData.numRegsPending--;
                }

                app_consoleData.nextState = app_consoleData.state;
                app_consoleData.state = APP_CONSOLE_STATE_DELAY;
                app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
            }

            break;
        }
//        {
//            if (app_consoleData.harmonicNumRequest == DRV_METROLOGY_HARMONICS_MAX_ORDER)
//            {
//                // All harmonics have been read
//                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
//            }
//            else
//            {
//                DRV_METROLOGY_HARMONICS_RMS *pHarmonicRMSData;
//                uint8_t harmonicNum;
//            
//                harmonicNum = app_consoleData.harmonicNumRequest;
//                pHarmonicRMSData = &harmonicAnalysisRMSData[harmonicNum];
//
//                if (app_consoleData.harmonicNumPrint == 1)
//                {
//                    // Show received data on console
//                    SYS_CMD_PRINT("The calculated harmonic %u Irms/Vrms:\r\n", harmonicNum + 1);
//
//                    SYS_CMD_MESSAGE("Irms_Har_A(A)      Irms_Har_B(A)      Irms_Har_C(A)\r\n");
//                    SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f\r\n", pHarmonicRMSData->Irms_A_m, 
//                            pHarmonicRMSData->Irms_B_m, pHarmonicRMSData->Irms_C_m);
//                }
//                else if (app_consoleData.harmonicNumPrint == 2)
//                {
//                    SYS_CMD_MESSAGE("Irms_Har_N(A)      Vrms_Har_A(V)      Vrms_Har_B(V)\r\n");
//                    SYS_CMD_PRINT("%-19.3f%-19.3f%-19.3f\r\n", pHarmonicRMSData->Irms_N_m,
//                            pHarmonicRMSData->Vrms_A_m, pHarmonicRMSData->Vrms_B_m);
//                }
//                else if (app_consoleData.harmonicNumPrint == 3)
//                {
//                    SYS_CMD_MESSAGE("Vrms_Har_C(V)\r\n");
//                    SYS_CMD_PRINT("%-19.3f\r\n", pHarmonicRMSData->Vrms_C_m);
//                    
//                    app_consoleData.harmonicNumPrint = 0;
//                    app_consoleData.harmonicNumRequest++;
//                }
//
//                if (app_consoleData.state != APP_CONSOLE_STATE_PROMPT)
//                {
//                    app_consoleData.harmonicNumPrint++;
//                    app_consoleData.nextState = app_consoleData.state;
//                    app_consoleData.state = APP_CONSOLE_STATE_DELAY;
//                    app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;
//                }
//            }
//            break;
//        }

        case APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY:
        {
            uint64_t total = 0;
            int8_t idx;

            for (idx = 0; idx < TARIFF_NUM_TYPE; idx ++)
            {
                total += energyData.tariff[idx];
            }

            // Remove Prompt symbol
            _removePrompt();

            // Show received data on console
            idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);
            SYS_CMD_PRINT("Last %d Month Energy is:\r\n", idx);

            SYS_CMD_PRINT("TT=%.2fkWh T1=%.2fkWh T2=%.2fkWh T3=%.2fkWh T4=%.2fkWh\r\n",
                (float)total/10000000, (float)energyData.tariff[0]/10000000, (float)energyData.tariff[1]/10000000,
                    (float)energyData.tariff[2]/10000000, (float)energyData.tariff[3]/10000000);

            /* Introduce a delay to wait console visualization */
            app_consoleData.nextState = APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY_NEXT;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;

            break;
        }

        case APP_CONSOLE_STATE_PRINT_MONTHLY_ENERGY_NEXT:
        {
            // Check pending monthly requests
            app_consoleData.requestCounter--;
            if (app_consoleData.requestCounter > 0)
            {
                int8_t idx;

                idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);

                _getSysTimeFromMonthIndex(&app_consoleData.timeRequest, idx + 1);
                APP_ENERGY_GetMonthEnergy(&app_consoleData.timeRequest);
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_EVENT:
        {
            APP_EVENTS_EVENT_INFO eventInfo;
            uint8_t numEvents;
            struct tm invalidTime = {0};

            // Remove Prompt symbol
            _removePrompt();

            APP_EVENTS_GetNumEvents(app_consoleData.eventIdRequest, &numEvents);
            if (APP_EVENTS_GetEventInfo(app_consoleData.eventIdRequest, app_consoleData.eventLastTimeRequest, &eventInfo))
            {
                // Print Event ID and requested Times
                if (app_consoleData.eventIdRequest == SAG_UA_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Ua Sag is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == SAG_UB_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Ub Sag is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == SAG_UC_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Uc Sag is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == POW_UA_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Pa reverse is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == POW_UB_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Pb reverse is:\r\n", app_consoleData.eventLastTimeRequest);
                }
                else if (app_consoleData.eventIdRequest == POW_UC_EVENT_ID)
                {
                    SYS_CMD_PRINT("Last %d Times Pc reverse is:\r\n", app_consoleData.eventLastTimeRequest);
                }

                // Show received data on console
                SYS_CMD_PRINT("Total Num=%d ", numEvents);

                if (memcmp(&eventInfo.startTime, &invalidTime, sizeof(struct tm)) == 0)
                {
                    SYS_CMD_PRINT("start time is invalid ");
                }
                else
                {
                    SYS_CMD_PRINT("start time is %02d-%02d %02d:%02d ",
                    eventInfo.startTime.tm_mon + 1,
                    eventInfo.startTime.tm_mday,
                    eventInfo.startTime.tm_hour,
                    eventInfo.startTime.tm_min);
                }

                if (memcmp(&eventInfo.endTime, &invalidTime, sizeof(struct tm)) == 0)
                {
                    SYS_CMD_PRINT("end time is invalid\r\n");
                }
                else
                {
                    SYS_CMD_PRINT("end time is %02d-%02d %02d:%02d\r\n",
                    eventInfo.endTime.tm_mon + 1,
                    eventInfo.endTime.tm_mday,
                    eventInfo.endTime.tm_hour,
                    eventInfo.endTime.tm_min);
                }
            }
            else
            {
                SYS_CMD_MESSAGE("Maximum number of reported events exceeded: 10\r\n");
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_MAX_DEMAND:
        {
            APP_ENERGY_DEMAND_DATA *pDataMax;
            APP_ENERGY_DEMAND_DATA *pDataTariff;
            int8_t idx;

            // Remove Prompt symbol
            _removePrompt();

            // Show received data on console
            idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);
            SYS_CMD_PRINT("Last %d Month MaxDemand is:\r\n", idx);

            pDataMax = &maxDemandLocalObject.maxDemad;
            pDataTariff = &maxDemandLocalObject.tariff[0];
            SYS_CMD_PRINT("TT=%.3fkW %d-%d %02d:%02d T1=%.3fkW %d-%d %02d:%02d T2=%.3fkW %d-%d %02d:%02d T3=%.3fkW %d-%d %02d:%02d T4=%.3fkW %d-%d %02d:%02d\r\n",
                    (float)pDataMax->value/1000, pDataMax->month + 1, pDataMax->day, pDataMax->hour, pDataMax->minute,
                    (float)pDataTariff->value/1000, pDataTariff->month + 1, pDataTariff->day, pDataTariff->hour, pDataTariff->minute,
                    (float)(pDataTariff + 1)->value/1000, (pDataTariff + 1)->month + 1, (pDataTariff + 1)->day, (pDataTariff + 1)->hour, (pDataTariff + 1)->minute,
                    (float)(pDataTariff + 2)->value/1000, (pDataTariff + 2)->month + 1, (pDataTariff + 2)->day, (pDataTariff + 2)->hour, (pDataTariff + 2)->minute,
                    (float)(pDataTariff + 3)->value/1000, (pDataTariff + 3)->month + 1, (pDataTariff + 3)->day, (pDataTariff + 3)->hour, (pDataTariff + 3)->minute);

            /* Introduce a delay to wait console visualization */
            app_consoleData.nextState = APP_CONSOLE_STATE_PRINT_MAX_DEMAND_NEXT;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DELAY_MS_BETWEEN_REGS_PRINT;

            break;
        }

        case APP_CONSOLE_STATE_PRINT_MAX_DEMAND_NEXT:
        {
            // Check pending monthly requests
            app_consoleData.requestCounter--;
            if (app_consoleData.requestCounter > 0)
            {
                int8_t idx;

                idx = _getMonthIndexFromSysTime(&app_consoleData.timeRequest);

                _getSysTimeFromMonthIndex(&app_consoleData.timeRequest, idx + 1);
                APP_ENERGY_GetMonthMaxDemand(&app_consoleData.timeRequest);
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;

            break;
        }

        case APP_CONSOLE_STATE_PRINT_VOLTAGE:
        {
            uint32_t va, vb, vc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetRMS(RMS_UA, &va, 0);
            APP_METROLOGY_GetRMS(RMS_UB, &vb, 0);
            APP_METROLOGY_GetRMS(RMS_UC, &vc, 0);
            // Show received data on console
            SYS_CMD_PRINT("Present voltage is :\r\nUa=%.3fV Ub=%.3fV Uc=%.3fV\r\n",(float)va/10000, (float)vb/10000, (float)vc/10000);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_CURRENT:
        {
            uint32_t ia, ib, ic;
            uint32_t ini, inm, inmi;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetRMS(RMS_IA, &ia, 0);
            APP_METROLOGY_GetRMS(RMS_IB, &ib, 0);
            APP_METROLOGY_GetRMS(RMS_IC, &ic, 0);
            APP_METROLOGY_GetRMS(RMS_INI, &ini, 0);
            APP_METROLOGY_GetRMS(RMS_INM, &inm, 0);
            APP_METROLOGY_GetRMS(RMS_INMI, &inmi, 0);
            // Show received data on console
            SYS_CMD_PRINT("Present current is :\r\nIa=%.4fA Ib=%.4fA Ic=%.4fA Ini=%.4fA Inm=%.4fA Inmi=%.4fA\r\n",
                    (float)ia/10000, (float)ib/10000, (float)ic/10000, (float)ini/10000,
                    (float)inm/10000, (float)inmi/10000);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_ACTIVE_POWER:
        {
            uint32_t pt, pa, pb, pc;
            DRV_METROLOGY_RMS_SIGN signt, signa, signb, signc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetRMS(RMS_PT, &pt, &signt);
            APP_METROLOGY_GetRMS(RMS_PA, &pa, &signa);
            APP_METROLOGY_GetRMS(RMS_PB, &pb, &signb);
            APP_METROLOGY_GetRMS(RMS_PC, &pc, &signc);
            // Show received data on console
            SYS_CMD_PRINT("Present active power is :\r\nPt=%c%.1fW Pa=%c%.1fW Pb=%c%.1fW Pc=%c%.1fW\r\n",
                   sign[signt], (float)pt/10, sign[signa], (float)pa/10, sign[signb],
                    (float)pb/10, sign[signc], (float)pc/10);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_REACTIVE_POWER:
        {
            uint32_t qt, qa, qb, qc;
            DRV_METROLOGY_RMS_SIGN signt, signa, signb, signc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetRMS(RMS_QT, &qt, &signt);
            APP_METROLOGY_GetRMS(RMS_QA, &qa, &signa);
            APP_METROLOGY_GetRMS(RMS_QB, &qb, &signb);
            APP_METROLOGY_GetRMS(RMS_QC, &qc, &signc);
            // Show received data on console
            SYS_CMD_PRINT("Present reactive power is :\r\nQt=%c%.1fW Qa=%c%.1fW Qb=%c%.1fW Qc=%c%.1fW\r\n",
                   sign[signt], (float)qt/10, sign[signa], (float)qa/10, sign[signb],
                    (float)qb/10, sign[signc], (float)qc/10);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_APARENT_POWER:
        {
            uint32_t st, sa, sb, sc;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetRMS(RMS_ST, &st, 0);
            APP_METROLOGY_GetRMS(RMS_SA, &sa, 0);
            APP_METROLOGY_GetRMS(RMS_SB, &sb, 0);
            APP_METROLOGY_GetRMS(RMS_SC, &sc, 0);
            // Show received data on console
            SYS_CMD_PRINT("Present apparent power is :\r\nSt=%.1fVA Sa=%.1fVA Sb=%.1fVA Sc=%.1fVA\r\n",
                   (float)st/10, (float)sa/10, (float)sb/10, (float)sc/10);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_FREQUENCY:
        {
            uint32_t freq;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetRMS(RMS_FREQ, &freq, 0);
            // Show received data on console
            SYS_CMD_PRINT("Present frequency is : \r\nFreq=%.2fHz\r\n", (float)freq/100);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_ANGLE:
        {
            uint32_t aa, ab, ac, an;
            DRV_METROLOGY_RMS_SIGN signa, signb, signc, signn;

            // Remove Prompt symbol
            _removePrompt();

            APP_METROLOGY_GetRMS(RMS_ANGLEA, &aa, &signa);
            APP_METROLOGY_GetRMS(RMS_ANGLEB, &ab, &signb);
            APP_METROLOGY_GetRMS(RMS_ANGLEC, &ac, &signc);
            APP_METROLOGY_GetRMS(RMS_ANGLEN, &an, &signn);
            // Show received data on console
            SYS_CMD_PRINT("Voltage and current angle is : \r\nAngle_A=%c%.3f Angle_B=%c%.3f Angle_C=%c%.3f Angle_N=%c%.3f\r\n",
                    sign[signa], (float)aa/100000, sign[signb], (float)ab/100000,
                    sign[signc], (float)ac/100000, sign[signn], (float)an/100000);

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_WAVEFORM_DATA:
        {
            uint8_t idx;

            if (app_consoleData.rawDataFlag)
            {
                _removePrompt();
                app_consoleData.rawDataFlag = false;
            }

            for (idx = 0; idx < 10; idx++) {
                if (app_consoleData.rawDataLen > 0)
                {
                    // Print value
                    SYS_CMD_PRINT("%08X\r\n", *(app_consoleData.rawData));
                    // Advance to next value
                    app_consoleData.rawData++;
                    app_consoleData.rawDataLen--;
                }
                else
                {
                    // All registers have been read
                    app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
                    break;
                }
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_CALIBRATION_RESULT:
        {
            // Remove Prompt symbol
            _removePrompt();

            // Show calibration result
            if (app_consoleData.calibrationResult)
            {
                SYS_CMD_MESSAGE("Calibrating Done!\r\n");
            }
            else
            {
                SYS_CMD_MESSAGE("Calibrating Fails!\r\n");
            }

            // Go back to IDLE
            app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
            break;
        }

        case APP_CONSOLE_STATE_PRINT_HELP:
        {
            uint8_t idx;
            uint8_t idxMax = 2;

            if (app_consoleData.cmdNumToShowHelp > 0)
            {
                if (app_consoleData.cmdNumToShowHelp < 5)
                {
                    idxMax = app_consoleData.cmdNumToShowHelp;
                }

                for (idx = 0; idx < idxMax; idx++, app_consoleData.pCmdDescToShowHelp++)
                {
                    SYS_CMD_PRINT("%s\t%s\r\n", app_consoleData.pCmdDescToShowHelp->cmdStr,
                            app_consoleData.pCmdDescToShowHelp->cmdDescr);
                    app_consoleData.cmdNumToShowHelp--;
                }
            }
            else
            {
                // All commands have been represented
                app_consoleData.state = APP_CONSOLE_STATE_PROMPT;
                break;
            }

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = CONSOLE_TASK_DEFAULT_DELAY_MS_BETWEEN_STATES;
            break;
        }

        case APP_CONSOLE_STATE_LOW_POWER_MODE:
        {
            // Remove Prompt symbol
            _removePrompt();

            SYS_CMD_MESSAGE("Entering Low Power... Press FWUP/TAMPER switch to wake up.\r\n");

            // Update display info
            APP_DISPLAY_ShowLowPowerMode();

            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = 100;
            break;
        }

        case APP_CONSOLE_STATE_SW_RESET:
        {
            /* Wait time to show message through the Console */
            app_consoleData.nextState = app_consoleData.state;
            app_consoleData.state = APP_CONSOLE_STATE_DELAY;
            app_consoleData.delayMs = 100;
            break;
        }

        case APP_CONSOLE_STATE_DELAY:
        {
            // Wait delay time
            if (APP_CONSOLE_TaskDelay(app_consoleData.delayMs, &app_consoleData.timer))
            {
                // Check low power state
                if (app_consoleData.nextState == APP_CONSOLE_STATE_LOW_POWER_MODE)
                {
                    // Go to Low Power mode
                    APP_METROLOGY_SetLowPowerMode();

                    // Execution should not come here during normal operation
                }
                else if (app_consoleData.nextState == APP_CONSOLE_STATE_SW_RESET)
                {
                    // Perform Reset
                    RSTC_Reset(RSTC_PROCESSOR_RESET);

                    // Execution should not come here during normal operation
                }
                else
                {
                    // Set next app state
                    app_consoleData.state = app_consoleData.nextState;
                }
            }
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
