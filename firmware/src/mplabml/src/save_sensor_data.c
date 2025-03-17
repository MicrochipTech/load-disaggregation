
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/

#include "rb.h"
#include "kbutils.h"
/*!
 * \brief Save raw sensor data into RAW_DATA_BUFFER in s16 format.
 *
 * \param rawdata Raw sensor data array pointer.
 * \param count Count of data values to save into the RAW_DATA_BUFFER array.
 */
void saveSensorData(ringb *pringb, int16_t *rawdata, int32_t count)
{
    int32_t i;

    for (i = 0; i < count; i++)
    {
        rb_add(pringb + i, *rawdata++);
    }

    return;
}

/*!
 * \brief Save raw sensor data into RAW_DATA_BUFFER in s16 format.
 *
 * \param rawdata Mixed size sensor data array.
 * \param row startcol in rows to the frame to update.
 * \param count Count of data values to save into the RAW_DATA_BUFFER array.
 */
void saveSenseDataOffset(ringb *pringb, int16_t *rawdata, int32_t startcol, int32_t count)
{
    int32_t icol;

    for (icol = startcol; icol < (count + startcol); icol++)
    {
        rb_add(pringb + icol, *rawdata++);
    }
    return;
}
