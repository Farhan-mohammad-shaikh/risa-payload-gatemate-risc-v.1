/*
 * Copyright (c) 2017, Muhammad Bassam
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "rmap_status.h"

void
outpost::comm::RmapReplyStatus::replyStatus(uint8_t status)
{
    switch (status)
    {
        case 0x00: console_out("Successfully Executed (0x00)\n"); break;
        case 0x01: console_out("General Error (0x01)\n"); break;
        case 0x02: console_out("Unused RMAP Packet Type or Command Code (0x02)\n"); break;
        case 0x03: console_out("Invalid Target Key (0x03)\n"); break;
        case 0x04: console_out("Invalid Data CRC (0x04)\n"); break;
        case 0x05: console_out("Early EOP (0x05)\n"); break;
        case 0x06: console_out("Cargo Too Large (0x06)\n"); break;
        case 0x07: console_out("EEP (0x07)\n"); break;
        case 0x08: console_out("Reserved (0x08)\n"); break;
        case 0x09: console_out("Verify Buffer Overrun (0x09)\n"); break;
        case 0x0a: console_out("RMAP Command Not Implemented or Not Authorized (0x0a)\n"); break;
        case 0x0b: console_out("Invalid Target Logical Address (0x0b)\n"); break;
        default: console_out("Reserved (0x%02X\n", status); break;
    }
}
