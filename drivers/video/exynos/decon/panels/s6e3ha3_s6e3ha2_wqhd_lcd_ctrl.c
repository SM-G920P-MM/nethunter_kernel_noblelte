/*
 * drivers/video/decon/panels/S6E3HA0_lcd_ctrl.c
 *
 * Samsung SoC MIPI LCD CONTROL functions
 *
 * Copyright (c) 2014 Samsung Electronics
 *
 * Jiun Yu, <jiun.yu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifdef CONFIG_PANEL_AID_DIMMING
#include "aid_dimming.h"
#include "dimming_core.h"
#include "s6e3ha3_wqhd_aid_dimming.h"
#endif
#include "s6e3ha3_s6e3ha2_wqhd_param.h"
#include "../dsim.h"
#include <video/mipi_display.h>

extern unsigned dynamic_lcd_type;
static unsigned int hw_rev = 0; // for dcdc set


unsigned char ELVSS_LEN;
unsigned char ELVSS_REG;
unsigned char ELVSS_CMD_CNT;
unsigned char AID_CMD_CNT;
unsigned char AID_REG_OFFSET;
unsigned char TSET_LEN;
unsigned char TSET_REG;
unsigned char TSET_MINUS_OFFSET;
unsigned char VINT_REG2;

#ifdef CONFIG_ALWAYS_RELOAD_MTP_FACTORY_BUILD
void update_mdnie_coordinate( u16 coordinate0, u16 coordinate1 );
static int lcd_reload_mtp(int lcd_type, struct dsim_device *dsim);
#endif

#ifdef CONFIG_PANEL_AID_DIMMING
static const unsigned char *HBM_TABLE[HBM_STATUS_MAX] = { SEQ_HBM_OFF, SEQ_HBM_ON };
static const unsigned char *ACL_CUTOFF_TABLE[ACL_STATUS_MAX] = { S6E3HA2_SEQ_ACL_OFF, S6E3HA2_SEQ_ACL_8 };
static const unsigned char *ACL_OPR_TABLE_HA2[ACL_OPR_MAX] = { S6E3HA2_SEQ_ACL_OFF_OPR, S6E3HA2_SEQ_ACL_ON_OPR };
static const unsigned char *ACL_OPR_TABLE_HA3[ACL_OPR_MAX] = { S6E3HA3_SEQ_ACL_OFF_OPR, S6E3HA3_SEQ_ACL_ON_OPR };
static const unsigned char *ACL_OPR_TABLE_HF3[ACL_OPR_MAX] = { S6E3HF3_SEQ_ACL_OFF_OPR, S6E3HF3_SEQ_ACL_ON_OPR };

static const unsigned int br_tbl[256] = {
	2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,   // 16
	16, 17, 18, 19, 20, 21, 22, 23, 25, 27, 29, 31, 33, 36, // 14
	39, 41, 41, 44, 44, 47, 47, 50, 50, 53, 53, 56, 56, 56, // 14
	60, 60, 60, 64, 64, 64, 68, 68, 68, 72, 72, 72, 72, 77, // 14
	77, 77, 82, 82, 82, 82, 87, 87, 87, 87, 93, 93, 93, 93, // 14
	98, 98, 98, 98, 98, 105, 105, 105, 105, 111, 111, 111,  // 12
	111, 111, 111, 119, 119, 119, 119, 119, 126, 126, 126,  // 11
	126, 126, 126, 134, 134, 134, 134, 134, 134, 134, 143,
	143, 143, 143, 143, 143, 152, 152, 152, 152, 152, 152,
	152, 152, 162, 162, 162, 162, 162, 162, 162, 172, 172,
	172, 172, 172, 172, 172, 172, 183, 183, 183, 183, 183,
	183, 183, 183, 183, 195, 195, 195, 195, 195, 195, 195,
	195, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
	220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 234,
	234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 249,
	249, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
	265, 265, 265, 265, 265, 265, 265, 265, 265, 265, 265,
	265, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
	282, 282, 282, 300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 316, 316, 316, 316, 316, 316, 316,
	316, 316, 316, 316, 316, 333, 333, 333, 333, 333, 333,
	333, 333, 333, 333, 333, 333, 360       //7
};
static const unsigned int br_tbl_420[256] = {
	2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 19, 20, 21, 22, 24, 25, 25, 27, 29, 30, 32, 34, 37, 39,
	41, 44, 47, 47, 50, 50, 53, 56, 56, 60, 60, 64, 64, 68, 68, 68,
	72, 72, 72, 77, 77, 77, 82, 82, 82, 87, 87, 87, 87, 93, 93, 93,
	98, 98, 98, 98, 105, 105, 105, 105, 111, 111, 111, 111, 119, 119, 119, 119,
	119, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 134, 134, 134, 134, 134,
	143, 143, 143, 143, 143, 143, 152, 152, 152, 152, 152, 152, 152, 162, 162, 162,
	162, 162, 162, 172, 172, 172, 172, 172, 172, 172, 172, 183, 183, 183, 183, 183,
	183, 183, 195, 195, 195, 195, 195, 195, 195, 195, 207, 207, 207, 207, 207, 207,
	207, 207, 207, 220, 220, 220, 220, 220, 220, 220, 234, 234, 234, 234, 234, 234,
	234, 234, 234, 249, 249, 249, 249, 249, 249, 249, 249, 249, 265, 265, 265, 265,
	265, 265, 265, 265, 265, 265, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
	300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 316, 316, 316, 316, 316, 316,
	316, 316, 316, 316, 316, 316, 333, 333, 333, 333, 350, 350, 350, 350, 350, 350,
	350, 350, 350, 357, 357, 357, 357, 357, 372, 372, 372, 372, 372, 372, 380, 380,
	380, 380, 387, 387, 387, 395, 395, 395, 395, 395, 403, 403, 403, 412, 412, 420,

};

static unsigned int nb_br_tbl_420[256] = {
        2,        3,        4,        4,        5,        6,        7,        8,        9,        10,        11,        12,
        13,        14,        15,        16,        17,        19,        19,        20,        21,        22,        24,
        25,        27,        27,        29,        30,        32,        32,        34,        34,        37,        37,
        39,        41,        41,        44,        44,        47,        47,        47,        50,        50,        53,
        53,        56,        56,        60,        60,        60,        64,        64,        64,        68,        68,
        68,        72,        72,        77,        77,        77,        77,        82,        82,        82,        87,
        87,        87,        87,        93,        93,        93,        93,        98,        98,        98,        105,
        105,        105,        105,        111,        111,        111,        111,        119,        119,        119,
        119,        119,        126,        126,        126,        126,        126,        134,        134,        134,
        134,        134,        143,        143,        143,        143,        143,        152,        152,        152,
        152,        152,        152,        162,        162,        162,        162,        162,        162,        172,
        172,        172,        172,        172,        183,        183,        183,        183,        183,        183,
        183,        195,        195,        195,        195,        195,        195,        195,        207,        207,
        207,        207,        207,        207,        207,        220,        220,        220,        220,        220,
        220,        220,        234,        234,        234,        234,        234,        234,        234,        234,
        249,        249,        249,        249,        249,        249,        249,        249,        265,        265,
        265,        265,        265,        265,        265,        265,        265,        282,        282,        282,
        282,        282,        282,        282,        282,        282,        300,        300,        300,        300,
        300,        300,        300,        300,        300,        300,        316,        316,        316,        316,
        316,        316,        316,        316,        333,        333,        333,        333,        333,        333,
        333,        333,        333,        350,        350,        350,        350,        350,        350,        350,
        350,        350,        357,        357,        357,        357,        365,        365,        365,        365,
        372,        372,        372,        380,        380,        380,        380,        387,        387,        387,
        387,        395,        395,        395,        395,        403,        403,        403,        403,        412,
        412,        412,        412,        420,        420,        420,        420,        420,
};

static unsigned int zen_br_tbl_420[256] ={
        2,        3,        4,        4,        5,        6,        7,        8,        9,        10,        11,        12,
        13,        14,        15,        16,        19,        19,        19,        20,        21,        22,        24,
        25,        27,        27,        29,        30,        32,        32,        34,        34,        37,        37,
        41,        41,        41,        44,        44,        47,        47,        47,        50,        50,        53,
        53,        56,        56,        60,        60,        60,        64,        64,        64,        68,        68,
        68,        72,        72,        77,        77,        77,        77,        82,        82,        82,        87,
        87,        87,        87,        93,        93,        93,        93,        98,        98,        98,        105,
        105,        105,        105,        111,        111,        111,        111,        119,        119,        119,
        119,        119,        126,        126,        126,        126,        126,        134,        134,        134,
        134,        134,        143,        143,        143,        143,        143,        152,        152,        152,
        152,        152,        152,        162,        162,        162,        162,        162,        162,        172,
        172,        172,        172,        172,        183,        183,        183,        183,        183,        183,
        183,        195,        195,        195,        195,        195,        195,        195,        207,        207,
        207,        207,        207,        207,        207,        220,        220,        220,        220,        220,
        220,        220,        234,        234,        234,        234,        234,        234,        234,        234,
        249,        249,        249,        249,        249,        249,        249,        249,        265,        265,
        265,        265,        265,        265,        265,        265,        265,        282,        282,        282,
        282,        282,        282,        282,        282,        282,        300,        300,        300,        300,
        300,        300,        300,        300,        300,        300,        316,        316,        316,        316,
        316,        316,        316,        316,        333,        333,        333,        333,        333,        333,
        333,        333,        333,        350,        350,        350,        350,        350,        350,        350,
        350,        350,        357,        357,        357,        357,        365,        365,        365,        365,
        372,        372,        372,        380,        380,        380,        380,        387,        387,        387,
        387,        395,        395,        395,        395,        403,        403,        403,        403,        412,
        412,        412,        412,        420,        420,        420,        420,        420,
};

static unsigned char nb_inter_aor_tbl [512] = {
        0x90, 0xE5,        0x90, 0xD1,        0x90, 0xC6,        0x90, 0xBB,        0x90, 0xA0,
        0x90, 0x82,        0x90, 0x72,        0x90, 0x5C,        0x90, 0x42,        0x90, 0x32,
        0x90, 0x21,        0x90, 0x11,        0x80, 0xF6,        0x80, 0xE5,        0x80, 0xD5,
        0x80, 0xC1,        0x80, 0xAB,        0x80, 0x96,        0x80, 0x80,        0x80, 0x68,
        0x80, 0x52,        0x80, 0x3F,        0x80, 0x12,        0x70, 0xFC,        0x70, 0xE6,
        0x70, 0xD1,        0x70, 0xAB,        0x70, 0x94,        0x70, 0x80,        0x70, 0x6C,
        0x70, 0x56,        0x70, 0x41,        0x70, 0x1F,        0x60, 0xFD,        0x60, 0xD1,
        0x60, 0xBA,        0x60, 0xA2,        0x60, 0x81,        0x60, 0x61,        0x60, 0x4B,
        0x60, 0x34,        0x60, 0x1E,        0x50, 0xFA,        0x50, 0xD6,        0x50, 0xB5,
        0x50, 0x93,        0x50, 0x70,        0x50, 0x4D,        0x50, 0x2F,        0x50, 0x10,
        0x40, 0xF2,        0x40, 0xD3,        0x40, 0xB3,        0x40, 0x94,        0x40, 0x75,
        0x40, 0x55,        0x40, 0x36,        0x40, 0x05,        0x30, 0xD5,        0x40, 0x28,
        0x30, 0xFE,        0x30, 0xEA,        0x30, 0xD5,        0x40, 0x06,        0x30, 0xF2,
        0x30, 0xCB,        0x40, 0x15,        0x30, 0xF0,        0x30, 0xDE,        0x30, 0xCB,
        0x40, 0x24,        0x40, 0x13,        0x30, 0xF1,        0x30, 0xE0,        0x30, 0xFE,
        0x30, 0xEE,        0x30, 0xCD,        0x40, 0x22,        0x40, 0x04,        0x30, 0xF4,
        0x30, 0xD6,        0x40, 0x19,        0x40, 0x0B,        0x30, 0xEE,        0x30, 0xE0,
        0x40, 0x30,        0x40, 0x23,        0x40, 0x08,        0x30, 0xFB,        0x30, 0xE0,
        0x40, 0x18,        0x30, 0xFE,        0x30, 0xF1,        0x30, 0xD8,        0x30, 0xCB,
        0x40, 0x18,        0x40, 0x0C,        0x30, 0xF4,        0x30, 0xE8,        0x30, 0xD0,
        0x40, 0x1A,        0x40, 0x03,        0x30, 0xF8,        0x30, 0xE2,        0x30, 0xCB,
        0x40, 0x19,        0x40, 0x0E,        0x30, 0xF9,        0x30, 0xEF,        0x30, 0xDA,
        0x30, 0xCF,        0x40, 0x25,        0x40, 0x1B,        0x40, 0x07,        0x30, 0xF4,
        0x30, 0xEA,        0x30, 0xD6,        0x40, 0x2A,        0x40, 0x17,        0x40, 0x05,
        0x30, 0xF2,        0x30, 0xE0,        0x40, 0x1F,        0x40, 0x16,        0x40, 0x05,
        0x30, 0xFC,        0x30, 0xEA,        0x30, 0xE2,        0x30, 0xD0,        0x30, 0xBE,
        0x30, 0xAC,        0x30, 0x9A,        0x30, 0x88,        0x30, 0x76,        0x30, 0x64,
        0x30, 0x52,        0x30, 0x43,        0x30, 0x33,        0x30, 0x24,        0x30, 0x14,
        0x30, 0x05,        0x20, 0xF5,        0x20, 0xE6,        0x20, 0xD1,        0x20, 0xBC,
        0x20, 0xA7,        0x20, 0x91,        0x20, 0x7C,        0x20, 0x67,        0x20, 0x52,
        0x20, 0x40,        0x20, 0x2E,        0x20, 0x1B,        0x20, 0x09,        0x10, 0xF7,
        0x10, 0xE5,        0x10, 0xD2,        0x10, 0xC0,        0x10, 0xAB,        0x10, 0x96,
        0x10, 0x80,        0x10, 0x6C,        0x10, 0x56,        0x10, 0x41,        0x10, 0x2C,
        0x10, 0x17,        0x10, 0x7E,        0x10, 0x6C,        0x10, 0x63,        0x10, 0x52,
        0x10, 0x40,        0x10, 0x2F,        0x10, 0x26,        0x10, 0x15,        0x10, 0x03,
        0x10, 0x7E,        0x10, 0x6E,        0x10, 0x5E,        0x10, 0x4D,        0x10, 0x45,
        0x10, 0x34,        0x10, 0x24,        0x10, 0x13,        0x10, 0x03,        0x10, 0x7F,
        0x10, 0x6F,        0x10, 0x68,        0x10, 0x58,        0x10, 0x49,        0x10, 0x39,
        0x10, 0x2A,        0x10, 0x22,        0x10, 0x13,        0x10, 0x03,        0x10, 0x6A,
        0x10, 0x5B,        0x10, 0x4C,        0x10, 0x3E,        0x10, 0x2F,        0x10, 0x20,
        0x10, 0x12,        0x10, 0x03,        0x10, 0x6C,        0x10, 0x5E,        0x10, 0x50,
        0x10, 0x42,        0x10, 0x3B,        0x10, 0x2D,        0x10, 0x1F,        0x10, 0x11,
        0x10, 0x03,        0x10, 0x67,        0x10, 0x59,        0x10, 0x4C,        0x10, 0x3F,
        0x10, 0x38,        0x10, 0x2B,        0x10, 0x1E,        0x10, 0x10,        0x10, 0x03,
        0x10, 0x24,        0x10, 0x17,        0x10, 0x10,        0x10, 0x03,        0x10, 0x29,
        0x10, 0x1C,        0x10, 0x10,        0x10, 0x03,        0x00, 0xED,        0x00, 0xD6,
        0x00, 0xC0,        0x00, 0xB5,        0x00, 0xAA,        0x00, 0x9F,        0x00, 0x94,
        0x00, 0x89,        0x00, 0x7E,        0x00, 0x72,        0x00, 0x67,        0x00, 0x56,
        0x00, 0x45,        0x00, 0x33,        0x00, 0x22,        0x00, 0x30,        0x00, 0x24,
        0x00, 0x17,        0x00, 0x0A,        0x00, 0x36,        0x00, 0x23,        0x00, 0x17,
        0x00, 0x0A,        0x00, 0x2F,        0x00, 0x29,        0x00, 0x1C,        0x00, 0x0A,
        0x00, 0x0A,
};


static unsigned char zen_inter_aor_tbl [512] = {
        0x90,0xDC,        0x90,0xC1,        0x90,0xB1,        0x90,0xA2,        0x90,0x85,
        0x90,0x6B,        0x90,0x52,        0x90,0x3B,        0x90,0x22,        0x90,0x0B,
        0x80,0xFA,        0x80,0xE2,        0x80,0xD0,        0x80,0xBC,        0x80,0xA4,
        0x80,0x8B,        0x80,0x73,        0x80,0x5C,        0x80,0x44,        0x80,0x2F,
        0x80,0x12,        0x70,0xFF,        0x70,0xCB,        0x70,0xB1,        0x70,0x99,
        0x70,0x80,        0x70,0x4C,        0x70,0x33,        0x70,0x1B,        0x70,0x04,
        0x60,0xEA,        0x60,0xD0,        0x60,0xAB,        0x60,0x85,        0x60,0x4F,
        0x60,0x37,        0x60,0x1F,        0x50,0xF8,        0x50,0xD1,        0x50,0xB7,
        0x50,0x9D,        0x50,0x83,        0x50,0x6D,        0x50,0x56,        0x50,0x1E,
        0x40,0xE6,        0x40,0xC3,        0x40,0x9F,        0x40,0x88,        0x40,0x72,
        0x40,0x5B,        0x40,0x26,        0x30,0xF1,        0x30,0xBB,        0x40,0x03,
        0x30,0xD3,        0x30,0xBB,        0x30,0xE8,        0x30,0xBB,        0x40,0x10,
        0x30,0xE5,        0x30,0xD0,        0x30,0xBB,        0x30,0xF7,        0x30,0xE3,
        0x30,0xBB,        0x40,0x06,        0x30,0xE0,        0x30,0xCE,        0x30,0xBB,
        0x40,0x01,        0x30,0xF0,        0x30,0xCD,        0x30,0xBB,        0x30,0xED,
        0x30,0xDC,        0x30,0xBB,        0x40,0x08,        0x30,0xEA,        0x30,0xDA,
        0x30,0xBB,        0x30,0xF6,        0x30,0xE7,        0x30,0xCA,        0x30,0xBB,
        0x40,0x0D,        0x30,0xFF,        0x30,0xE4,        0x30,0xD6,        0x30,0xBB,
        0x40,0x08,        0x30,0xEF,        0x30,0xE2,        0x30,0xC8,        0x30,0xBB,
        0x40,0x04,        0x30,0xF8,        0x30,0xDF,        0x30,0xD3,        0x30,0xBB,
        0x40,0x0B,        0x30,0xF4,        0x30,0xE9,        0x30,0xD2,        0x30,0xBB,
        0x40,0x06,        0x30,0xFB,        0x30,0xE6,        0x30,0xDB,        0x30,0xC6,
        0x30,0xBB,        0x40,0x0B,        0x40,0x01,        0x30,0xED,        0x30,0xD9,
        0x30,0xCF,        0x30,0xBB,        0x40,0x07,        0x30,0xF4,        0x30,0xE1,
        0x30,0xCE,        0x30,0xBB,        0x40,0x0B,        0x40,0x02,        0x30,0xF0,
        0x30,0xE8,        0x30,0xD6,        0x30,0xCD,        0x30,0xBB,        0x30,0xA2,
        0x30,0x89,        0x30,0x6F,        0x30,0x56,        0x30,0x3D,        0x30,0x23,
        0x30,0x0A,        0x20,0xFB,        0x20,0xEB,        0x20,0xDC,        0x20,0xCD,
        0x20,0xBE,        0x20,0xAE,        0x20,0x9F,        0x20,0x8A,        0x20,0x74,
        0x20,0x5F,        0x20,0x4A,        0x20,0x35,        0x20,0x1F,        0x20,0x0A,
        0x10,0xF6,        0x10,0xE2,        0x10,0xCE,        0x10,0xBA,        0x10,0xA6,
        0x10,0x92,        0x10,0x7E,        0x10,0x6A,        0x10,0x5D,        0x10,0x50,
        0x10,0x43,        0x10,0x37,        0x10,0x2A,        0x10,0x1D,        0x10,0x10,
        0x10,0x03,        0x10,0x7E,        0x10,0x6C,        0x10,0x63,        0x10,0x52,
        0x10,0x40,        0x10,0x2F,        0x10,0x26,        0x10,0x15,        0x10,0x03,
        0x10,0x7E,        0x10,0x6E,        0x10,0x5E,        0x10,0x4D,        0x10,0x45,
        0x10,0x34,        0x10,0x24,        0x10,0x13,        0x10,0x03,        0x10,0x7F,
        0x10,0x6F,        0x10,0x68,        0x10,0x58,        0x10,0x49,        0x10,0x39,
        0x10,0x2A,        0x10,0x22,        0x10,0x13,        0x10,0x03,        0x10,0x6A,
        0x10,0x5B,        0x10,0x4C,        0x10,0x3E,        0x10,0x2F,        0x10,0x20,
        0x10,0x12,        0x10,0x03,        0x10,0x6C,        0x10,0x5E,        0x10,0x50,
        0x10,0x42,        0x10,0x3B,        0x10,0x2D,        0x10,0x1F,        0x10,0x11,
        0x10,0x03,        0x10,0x67,        0x10,0x59,        0x10,0x4C,        0x10,0x3F,
        0x10,0x38,        0x10,0x2B,        0x10,0x1E,        0x10,0x10,        0x10,0x03,
        0x10,0x24,        0x10,0x17,        0x10,0x10,        0x10,0x03,        0x10,0x29,
        0x10,0x1C,        0x10,0x10,        0x10,0x03,        0x00,0xEE,        0x00,0xDB,
        0x00,0xCE,        0x00,0xC1,        0x00,0xB5,        0x00,0xA8,        0x00,0x9B,
        0x00,0x87,        0x00,0x7A,        0x00,0x74,        0x00,0x67,        0x00,0x5B,
        0x00,0x4E,        0x00,0x41,        0x00,0x34,        0x00,0x30,        0x00,0x24,
        0x00,0x17,        0x00,0x0A,        0x00,0x36,        0x00,0x23,        0x00,0x17,
        0x00,0x0A,        0x00,0x2F,        0x00,0x29,        0x00,0x1C,        0x00,0x0A,
        0x00,0x0A,
};

static unsigned char zen_inter_aor_tbl_revd[512] = {
	0x90,0xDE, 0x90,0xBF, 0x90,0xAF, 0x90,0x9F, 0x90,0x80, 0x90,0x66, 0x90,0x4F, 0x90,0x35, 0x90,0x20, 0x90,0x0B,
	0x80,0xF4, 0x80,0xE1, 0x80,0xCF, 0x80,0xBC, 0x80,0xA4, 0x80,0x8F, 0x80,0x75, 0x80,0x5B, 0x80,0x41, 0x80,0x2B,
	0x80,0x11, 0x70,0xF5, 0x70,0xC7, 0x70,0xAB, 0x70,0x93, 0x70,0x7B, 0x70,0x45, 0x70,0x30, 0x70,0x19, 0x70,0x01,
	0x60,0xE7, 0x60,0xCC, 0x60,0xA4, 0x60,0x7C, 0x60,0x50, 0x60,0x36, 0x60,0x1B, 0x50,0xF4, 0x50,0xCC, 0x50,0xB1,
	0x50,0x96, 0x50,0x7B, 0x50,0x56, 0x50,0x30, 0x50,0x09, 0x40,0xE2, 0x40,0xB7, 0x40,0x8B, 0x40,0x64, 0x40,0x3C,
	0x40,0x15, 0x40,0x1C, 0x30,0xEA, 0x30,0xD1, 0x40,0x19, 0x30,0xEA, 0x30,0xD3, 0x30,0xFF, 0x30,0xD3, 0x40,0x26,
	0x30,0xFC, 0x30,0xE8, 0x30,0xD3, 0x40,0x0C, 0x30,0xF8, 0x30,0xD1, 0x40,0x1B, 0x30,0xF6, 0x30,0xE3, 0x30,0xD1,
	0x40,0x16, 0x40,0x05, 0x30,0xE2, 0x30,0xD1, 0x40,0x02, 0x30,0xF2, 0x30,0xD1, 0x40,0x1D, 0x30,0xFF, 0x30,0xF0,
	0x30,0xD1, 0x40,0x0B, 0x30,0xFC, 0x30,0xDF, 0x30,0xD1, 0x40,0x22, 0x40,0x14, 0x30,0xF9, 0x30,0xEC, 0x30,0xD1,
	0x40,0x1D, 0x40,0x04, 0x30,0xF7, 0x30,0xDE, 0x30,0xD1, 0x40,0x19, 0x40,0x0D, 0x30,0xF5, 0x30,0xE9, 0x30,0xD1,
	0x40,0x1F, 0x40,0x09, 0x30,0xFE, 0x30,0xE7, 0x30,0xD1, 0x40,0x1B, 0x40,0x10, 0x30,0xFB, 0x30,0xF1, 0x30,0xDC,
	0x30,0xD1, 0x40,0x20, 0x40,0x16, 0x40,0x02, 0x30,0xEF, 0x30,0xE5, 0x30,0xD1, 0x40,0x05, 0x30,0xF2, 0x30,0xDF,
	0x30,0xCC, 0x30,0xBA, 0x40,0x20, 0x40,0x17, 0x40,0x06, 0x30,0xFD, 0x30,0xEB, 0x30,0xE3, 0x30,0xD1, 0x30,0xC0,
	0x30,0xAF, 0x30,0x9F, 0x30,0x8E, 0x30,0x7D, 0x30,0x6C, 0x30,0x5B, 0x30,0x49, 0x30,0x38, 0x30,0x26, 0x30,0x14,
	0x30,0x02, 0x20,0xF1, 0x20,0xDF, 0x20,0xCB, 0x20,0xB7, 0x20,0xA3, 0x20,0x8E, 0x20,0x7A, 0x20,0x66, 0x20,0x52,
	0x20,0x3F, 0x20,0x2D, 0x20,0x1A, 0x20,0x08, 0x10,0xF5, 0x10,0xE2, 0x10,0xCF, 0x10,0xBD, 0x10,0xAB, 0x10,0x9A,
	0x10,0x88, 0x10,0x77, 0x10,0x66, 0x10,0x54, 0x10,0x43, 0x10,0x31, 0x10,0x7D, 0x10,0x6C, 0x10,0x63, 0x10,0x52,
	0x10,0x40, 0x10,0x2F, 0x10,0x26, 0x10,0x14, 0x10,0x03, 0x10,0x7E, 0x10,0x6E, 0x10,0x5D, 0x10,0x4D, 0x10,0x45,
	0x10,0x34, 0x10,0x24, 0x10,0x13, 0x10,0x03, 0x10,0x7E, 0x10,0x6F, 0x10,0x67, 0x10,0x58, 0x10,0x48, 0x10,0x39,
	0x10,0x2A, 0x10,0x22, 0x10,0x12, 0x10,0x03, 0x10,0x6A, 0x10,0x5B, 0x10,0x4C, 0x10,0x3E, 0x10,0x2F, 0x10,0x20,
	0x10,0x12, 0x10,0x03, 0x10,0x6B, 0x10,0x5D, 0x10,0x4F, 0x10,0x42, 0x10,0x3B, 0x10,0x2D, 0x10,0x1F, 0x10,0x11,
	0x10,0x03, 0x10,0x66, 0x10,0x59, 0x10,0x4C, 0x10,0x3E, 0x10,0x38, 0x10,0x2B, 0x10,0x1D, 0x10,0x10, 0x10,0x03,
	0x10,0x23, 0x10,0x16, 0x10,0x10, 0x10,0x03, 0x10,0x29, 0x10,0x1C, 0x10,0x10, 0x10,0x03, 0x00,0xF1, 0x00,0xE0,
	0x00,0xCE, 0x00,0xC2, 0x00,0xB5, 0x00,0xA9, 0x00,0x9C, 0x00,0x8E, 0x00,0x81, 0x00,0x73, 0x00,0x65, 0x00,0x53,
	0x00,0x41, 0x00,0x2F, 0x00,0x1D, 0x00,0x30, 0x00,0x24, 0x00,0x17, 0x00,0x0A, 0x00,0x36, 0x00,0x23, 0x00,0x17,
	0x00,0x0A, 0x00,0x2F, 0x00,0x29, 0x00,0x1C, 0x00,0x0A, 0x00,0x0A
};


static const unsigned int hbm_interpolation_br_tbl[256] = {
	2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 19, 20, 21, 22, 23, 24, 25, 25, 27, 29, 30, 32, 34, 37, 37,
	39, 41, 44, 47, 50, 53, 56, 60, 64, 68, 68, 72, 72, 77, 82, 82,
	87, 87, 93, 93, 98, 98, 98, 105, 105, 105, 111, 111, 111, 119, 119, 119,
	126, 126, 126, 126, 134, 134, 134, 143, 143, 143, 143, 152, 152, 152, 152, 162,
	162, 162, 162, 172, 172, 172, 172, 172, 183, 183, 183, 183, 183, 183, 183, 183,
	183, 183, 195, 195, 195, 195, 195, 207, 207, 207, 207, 207, 207, 220, 220, 220,
	220, 220, 220, 220, 234, 234, 234, 234, 234, 234, 249, 249, 249, 249, 249, 249,
	249, 249, 265, 265, 265, 265, 265, 265, 265, 282, 282, 282, 282, 282, 282, 282,
	282, 300, 300, 300, 300, 300, 300, 316, 316, 316, 316, 316, 316, 316, 333, 333,
	333, 333, 333, 333, 350, 350, 350, 350, 357, 357, 357, 357, 357, 357, 357, 357,
	357, 365, 365, 365, 365, 372, 372, 372, 372, 372, 372, 380, 380, 380, 380, 403,
	403, 403, 403, 403, 403, 412, 412, 412, 412, 412, 412, 420, 420, 420, 420, 420,
	420, 420, 420, 420, 420, 443, 443, 443, 443, 443, 443, 443, 443, 465, 465, 465,
	465, 488, 488, 488, 488, 488, 488, 510, 510, 510, 510, 510, 533, 533, 533, 533,
	533, 533, 555, 555, 555, 555, 555, 555, 555, 578, 578, 578, 578, 578, 578, 600
};

static const short center_gamma[NUM_VREF][CI_MAX] = {
	{ 0x000, 0x000, 0x000 },
	{ 0x080, 0x080, 0x080 },
	{ 0x080, 0x080, 0x080 },
	{ 0x080, 0x080, 0x080 },
	{ 0x080, 0x080, 0x080 },
	{ 0x080, 0x080, 0x080 },
	{ 0x080, 0x080, 0x080 },
	{ 0x080, 0x080, 0x080 },
	{ 0x080, 0x080, 0x080 },
	{ 0x100, 0x100, 0x100 },
};

// aid sheet in rev.d opmanual
struct SmtDimInfo dimming_info_HA3[MAX_BR_INFO] = { // add hbm array
	{ .br = 2, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl2nit, .cTbl = HA3_ctbl2nit, .aid = HA3_aid9818, .elvCaps = HA3_elvCaps5, .elv = HA3_elv5, .way = W1 },
	{ .br = 3, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl3nit, .cTbl = HA3_ctbl3nit, .aid = HA3_aid9740, .elvCaps = HA3_elvCaps4, .elv = HA3_elv4, .way = W1 },
	{ .br = 4, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl4nit, .cTbl = HA3_ctbl4nit, .aid = HA3_aid9655, .elvCaps = HA3_elvCaps3, .elv = HA3_elv3, .way = W1 },
	{ .br = 5, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl5nit, .cTbl = HA3_ctbl5nit, .aid = HA3_aid9550, .elvCaps = HA3_elvCaps2, .elv = HA3_elv2, .way = W1 },
	{ .br = 6, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl6nit, .cTbl = HA3_ctbl6nit, .aid = HA3_aid9434, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 7, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl7nit, .cTbl = HA3_ctbl7nit, .aid = HA3_aid9372, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 8, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl8nit, .cTbl = HA3_ctbl8nit, .aid = HA3_aid9287, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 9, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl9nit, .cTbl = HA3_ctbl9nit, .aid = HA3_aid9186, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 10, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl10nit, .cTbl = HA3_ctbl10nit, .aid = HA3_aid9124, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 11, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl11nit, .cTbl = HA3_ctbl11nit, .aid = HA3_aid9058, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 12, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl12nit, .cTbl = HA3_ctbl12nit, .aid = HA3_aid8996, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 13, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl13nit, .cTbl = HA3_ctbl13nit, .aid = HA3_aid8891, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 14, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl14nit, .cTbl = HA3_ctbl14nit, .aid = HA3_aid8826, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 15, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl15nit, .cTbl = HA3_ctbl15nit, .aid = HA3_aid8764, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 16, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl16nit, .cTbl = HA3_ctbl16nit, .aid = HA3_aid8686, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 17, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl17nit, .cTbl = HA3_ctbl17nit, .aid = HA3_aid8601, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 19, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl19nit, .cTbl = HA3_ctbl19nit, .aid = HA3_aid8434, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 20, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl20nit, .cTbl = HA3_ctbl20nit, .aid = HA3_aid8341, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 21, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl21nit, .cTbl = HA3_ctbl21nit, .aid = HA3_aid8256, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 22, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl22nit, .cTbl = HA3_ctbl22nit, .aid = HA3_aid8182, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 24, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl24nit, .cTbl = HA3_ctbl24nit, .aid = HA3_aid8008, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 25, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl25nit, .cTbl = HA3_ctbl25nit, .aid = HA3_aid7922, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 27, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl27nit, .cTbl = HA3_ctbl27nit, .aid = HA3_aid7756, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 29, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl29nit, .cTbl = HA3_ctbl29nit, .aid = HA3_aid7609, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 30, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl30nit, .cTbl = HA3_ctbl30nit, .aid = HA3_aid7519, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 32, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl32nit, .cTbl = HA3_ctbl32nit, .aid = HA3_aid7364, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 34, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl34nit, .cTbl = HA3_ctbl34nit, .aid = HA3_aid7198, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 37, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl37nit, .cTbl = HA3_ctbl37nit, .aid = HA3_aid6934, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 39, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl39nit, .cTbl = HA3_ctbl39nit, .aid = HA3_aid6764, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 41, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl41nit, .cTbl = HA3_ctbl41nit, .aid = HA3_aid6581, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 44, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl44nit, .cTbl = HA3_ctbl44nit, .aid = HA3_aid6329, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 47, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl47nit, .cTbl = HA3_ctbl47nit, .aid = HA3_aid6070, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 50, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl50nit, .cTbl = HA3_ctbl50nit, .aid = HA3_aid5791, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 53, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl53nit, .cTbl = HA3_ctbl53nit, .aid = HA3_aid5531, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 56, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl56nit, .cTbl = HA3_ctbl56nit, .aid = HA3_aid5260, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 60, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl60nit, .cTbl = HA3_ctbl60nit, .aid = HA3_aid4907, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 64, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl64nit, .cTbl = HA3_ctbl64nit, .aid = HA3_aid4543, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 68, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl68nit, .cTbl = HA3_ctbl68nit, .aid = HA3_aid4178, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 72, .refBr = 125, .cGma = gma2p15, .rTbl = HA3_rtbl72nit, .cTbl = HA3_ctbl72nit, .aid = HA3_aid3802, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 77, .refBr = 132, .cGma = gma2p15, .rTbl = HA3_rtbl77nit, .cTbl = HA3_ctbl77nit, .aid = HA3_aid3802, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 82, .refBr = 141, .cGma = gma2p15, .rTbl = HA3_rtbl82nit, .cTbl = HA3_ctbl82nit, .aid = HA3_aid3764, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 87, .refBr = 148, .cGma = gma2p15, .rTbl = HA3_rtbl87nit, .cTbl = HA3_ctbl87nit, .aid = HA3_aid3764, .elvCaps = HA3_elvCaps1, .elv = HA3_elv1, .way = W1 },
	{ .br = 93, .refBr = 158, .cGma = gma2p15, .rTbl = HA3_rtbl93nit, .cTbl = HA3_ctbl93nit, .aid = HA3_aid3845, .elvCaps = HA3_elvCaps2, .elv = HA3_elv2, .way = W1 },
	{ .br = 98, .refBr = 167, .cGma = gma2p15, .rTbl = HA3_rtbl98nit, .cTbl = HA3_ctbl98nit, .aid = HA3_aid3771, .elvCaps = HA3_elvCaps2, .elv = HA3_elv2, .way = W1 },
	{ .br = 105, .refBr = 176, .cGma = gma2p15, .rTbl = HA3_rtbl105nit, .cTbl = HA3_ctbl105nit, .aid = HA3_aid3806, .elvCaps = HA3_elvCaps2, .elv = HA3_elv2, .way = W1 },
	{ .br = 111, .refBr = 185, .cGma = gma2p15, .rTbl = HA3_rtbl111nit, .cTbl = HA3_ctbl111nit, .aid = HA3_aid3845, .elvCaps = HA3_elvCaps3, .elv = HA3_elv3, .way = W1 },
	{ .br = 119, .refBr = 198, .cGma = gma2p15, .rTbl = HA3_rtbl119nit, .cTbl = HA3_ctbl119nit, .aid = HA3_aid3845, .elvCaps = HA3_elvCaps3, .elv = HA3_elv3, .way = W1 },
	{ .br = 126, .refBr = 206, .cGma = gma2p15, .rTbl = HA3_rtbl126nit, .cTbl = HA3_ctbl126nit, .aid = HA3_aid3764, .elvCaps = HA3_elvCaps3, .elv = HA3_elv3, .way = W1 },
	{ .br = 134, .refBr = 217, .cGma = gma2p15, .rTbl = HA3_rtbl134nit, .cTbl = HA3_ctbl134nit, .aid = HA3_aid3783, .elvCaps = HA3_elvCaps3, .elv = HA3_elv3, .way = W1 },
	{ .br = 143, .refBr = 229, .cGma = gma2p15, .rTbl = HA3_rtbl143nit, .cTbl = HA3_ctbl143nit, .aid = HA3_aid3764, .elvCaps = HA3_elvCaps4, .elv = HA3_elv4, .way = W1 },
	{ .br = 152, .refBr = 243, .cGma = gma2p15, .rTbl = HA3_rtbl152nit, .cTbl = HA3_ctbl152nit, .aid = HA3_aid3779, .elvCaps = HA3_elvCaps4, .elv = HA3_elv4, .way = W1 },
	{ .br = 162, .refBr = 256, .cGma = gma2p15, .rTbl = HA3_rtbl162nit, .cTbl = HA3_ctbl162nit, .aid = HA3_aid3806, .elvCaps = HA3_elvCaps4, .elv = HA3_elv4, .way = W1 },
	{ .br = 172, .refBr = 273, .cGma = gma2p15, .rTbl = HA3_rtbl172nit, .cTbl = HA3_ctbl172nit, .aid = HA3_aid3845, .elvCaps = HA3_elvCaps4, .elv = HA3_elv4, .way = W1 },
	{ .br = 183, .refBr = 285, .cGma = gma2p15, .rTbl = HA3_rtbl183nit, .cTbl = HA3_ctbl183nit, .aid = HA3_aid3783, .elvCaps = HA3_elvCaps4, .elv = HA3_elv4, .way = W1 },
	{ .br = 195, .refBr = 285, .cGma = gma2p15, .rTbl = HA3_rtbl195nit, .cTbl = HA3_ctbl195nit, .aid = HA3_aid3295, .elvCaps = HA3_elvCaps4, .elv = HA3_elv4, .way = W1 },
	{ .br = 207, .refBr = 285, .cGma = gma2p15, .rTbl = HA3_rtbl207nit, .cTbl = HA3_ctbl207nit, .aid = HA3_aid2876, .elvCaps = HA3_elvCaps5, .elv = HA3_elv5, .way = W1 },
	{ .br = 220, .refBr = 285, .cGma = gma2p15, .rTbl = HA3_rtbl220nit, .cTbl = HA3_ctbl220nit, .aid = HA3_aid2302, .elvCaps = HA3_elvCaps5, .elv = HA3_elv5, .way = W1 },
	{ .br = 234, .refBr = 285, .cGma = gma2p15, .rTbl = HA3_rtbl234nit, .cTbl = HA3_ctbl234nit, .aid = HA3_aid1736, .elvCaps = HA3_elvCaps5, .elv = HA3_elv5, .way = W1 },
	{ .br = 249, .refBr = 285, .cGma = gma2p15, .rTbl = HA3_rtbl249nit, .cTbl = HA3_ctbl249nit, .aid = HA3_aid1081, .elvCaps = HA3_elvCaps6, .elv = HA3_elv6, .way = W1 },    // 249 ~ 360 acl off
	{ .br = 265, .refBr = 298, .cGma = gma2p15, .rTbl = HA3_rtbl265nit, .cTbl = HA3_ctbl265nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps7, .elv = HA3_elv7, .way = W1 },
	{ .br = 282, .refBr = 317, .cGma = gma2p15, .rTbl = HA3_rtbl282nit, .cTbl = HA3_ctbl282nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps8, .elv = HA3_elv8, .way = W1 },
	{ .br = 300, .refBr = 336, .cGma = gma2p15, .rTbl = HA3_rtbl300nit, .cTbl = HA3_ctbl300nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps9, .elv = HA3_elv9, .way = W1 },
	{ .br = 316, .refBr = 352, .cGma = gma2p15, .rTbl = HA3_rtbl316nit, .cTbl = HA3_ctbl316nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps10, .elv = HA3_elv10, .way = W1 },
	{ .br = 333, .refBr = 370, .cGma = gma2p15, .rTbl = HA3_rtbl333nit, .cTbl = HA3_ctbl333nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps11, .elv = HA3_elv11, .way = W1 },
	{ .br = 350, .refBr = 383, .cGma = gma2p15, .rTbl = HA3_rtbl350nit, .cTbl = HA3_ctbl350nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps12, .elv = HA3_elv12, .way = W1 },
	{ .br = 357, .refBr = 390, .cGma = gma2p15, .rTbl = HA3_rtbl357nit, .cTbl = HA3_ctbl357nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps12, .elv = HA3_elv12, .way = W1 },
	{ .br = 365, .refBr = 400, .cGma = gma2p15, .rTbl = HA3_rtbl365nit, .cTbl = HA3_ctbl365nit, .aid = HA3_aid1004, .elvCaps = HA3_elvCaps12, .elv = HA3_elv12, .way = W1 },
	{ .br = 372, .refBr = 400, .cGma = gma2p15, .rTbl = HA3_rtbl372nit, .cTbl = HA3_ctbl372nit, .aid = HA3_aid744, .elvCaps = HA3_elvCaps12, .elv = HA3_elv12, .way = W1 },
	{ .br = 380, .refBr = 400, .cGma = gma2p15, .rTbl = HA3_rtbl380nit, .cTbl = HA3_ctbl380nit, .aid = HA3_aid574, .elvCaps = HA3_elvCaps13, .elv = HA3_elv13, .way = W1 },
	{ .br = 387, .refBr = 400, .cGma = gma2p15, .rTbl = HA3_rtbl387nit, .cTbl = HA3_ctbl387nit, .aid = HA3_aid399, .elvCaps = HA3_elvCaps13, .elv = HA3_elv13, .way = W1 },
	{ .br = 395, .refBr = 400, .cGma = gma2p15, .rTbl = HA3_rtbl395nit, .cTbl = HA3_ctbl395nit, .aid = HA3_aid132, .elvCaps = HA3_elvCaps13, .elv = HA3_elv13, .way = W1 },
	{ .br = 403, .refBr = 404, .cGma = gma2p15, .rTbl = HA3_rtbl403nit, .cTbl = HA3_ctbl403nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps13, .elv = HA3_elv13, .way = W1 },
	{ .br = 412, .refBr = 412, .cGma = gma2p15, .rTbl = HA3_rtbl412nit, .cTbl = HA3_ctbl412nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps14, .elv = HA3_elv14, .way = W1 },
	{ .br = 420, .refBr = 420, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps14, .elv = HA3_elv14, .way = W2 },
/*hbm interpolation */
	{ .br = 443, .refBr = 443, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps14, .elv = HA3_elv14, .way = W3},	// hbm is acl on
	{ .br = 465, .refBr = 465, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps14, .elv = HA3_elv14, .way = W3},	// hbm is acl on
	{ .br = 488, .refBr = 488, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps14, .elv = HA3_elv14, .way = W3},	// hbm is acl on
	{ .br = 510, .refBr = 510, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps14, .elv = HA3_elv14, .way = W3},	// hbm is acl on
	{ .br = 533, .refBr = 533, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps15, .elv = HA3_elv15, .way = W3},	// hbm is acl on
	{ .br = 555, .refBr = 555, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps16, .elv = HA3_elv16, .way = W3},	// hbm is acl on
	{ .br = 578, .refBr = 578, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps17, .elv = HA3_elv17, .way = W3},	// hbm is acl on
/* hbm */
	{ .br = 600, .refBr = 600, .cGma = gma2p20, .rTbl = HA3_rtbl420nit, .cTbl = HA3_ctbl420nit, .aid = HA3_aid39, .elvCaps = HA3_elvCaps18, .elv = HA3_elv18, .way = W4 },
};

// aid sheet in rev.C
struct SmtDimInfo dimming_info_HF3[MAX_BR_INFO] = { // add hbm array
	{ .br = 2, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl2nit, .cTbl = HF3_ctbl2nit, .aid = HF3_aid9783, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 3, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl3nit, .cTbl = HF3_ctbl3nit, .aid = HF3_aid9678, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 4, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl4nit, .cTbl = HF3_ctbl4nit, .aid = HF3_aid9558, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 5, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl5nit, .cTbl = HF3_ctbl5nit, .aid = HF3_aid9446, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 6, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl6nit, .cTbl = HF3_ctbl6nit, .aid = HF3_aid9345, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 7, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl7nit, .cTbl = HF3_ctbl7nit, .aid = HF3_aid9248, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 8, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl8nit, .cTbl = HF3_ctbl8nit, .aid = HF3_aid9159, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 9, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl9nit, .cTbl = HF3_ctbl9nit, .aid = HF3_aid9062, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 10, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl10nit, .cTbl = HF3_ctbl10nit, .aid = HF3_aid8973, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 11, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl11nit, .cTbl = HF3_ctbl11nit, .aid = HF3_aid8907, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 12, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl12nit, .cTbl = HF3_ctbl12nit, .aid = HF3_aid8814, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 13, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl13nit, .cTbl = HF3_ctbl13nit, .aid = HF3_aid8744, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 14, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl14nit, .cTbl = HF3_ctbl14nit, .aid = HF3_aid8667, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 15, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl15nit, .cTbl = HF3_ctbl15nit, .aid = HF3_aid8574, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 16, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl16nit, .cTbl = HF3_ctbl16nit, .aid = HF3_aid8477, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 17, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl17nit, .cTbl = HF3_ctbl17nit, .aid = HF3_aid8380, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 19, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl19nit, .cTbl = HF3_ctbl19nit, .aid = HF3_aid8202, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 20, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl20nit, .cTbl = HF3_ctbl20nit, .aid = HF3_aid8120, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 21, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl21nit, .cTbl = HF3_ctbl21nit, .aid = HF3_aid8008, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 22, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl22nit, .cTbl = HF3_ctbl22nit, .aid = HF3_aid7934, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 24, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl24nit, .cTbl = HF3_ctbl24nit, .aid = HF3_aid7733, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 25, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl25nit, .cTbl = HF3_ctbl25nit, .aid = HF3_aid7632, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 27, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl27nit, .cTbl = HF3_ctbl27nit, .aid = HF3_aid7442, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 29, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl29nit, .cTbl = HF3_ctbl29nit, .aid = HF3_aid7240, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 30, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl30nit, .cTbl = HF3_ctbl30nit, .aid = HF3_aid7143, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 32, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl32nit, .cTbl = HF3_ctbl32nit, .aid = HF3_aid6961, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 34, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl34nit, .cTbl = HF3_ctbl34nit, .aid = HF3_aid6760, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 37, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl37nit, .cTbl = HF3_ctbl37nit, .aid = HF3_aid6469, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 39, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl39nit, .cTbl = HF3_ctbl39nit, .aid = HF3_aid6260, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 41, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl41nit, .cTbl = HF3_ctbl41nit, .aid = HF3_aid6074, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 44, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl44nit, .cTbl = HF3_ctbl44nit, .aid = HF3_aid5771, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 47, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl47nit, .cTbl = HF3_ctbl47nit, .aid = HF3_aid5469, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 50, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl50nit, .cTbl = HF3_ctbl50nit, .aid = HF3_aid5295, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 53, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl53nit, .cTbl = HF3_ctbl53nit, .aid = HF3_aid4860, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 56, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl56nit, .cTbl = HF3_ctbl56nit, .aid = HF3_aid4585, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 60, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl60nit, .cTbl = HF3_ctbl60nit, .aid = HF3_aid4322, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 64, .refBr = 111, .cGma = gma2p15, .rTbl = HF3_rtbl64nit, .cTbl = HF3_ctbl64nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 68, .refBr = 117, .cGma = gma2p15, .rTbl = HF3_rtbl68nit, .cTbl = HF3_ctbl68nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 72, .refBr = 123, .cGma = gma2p15, .rTbl = HF3_rtbl72nit, .cTbl = HF3_ctbl72nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 77, .refBr = 131, .cGma = gma2p15, .rTbl = HF3_rtbl77nit, .cTbl = HF3_ctbl77nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 82, .refBr = 139, .cGma = gma2p15, .rTbl = HF3_rtbl82nit, .cTbl = HF3_ctbl82nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 87, .refBr = 147, .cGma = gma2p15, .rTbl = HF3_rtbl87nit, .cTbl = HF3_ctbl87nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 93, .refBr = 154, .cGma = gma2p15, .rTbl = HF3_rtbl93nit, .cTbl = HF3_ctbl93nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 98, .refBr = 162, .cGma = gma2p15, .rTbl = HF3_rtbl98nit, .cTbl = HF3_ctbl98nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 105, .refBr = 172, .cGma = gma2p15, .rTbl = HF3_rtbl105nit, .cTbl = HF3_ctbl105nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 111, .refBr = 180, .cGma = gma2p15, .rTbl = HF3_rtbl111nit, .cTbl = HF3_ctbl111nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 119, .refBr = 190, .cGma = gma2p15, .rTbl = HF3_rtbl119nit, .cTbl = HF3_ctbl119nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 126, .refBr = 203, .cGma = gma2p15, .rTbl = HF3_rtbl126nit, .cTbl = HF3_ctbl126nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 134, .refBr = 214, .cGma = gma2p15, .rTbl = HF3_rtbl134nit, .cTbl = HF3_ctbl134nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 143, .refBr = 226, .cGma = gma2p15, .rTbl = HF3_rtbl143nit, .cTbl = HF3_ctbl143nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 152, .refBr = 242, .cGma = gma2p15, .rTbl = HF3_rtbl152nit, .cTbl = HF3_ctbl152nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 162, .refBr = 252, .cGma = gma2p15, .rTbl = HF3_rtbl162nit, .cTbl = HF3_ctbl162nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps6, .elv = HF3_elv6, .way = W1 },
	{ .br = 172, .refBr = 264, .cGma = gma2p15, .rTbl = HF3_rtbl172nit, .cTbl = HF3_ctbl172nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps6, .elv = HF3_elv6, .way = W1 },
	{ .br = 183, .refBr = 271, .cGma = gma2p15, .rTbl = HF3_rtbl183nit, .cTbl = HF3_ctbl183nit, .aid = HF3_aid3702, .elvCaps = HF3_elvCaps7, .elv = HF3_elv7, .way = W1 },
	{ .br = 195, .refBr = 271, .cGma = gma2p15, .rTbl = HF3_rtbl195nit, .cTbl = HF3_ctbl195nit, .aid = HF3_aid3016, .elvCaps = HF3_elvCaps7, .elv = HF3_elv7, .way = W1 },
	{ .br = 207, .refBr = 271, .cGma = gma2p15, .rTbl = HF3_rtbl207nit, .cTbl = HF3_ctbl207nit, .aid = HF3_aid2601, .elvCaps = HF3_elvCaps8, .elv = HF3_elv8, .way = W1 },
	{ .br = 220, .refBr = 271, .cGma = gma2p15, .rTbl = HF3_rtbl220nit, .cTbl = HF3_ctbl220nit, .aid = HF3_aid2023, .elvCaps = HF3_elvCaps8, .elv = HF3_elv8, .way = W1 },
	{ .br = 234, .refBr = 271, .cGma = gma2p15, .rTbl = HF3_rtbl234nit, .cTbl = HF3_ctbl234nit, .aid = HF3_aid1403, .elvCaps = HF3_elvCaps8, .elv = HF3_elv8, .way = W1 },
	{ .br = 249, .refBr = 276, .cGma = gma2p15, .rTbl = HF3_rtbl249nit, .cTbl = HF3_ctbl249nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps8, .elv = HF3_elv8, .way = W1 },
	{ .br = 265, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl265nit, .cTbl = HF3_ctbl265nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps9, .elv = HF3_elv9, .way = W1 },
	{ .br = 282, .refBr = 312, .cGma = gma2p15, .rTbl = HF3_rtbl282nit, .cTbl = HF3_ctbl282nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps9, .elv = HF3_elv9, .way = W1 },
	{ .br = 300, .refBr = 333, .cGma = gma2p15, .rTbl = HF3_rtbl300nit, .cTbl = HF3_ctbl300nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps10, .elv = HF3_elv10, .way = W1 },
	{ .br = 316, .refBr = 350, .cGma = gma2p15, .rTbl = HF3_rtbl316nit, .cTbl = HF3_ctbl316nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps10, .elv = HF3_elv10, .way = W1 },
	{ .br = 333, .refBr = 370, .cGma = gma2p15, .rTbl = HF3_rtbl333nit, .cTbl = HF3_ctbl333nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps11, .elv = HF3_elv11, .way = W1 },
	{ .br = 350, .refBr = 385, .cGma = gma2p15, .rTbl = HF3_rtbl350nit, .cTbl = HF3_ctbl350nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 357, .refBr = 394, .cGma = gma2p15, .rTbl = HF3_rtbl357nit, .cTbl = HF3_ctbl357nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 365, .refBr = 404, .cGma = gma2p15, .rTbl = HF3_rtbl365nit, .cTbl = HF3_ctbl365nit, .aid = HF3_aid1004, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 372, .refBr = 404, .cGma = gma2p15, .rTbl = HF3_rtbl372nit, .cTbl = HF3_ctbl372nit, .aid = HF3_aid798, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 380, .refBr = 404, .cGma = gma2p15, .rTbl = HF3_rtbl380nit, .cTbl = HF3_ctbl380nit, .aid = HF3_aid601, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 387, .refBr = 404, .cGma = gma2p15, .rTbl = HF3_rtbl387nit, .cTbl = HF3_ctbl387nit, .aid = HF3_aid399, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 395, .refBr = 404, .cGma = gma2p15, .rTbl = HF3_rtbl395nit, .cTbl = HF3_ctbl395nit, .aid = HF3_aid202, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 403, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl403nit, .cTbl = HF3_ctbl403nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 412, .refBr = 415, .cGma = gma2p15, .rTbl = HF3_rtbl412nit, .cTbl = HF3_ctbl412nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W1 },
	{ .br = 420, .refBr = 420, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W2 },
/*hbm interpolation */
	{ .br = 443, .refBr = 443, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 465, .refBr = 465, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 488, .refBr = 488, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 510, .refBr = 510, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 533, .refBr = 533, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps15, .elv = HF3_elv15, .way = W3},	// hbm is acl on
	{ .br = 555, .refBr = 555, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps16, .elv = HF3_elv16, .way = W3},	// hbm is acl on
	{ .br = 578, .refBr = 578, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps17, .elv = HF3_elv17, .way = W3},	// hbm is acl on
/* hbm */
	{ .br = 600, .refBr = 600, .cGma = gma2p20, .rTbl = HF3_rtbl420nit, .cTbl = HF3_ctbl420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps18, .elv = HF3_elv18, .way = W4 }
};


// aid sheet in rev.D
struct SmtDimInfo dimming_info_HF3_REVD[MAX_BR_INFO] = { // add hbm array
	{ .br = 2, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_2nit, .cTbl = HF3_ctbl_revd_2nit, .aid = HF3_aid9806, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 3, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_3nit, .cTbl = HF3_ctbl_revd_3nit, .aid = HF3_aid9686, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 4, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_4nit, .cTbl = HF3_ctbl_revd_4nit, .aid = HF3_aid9561, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 5, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_5nit, .cTbl = HF3_ctbl_revd_5nit, .aid = HF3_aid9441, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 6, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_6nit, .cTbl = HF3_ctbl_revd_6nit, .aid = HF3_aid9340, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 7, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_7nit, .cTbl = HF3_ctbl_revd_7nit, .aid = HF3_aid9251, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 8, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_8nit, .cTbl = HF3_ctbl_revd_8nit, .aid = HF3_aid9150, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 9, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_9nit, .cTbl = HF3_ctbl_revd_9nit, .aid = HF3_aid9068, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 10, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_10nit, .cTbl = HF3_ctbl_revd_10nit, .aid = HF3_aid8987, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 11, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_11nit, .cTbl = HF3_ctbl_revd_11nit, .aid = HF3_aid8898, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 12, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_12nit, .cTbl = HF3_ctbl_revd_12nit, .aid = HF3_aid8824, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 13, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_13nit, .cTbl = HF3_ctbl_revd_13nit, .aid = HF3_aid8754, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 14, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_14nit, .cTbl = HF3_ctbl_revd_14nit, .aid = HF3_aid8680, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 15, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_15nit, .cTbl = HF3_ctbl_revd_15nit, .aid = HF3_aid8587, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 16, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_16nit, .cTbl = HF3_ctbl_revd_16nit, .aid = HF3_aid8505, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 17, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_17nit, .cTbl = HF3_ctbl_revd_17nit, .aid = HF3_aid8405, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 19, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_19nit, .cTbl = HF3_ctbl_revd_19nit, .aid = HF3_aid8203, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 20, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_20nit, .cTbl = HF3_ctbl_revd_20nit, .aid = HF3_aid8117, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 21, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_21nit, .cTbl = HF3_ctbl_revd_21nit, .aid = HF3_aid8016, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 22, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_22nit, .cTbl = HF3_ctbl_revd_22nit, .aid = HF3_aid7908, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 24, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_24nit, .cTbl = HF3_ctbl_revd_24nit, .aid = HF3_aid7729, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 25, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_25nit, .cTbl = HF3_ctbl_revd_25nit, .aid = HF3_aid7620, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 27, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_27nit, .cTbl = HF3_ctbl_revd_27nit, .aid = HF3_aid7434, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 29, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_29nit, .cTbl = HF3_ctbl_revd_29nit, .aid = HF3_aid7224, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 30, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_30nit, .cTbl = HF3_ctbl_revd_30nit, .aid = HF3_aid7143, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 32, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_32nit, .cTbl = HF3_ctbl_revd_32nit, .aid = HF3_aid6960, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 34, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_34nit, .cTbl = HF3_ctbl_revd_34nit, .aid = HF3_aid6755, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 37, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_37nit, .cTbl = HF3_ctbl_revd_37nit, .aid = HF3_aid6444, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 39, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_39nit, .cTbl = HF3_ctbl_revd_39nit, .aid = HF3_aid6273, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 41, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_41nit, .cTbl = HF3_ctbl_revd_41nit, .aid = HF3_aid6068, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 44, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_44nit, .cTbl = HF3_ctbl_revd_44nit, .aid = HF3_aid5761, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 47, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_47nit, .cTbl = HF3_ctbl_revd_47nit, .aid = HF3_aid5446, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 50, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_50nit, .cTbl = HF3_ctbl_revd_50nit, .aid = HF3_aid5155, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 53, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_53nit, .cTbl = HF3_ctbl_revd_53nit, .aid = HF3_aid4852, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 56, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_56nit, .cTbl = HF3_ctbl_revd_56nit, .aid = HF3_aid4515, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 60, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_60nit, .cTbl = HF3_ctbl_revd_60nit, .aid = HF3_aid4057, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 64, .refBr = 117, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_64nit, .cTbl = HF3_ctbl_revd_64nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 68, .refBr = 122, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_68nit, .cTbl = HF3_ctbl_revd_68nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 72, .refBr = 127, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_72nit, .cTbl = HF3_ctbl_revd_72nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 77, .refBr = 135, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_77nit, .cTbl = HF3_ctbl_revd_77nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 82, .refBr = 143, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_82nit, .cTbl = HF3_ctbl_revd_82nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 87, .refBr = 151, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_87nit, .cTbl = HF3_ctbl_revd_87nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 93, .refBr = 160, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_93nit, .cTbl = HF3_ctbl_revd_93nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 98, .refBr = 168, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_98nit, .cTbl = HF3_ctbl_revd_98nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 105, .refBr = 180, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_105nit, .cTbl = HF3_ctbl_revd_105nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 111, .refBr = 189, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_111nit, .cTbl = HF3_ctbl_revd_111nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 119, .refBr = 201, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_119nit, .cTbl = HF3_ctbl_revd_119nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 126, .refBr = 211, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_126nit, .cTbl = HF3_ctbl_revd_126nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 134, .refBr = 222, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_134nit, .cTbl = HF3_ctbl_revd_134nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 143, .refBr = 236, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_143nit, .cTbl = HF3_ctbl_revd_143nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 152, .refBr = 250, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_152nit, .cTbl = HF3_ctbl_revd_152nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 162, .refBr = 262, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_162nit, .cTbl = HF3_ctbl_revd_162nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 172, .refBr = 278, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_172nit, .cTbl = HF3_ctbl_revd_172nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 183, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_183nit, .cTbl = HF3_ctbl_revd_183nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 195, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_195nit, .cTbl = HF3_ctbl_revd_195nit, .aid = HF3_aid3335, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 207, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_207nit, .cTbl = HF3_ctbl_revd_207nit, .aid = HF3_aid2853, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 220, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_220nit, .cTbl = HF3_ctbl_revd_220nit, .aid = HF3_aid2306, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 234, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_234nit, .cTbl = HF3_ctbl_revd_234nit, .aid = HF3_aid1727, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 249, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_249nit, .cTbl = HF3_ctbl_revd_249nit, .aid = HF3_aid1184, .elvCaps = HF3_elvCaps6, .elv = HF3_elv6, .way = W1 },
	{ .br = 265, .refBr = 308, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_265nit, .cTbl = HF3_ctbl_revd_265nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps7, .elv = HF3_elv7, .way = W1 },
	{ .br = 282, .refBr = 326, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_282nit, .cTbl = HF3_ctbl_revd_282nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps8, .elv = HF3_elv8, .way = W1 },
	{ .br = 300, .refBr = 342, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_300nit, .cTbl = HF3_ctbl_revd_300nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps9, .elv = HF3_elv9, .way = W1 },
	{ .br = 316, .refBr = 359, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_316nit, .cTbl = HF3_ctbl_revd_316nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps10, .elv = HF3_elv10, .way = W1 },
	{ .br = 333, .refBr = 372, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_333nit, .cTbl = HF3_ctbl_revd_333nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps11, .elv = HF3_elv11, .way = W1 },
	{ .br = 350, .refBr = 387, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_350nit, .cTbl = HF3_ctbl_revd_350nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 357, .refBr = 393, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_357nit, .cTbl = HF3_ctbl_revd_357nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 365, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_365nit, .cTbl = HF3_ctbl_revd_365nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 372, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_372nit, .cTbl = HF3_ctbl_revd_372nit, .aid = HF3_aid800, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 380, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_380nit, .cTbl = HF3_ctbl_revd_380nit, .aid = HF3_aid606, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 387, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_387nit, .cTbl = HF3_ctbl_revd_387nit, .aid = HF3_aid392, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 395, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_395nit, .cTbl = HF3_ctbl_revd_395nit, .aid = HF3_aid113, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 403, .refBr = 407, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_403nit, .cTbl = HF3_ctbl_revd_403nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 412, .refBr = 415, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_412nit, .cTbl = HF3_ctbl_revd_412nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W1 },
	{ .br = 420, .refBr = 420, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W2 },
/*hbm interpolation */
	{ .br = 443, .refBr = 443, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 465, .refBr = 465, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 488, .refBr = 488, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 510, .refBr = 510, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 533, .refBr = 533, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps15, .elv = HF3_elv15, .way = W3},	// hbm is acl on
	{ .br = 555, .refBr = 555, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps16, .elv = HF3_elv16, .way = W3},	// hbm is acl on
	{ .br = 578, .refBr = 578, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps17, .elv = HF3_elv17, .way = W3},	// hbm is acl on
/* hbm */
	{ .br = 600, .refBr = 600, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps18, .elv = HF3_elv18, .way = W4 }
};


static int set_gamma_to_center(struct SmtDimInfo *brInfo)
{
	int     i, j;
	int     ret = 0;
	unsigned int index = 0;
	unsigned char *result = brInfo->gamma;

	result[index++] = OLED_CMD_GAMMA;

	for (i = V255; i >= V0; i--) {
		for (j = 0; j < CI_MAX; j++) {
			if (i == V255) {
				result[index++] = (unsigned char)((center_gamma[i][j] >> 8) & 0x01);
				result[index++] = (unsigned char)center_gamma[i][j] & 0xff;
			}
			else {
				result[index++] = (unsigned char)center_gamma[i][j] & 0xff;
			}
		}
	}
	result[index++] = 0x00;
	result[index++] = 0x00;

	return ret;
}


static int set_gamma_to_hbm(struct SmtDimInfo *brInfo, u8 * hbm)
{
	int     ret = 0;
	unsigned int index = 0;
	unsigned char *result = brInfo->gamma;

	memset(result, 0, OLED_CMD_GAMMA_CNT);

	result[index++] = OLED_CMD_GAMMA;
	result[index++] = ((hbm[0] >> 2) & 0x1);
	result[index++] = hbm[1];
	result[index++] = ((hbm[0] >> 1) & 0x1);
	result[index++] = hbm[2];
	result[index++] = (hbm[0] & 0x1);
	result[index++] = hbm[3];
	memcpy(result + 7, hbm + 4, S6E3HF3_HBMGAMMA_LEN - 4);

	return ret;
}

/* gamma interpolaion table */
const unsigned int tbl_hbm_inter[7] = {
	131, 256, 387, 512, 643, 768, 899
};

static int interpolation_gamma_to_hbm(struct SmtDimInfo *dimInfo, int br_idx)
{
	int     i, j;
	int     ret = 0;
	int     idx = 0;
	int     tmp = 0;
	int     hbmcnt, refcnt, gap = 0;
	int     ref_idx = 0;
	int     hbm_idx = 0;
	int     rst = 0;
	int     hbm_tmp, ref_tmp;
	unsigned char *result = dimInfo[br_idx].gamma;

	for (i = 0; i < MAX_BR_INFO; i++) {
		if (dimInfo[i].br == S6E3HF3_MAX_BRIGHTNESS)
			ref_idx = i;

		if (dimInfo[i].br == S6E3HF3_HBM_BRIGHTNESS)
			hbm_idx = i;
	}

	if ((ref_idx == 0) || (hbm_idx == 0)) {
		dsim_info("%s failed to get index ref index : %d, hbm index : %d\n", __func__, ref_idx, hbm_idx);
		ret = -EINVAL;
		goto exit;
	}

	result[idx++] = OLED_CMD_GAMMA;
	tmp = (br_idx - ref_idx) - 1;

	hbmcnt = 1;
	refcnt = 1;

	for (i = V255; i >= V0; i--) {
		for (j = 0; j < CI_MAX; j++) {
			if (i == V255) {
				hbm_tmp = (dimInfo[hbm_idx].gamma[hbmcnt] << 8) | (dimInfo[hbm_idx].gamma[hbmcnt + 1]);
				ref_tmp = (dimInfo[ref_idx].gamma[refcnt] << 8) | (dimInfo[ref_idx].gamma[refcnt + 1]);

				if (hbm_tmp > ref_tmp) {
					gap = hbm_tmp - ref_tmp;
					rst = (gap * tbl_hbm_inter[tmp]) >> 10;
					rst += ref_tmp;
				}
				else {
					gap = ref_tmp - hbm_tmp;
					rst = (gap * tbl_hbm_inter[tmp]) >> 10;
					rst = ref_tmp - rst;
				}
				result[idx++] = (unsigned char)((rst >> 8) & 0x01);
				result[idx++] = (unsigned char)rst & 0xff;
				hbmcnt += 2;
				refcnt += 2;
			}
			else {
				hbm_tmp = dimInfo[hbm_idx].gamma[hbmcnt++];
				ref_tmp = dimInfo[ref_idx].gamma[refcnt++];

				if (hbm_tmp > ref_tmp) {
					gap = hbm_tmp - ref_tmp;
					rst = (gap * tbl_hbm_inter[tmp]) >> 10;
					rst += ref_tmp;
				}
				else {
					gap = ref_tmp - hbm_tmp;
					rst = (gap * tbl_hbm_inter[tmp]) >> 10;
					rst = ref_tmp - rst;
				}
				result[idx++] = (unsigned char)rst & 0xff;
			}
		}
	}

	dsim_info("tmp index : %d\n", tmp);

exit:
	return ret;
}

#ifdef CONFIG_PANEL_DUALIZATION
/* support a3 line panel efs data start */

#define OCTA_A3_REV1_DIM_FILE_NAME				"/efs/FactoryApp/a3_line_rev1.dat"
#define OCTA_A3_REV2_DIM_FILE_NAME				"/efs/FactoryApp/a3_line_rev2.dat"
#define OCTA_A2_REV2_DIM_FILE_NAME				"/efs/FactoryApp/a2_line_rev2.dat"

#define MAX_FILE_SIZE						30 * 1024
#define TEMP_BUF_SIZE						512

#define REF_GAMMA2_2 						220
#define REG_GAMMA2_15 						215

#define AID_REG_CNT							3

#define BASIC_PARAM_ELEMENT_CNT				7

#define COLOR_SHIFT_CNT						30
#define COLOR_PARAM_ELEMENT_CNT				31

#define REF_SHIFT_CNT						10
#define REG_PARAM_ELEMENT_CNT 				11

#define ELVSS_TEMP_COMP_CNT 				5
#define ELVSS_TEMP_ELEMENT_CNT 				6

#define ELVSS_MAX_INDEX 					2

#define HMT_AID_REG_CNT						4
#define HMT_BASIC_PARAM_ELEMENT_CNT			8

#define VINT_TBL_CNT 						10
#define VINT_ELEMENT_CNT 					11

#define INTERPOLATION_REG_CNT				2
#define INTERPOLATION_PARAM_CNT				4

#define PHASE_BASIC_STRING 					"basic data"
#define PHASE_REFERENCE_STRING 				"reference shift data"
#define PHASE_COLOR_STRING 					"color shift data"
#define PHASE_ELVSS_LOW_TEMP				"elvss low temperature"
#define PHASE_HMT_BASIC_STRING				"hmt basic data"
#define PHASE_HMT_REFERENCE_STRING 			"hmt reference shift data"
#define PHASE_HMT_COLOR_STRING 				"hmt color shift data"
#define PHASE_VINT_STRING					"vint data"
#define PHASE_AOR_INTERPOLATION_STRING		"aor interpolation"


#define PHASE_BASIC_INFO					0
#define PHASE_REFRENCE_SHIFT 				1
#define PHASE_COLOR_SHIFT 					2
#define PHASE_ELVSS_LOW_TEMP_COMP			3
#define PHASE_VINT							4
#define PHASE_AOR_INTERPOLATION				5
#define PHASE_HMT_BASIC_INFO				6
#define PHASE_HMT_REFRENCE_SHIFT			7
#define PHASE_HMT_COLOR_SHIFT				8


unsigned int zen_br_tbl_420_file[256] ={
        2,        3,        4,        4,        5,        6,        7,        8,        9,        10,        11,        12,
        13,        14,        15,        16,        19,        19,        19,        20,        21,        22,        24,
        25,        27,        27,        29,        30,        32,        32,        34,        34,        37,        37,
        41,        41,        41,        44,        44,        47,        47,        47,        50,        50,        53,
        53,        56,        56,        60,        60,        60,        64,        64,        64,        68,        68,
        68,        72,        72,        77,        77,        77,        77,        82,        82,        82,        87,
        87,        87,        87,        93,        93,        93,        93,        98,        98,        98,        105,
        105,        105,        105,        111,        111,        111,        111,        119,        119,        119,
        119,        119,        126,        126,        126,        126,        126,        134,        134,        134,
        134,        134,        143,        143,        143,        143,        143,        152,        152,        152,
        152,        152,        152,        162,        162,        162,        162,        162,        162,        172,
        172,        172,        172,        172,        183,        183,        183,        183,        183,        183,
        183,        195,        195,        195,        195,        195,        195,        195,        207,        207,
        207,        207,        207,        207,        207,        220,        220,        220,        220,        220,
        220,        220,        234,        234,        234,        234,        234,        234,        234,        234,
        249,        249,        249,        249,        249,        249,        249,        249,        265,        265,
        265,        265,        265,        265,        265,        265,        265,        282,        282,        282,
        282,        282,        282,        282,        282,        282,        300,        300,        300,        300,
        300,        300,        300,        300,        300,        300,        316,        316,        316,        316,
        316,        316,        316,        316,        333,        333,        333,        333,        333,        333,
        333,        333,        333,        350,        350,        350,        350,        350,        350,        350,
        350,        350,        357,        357,        357,        357,        365,        365,        365,        365,
        372,        372,        372,        380,        380,        380,        380,        387,        387,        387,
        387,        395,        395,        395,        395,        403,        403,        403,        403,        412,
        412,        412,        412,        420,        420,        420,        420,        420,
};


unsigned char zen_inter_aor_tbl_file[512] = {
	0x90,0xDE, 0x90,0xBF, 0x90,0xAF, 0x90,0x9F, 0x90,0x80, 0x90,0x66, 0x90,0x4F, 0x90,0x35, 0x90,0x20, 0x90,0x0B,
	0x80,0xF4, 0x80,0xE1, 0x80,0xCF, 0x80,0xBC, 0x80,0xA4, 0x80,0x8F, 0x80,0x75, 0x80,0x5B, 0x80,0x41, 0x80,0x2B,
	0x80,0x11, 0x70,0xF5, 0x70,0xC7, 0x70,0xAB, 0x70,0x93, 0x70,0x7B, 0x70,0x45, 0x70,0x30, 0x70,0x19, 0x70,0x01,
	0x60,0xE7, 0x60,0xCC, 0x60,0xA4, 0x60,0x7C, 0x60,0x50, 0x60,0x36, 0x60,0x1B, 0x50,0xF4, 0x50,0xCC, 0x50,0xB1,
	0x50,0x96, 0x50,0x7B, 0x50,0x56, 0x50,0x30, 0x50,0x09, 0x40,0xE2, 0x40,0xB7, 0x40,0x8B, 0x40,0x64, 0x40,0x3C,
	0x40,0x15, 0x40,0x1C, 0x30,0xEA, 0x30,0xD1, 0x40,0x19, 0x30,0xEA, 0x30,0xD3, 0x30,0xFF, 0x30,0xD3, 0x40,0x26,
	0x30,0xFC, 0x30,0xE8, 0x30,0xD3, 0x40,0x0C, 0x30,0xF8, 0x30,0xD1, 0x40,0x1B, 0x30,0xF6, 0x30,0xE3, 0x30,0xD1,
	0x40,0x16, 0x40,0x05, 0x30,0xE2, 0x30,0xD1, 0x40,0x02, 0x30,0xF2, 0x30,0xD1, 0x40,0x1D, 0x30,0xFF, 0x30,0xF0,
	0x30,0xD1, 0x40,0x0B, 0x30,0xFC, 0x30,0xDF, 0x30,0xD1, 0x40,0x22, 0x40,0x14, 0x30,0xF9, 0x30,0xEC, 0x30,0xD1,
	0x40,0x1D, 0x40,0x04, 0x30,0xF7, 0x30,0xDE, 0x30,0xD1, 0x40,0x19, 0x40,0x0D, 0x30,0xF5, 0x30,0xE9, 0x30,0xD1,
	0x40,0x1F, 0x40,0x09, 0x30,0xFE, 0x30,0xE7, 0x30,0xD1, 0x40,0x1B, 0x40,0x10, 0x30,0xFB, 0x30,0xF1, 0x30,0xDC,
	0x30,0xD1, 0x40,0x20, 0x40,0x16, 0x40,0x02, 0x30,0xEF, 0x30,0xE5, 0x30,0xD1, 0x40,0x05, 0x30,0xF2, 0x30,0xDF,
	0x30,0xCC, 0x30,0xBA, 0x40,0x20, 0x40,0x17, 0x40,0x06, 0x30,0xFD, 0x30,0xEB, 0x30,0xE3, 0x30,0xD1, 0x30,0xC0,
	0x30,0xAF, 0x30,0x9F, 0x30,0x8E, 0x30,0x7D, 0x30,0x6C, 0x30,0x5B, 0x30,0x49, 0x30,0x38, 0x30,0x26, 0x30,0x14,
	0x30,0x02, 0x20,0xF1, 0x20,0xDF, 0x20,0xCB, 0x20,0xB7, 0x20,0xA3, 0x20,0x8E, 0x20,0x7A, 0x20,0x66, 0x20,0x52,
	0x20,0x3F, 0x20,0x2D, 0x20,0x1A, 0x20,0x08, 0x10,0xF5, 0x10,0xE2, 0x10,0xCF, 0x10,0xBD, 0x10,0xAB, 0x10,0x9A,
	0x10,0x88, 0x10,0x77, 0x10,0x66, 0x10,0x54, 0x10,0x43, 0x10,0x31, 0x10,0x7D, 0x10,0x6C, 0x10,0x63, 0x10,0x52,
	0x10,0x40, 0x10,0x2F, 0x10,0x26, 0x10,0x14, 0x10,0x03, 0x10,0x7E, 0x10,0x6E, 0x10,0x5D, 0x10,0x4D, 0x10,0x45,
	0x10,0x34, 0x10,0x24, 0x10,0x13, 0x10,0x03, 0x10,0x7E, 0x10,0x6F, 0x10,0x67, 0x10,0x58, 0x10,0x48, 0x10,0x39,
	0x10,0x2A, 0x10,0x22, 0x10,0x12, 0x10,0x03, 0x10,0x6A, 0x10,0x5B, 0x10,0x4C, 0x10,0x3E, 0x10,0x2F, 0x10,0x20,
	0x10,0x12, 0x10,0x03, 0x10,0x6B, 0x10,0x5D, 0x10,0x4F, 0x10,0x42, 0x10,0x3B, 0x10,0x2D, 0x10,0x1F, 0x10,0x11,
	0x10,0x03, 0x10,0x66, 0x10,0x59, 0x10,0x4C, 0x10,0x3E, 0x10,0x38, 0x10,0x2B, 0x10,0x1D, 0x10,0x10, 0x10,0x03,
	0x10,0x23, 0x10,0x16, 0x10,0x10, 0x10,0x03, 0x10,0x29, 0x10,0x1C, 0x10,0x10, 0x10,0x03, 0x00,0xF1, 0x00,0xE0,
	0x00,0xCE, 0x00,0xC2, 0x00,0xB5, 0x00,0xA9, 0x00,0x9C, 0x00,0x8E, 0x00,0x81, 0x00,0x73, 0x00,0x65, 0x00,0x53,
	0x00,0x41, 0x00,0x2F, 0x00,0x1D, 0x00,0x30, 0x00,0x24, 0x00,0x17, 0x00,0x0A, 0x00,0x36, 0x00,0x23, 0x00,0x17,
	0x00,0x0A, 0x00,0x2F, 0x00,0x29, 0x00,0x1C, 0x00,0x0A, 0x00,0x0A
};


struct SmtDimInfo dimming_info_HF3_FILE[MAX_BR_INFO] = { // add hbm array
	{ .br = 2, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_2nit, .cTbl = HF3_ctbl_revd_2nit, .aid = HF3_aid9806, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 3, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_3nit, .cTbl = HF3_ctbl_revd_3nit, .aid = HF3_aid9686, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 4, .refBr = 100, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_4nit, .cTbl = HF3_ctbl_revd_4nit, .aid = HF3_aid9561, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 5, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_5nit, .cTbl = HF3_ctbl_revd_5nit, .aid = HF3_aid9441, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 6, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_6nit, .cTbl = HF3_ctbl_revd_6nit, .aid = HF3_aid9340, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 7, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_7nit, .cTbl = HF3_ctbl_revd_7nit, .aid = HF3_aid9251, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 8, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_8nit, .cTbl = HF3_ctbl_revd_8nit, .aid = HF3_aid9150, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 9, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_9nit, .cTbl = HF3_ctbl_revd_9nit, .aid = HF3_aid9068, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 10, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_10nit, .cTbl = HF3_ctbl_revd_10nit, .aid = HF3_aid8987, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 11, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_11nit, .cTbl = HF3_ctbl_revd_11nit, .aid = HF3_aid8898, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 12, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_12nit, .cTbl = HF3_ctbl_revd_12nit, .aid = HF3_aid8824, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 13, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_13nit, .cTbl = HF3_ctbl_revd_13nit, .aid = HF3_aid8754, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 14, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_14nit, .cTbl = HF3_ctbl_revd_14nit, .aid = HF3_aid8680, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 15, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_15nit, .cTbl = HF3_ctbl_revd_15nit, .aid = HF3_aid8587, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 16, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_16nit, .cTbl = HF3_ctbl_revd_16nit, .aid = HF3_aid8505, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 17, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_17nit, .cTbl = HF3_ctbl_revd_17nit, .aid = HF3_aid8405, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 19, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_19nit, .cTbl = HF3_ctbl_revd_19nit, .aid = HF3_aid8203, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 20, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_20nit, .cTbl = HF3_ctbl_revd_20nit, .aid = HF3_aid8117, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 21, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_21nit, .cTbl = HF3_ctbl_revd_21nit, .aid = HF3_aid8016, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 22, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_22nit, .cTbl = HF3_ctbl_revd_22nit, .aid = HF3_aid7908, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 24, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_24nit, .cTbl = HF3_ctbl_revd_24nit, .aid = HF3_aid7729, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 25, .refBr = 100, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_25nit, .cTbl = HF3_ctbl_revd_25nit, .aid = HF3_aid7620, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 27, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_27nit, .cTbl = HF3_ctbl_revd_27nit, .aid = HF3_aid7434, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 29, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_29nit, .cTbl = HF3_ctbl_revd_29nit, .aid = HF3_aid7224, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 30, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_30nit, .cTbl = HF3_ctbl_revd_30nit, .aid = HF3_aid7143, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 32, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_32nit, .cTbl = HF3_ctbl_revd_32nit, .aid = HF3_aid6960, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 34, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_34nit, .cTbl = HF3_ctbl_revd_34nit, .aid = HF3_aid6755, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 37, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_37nit, .cTbl = HF3_ctbl_revd_37nit, .aid = HF3_aid6444, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 39, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_39nit, .cTbl = HF3_ctbl_revd_39nit, .aid = HF3_aid6273, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 41, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_41nit, .cTbl = HF3_ctbl_revd_41nit, .aid = HF3_aid6068, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 44, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_44nit, .cTbl = HF3_ctbl_revd_44nit, .aid = HF3_aid5761, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 47, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_47nit, .cTbl = HF3_ctbl_revd_47nit, .aid = HF3_aid5446, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 50, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_50nit, .cTbl = HF3_ctbl_revd_50nit, .aid = HF3_aid5155, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 53, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_53nit, .cTbl = HF3_ctbl_revd_53nit, .aid = HF3_aid4852, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 56, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_56nit, .cTbl = HF3_ctbl_revd_56nit, .aid = HF3_aid4515, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 60, .refBr = 112, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_60nit, .cTbl = HF3_ctbl_revd_60nit, .aid = HF3_aid4057, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 64, .refBr = 100, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_64nit, .cTbl = HF3_ctbl_revd_64nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 68, .refBr = 122, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_68nit, .cTbl = HF3_ctbl_revd_68nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 72, .refBr = 127, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_72nit, .cTbl = HF3_ctbl_revd_72nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 77, .refBr = 135, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_77nit, .cTbl = HF3_ctbl_revd_77nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 82, .refBr = 143, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_82nit, .cTbl = HF3_ctbl_revd_82nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 87, .refBr = 151, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_87nit, .cTbl = HF3_ctbl_revd_87nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps1, .elv = HF3_elv1, .way = W1 },
	{ .br = 93, .refBr = 160, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_93nit, .cTbl = HF3_ctbl_revd_93nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 98, .refBr = 168, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_98nit, .cTbl = HF3_ctbl_revd_98nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 105, .refBr = 180, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_105nit, .cTbl = HF3_ctbl_revd_105nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 111, .refBr = 189, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_111nit, .cTbl = HF3_ctbl_revd_111nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps2, .elv = HF3_elv2, .way = W1 },
	{ .br = 119, .refBr = 201, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_119nit, .cTbl = HF3_ctbl_revd_119nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 126, .refBr = 211, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_126nit, .cTbl = HF3_ctbl_revd_126nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 134, .refBr = 222, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_134nit, .cTbl = HF3_ctbl_revd_134nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps3, .elv = HF3_elv3, .way = W1 },
	{ .br = 143, .refBr = 236, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_143nit, .cTbl = HF3_ctbl_revd_143nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 152, .refBr = 250, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_152nit, .cTbl = HF3_ctbl_revd_152nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 162, .refBr = 262, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_162nit, .cTbl = HF3_ctbl_revd_162nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 172, .refBr = 278, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_172nit, .cTbl = HF3_ctbl_revd_172nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 183, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_183nit, .cTbl = HF3_ctbl_revd_183nit, .aid = HF3_aid3793, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 195, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_195nit, .cTbl = HF3_ctbl_revd_195nit, .aid = HF3_aid3335, .elvCaps = HF3_elvCaps4, .elv = HF3_elv4, .way = W1 },
	{ .br = 207, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_207nit, .cTbl = HF3_ctbl_revd_207nit, .aid = HF3_aid2853, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 220, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_220nit, .cTbl = HF3_ctbl_revd_220nit, .aid = HF3_aid2306, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 234, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_234nit, .cTbl = HF3_ctbl_revd_234nit, .aid = HF3_aid1727, .elvCaps = HF3_elvCaps5, .elv = HF3_elv5, .way = W1 },
	{ .br = 249, .refBr = 294, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_249nit, .cTbl = HF3_ctbl_revd_249nit, .aid = HF3_aid1184, .elvCaps = HF3_elvCaps6, .elv = HF3_elv6, .way = W1 },
	{ .br = 265, .refBr = 308, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_265nit, .cTbl = HF3_ctbl_revd_265nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps7, .elv = HF3_elv7, .way = W1 },
	{ .br = 282, .refBr = 326, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_282nit, .cTbl = HF3_ctbl_revd_282nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps8, .elv = HF3_elv8, .way = W1 },
	{ .br = 300, .refBr = 342, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_300nit, .cTbl = HF3_ctbl_revd_300nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps9, .elv = HF3_elv9, .way = W1 },
	{ .br = 316, .refBr = 359, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_316nit, .cTbl = HF3_ctbl_revd_316nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps10, .elv = HF3_elv10, .way = W1 },
	{ .br = 333, .refBr = 372, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_333nit, .cTbl = HF3_ctbl_revd_333nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps11, .elv = HF3_elv11, .way = W1 },
	{ .br = 350, .refBr = 387, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_350nit, .cTbl = HF3_ctbl_revd_350nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 357, .refBr = 393, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_357nit, .cTbl = HF3_ctbl_revd_357nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 365, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_365nit, .cTbl = HF3_ctbl_revd_365nit, .aid = HF3_aid1005, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 372, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_372nit, .cTbl = HF3_ctbl_revd_372nit, .aid = HF3_aid800, .elvCaps = HF3_elvCaps12, .elv = HF3_elv12, .way = W1 },
	{ .br = 380, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_380nit, .cTbl = HF3_ctbl_revd_380nit, .aid = HF3_aid606, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 387, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_387nit, .cTbl = HF3_ctbl_revd_387nit, .aid = HF3_aid392, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 395, .refBr = 403, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_395nit, .cTbl = HF3_ctbl_revd_395nit, .aid = HF3_aid113, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 403, .refBr = 407, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_403nit, .cTbl = HF3_ctbl_revd_403nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps13, .elv = HF3_elv13, .way = W1 },
	{ .br = 412, .refBr = 415, .cGma = gma2p15, .rTbl = HF3_rtbl_revd_412nit, .cTbl = HF3_ctbl_revd_412nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W1 },
	{ .br = 420, .refBr = 420, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W2 },
/*hbm interpolation */
	{ .br = 443, .refBr = 443, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 465, .refBr = 465, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 488, .refBr = 488, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 510, .refBr = 510, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps14, .elv = HF3_elv14, .way = W3},	// hbm is acl on
	{ .br = 533, .refBr = 533, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps15, .elv = HF3_elv15, .way = W3},	// hbm is acl on
	{ .br = 555, .refBr = 555, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps16, .elv = HF3_elv16, .way = W3},	// hbm is acl on
	{ .br = 578, .refBr = 578, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps17, .elv = HF3_elv17, .way = W3},	// hbm is acl on
/* hbm */
	{ .br = 600, .refBr = 600, .cGma = gma2p20, .rTbl = HF3_rtbl_revd_420nit, .cTbl = HF3_ctbl_revd_420nit, .aid = HF3_aid39, .elvCaps = HF3_elvCaps18, .elv = HF3_elv18, .way = W4 }
};


static int read_buf_to_finalchar(char *buf, char *result, unsigned int offset)
{
	int i = 0;
	int exit = 1;
	char *tbuf = &buf[offset];

	while(exit) {
		if (tbuf[i] == 0)
			return i;
		else if (tbuf[i] == '\n')
			exit = 0;
		else
			result[i] = tbuf[i];

		i++;

		if (i > TEMP_BUF_SIZE-1)
			exit = 0;
		if (i + offset > MAX_FILE_SIZE - 1)
			exit = 0;
	}
	return i;
}



/* basic data format
 index, realbr, refbr, reference gamma, aid_reg_value0(hex), aid_reg_value1(hex) ,aid_reg_value2(hex)
 0, 2, 113, 220/215, aid[0], ... , aid[2]
*/

static int parse_basic_data(struct panel_private *panel, char *buf)
{
	int i;
	int ret = 0;
	int index, realbr, refbr, gamma;
	int aid[AID_REG_CNT];
	char *aidtbl = NULL;
	struct SmtDimInfo *diminfo = (struct SmtDimInfo *)panel->dim_info;

	ret = sscanf(buf, "%d, %d, %d, %d, %x, %x, %x",
			&index, &realbr, &refbr, &gamma,
			&aid[0], &aid[1], &aid[2]);
	if (ret != BASIC_PARAM_ELEMENT_CNT) {
		dsim_err("%s : invalid arg\n", __func__);
		ret = -EINVAL;
		goto parse_exit;
	}
	if (index > MAX_BR_INFO) {
		dsim_err("%s : invalid index : %d\n", __func__, index);
		ret = -EINVAL;
		goto parse_exit;
	}

#ifdef A3_LINE_DEBUG
	dsim_info("index : %d, realbr : %d, refbr :%d, gamma : %d, aid[0] : %x, aid[1] : %x, aid[2] : %x\n",
		index, realbr, refbr, gamma, aid[0], aid[1], aid[2]);
#endif

	diminfo[index].br = realbr;
	diminfo[index].refBr= refbr;
	if (gamma == REF_GAMMA2_2) {
		diminfo[index].cGma = gma2p20;
	} else {
		diminfo[index].cGma = gma2p15;
	}
	aidtbl = (char *)diminfo[index].aid;
	if (aidtbl) {
		for (i = 0; i < AID_REG_CNT; i++)
			aidtbl[i] = aid[i];
	}

parse_exit:
	return ret;
}

/* color shift data format
 index, value[0], ...,  value[20]
 0, 2, 113,
*/


static int parse_color_shift(struct panel_private *panel, char *buf)
{
	int i;
	int ret = 0;
	int index;
	int color[COLOR_SHIFT_CNT];
	char *colortbl = NULL;
#ifdef A3_LINE_DEBUG
	int cnt = 0;
	char strbuf[512]= {0, };
#endif
	struct SmtDimInfo *diminfo = (struct SmtDimInfo *)panel->dim_info;

	ret = sscanf(buf, "%d, \
			%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \
			%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \
			%d, %d, %d, %d, %d, %d, %d, %d, %d, %d", &index,
			&color[0], &color[1], &color[2], &color[3], &color[4],
			&color[5], &color[6], &color[7], &color[8], &color[9],
			&color[10], &color[11], &color[12], &color[13], &color[14],
			&color[15], &color[16], &color[17], &color[18], &color[19],
			&color[20], &color[21], &color[22], &color[23], &color[24],
			&color[25], &color[26], &color[27], &color[28], &color[29]);

	if (ret != COLOR_PARAM_ELEMENT_CNT) {
		dsim_err("%s : invalid arg\n", __func__);
		ret = -EINVAL;
		goto parse_exit;
	}
	if (index > MAX_BR_INFO) {
		dsim_err("%s : invalid index : %d\n", __func__, index);
		ret = -EINVAL;
		goto parse_exit;
	}

#ifdef A3_LINE_DEBUG
	cnt = sprintf(strbuf, "index : %d ", index);
	for (i = 0; i < COLOR_SHIFT_CNT; i++) {
		cnt += sprintf(strbuf+cnt, "%d ", color[i]);
	}
	dsim_info("%s\n",strbuf);
#endif

	colortbl = (char *)diminfo[index].cTbl;
	if (colortbl) {
		for (i = 0; i < COLOR_SHIFT_CNT; i++)
			colortbl[i] = color[i];
	}

parse_exit:
	return ret;
}

/* color shift data format
 index, value[0], ...,  value[20]
 0, 2, 113,
*/
static int parse_ref_shift(struct panel_private *panel, char *buf)
{
	int i;
	int ret = 0;
	int index;
	int shift[REF_SHIFT_CNT];
	char *shifttbl = NULL;
#ifdef A3_LINE_DEBUG
	int cnt = 0;
	char strbuf[512]= {0, };
#endif
	struct SmtDimInfo *diminfo = (struct SmtDimInfo *)panel->dim_info;

	ret = sscanf(buf, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			&index,
			&shift[0], &shift[1], &shift[2], &shift[3], &shift[4],
			&shift[5], &shift[6], &shift[7], &shift[8], &shift[9]);
	if (ret != REG_PARAM_ELEMENT_CNT) {
		dsim_err("%s : invalid arg\n", __func__);
		ret = -EINVAL;
		goto parse_exit;
	}
	if (index > MAX_BR_INFO) {
		dsim_err("%s : invalid index : %d\n", __func__, index);
		ret = -EINVAL;
		goto parse_exit;
	}

#ifdef A3_LINE_DEBUG
	cnt = sprintf(strbuf, "index : %d ", index);
	for (i = 0; i < REF_SHIFT_CNT; i++) {
		cnt += sprintf(strbuf+cnt, "%d ", shift[i]);
	}
	dsim_info("%s\n", strbuf);
#endif

	shifttbl = (char *)diminfo[index].rTbl;
	if (shifttbl) {
		for (i = 0; i < REF_SHIFT_CNT; i++)
			shifttbl[i] = shift[i];
	}

parse_exit:
	return ret;
}

/* elvss for low temp
 index, value[0], ...,  value[4]
 0, 2, 113,
*/


static int parse_elvss_low_temp(struct panel_private *panel, char *buf)
{
	int i;
	int ret = 0;
	int index;
	int elvss[ELVSS_TEMP_COMP_CNT];
#ifdef A3_LINE_DEBUG
	int cnt = 0;
	char strbuf[512]= {0, };
#endif

	ret = sscanf(buf, "%d, %d, %d, %d, %d, %d",
			&index,
			&elvss[0], &elvss[1], &elvss[2], &elvss[3], &elvss[4]);

	if (ret != ELVSS_TEMP_ELEMENT_CNT) {
		dsim_err("%s : invalid arg\n", __func__);
		ret = -EINVAL;
		goto parse_exit;
	}
	if (index > ELVSS_MAX_INDEX) {
		dsim_err("%s : invalid index : %d\n", __func__, index);
		ret = -EINVAL;
		goto parse_exit;
	}

#ifdef A3_LINE_DEBUG
	cnt = sprintf(strbuf, "index : %d ", index);
	for (i = 0; i < ELVSS_TEMP_COMP_CNT; i++) {
		cnt += sprintf(strbuf+cnt, "%d ", elvss[i]);
	}
	dsim_info("%s\n", strbuf);
#endif
	for (i = 0; i < ELVSS_TEMP_COMP_CNT; i++) 	{
		aid_dimming_dynamic.elvss_minus_offset[index][i] = elvss[i];
	}
	panel->a3_elvss_updated = 1;

parse_exit:
	return ret;
}

static int parse_vint_tbl(struct panel_private *panel, char *buf)
{
	int i;
	int ret = 0;
	int index;
	int vint[VINT_TBL_CNT];
#ifdef A3_LINE_DEBUG
	int cnt = 0;
	char strbuf[512]= {0, };
#endif

	ret = sscanf(buf, "%d, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x",
			&index,
			&vint[0], &vint[1], &vint[2], &vint[3], &vint[4],
			&vint[5], &vint[6], &vint[7], &vint[8], &vint[9]);
	if (ret != VINT_ELEMENT_CNT) {
		dsim_err("%s : invalid arg\n", __func__);
		ret = -EINVAL;
		goto parse_exit;
	}

#ifdef A3_LINE_DEBUG
	cnt = sprintf(strbuf, "index : %d ", index);
	for (i = 0; i < REF_SHIFT_CNT; i++) {
		cnt += sprintf(strbuf+cnt, "%d ", vint[i]);
	}
	dsim_info("%s\n", strbuf);
#endif
	for (i = 0; i < VINT_TBL_CNT; i++)
		aid_dimming_dynamic.vint_dim_offset[i] = vint[i];

parse_exit:
	return ret;
}
/* aor inter polation format
 index, brightness, aor[9], aor[10]
 0, 2, 0xXX, 0xXX,
*/

static int parse_aor_interpolation(struct panel_private *panel, char *buf)
{
	int ret = 0;
	int index, realbr;
	int aid[INTERPOLATION_REG_CNT] = {0x00, };

	ret = sscanf(buf, "%d, %d, %x, %x",
			&index, &realbr, &aid[0], &aid[1]);
	if (ret != INTERPOLATION_PARAM_CNT) {
		dsim_err("%s : invalid arg %d\n", __func__, ret);
		ret = -EINVAL;
		goto parse_exit;
	}
	if (index > UI_MAX_BRIGHTNESS) {
		dsim_err("%s : invalid index : %d\n", __func__, index);
		ret = -EINVAL;
		goto parse_exit;
	}

#ifdef A3_LINE_DEBUG
	dsim_info("index : %d, realbr : %d, aid[0] : %x, aid[1] : %x\n", index, realbr, aid[0], aid[1]);
#endif

	zen_br_tbl_420_file[index] = realbr;
	zen_inter_aor_tbl_file[index * 2] = aid[0];
	zen_inter_aor_tbl_file[index * 2 + 1] = aid[1];

parse_exit:
	return ret;
}


static int octa_a3_read_dim_data(struct panel_private *panel)
{
	int i;
	int ret;
	int filesize;
	int exit = 1;
	int offset = 0;
	int parsephase = 0;
	struct file* filp = NULL;
	mm_segment_t oldfs;
	char *filebuf = NULL;
	char tempbuf[TEMP_BUF_SIZE];
	struct dim_data *dimming;

#ifdef A3_LINE_DEBUG
	int j;
	int debugbufcnt;
	char debugbuf[512] = {0, };
#endif
	struct SmtDimInfo *diminfo = (struct SmtDimInfo *)panel->dim_info;

	int (*parse_fn[6])(struct panel_private*, char *) = {
		parse_basic_data,
		parse_ref_shift,
		parse_color_shift,
		parse_elvss_low_temp,
		parse_vint_tbl,
		parse_aor_interpolation,
	};

	dsim_info("%s was called\n", __func__);
	if(panel->strDatFile == NULL) {
		ret = -EINVAL;
		dsim_info("%s : file for octa a2/a3 is NULL\n", __func__);
		goto read_data_exit;
	}

	oldfs = get_fs();
	set_fs(get_ds());

	filp = filp_open(panel->strDatFile, O_RDONLY, 0);
	if (IS_ERR(filp)) {
		ret = -EAGAIN;
		dsim_info("%s : failed to file open for octa a2/a3 : %s\n", __func__, panel->strDatFile);
		goto read_data_exit;
	}
	dsim_info("flip open success\n");


	filebuf = kmalloc(MAX_FILE_SIZE, GFP_KERNEL);
	if (!filebuf) {
		dsim_err("%s : failed to kmalloc for file buffer\n", __func__);
		ret = -ENOMEM;
		goto exit_close;
	}

	memset(filebuf, 0, MAX_FILE_SIZE);

	filesize = filp->f_op->read(filp, filebuf, MAX_FILE_SIZE, &filp->f_pos);
	if (filesize <= 0) {
		dsim_err("%s : failed to read %s\n", __func__, panel->strDatFile);
		ret = -EINVAL;
		goto exit_kfree;
	}

#ifdef A3_LINE_DEBUG
	dsim_info("a3_line dimming data read count : %d\n", filesize);
#endif
	while(exit) {
		memset(tempbuf, 0, TEMP_BUF_SIZE);
		ret = read_buf_to_finalchar(filebuf, tempbuf, offset);
		tempbuf[TEMP_BUF_SIZE-1] = 0;
		dsim_info("read ret : %d\n", ret);
		if (ret == 0) {
			dsim_info("a3_line end of file\n");
			exit = 0;
		} else {
#ifdef A3_LINE_DEBUG
			dsim_info("str : %s\n", tempbuf);
#endif
			if (strncmp(tempbuf, PHASE_BASIC_STRING, strlen(PHASE_BASIC_STRING)) == 0) {
				dsim_info("a3 line parse basic info\n");
				parsephase = PHASE_BASIC_INFO;
			} else if (strncmp(tempbuf, PHASE_REFERENCE_STRING, strlen(PHASE_REFERENCE_STRING)) == 0) {
				dsim_info("a3 line parse reference info\n");
				parsephase = PHASE_REFRENCE_SHIFT;
			} else if (strncmp(tempbuf, PHASE_COLOR_STRING, strlen(PHASE_COLOR_STRING)) == 0) {
				dsim_info("a3 line parse color info\n");
				parsephase = PHASE_COLOR_SHIFT;
			} else if (strncmp(tempbuf, PHASE_ELVSS_LOW_TEMP, strlen(PHASE_ELVSS_LOW_TEMP)) == 0) {
				dsim_info("a3 line parse color info\n");
				parsephase = PHASE_ELVSS_LOW_TEMP_COMP;
			} else if (strncmp(tempbuf, PHASE_VINT_STRING, strlen(PHASE_VINT_STRING)) == 0) {
				dsim_info("a3 line parse VINT info\n");
				parsephase = PHASE_VINT;
			} else if (strncmp(tempbuf, PHASE_AOR_INTERPOLATION_STRING, strlen(PHASE_AOR_INTERPOLATION_STRING)) == 0) {
				dsim_info("a3 line parse aor interpolation info\n");
				parsephase = PHASE_AOR_INTERPOLATION;
			}
			else {
				parse_fn[parsephase](panel, tempbuf);
			}
			offset += ret;
		}
	}

#ifdef A3_LINE_DEBUG
	dsim_info("------------ ref br ------------ \n");
	for (i = 0; i < MAX_BR_INFO; i++) {
		dsim_info("real br : %d, ref br : %d\n", diminfo[i].br, diminfo[i].refBr);
	}

	dsim_info("------------ aid ------------ \n");
	for (i = 0; i < MAX_BR_INFO; i++) {
		dsim_info("index : %d, aid[0] : %x, aid[1] : %x, aid[2] : %x\n",
			i, diminfo[i].aid[0], diminfo[i].aid[1], diminfo[i].aid[2]);
	}
	dsim_info("------------ reference shift ------------ \n");

	for (i = 0; i < MAX_BR_INFO; i++) {
		debugbufcnt = 0;
		memset(debugbuf, 0, 512);
		for (j = 0 ; j < REF_SHIFT_CNT; j++) {
			debugbufcnt += sprintf(debugbuf+debugbufcnt, "%d ", diminfo[i].rTbl[j]);
		}
		dsim_info("%s\n", debugbuf);
	}

	dsim_info("------------ color shift ------------ \n");

	for (i = 0; i < MAX_BR_INFO; i++) {
		debugbufcnt = 0;
		memset(debugbuf, 0, 512);
		for (j = 0 ; j < COLOR_SHIFT_CNT; j++) {
			debugbufcnt += sprintf(debugbuf+debugbufcnt, "%d ", diminfo[i].cTbl[j]);
		}
		dsim_info("%s\n", debugbuf);
	}
	dsim_info("------------ aor interpolation ------------ \n");

	for (i = 0; i <= UI_MAX_BRIGHTNESS; i++) {
		debugbufcnt = 0;
		memset(debugbuf, 0, 512);
		debugbufcnt += sprintf(debugbuf, "code:%d nit:%d aor: %02x %02x",
			i, panel->br_tbl[i], panel->inter_aor_tbl[i * 2], panel->inter_aor_tbl[i * 2 + 1]);
		dsim_info("%s\n", debugbuf);
	}

#endif

	dimming = (struct dim_data *)panel->dim_data;
	if (!dimming) {
		dsim_err("%s : failed to get dimming data\n", __func__);
		goto exit_kfree;
	}

	for (i = 0; i < MAX_BR_INFO; i++) {
		if (diminfo[i].way == DIMMING_METHOD_AID) {
			ret = cal_gamma_from_index(dimming, &diminfo[i]);
			if (ret) {
				dsim_err("%s : failed to calculate gamma : index : %d\n", __func__, i);
				goto exit_kfree;
			}
		}
	}
#ifdef A3_LINE_DEBUG
	for (i = 0; i < MAX_BR_INFO; i++) {
		memset(debugbuf, 0, sizeof(debugbuf));
		debugbufcnt = sprintf(debugbuf, "gamma[%3d] : ",diminfo[i].br);

		for(j = 0; j < GAMMA_CMD_CNT; j++)
			debugbufcnt += sprintf(debugbuf + debugbufcnt, "%02x ", diminfo[i].gamma[j]);

		dsim_info("%s\n", debugbuf);
	}
#endif

exit_kfree:
	kfree(filebuf);

exit_close:
	filp_close(filp, NULL);
	set_fs(oldfs);

read_data_exit:
	return ret;

}

static void a3_line_read_work_fn(struct delayed_work *work)
{
	int ret = 0;
	struct panel_private *panel =
		container_of(work, struct panel_private, octa_a3_read_data_work);

	dsim_info("a3 line read data : %d\n", panel->octa_a3_read_cnt);

	ret = octa_a3_read_dim_data(panel);

	if (ret == -EAGAIN && panel->octa_a3_read_cnt) {

		queue_delayed_work(panel->octa_a3_read_data_work_q,
					&panel->octa_a3_read_data_work, msecs_to_jiffies(500));
		panel->octa_a3_read_cnt--;

	}
	else if (ret == 0) {
		dsim_info("a3 line read success \n");

	}

}

/* support a3 line panel efs data end */



#endif


static int init_dimming(struct dsim_device *dsim, u8 * mtp, u8 * hbm)
{
	int     i, j;
	int     pos = 0;
	int     ret = 0;
	short   temp;
	int     method = 0;
    static struct dim_data *dimming = NULL;
	unsigned char panel_rev = 0x00;
	unsigned char panel_line = 0x00;

	struct panel_private *panel = &dsim->priv;
	struct SmtDimInfo *diminfo = NULL;
	int     string_offset;
	char    string_buf[1024];

	if( dimming == NULL ) {
        dimming = (struct dim_data *) kmalloc(sizeof(struct dim_data), GFP_KERNEL);
        if (!dimming) {
	        dsim_err("failed to allocate memory for dim data\n");
	        ret = -ENOMEM;
	        goto error;
        }
	}
	panel_line = panel->id[0] & 0xF0;
	panel_rev = panel->id[2] & 0x0F;

	dsim_info("%s panel line : %x,  Panel rev : %x\n",__func__, panel_line, panel_rev);
	panel->hbm_inter_br_tbl = NULL;

	switch (dynamic_lcd_type) {
	case LCD_TYPE_S6E3HA2_WQHD:
		dsim_info("%s init dimming info for daisy HA2 rev.E panel\n", __func__);
		diminfo = (void *)dimming_info_HA3;
		panel->acl_opr_tbl = (unsigned char **)ACL_OPR_TABLE_HA2;
		memcpy(aid_dimming_dynamic.vint_dim_offset, VINT_TABLE_HA2, sizeof(aid_dimming_dynamic.vint_dim_offset));
		memcpy(aid_dimming_dynamic.elvss_minus_offset, ELVSS_OFFSET_HA2, sizeof(aid_dimming_dynamic.elvss_minus_offset));
		panel->br_tbl = (unsigned int *)br_tbl;
		panel->inter_aor_tbl = (unsigned char *)nb_inter_aor_tbl;
		break;
	case LCD_TYPE_S6E3HA3_WQHD:
		dsim_info("%s init dimming info for daisy HA3 pre panel\n", __func__);
		diminfo = (void *)dimming_info_HA3;
		panel->acl_opr_tbl = (unsigned char **)ACL_OPR_TABLE_HA3;
		memcpy(aid_dimming_dynamic.vint_dim_offset, VINT_TABLE_HA3, sizeof(aid_dimming_dynamic.vint_dim_offset));
		memcpy(aid_dimming_dynamic.elvss_minus_offset, ELVSS_OFFSET_HA3, sizeof(aid_dimming_dynamic.elvss_minus_offset));
		panel->br_tbl = (unsigned int *)nb_br_tbl_420;
		panel->hbm_inter_br_tbl = (unsigned int *)hbm_interpolation_br_tbl;
		panel->inter_aor_tbl = (unsigned char *)nb_inter_aor_tbl;
		break;
	case LCD_TYPE_S6E3HF3_WQHD:
		panel->octa_a3_read_data_work_q = NULL;
		panel->strDatFile = NULL;

		switch(panel_line) {
		// code
		case S6E3HF3_A2_INIT_ID:
		case S6E3HF3_A2_REV01_ID:
			panel->br_tbl = (unsigned int *)zen_br_tbl_420;
			if(panel_rev <= 3) {
				dsim_info("%s init dimming info for daisy HF3 panel under C\n", __func__);
				diminfo = (void *)dimming_info_HF3;
				panel->inter_aor_tbl = (unsigned char *)zen_inter_aor_tbl;
			} else {
				dsim_info("%s init dimming info for daisy HF3 panel over D\n", __func__);
				diminfo = (void *)dimming_info_HF3_REVD;
				panel->inter_aor_tbl = (unsigned char *)zen_inter_aor_tbl_revd;
			}
			break;
#ifdef CONFIG_PANEL_DUALIZATION
		// file
		case S6E3HF3_A2_REV02_ID:
			diminfo = (void *)dimming_info_HF3_FILE;
			panel->br_tbl = (unsigned int *)zen_br_tbl_420_file;
			panel->inter_aor_tbl = (unsigned char*)zen_inter_aor_tbl_file;
			panel->strDatFile = OCTA_A2_REV2_DIM_FILE_NAME;
			break;
		case S6E3HF3_A3_REV01_ID:
			diminfo = (void *)dimming_info_HF3_FILE;
			panel->br_tbl = (unsigned int *)zen_br_tbl_420_file;
			panel->inter_aor_tbl = (unsigned char*)zen_inter_aor_tbl_file;
			panel->strDatFile = OCTA_A3_REV1_DIM_FILE_NAME;
			break;
		case S6E3HF3_A3_REV02_ID:
			diminfo = (void *)dimming_info_HF3_FILE;
			panel->br_tbl = (unsigned int *)zen_br_tbl_420_file;
			panel->inter_aor_tbl = (unsigned char*)zen_inter_aor_tbl_file;
			panel->strDatFile = OCTA_A3_REV2_DIM_FILE_NAME;
			break;
#endif
		default:
			panel->br_tbl = (unsigned int *)zen_br_tbl_420;
			dsim_info("%s init dimming info for invalid line id\n", __func__);
			diminfo = (void *)dimming_info_HF3_REVD;
			panel->inter_aor_tbl = (unsigned char *)zen_inter_aor_tbl_revd;
			break;
		}

		panel->acl_opr_tbl = (unsigned char **)ACL_OPR_TABLE_HF3;
		memcpy(aid_dimming_dynamic.vint_dim_offset, VINT_TABLE_HF3, sizeof(aid_dimming_dynamic.vint_dim_offset));
		memcpy(aid_dimming_dynamic.elvss_minus_offset, ELVSS_OFFSET_HF3, sizeof(aid_dimming_dynamic.elvss_minus_offset));
		panel->hbm_inter_br_tbl = (unsigned int *)hbm_interpolation_br_tbl;
		break;
	default:
		dsim_info("%s init dimming info for daisy (UNKNOWN) HA2 panel\n", __func__);
		diminfo = (void *)dimming_info_HA3;
		panel->acl_opr_tbl = (unsigned char **)ACL_OPR_TABLE_HA2;
		memcpy(aid_dimming_dynamic.vint_dim_offset, VINT_TABLE_HA2, sizeof(aid_dimming_dynamic.vint_dim_offset));
		memcpy(aid_dimming_dynamic.elvss_minus_offset, ELVSS_OFFSET_HA2, sizeof(aid_dimming_dynamic.elvss_minus_offset));
		panel->br_tbl = (unsigned int *)br_tbl;
		panel->inter_aor_tbl = (unsigned char *)nb_inter_aor_tbl;
		break;
	}


	panel->dim_data = (void *)dimming;
	panel->dim_info = (void *)diminfo;

	panel->hbm_tbl = (unsigned char **)HBM_TABLE;
	panel->acl_cutoff_tbl = (unsigned char **)ACL_CUTOFF_TABLE;
	panel->hbm_index = MAX_BR_INFO - 1;
	panel->hbm_elvss_comp = S6E3HA2_HBM_ELVSS_COMP;

	for (j = 0; j < CI_MAX; j++) {
		temp = ((mtp[pos] & 0x01) ? -1 : 1) * mtp[pos + 1];
		dimming->t_gamma[V255][j] = (int)center_gamma[V255][j] + temp;
		dimming->mtp[V255][j] = temp;
		pos += 2;
	}

	for (i = V203; i >= V0; i--) {
		for (j = 0; j < CI_MAX; j++) {
			temp = ((mtp[pos] & 0x80) ? -1 : 1) * (mtp[pos] & 0x7f);
			dimming->t_gamma[i][j] = (int)center_gamma[i][j] + temp;
			dimming->mtp[i][j] = temp;
			pos++;
		}
	}
	/* for vt */
	temp = (mtp[pos + 1]) << 8 | mtp[pos];

	for (i = 0; i < CI_MAX; i++)
		dimming->vt_mtp[i] = (temp >> (i * 4)) & 0x0f;
#ifdef SMART_DIMMING_DEBUG
	dimm_info("Center Gamma Info : \n");
	for (i = 0; i < VMAX; i++) {
		dsim_info("Gamma : %3d %3d %3d : %3x %3x %3x\n",
			dimming->t_gamma[i][CI_RED], dimming->t_gamma[i][CI_GREEN], dimming->t_gamma[i][CI_BLUE],
			dimming->t_gamma[i][CI_RED], dimming->t_gamma[i][CI_GREEN], dimming->t_gamma[i][CI_BLUE]);
	}
#endif
	dimm_info("VT MTP : \n");
	dimm_info("Gamma : %3d %3d %3d : %3x %3x %3x\n",
		dimming->vt_mtp[CI_RED], dimming->vt_mtp[CI_GREEN], dimming->vt_mtp[CI_BLUE],
		dimming->vt_mtp[CI_RED], dimming->vt_mtp[CI_GREEN], dimming->vt_mtp[CI_BLUE]);

	dimm_info("MTP Info : \n");
	for (i = 0; i < VMAX; i++) {
		dimm_info("Gamma : %3d %3d %3d : %3x %3x %3x\n",
			dimming->mtp[i][CI_RED], dimming->mtp[i][CI_GREEN], dimming->mtp[i][CI_BLUE],
			dimming->mtp[i][CI_RED], dimming->mtp[i][CI_GREEN], dimming->mtp[i][CI_BLUE]);
	}

	ret = generate_volt_table(dimming);
	if (ret) {
		dimm_err("[ERR:%s] failed to generate volt table\n", __func__);
		goto error;
	}

	for (i = 0; i < MAX_BR_INFO; i++) {
		method = diminfo[i].way;

		if (method == DIMMING_METHOD_FILL_CENTER) {
			ret = set_gamma_to_center(&diminfo[i]);
			if (ret) {
				dsim_err("%s : failed to get center gamma\n", __func__);
				goto error;
			}
		}
		else if (method == DIMMING_METHOD_FILL_HBM) {
			ret = set_gamma_to_hbm(&diminfo[i], hbm);
			if (ret) {
				dsim_err("%s : failed to get hbm gamma\n", __func__);
				goto error;
			}
		}
	}

	for (i = 0; i < MAX_BR_INFO; i++) {
		method = diminfo[i].way;
		if (method == DIMMING_METHOD_AID) {
			ret = cal_gamma_from_index(dimming, &diminfo[i]);
			if (ret) {
				dsim_err("%s : failed to calculate gamma : index : %d\n", __func__, i);
				goto error;
			}
		}
		if (method == DIMMING_METHOD_INTERPOLATION) {
			ret = interpolation_gamma_to_hbm(diminfo, i);
			if (ret) {
				dsim_err("%s : failed to calculate gamma : index : %d\n", __func__, i);
				goto error;
			}
		}
	}

	for (i = 0; i < MAX_BR_INFO; i++) {
		memset(string_buf, 0, sizeof(string_buf));
		string_offset = sprintf(string_buf, "gamma[%3d] : ", diminfo[i].br);

		for (j = 0; j < GAMMA_CMD_CNT; j++)
			string_offset += sprintf(string_buf + string_offset, "%02x ", diminfo[i].gamma[j]);

		dsim_info("%s\n", string_buf);
	}
error:
	return ret;

}


#ifdef CONFIG_LCD_HMT
static const unsigned int hmt_br_tbl[256] = {
	10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 12, 12,
	13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 17, 17, 19,
	19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 23, 23, 25, 25, 25,
	25, 25, 27, 27, 27, 27, 27, 29, 29, 29, 29, 29, 31, 31, 31, 31,
	31, 33, 33, 33, 33, 35, 35, 35, 35, 35, 37, 37, 37, 37, 37, 39,
	39, 39, 39, 39, 41, 41, 41, 41, 41, 41, 41, 44, 44, 44, 44, 44,
	44, 44, 44, 47, 47, 47, 47, 47, 47, 47, 50, 50, 50, 50, 50, 50,
	50, 53, 53, 53, 53, 53, 53, 53, 56, 56, 56, 56, 56, 56, 56, 56,
	56, 56, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 68, 68, 68, 68, 68, 68, 68, 68, 68, 72,
	72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 77, 77, 77, 77, 77,
	77, 77, 77, 77, 77, 77, 77, 77, 82, 82, 82, 82, 82, 82, 82, 82,
	82, 82, 82, 82, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,
	87, 87, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
	93, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 105
};

struct SmtDimInfo hmt_dimming_info_HA3[HMT_MAX_BR_INFO] = {
	{.br = 10, .refBr = 44, .cGma = gma2p15, .rTbl = HA3_HMTrtbl10nit, .cTbl = HA3_HMTctbl10nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 11, .refBr = 48, .cGma = gma2p15, .rTbl = HA3_HMTrtbl11nit, .cTbl = HA3_HMTctbl11nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 12, .refBr = 57, .cGma = gma2p15, .rTbl = HA3_HMTrtbl12nit, .cTbl = HA3_HMTctbl12nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 13, .refBr = 61, .cGma = gma2p15, .rTbl = HA3_HMTrtbl13nit, .cTbl = HA3_HMTctbl13nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 14, .refBr = 65, .cGma = gma2p15, .rTbl = HA3_HMTrtbl14nit, .cTbl = HA3_HMTctbl14nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 15, .refBr = 69, .cGma = gma2p15, .rTbl = HA3_HMTrtbl15nit, .cTbl = HA3_HMTctbl15nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 16, .refBr = 73, .cGma = gma2p15, .rTbl = HA3_HMTrtbl16nit, .cTbl = HA3_HMTctbl16nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 17, .refBr = 75, .cGma = gma2p15, .rTbl = HA3_HMTrtbl17nit, .cTbl = HA3_HMTctbl17nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 19, .refBr = 84, .cGma = gma2p15, .rTbl = HA3_HMTrtbl19nit, .cTbl = HA3_HMTctbl19nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 20, .refBr = 87, .cGma = gma2p15, .rTbl = HA3_HMTrtbl20nit, .cTbl = HA3_HMTctbl20nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 21, .refBr = 89, .cGma = gma2p15, .rTbl = HA3_HMTrtbl21nit, .cTbl = HA3_HMTctbl21nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 22, .refBr = 96, .cGma = gma2p15, .rTbl = HA3_HMTrtbl22nit, .cTbl = HA3_HMTctbl22nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 23, .refBr = 101, .cGma = gma2p15, .rTbl = HA3_HMTrtbl23nit, .cTbl = HA3_HMTctbl23nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 25, .refBr = 107, .cGma = gma2p15, .rTbl = HA3_HMTrtbl25nit, .cTbl = HA3_HMTctbl25nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 27, .refBr = 114, .cGma = gma2p15, .rTbl = HA3_HMTrtbl27nit, .cTbl = HA3_HMTctbl27nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 29, .refBr = 121, .cGma = gma2p15, .rTbl = HA3_HMTrtbl29nit, .cTbl = HA3_HMTctbl29nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 31, .refBr = 128, .cGma = gma2p15, .rTbl = HA3_HMTrtbl31nit, .cTbl = HA3_HMTctbl31nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 33, .refBr = 136, .cGma = gma2p15, .rTbl = HA3_HMTrtbl33nit, .cTbl = HA3_HMTctbl33nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 35, .refBr = 142, .cGma = gma2p15, .rTbl = HA3_HMTrtbl35nit, .cTbl = HA3_HMTctbl35nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 37, .refBr = 150, .cGma = gma2p15, .rTbl = HA3_HMTrtbl37nit, .cTbl = HA3_HMTctbl37nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 39, .refBr = 156, .cGma = gma2p15, .rTbl = HA3_HMTrtbl39nit, .cTbl = HA3_HMTctbl39nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 41, .refBr = 165, .cGma = gma2p15, .rTbl = HA3_HMTrtbl41nit, .cTbl = HA3_HMTctbl41nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 44, .refBr = 179, .cGma = gma2p15, .rTbl = HA3_HMTrtbl44nit, .cTbl = HA3_HMTctbl44nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 47, .refBr = 188, .cGma = gma2p15, .rTbl = HA3_HMTrtbl47nit, .cTbl = HA3_HMTctbl47nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 50, .refBr = 199, .cGma = gma2p15, .rTbl = HA3_HMTrtbl50nit, .cTbl = HA3_HMTctbl50nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 53, .refBr = 211, .cGma = gma2p15, .rTbl = HA3_HMTrtbl53nit, .cTbl = HA3_HMTctbl53nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 56, .refBr = 220, .cGma = gma2p15, .rTbl = HA3_HMTrtbl56nit, .cTbl = HA3_HMTctbl56nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 60, .refBr = 233, .cGma = gma2p15, .rTbl = HA3_HMTrtbl60nit, .cTbl = HA3_HMTctbl60nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 64, .refBr = 246, .cGma = gma2p15, .rTbl = HA3_HMTrtbl64nit, .cTbl = HA3_HMTctbl64nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 68, .refBr = 260, .cGma = gma2p15, .rTbl = HA3_HMTrtbl68nit, .cTbl = HA3_HMTctbl68nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 72, .refBr = 274, .cGma = gma2p15, .rTbl = HA3_HMTrtbl72nit, .cTbl = HA3_HMTctbl72nit, .aid = HA3_HMTaid8004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 77, .refBr = 211, .cGma = gma2p15, .rTbl = HA3_HMTrtbl77nit, .cTbl = HA3_HMTctbl77nit, .aid = HA3_HMTaid7004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 82, .refBr = 222, .cGma = gma2p15, .rTbl = HA3_HMTrtbl82nit, .cTbl = HA3_HMTctbl82nit, .aid = HA3_HMTaid7004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 87, .refBr = 235, .cGma = gma2p15, .rTbl = HA3_HMTrtbl87nit, .cTbl = HA3_HMTctbl87nit, .aid = HA3_HMTaid7004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 93, .refBr = 248, .cGma = gma2p15, .rTbl = HA3_HMTrtbl93nit, .cTbl = HA3_HMTctbl93nit, .aid = HA3_HMTaid7004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 99, .refBr = 263, .cGma = gma2p15, .rTbl = HA3_HMTrtbl99nit, .cTbl = HA3_HMTctbl99nit, .aid = HA3_HMTaid7004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
	{.br = 105, .refBr = 274, .cGma = gma2p15, .rTbl = HA3_HMTrtbl105nit, .cTbl = HA3_HMTctbl105nit, .aid = HA3_HMTaid7004, .elvCaps = HA3_HMTelvCaps, .elv = HA3_HMTelv},
};

struct SmtDimInfo hmt_dimming_info_HF3[HMT_MAX_BR_INFO] = {
	{.br = 10, .refBr = 39, .cGma = gma2p15, .rTbl = HF3_HMTrtbl10nit, .cTbl = HF3_HMTctbl10nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 11, .refBr = 44, .cGma = gma2p15, .rTbl = HF3_HMTrtbl11nit, .cTbl = HF3_HMTctbl11nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 12, .refBr = 48, .cGma = gma2p15, .rTbl = HF3_HMTrtbl12nit, .cTbl = HF3_HMTctbl12nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 13, .refBr = 52, .cGma = gma2p15, .rTbl = HF3_HMTrtbl13nit, .cTbl = HF3_HMTctbl13nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 14, .refBr = 56, .cGma = gma2p15, .rTbl = HF3_HMTrtbl14nit, .cTbl = HF3_HMTctbl14nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 15, .refBr = 60, .cGma = gma2p15, .rTbl = HF3_HMTrtbl15nit, .cTbl = HF3_HMTctbl15nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 16, .refBr = 63, .cGma = gma2p15, .rTbl = HF3_HMTrtbl16nit, .cTbl = HF3_HMTctbl16nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 17, .refBr = 66, .cGma = gma2p15, .rTbl = HF3_HMTrtbl17nit, .cTbl = HF3_HMTctbl17nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 19, .refBr = 74, .cGma = gma2p15, .rTbl = HF3_HMTrtbl19nit, .cTbl = HF3_HMTctbl19nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 20, .refBr = 77, .cGma = gma2p15, .rTbl = HF3_HMTrtbl20nit, .cTbl = HF3_HMTctbl20nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 21, .refBr = 82, .cGma = gma2p15, .rTbl = HF3_HMTrtbl21nit, .cTbl = HF3_HMTctbl21nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 22, .refBr = 85, .cGma = gma2p15, .rTbl = HF3_HMTrtbl22nit, .cTbl = HF3_HMTctbl22nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 23, .refBr = 87, .cGma = gma2p15, .rTbl = HF3_HMTrtbl23nit, .cTbl = HF3_HMTctbl23nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 25, .refBr = 95, .cGma = gma2p15, .rTbl = HF3_HMTrtbl25nit, .cTbl = HF3_HMTctbl25nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 27, .refBr = 102, .cGma = gma2p15, .rTbl = HF3_HMTrtbl27nit, .cTbl = HF3_HMTctbl27nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 29, .refBr = 107, .cGma = gma2p15, .rTbl = HF3_HMTrtbl29nit, .cTbl = HF3_HMTctbl29nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 31, .refBr = 114, .cGma = gma2p15, .rTbl = HF3_HMTrtbl31nit, .cTbl = HF3_HMTctbl31nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 33, .refBr = 120, .cGma = gma2p15, .rTbl = HF3_HMTrtbl33nit, .cTbl = HF3_HMTctbl33nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 35, .refBr = 126, .cGma = gma2p15, .rTbl = HF3_HMTrtbl35nit, .cTbl = HF3_HMTctbl35nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 37, .refBr = 131, .cGma = gma2p15, .rTbl = HF3_HMTrtbl37nit, .cTbl = HF3_HMTctbl37nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 39, .refBr = 138, .cGma = gma2p15, .rTbl = HF3_HMTrtbl39nit, .cTbl = HF3_HMTctbl39nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 41, .refBr = 147, .cGma = gma2p15, .rTbl = HF3_HMTrtbl41nit, .cTbl = HF3_HMTctbl41nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 44, .refBr = 157, .cGma = gma2p15, .rTbl = HF3_HMTrtbl44nit, .cTbl = HF3_HMTctbl44nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 47, .refBr = 165, .cGma = gma2p15, .rTbl = HF3_HMTrtbl47nit, .cTbl = HF3_HMTctbl47nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 50, .refBr = 175, .cGma = gma2p15, .rTbl = HF3_HMTrtbl50nit, .cTbl = HF3_HMTctbl50nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 53, .refBr = 184, .cGma = gma2p15, .rTbl = HF3_HMTrtbl53nit, .cTbl = HF3_HMTctbl53nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 56, .refBr = 192, .cGma = gma2p15, .rTbl = HF3_HMTrtbl56nit, .cTbl = HF3_HMTctbl56nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 60, .refBr = 204, .cGma = gma2p15, .rTbl = HF3_HMTrtbl60nit, .cTbl = HF3_HMTctbl60nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 64, .refBr = 218, .cGma = gma2p15, .rTbl = HF3_HMTrtbl64nit, .cTbl = HF3_HMTctbl64nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 68, .refBr = 230, .cGma = gma2p15, .rTbl = HF3_HMTrtbl68nit, .cTbl = HF3_HMTctbl68nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 72, .refBr = 241, .cGma = gma2p15, .rTbl = HF3_HMTrtbl72nit, .cTbl = HF3_HMTctbl72nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 77, .refBr = 186, .cGma = gma2p15, .rTbl = HF3_HMTrtbl77nit, .cTbl = HF3_HMTctbl77nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 82, .refBr = 199, .cGma = gma2p15, .rTbl = HF3_HMTrtbl82nit, .cTbl = HF3_HMTctbl82nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 87, .refBr = 209, .cGma = gma2p15, .rTbl = HF3_HMTrtbl87nit, .cTbl = HF3_HMTctbl87nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 93, .refBr = 223, .cGma = gma2p15, .rTbl = HF3_HMTrtbl93nit, .cTbl = HF3_HMTctbl93nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 99, .refBr = 234, .cGma = gma2p15, .rTbl = HF3_HMTrtbl99nit, .cTbl = HF3_HMTctbl99nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 105, .refBr = 247, .cGma = gma2p15, .rTbl = HF3_HMTrtbl105nit, .cTbl = HF3_HMTctbl105nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
};

struct SmtDimInfo hmt_dimming_info_HF3_A3_REV1[HMT_MAX_BR_INFO] = {
	{.br = 10, .refBr = 40, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl10nit, .cTbl = HF3_A3_HMTctbl10nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 11, .refBr = 44, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl11nit, .cTbl = HF3_A3_HMTctbl11nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 12, .refBr = 47, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl12nit, .cTbl = HF3_A3_HMTctbl12nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 13, .refBr = 52, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl13nit, .cTbl = HF3_A3_HMTctbl13nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 14, .refBr = 56, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl14nit, .cTbl = HF3_A3_HMTctbl14nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 15, .refBr = 60, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl15nit, .cTbl = HF3_A3_HMTctbl15nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 16, .refBr = 63, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl16nit, .cTbl = HF3_A3_HMTctbl16nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 17, .refBr = 67, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl17nit, .cTbl = HF3_A3_HMTctbl17nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 19, .refBr = 76, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl19nit, .cTbl = HF3_A3_HMTctbl19nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 20, .refBr = 79, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl20nit, .cTbl = HF3_A3_HMTctbl20nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 21, .refBr = 83, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl21nit, .cTbl = HF3_A3_HMTctbl21nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 22, .refBr = 88, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl22nit, .cTbl = HF3_A3_HMTctbl22nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 23, .refBr = 91, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl23nit, .cTbl = HF3_A3_HMTctbl23nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 25, .refBr = 97, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl25nit, .cTbl = HF3_A3_HMTctbl25nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 27, .refBr = 103, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl27nit, .cTbl = HF3_A3_HMTctbl27nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 29, .refBr = 113, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl29nit, .cTbl = HF3_A3_HMTctbl29nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 31, .refBr = 117, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl31nit, .cTbl = HF3_A3_HMTctbl31nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 33, .refBr = 123, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl33nit, .cTbl = HF3_A3_HMTctbl33nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 35, .refBr = 130, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl35nit, .cTbl = HF3_A3_HMTctbl35nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 37, .refBr = 137, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl37nit, .cTbl = HF3_A3_HMTctbl37nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 39, .refBr = 144, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl39nit, .cTbl = HF3_A3_HMTctbl39nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 41, .refBr = 152, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl41nit, .cTbl = HF3_A3_HMTctbl41nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 44, .refBr = 164, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl44nit, .cTbl = HF3_A3_HMTctbl44nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 47, .refBr = 174, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl47nit, .cTbl = HF3_A3_HMTctbl47nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 50, .refBr = 183, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl50nit, .cTbl = HF3_A3_HMTctbl50nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 53, .refBr = 195, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl53nit, .cTbl = HF3_A3_HMTctbl53nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 56, .refBr = 205, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl56nit, .cTbl = HF3_A3_HMTctbl56nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 60, .refBr = 218, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl60nit, .cTbl = HF3_A3_HMTctbl60nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 64, .refBr = 229, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl64nit, .cTbl = HF3_A3_HMTctbl64nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 68, .refBr = 243, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl68nit, .cTbl = HF3_A3_HMTctbl68nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 72, .refBr = 256, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl72nit, .cTbl = HF3_A3_HMTctbl72nit, .aid = HF3_HMTaid8001, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 77, .refBr = 197, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl77nit, .cTbl = HF3_A3_HMTctbl77nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 82, .refBr = 207, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl82nit, .cTbl = HF3_A3_HMTctbl82nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 87, .refBr = 222, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl87nit, .cTbl = HF3_A3_HMTctbl87nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 93, .refBr = 236, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl93nit, .cTbl = HF3_A3_HMTctbl93nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 99, .refBr = 249, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl99nit, .cTbl = HF3_A3_HMTctbl99nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
	{.br = 105, .refBr = 262, .cGma = gma2p15, .rTbl = HF3_A3_HMTrtbl105nit, .cTbl = HF3_A3_HMTctbl105nit, .aid = HF3_HMTaid6999, .elvCaps = HF3_HMTelvCaps, .elv = HF3_HMTelv},
};

static int hmt_init_dimming(struct dsim_device *dsim, u8 * mtp)
{
	int     i, j;
	int     pos = 0;
	int     ret = 0;
	short   temp;
	static struct dim_data *dimming = NULL;
	struct panel_private *panel = &dsim->priv;
	struct SmtDimInfo *diminfo = NULL;
	unsigned char panel_line = panel->id[0] & 0xF0;

	if( dimming == NULL ) {
		dimming = (struct dim_data *) kmalloc(sizeof(struct dim_data), GFP_KERNEL);
		if (!dimming) {
			dsim_err("failed to allocate memory for dim data\n");
			ret = -ENOMEM;
			goto error;
		}
	}
	switch (dynamic_lcd_type) {
	case LCD_TYPE_S6E3HA3_WQHD:
		dsim_info("%s init HMT dimming info for HA3 panel\n", __func__);
		diminfo = (void *) hmt_dimming_info_HA3;
		break;
	case LCD_TYPE_S6E3HF3_WQHD:
		switch(panel_line) {
				// code
		case S6E3HF3_A2_INIT_ID:
		case S6E3HF3_A2_REV01_ID:
			dsim_info("%s init HMT dimming info for HF3 A2 init panel\n", __func__);
			diminfo = (void *) hmt_dimming_info_HF3;
			break;
		case S6E3HF3_A3_REV01_ID:
			dsim_info("%s init HMT dimming info for HF3 A3 rev1 init panel\n", __func__);
			diminfo = (void *) hmt_dimming_info_HF3_A3_REV1;
			break;
		default:
			dsim_info("%s init HMT dimming info for (UNKNOWN) HF3 panel\n", __func__);
			diminfo = (void *) hmt_dimming_info_HF3;
			break;
		}
		break;
	default:
		dsim_info("%s init HMT dimming info for (UNKNOWN) HA3 panel\n", __func__);
		diminfo = (void *) hmt_dimming_info_HA3;
		break;
	}
	panel->hmt_dim_data = (void *) dimming;
	panel->hmt_dim_info = (void *) diminfo;
	panel->hmt_br_tbl = (unsigned int *) hmt_br_tbl;
	for (j = 0; j < CI_MAX; j++) {
		temp = ((mtp[pos] & 0x01) ? -1 : 1) * mtp[pos + 1];
		dimming->t_gamma[V255][j] = (int) center_gamma[V255][j] + temp;
		dimming->mtp[V255][j] = temp;
		pos += 2;
	}
	for (i = V203; i >= V0; i--) {
		for (j = 0; j < CI_MAX; j++) {
			temp = ((mtp[pos] & 0x80) ? -1 : 1) * (mtp[pos] & 0x7f);
			dimming->t_gamma[i][j] = (int) center_gamma[i][j] + temp;
			dimming->mtp[i][j] = temp;
			pos++;
			}
	}
        /* for vt */
	temp = (mtp[pos + 1]) << 8 | mtp[pos];
	for (i = 0; i < CI_MAX; i++)
		dimming->vt_mtp[i] = (temp >> (i * 4)) & 0x0f;
	ret = generate_volt_table(dimming);
	if (ret) {
		dimm_err("[ERR:%s] failed to generate volt table\n", __func__);
		goto error;
	}
	for (i = 0; i < HMT_MAX_BR_INFO; i++) {
		ret = cal_gamma_from_index(dimming, &diminfo[i]);
		if (ret) {
			dsim_err("failed to calculate gamma : index : %d\n", i);
			goto error;
		}
	}
error:
	return ret;

}

#endif

#endif


/************************************ HA2 *****************************************/


static int s6e3ha2_read_init_info(struct dsim_device *dsim, unsigned char *mtp, unsigned char *hbm)
{
        int     i = 0;
        int     ret = 0;
        struct panel_private *panel = &dsim->priv;
        unsigned char buf[S6E3HA2_MTP_DATE_SIZE] = { 0, };
        unsigned char bufForCoordi[S6E3HA2_COORDINATE_LEN] = { 0, };
        unsigned char hbm_gamma[S6E3HA2_HBMGAMMA_LEN + 1] = { 0, };
        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
        }

        ret = dsim_read_hl_data(dsim, S6E3HA2_ID_REG, S6E3HA2_ID_LEN, dsim->priv.id);
        if (ret != S6E3HA2_ID_LEN) {
                dsim_err("%s : can't find connected panel. check panel connection\n", __func__);
                panel->lcdConnected = PANEL_DISCONNEDTED;
                goto read_exit;
        }

        dsim_info("READ ID : ");
        for (i = 0; i < S6E3HA2_ID_LEN; i++)
                dsim_info("%02x, ", dsim->priv.id[i]);
        dsim_info("\n");

        ret = dsim_read_hl_data(dsim, S6E3HA2_MTP_ADDR, S6E3HA2_MTP_DATE_SIZE, buf);
        if (ret != S6E3HA2_MTP_DATE_SIZE) {
                dsim_err("failed to read mtp, check panel connection\n");
                goto read_fail;
        }
        memcpy(mtp, buf, S6E3HA2_MTP_SIZE);
        memcpy(dsim->priv.date, &buf[40], ARRAY_SIZE(dsim->priv.date));
        dsim_info("READ MTP SIZE : %d\n", S6E3HA2_MTP_SIZE);
        dsim_info("=========== MTP INFO =========== \n");
        for (i = 0; i < S6E3HA2_MTP_SIZE; i++)
                dsim_info("MTP[%2d] : %2d : %2x\n", i, mtp[i], mtp[i]);

        // coordinate
        ret = dsim_read_hl_data(dsim, S6E3HA2_COORDINATE_REG, S6E3HA2_COORDINATE_LEN, bufForCoordi);
        if (ret != S6E3HA2_COORDINATE_LEN) {
                dsim_err("fail to read coordinate on command.\n");
                goto read_fail;
        }
        dsim->priv.coordinate[0] = bufForCoordi[0] << 8 | bufForCoordi[1];      /* X */
        dsim->priv.coordinate[1] = bufForCoordi[2] << 8 | bufForCoordi[3];      /* Y */
        dsim_info("READ coordi : ");
        for (i = 0; i < 2; i++)
                dsim_info("%d, ", dsim->priv.coordinate[i]);
        dsim_info("\n");

        // code
        ret = dsim_read_hl_data(dsim, S6E3HA2_CODE_REG, S6E3HA2_CODE_LEN, dsim->priv.code);
        if (ret != S6E3HA2_CODE_LEN) {
                dsim_err("fail to read code on command.\n");
                goto read_fail;
        }
        dsim_info("READ code : ");
        for (i = 0; i < S6E3HA2_CODE_LEN; i++)
                dsim_info("%x, ", dsim->priv.code[i]);
        dsim_info("\n");

        // tset
        ret = dsim_read_hl_data(dsim, S6E3HA2_TSET_REG, S6E3HA2_TSET_LEN - 1, dsim->priv.tset);
        if (ret < TSET_LEN - 1) {
                dsim_err("fail to read code on command.\n");
                goto read_fail;
        }
        dsim_info("READ tset : ");
        for (i = 0; i < TSET_LEN - 1; i++)
                dsim_info("%x, ", dsim->priv.tset[i]);
        dsim_info("\n");


        // elvss
        ret = dsim_read_hl_data(dsim, S6E3HA2_ELVSS_REG, S6E3HA2_ELVSS_LEN - 1, dsim->priv.elvss_set);
        if (ret < ELVSS_LEN - 1) {
                dsim_err("fail to read elvss on command.\n");
                goto read_fail;
        }
        dsim_info("READ elvss : ");
        for (i = 0; i < ELVSS_LEN - 1; i++)
                dsim_info("%x \n", dsim->priv.elvss_set[i]);

/* read hbm elvss for hbm interpolation */
        panel->hbm_elvss = dsim->priv.elvss_set[S6E3HA2_HBM_ELVSS_INDEX];

        ret = dsim_read_hl_data(dsim, S6E3HA2_HBMGAMMA_REG, S6E3HA2_HBMGAMMA_LEN + 1, hbm_gamma);
        if (ret != S6E3HA2_HBMGAMMA_LEN + 1) {
                dsim_err("fail to read elvss on command.\n");
                goto read_fail;
        }
        dsim_info("HBM Gamma : ");
        memcpy(hbm, hbm_gamma + 1, S6E3HA2_HBMGAMMA_LEN);

        for (i = 1; i < S6E3HA2_HBMGAMMA_LEN + 1; i++)
                dsim_info("hbm gamma[%d] : %x\n", i, hbm_gamma[i]);
        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
                goto read_exit;
        }
        ret = 0;
      read_exit:
        return 0;

      read_fail:
        return -ENODEV;
}
static int s6e3ha2_wqhd_dump(struct dsim_device *dsim)
{
        int     ret = 0;
        int     i;
        unsigned char id[S6E3HA2_ID_LEN];
        unsigned char rddpm[4];
        unsigned char rddsm[4];
        unsigned char err_buf[4];

        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
        }

        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_FC, ARRAY_SIZE(SEQ_TEST_KEY_ON_FC));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_FC\n", __func__);
        }

        ret = dsim_read_hl_data(dsim, 0xEA, 3, err_buf);
        if (ret != 3) {
                dsim_err("%s : can't read Panel's EA Reg\n", __func__);
                goto dump_exit;
        }

        dsim_info("=== Panel's 0xEA Reg Value ===\n");
        dsim_info("* 0xEA : buf[0] = %x\n", err_buf[0]);
        dsim_info("* 0xEA : buf[1] = %x\n", err_buf[1]);

        ret = dsim_read_hl_data(dsim, S6E3HA2_RDDPM_ADDR, 3, rddpm);
        if (ret != 3) {
                dsim_err("%s : can't read RDDPM Reg\n", __func__);
                goto dump_exit;
        }

        dsim_info("=== Panel's RDDPM Reg Value : %x ===\n", rddpm[0]);

        if (rddpm[0] & 0x80)
                dsim_info("* Booster Voltage Status : ON\n");
        else
                dsim_info("* Booster Voltage Status : OFF\n");

        if (rddpm[0] & 0x40)
                dsim_info("* Idle Mode : On\n");
        else
                dsim_info("* Idle Mode : OFF\n");

        if (rddpm[0] & 0x20)
                dsim_info("* Partial Mode : On\n");
        else
                dsim_info("* Partial Mode : OFF\n");

        if (rddpm[0] & 0x10)
                dsim_info("* Sleep OUT and Working Ok\n");
        else
                dsim_info("* Sleep IN\n");

        if (rddpm[0] & 0x08)
                dsim_info("* Normal Mode On and Working Ok\n");
        else
                dsim_info("* Sleep IN\n");

        if (rddpm[0] & 0x04)
                dsim_info("* Display On and Working Ok\n");
        else
                dsim_info("* Display Off\n");

        ret = dsim_read_hl_data(dsim, S6E3HA2_RDDSM_ADDR, 3, rddsm);
        if (ret != 3) {
                dsim_err("%s : can't read RDDSM Reg\n", __func__);
                goto dump_exit;
        }

        dsim_info("=== Panel's RDDSM Reg Value : %x ===\n", rddsm[0]);

        if (rddsm[0] & 0x80)
                dsim_info("* TE On\n");
        else
                dsim_info("* TE OFF\n");

        if (rddsm[0] & 0x02)
                dsim_info("* S_DSI_ERR : Found\n");

        if (rddsm[0] & 0x01)
                dsim_info("* DSI_ERR : Found\n");

        // id
        ret = dsim_read_hl_data(dsim, S6E3HA2_ID_REG, S6E3HA2_ID_LEN, id);
        if (ret != S6E3HA2_ID_LEN) {
                dsim_err("%s : can't read panel id\n", __func__);
                goto dump_exit;
        }

        dsim_info("READ ID : ");
        for (i = 0; i < S6E3HA2_ID_LEN; i++)
                dsim_info("%02x, ", id[i]);
        dsim_info("\n");

        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_FC, ARRAY_SIZE(SEQ_TEST_KEY_OFF_FC));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_FC\n", __func__);
        }

        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_F0, ARRAY_SIZE(SEQ_TEST_KEY_OFF_F0));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_F0\n", __func__);
        }
      dump_exit:
        dsim_info(" - %s\n", __func__);
        return ret;

}
static int s6e3ha2_wqhd_probe(struct dsim_device *dsim)
{
        int     ret = 0;
        struct panel_private *panel = &dsim->priv;
        unsigned char mtp[S6E3HA2_MTP_SIZE] = { 0, };
        unsigned char hbm[S6E3HA2_HBMGAMMA_LEN] = { 0, };
        panel->dim_data = (void *) NULL;
        panel->lcdConnected = PANEL_CONNECTED;

        dsim_info(" +  : %s\n", __func__);
#ifdef CONFIG_LCD_ALPM
		panel->alpm = 0;
		panel->current_alpm = 0;
		mutex_init(&panel->alpm_lock);
		panel->alpm_support = 0;
#endif

        ret = s6e3ha2_read_init_info(dsim, mtp, hbm);
        if (panel->lcdConnected == PANEL_DISCONNEDTED) {
                dsim_err("dsim : %s lcd was not connected\n", __func__);
                goto probe_exit;
        }

	dsim->priv.esd_disable = 0;

#ifdef CONFIG_PANEL_AID_DIMMING
        ret = init_dimming(dsim, mtp, hbm);
        if (ret) {
                dsim_err("%s : failed to generate gamma tablen\n", __func__);
        }
#endif
#ifdef CONFIG_LCD_HMT
        ret = hmt_init_dimming(dsim, mtp);
        if (ret) {
                dsim_err("%s : failed to generate gamma tablen\n", __func__);
        }
#endif
#ifdef CONFIG_EXYNOS_DECON_MDNIE_LITE
        panel->mdnie_support = 0;
#endif

probe_exit:
        return ret;

}


static int s6e3ha2_wqhd_displayon(struct dsim_device *dsim)
{
        int     ret = 0;

        dsim_info("MDD : %s was called\n", __func__);

        ret = dsim_write_hl_data(dsim, SEQ_DISPLAY_ON, ARRAY_SIZE(SEQ_DISPLAY_ON));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : DISPLAY_ON\n", __func__);
                goto displayon_err;
        }

      displayon_err:
        return ret;

}

static int s6e3ha2_wqhd_exit(struct dsim_device *dsim)
{
        int     ret = 0;

        dsim_info("MDD : %s was called\n", __func__);

        ret = dsim_write_hl_data(dsim, SEQ_DISPLAY_OFF, ARRAY_SIZE(SEQ_DISPLAY_OFF));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : DISPLAY_OFF\n", __func__);
                goto exit_err;
        }

        ret = dsim_write_hl_data(dsim, SEQ_SLEEP_IN, ARRAY_SIZE(SEQ_SLEEP_IN));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SLEEP_IN\n", __func__);
                goto exit_err;
        }

        msleep(120);

      exit_err:
        return ret;
}

static int s6e3ha2_wqhd_init(struct dsim_device *dsim)
{
        int     ret = 0;

        dsim_info("MDD : %s was called\n", __func__);

        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_FC, ARRAY_SIZE(SEQ_TEST_KEY_ON_FC));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_FC\n", __func__);
                goto init_exit;
        }

        /* 7. Sleep Out(11h) */
        ret = dsim_write_hl_data(dsim, SEQ_SLEEP_OUT, ARRAY_SIZE(SEQ_SLEEP_OUT));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_SLEEP_OUT\n", __func__);
                goto init_exit;
        }
        msleep(5);

        /* 9. Interface Setting */

        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_SINGLE_DSI_1, ARRAY_SIZE(S6E3HA2_SEQ_SINGLE_DSI_1));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_SINGLE_DSI_1\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_SINGLE_DSI_2, ARRAY_SIZE(S6E3HA2_SEQ_SINGLE_DSI_2));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_SINGLE_DSI_2\n", __func__);
                goto init_exit;
        }
#ifdef CONFIG_LCD_HMT
        if (dsim->priv.hmt_on != HMT_ON)
#endif
                msleep(120);

#ifdef CONFIG_ALWAYS_RELOAD_MTP_FACTORY_BUILD
	ret = lcd_reload_mtp(dynamic_lcd_type, dsim);
#endif

        /* Common Setting */
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_TE_ON, ARRAY_SIZE(S6E3HA2_SEQ_TE_ON));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TE_ON\n", __func__);
                goto init_exit;
        }

        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_TSP_TE, ARRAY_SIZE(S6E3HA2_SEQ_TSP_TE));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TSP_TE\n", __func__);
                goto init_exit;
        }

        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_PENTILE_SETTING, ARRAY_SIZE(S6E3HA2_SEQ_PENTILE_SETTING));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_PENTILE_SETTING\n", __func__);
                goto init_exit;
        }

        /* POC setting */
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_POC_SETTING1, ARRAY_SIZE(S6E3HA2_SEQ_POC_SETTING1));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_POC_SETTING1\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_POC_SETTING2, ARRAY_SIZE(S6E3HA2_SEQ_POC_SETTING2));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_POC_SETTING2\n", __func__);
                goto init_exit;
        }

        /* OSC setting */
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_OSC_SETTING1, ARRAY_SIZE(S6E3HA2_SEQ_OSC_SETTING1));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : S6E3HA2_SEQ_OSC1\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_OSC_SETTING2, ARRAY_SIZE(S6E3HA2_SEQ_OSC_SETTING2));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : S6E3HA2_SEQ_OSC2\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_OSC_SETTING3, ARRAY_SIZE(S6E3HA2_SEQ_OSC_SETTING3));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : S6E3HA2_SEQ_OSC3\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_FC, ARRAY_SIZE(SEQ_TEST_KEY_OFF_FC));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_FC\n", __func__);
                goto init_exit;
        }

        /* PCD setting */
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_PCD_SETTING, ARRAY_SIZE(S6E3HA2_SEQ_PCD_SETTING));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_PCD_SETTING\n", __func__);
                goto init_exit;
        }

        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_ERR_FG_SETTING, ARRAY_SIZE(S6E3HA2_SEQ_ERR_FG_SETTING));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_ERR_FG_SETTING\n", __func__);
                goto init_exit;
        }

#ifndef CONFIG_PANEL_AID_DIMMING
        /* Brightness Setting */
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_GAMMA_CONDITION_SET, ARRAY_SIZE(S6E3HA2_SEQ_GAMMA_CONDITION_SET));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_GAMMA_CONDITION_SET\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_AOR_CONTROL, ARRAY_SIZE(S6E3HA2_SEQ_AOR_CONTROL));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_AOR_CONTROL\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_ELVSS_SET, ARRAY_SIZE(S6E3HA2_SEQ_ELVSS_SET));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_ELVSS_SET\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_VINT_SET, ARRAY_SIZE(S6E3HA2_SEQ_VINT_SET));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_ELVSS_SET\n", __func__);
                goto init_exit;
        }

        ret = dsim_write_hl_data(dsim, SEQ_GAMMA_UPDATE, ARRAY_SIZE(SEQ_GAMMA_UPDATE));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_GAMMA_UPDATE\n", __func__);
                goto init_exit;
        }

        /* ACL Setting */
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_ACL_OFF, ARRAY_SIZE(S6E3HA2_SEQ_ACL_OFF));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_ACL_OFF\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_ACL_OFF_OPR, ARRAY_SIZE(S6E3HA2_SEQ_ACL_OFF_OPR));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_ACL_OFF_OPR\n", __func__);
                goto init_exit;
        }
        /* elvss */
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_TSET_GLOBAL, ARRAY_SIZE(S6E3HA2_SEQ_TSET_GLOBAL));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_TSET_GLOBAL\n", __func__);
                goto init_exit;
        }
        ret = dsim_write_hl_data(dsim, S6E3HA2_SEQ_TSET, ARRAY_SIZE(S6E3HA2_SEQ_TSET));
        if (ret < 0) {
                dsim_err(":%s fail to write CMD : SEQ_TSET\n", __func__);
                goto init_exit;
        }
#endif
        ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_F0, ARRAY_SIZE(SEQ_TEST_KEY_OFF_F0));
        if (ret < 0) {
                dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_F0\n", __func__);
                goto init_exit;
        }

      init_exit:
        return ret;
}



struct dsim_panel_ops s6e3ha2_panel_ops = {
        .probe = s6e3ha2_wqhd_probe,
        .displayon = s6e3ha2_wqhd_displayon,
        .exit = s6e3ha2_wqhd_exit,
        .init = s6e3ha2_wqhd_init,
        .dump = s6e3ha2_wqhd_dump,
};

/************************************ HA3 *****************************************/

// convert NEW SPEC to OLD SPEC
static int s6e3ha3_VT_RGB2GRB( unsigned char *VT )
{
	int ret = 0;
	int r, g, b;

	// new SPEC = RG0B
	r = (VT[0] & 0xF0) >>4;
	g = (VT[0] & 0x0F);
	b = (VT[1] & 0x0F);

	// old spec = GR0B
	VT[0] = g<<4 | r;
	VT[1] = b;

	return ret;
}

static int s6e3ha3_read_init_info(struct dsim_device *dsim, unsigned char *mtp, unsigned char *hbm)
{
	int     i = 0;
	int     ret = 0;
	struct panel_private *panel = &dsim->priv;
	unsigned char buf[S6E3HA3_MTP_DATE_SIZE] = { 0, };
	unsigned char bufForCoordi[S6E3HA3_COORDINATE_LEN] = { 0, };
	unsigned char hbm_gamma[S6E3HA3_HBMGAMMA_LEN] = { 0, };
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
	}

	ret = dsim_read_hl_data(dsim, S6E3HA3_ID_REG, S6E3HA3_ID_LEN, dsim->priv.id);
	if (ret != S6E3HA3_ID_LEN) {
		dsim_err("%s : can't find connected panel. check panel connection\n", __func__);
		panel->lcdConnected = PANEL_DISCONNEDTED;
		goto read_exit;
	}

	dsim_info("READ ID : ");
	for (i = 0; i < S6E3HA3_ID_LEN; i++)
		dsim_info("%02x, ", dsim->priv.id[i]);
	dsim_info("\n");

	ret = dsim_read_hl_data(dsim, S6E3HA3_MTP_ADDR, S6E3HA3_MTP_DATE_SIZE, buf);
	if (ret != S6E3HA3_MTP_DATE_SIZE) {
		dsim_err("failed to read mtp, check panel connection\n");
		goto read_fail;
	}
	s6e3ha3_VT_RGB2GRB(buf + S6E3HA3_MTP_VT_ADDR);
	memcpy(mtp, buf, S6E3HA3_MTP_SIZE);
	memcpy(dsim->priv.date, &buf[40], ARRAY_SIZE(dsim->priv.date));
	dsim_info("READ MTP SIZE : %d\n", S6E3HA3_MTP_SIZE);
	dsim_info("=========== MTP INFO =========== \n");
	for (i = 0; i < S6E3HA3_MTP_SIZE; i++)
		dsim_info("MTP[%2d] : %2d : %2x\n", i, mtp[i], mtp[i]);

	// coordinate
	ret = dsim_read_hl_data(dsim, S6E3HA3_COORDINATE_REG, S6E3HA3_COORDINATE_LEN, bufForCoordi);
	if (ret != S6E3HA3_COORDINATE_LEN) {
		dsim_err("fail to read coordinate on command.\n");
		goto read_fail;
	}
	dsim->priv.coordinate[0] = bufForCoordi[0] << 8 | bufForCoordi[1];      /* X */
	dsim->priv.coordinate[1] = bufForCoordi[2] << 8 | bufForCoordi[3];      /* Y */
	dsim_info("READ coordi : ");
	for (i = 0; i < 2; i++)
		dsim_info("%d, ", dsim->priv.coordinate[i]);
	dsim_info("\n");

	// code
	ret = dsim_read_hl_data(dsim, S6E3HA3_CODE_REG, S6E3HA3_CODE_LEN, dsim->priv.code);
	if (ret != S6E3HA3_CODE_LEN) {
		dsim_err("fail to read code on command.\n");
		goto read_fail;
	}
	dsim_info("READ code : ");
	for (i = 0; i < S6E3HA3_CODE_LEN; i++)
		dsim_info("%x, ", dsim->priv.code[i]);
	dsim_info("\n");

	// aid

	memcpy(dsim->priv.aid, &S6E3HA3_SEQ_AOR[1], ARRAY_SIZE(S6E3HA3_SEQ_AOR) - 1);
	dsim_info("READ aid : ");
	for (i = 0; i < S6E3HA3_AID_CMD_CNT - 1; i++)
		dsim_info("%x, ", dsim->priv.aid[i]);
	dsim_info("\n");

	// tset
	ret = dsim_read_hl_data(dsim, S6E3HA3_TSET_REG, S6E3HA3_TSET_LEN - 1, dsim->priv.tset);
	if (ret < S6E3HA3_TSET_LEN - 1) {
		dsim_err("fail to read code on command.\n");
		goto read_fail;
	}
	dsim_info("READ tset : ");
	for (i = 0; i < S6E3HA3_TSET_LEN - 1; i++)
		dsim_info("%x, ", dsim->priv.tset[i]);
	dsim_info("\n");

	// elvss
	ret = dsim_read_hl_data(dsim, S6E3HA3_ELVSS_REG, S6E3HA3_ELVSS_LEN - 1, dsim->priv.elvss_set);
	if (ret < S6E3HA3_ELVSS_LEN - 1) {
		dsim_err("fail to read elvss on command.\n");
		goto read_fail;
	}
	dsim_info("READ elvss : ");
	for (i = 0; i < S6E3HA3_ELVSS_LEN - 1; i++)
		dsim_info("%x \n", dsim->priv.elvss_set[i]);

	/* read hbm elvss for hbm interpolation */
	panel->hbm_elvss = dsim->priv.elvss_set[S6E3HA3_HBM_ELVSS_INDEX];

	ret = dsim_read_hl_data(dsim, S6E3HA3_HBMGAMMA_REG, S6E3HA3_HBMGAMMA_LEN, hbm_gamma);
	if (ret != S6E3HA3_HBMGAMMA_LEN) {
		dsim_err("fail to read elvss on command.\n");
		goto read_fail;
	}
	dsim_info("HBM Gamma : ");
	memcpy(hbm, hbm_gamma, S6E3HA3_HBMGAMMA_LEN);

	for (i = 0; i < S6E3HA3_HBMGAMMA_LEN; i++)
		dsim_info("hbm gamma[%d] : %x\n", i, hbm_gamma[i]);
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
		goto read_exit;
	}
	ret = 0;

read_exit:
	return 0;

read_fail:
	return -ENODEV;
}
static int s6e3ha3_wqhd_dump(struct dsim_device *dsim)
{
	int     ret = 0;
	int     i;
	unsigned char id[S6E3HA3_ID_LEN];
	unsigned char rddpm[4];
	unsigned char rddsm[4];
	unsigned char err_buf[4];

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
	}

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_FC, ARRAY_SIZE(SEQ_TEST_KEY_ON_FC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_FC\n", __func__);
	}

	ret = dsim_read_hl_data(dsim, 0xEA, 3, err_buf);
	if (ret != 3) {
		dsim_err("%s : can't read Panel's EA Reg\n", __func__);
		goto dump_exit;
	}

	dsim_info("=== Panel's 0xEA Reg Value ===\n");
	dsim_info("* 0xEA : buf[0] = %x\n", err_buf[0]);
	dsim_info("* 0xEA : buf[1] = %x\n", err_buf[1]);

	ret = dsim_read_hl_data(dsim, S6E3HA3_RDDPM_ADDR, 3, rddpm);
	if (ret != 3) {
		dsim_err("%s : can't read RDDPM Reg\n", __func__);
		goto dump_exit;
	}

	dsim_info("=== Panel's RDDPM Reg Value : %x ===\n", rddpm[0]);

	if (rddpm[0] & 0x80)
		dsim_info("* Booster Voltage Status : ON\n");
	else
		dsim_info("* Booster Voltage Status : OFF\n");

	if (rddpm[0] & 0x40)
		dsim_info("* Idle Mode : On\n");
	else
		dsim_info("* Idle Mode : OFF\n");

	if (rddpm[0] & 0x20)
		dsim_info("* Partial Mode : On\n");
	else
		dsim_info("* Partial Mode : OFF\n");

	if (rddpm[0] & 0x10)
		dsim_info("* Sleep OUT and Working Ok\n");
	else
		dsim_info("* Sleep IN\n");

	if (rddpm[0] & 0x08)
		dsim_info("* Normal Mode On and Working Ok\n");
	else
		dsim_info("* Sleep IN\n");

	if (rddpm[0] & 0x04)
		dsim_info("* Display On and Working Ok\n");
	else
		dsim_info("* Display Off\n");

	ret = dsim_read_hl_data(dsim, S6E3HA3_RDDSM_ADDR, 3, rddsm);
	if (ret != 3) {
		dsim_err("%s : can't read RDDSM Reg\n", __func__);
		goto dump_exit;
	}

	dsim_info("=== Panel's RDDSM Reg Value : %x ===\n", rddsm[0]);

	if (rddsm[0] & 0x80)
		dsim_info("* TE On\n");
	else
		dsim_info("* TE OFF\n");

	if (rddsm[0] & 0x02)
		dsim_info("* S_DSI_ERR : Found\n");

	if (rddsm[0] & 0x01)
		dsim_info("* DSI_ERR : Found\n");

	// id
	ret = dsim_read_hl_data(dsim, S6E3HA3_ID_REG, S6E3HA3_ID_LEN, id);
	if (ret != S6E3HA3_ID_LEN) {
		dsim_err("%s : can't read panel id\n", __func__);
		goto dump_exit;
	}

	dsim_info("READ ID : ");
	for (i = 0; i < S6E3HA3_ID_LEN; i++)
		dsim_info("%02x, ", id[i]);
	dsim_info("\n");

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_FC, ARRAY_SIZE(SEQ_TEST_KEY_OFF_FC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_FC\n", __func__);
	}

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_F0, ARRAY_SIZE(SEQ_TEST_KEY_OFF_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_F0\n", __func__);
	}
dump_exit:
	dsim_info(" - %s\n", __func__);
	return ret;

}
static int s6e3ha3_wqhd_probe(struct dsim_device *dsim)
{
	int     ret = 0;
	struct panel_private *panel = &dsim->priv;
	unsigned char mtp[S6E3HA3_MTP_SIZE] = { 0, };
	unsigned char hbm[S6E3HA3_HBMGAMMA_LEN] = { 0, };
	panel->dim_data = (void *)NULL;
	panel->lcdConnected = PANEL_CONNECTED;
#ifdef CONFIG_LCD_ALPM
	panel->alpm = 0;
	panel->current_alpm = 0;
	mutex_init(&panel->alpm_lock);
	panel->alpm_support = 0;
#endif

	dsim_info(" +  : %s\n", __func__);

	ret = s6e3ha3_read_init_info(dsim, mtp, hbm);
	if (panel->lcdConnected == PANEL_DISCONNEDTED) {
		dsim_err("dsim : %s lcd was not connected\n", __func__);
		goto probe_exit;
	}

	if ((panel->id[2] & 0x0F) < 2)	// under rev.B
		dsim->priv.esd_disable = 1;
	else dsim->priv.esd_disable = 0;
	panel->auto_brightness_level = AUTOBRIGHTNESS_LEVEL;
#ifdef CONFIG_PANEL_AID_DIMMING
	ret = init_dimming(dsim, mtp, hbm);
	if (ret) {
		dsim_err("%s : failed to generate gamma tablen\n", __func__);
	}
#endif
#ifdef CONFIG_LCD_HMT
	ret = hmt_init_dimming(dsim, mtp);
	if (ret) {
		dsim_err("%s : failed to generate gamma tablen\n", __func__);
	}
#endif
#ifdef CONFIG_EXYNOS_DECON_MDNIE_LITE
	panel->mdnie_support = 1;
#endif

probe_exit:
	return ret;

}


static int s6e3ha3_wqhd_displayon(struct dsim_device *dsim)
{
	int     ret = 0;

	dsim_info("MDD : %s was called\n", __func__);

	ret = dsim_write_hl_data(dsim, SEQ_DISPLAY_ON, ARRAY_SIZE(SEQ_DISPLAY_ON));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : DISPLAY_ON\n", __func__);
		goto displayon_err;
	}

displayon_err:
	return ret;

}

static int s6e3ha3_wqhd_exit(struct dsim_device *dsim)
{
	int     ret = 0;

	dsim_info("MDD : %s was called\n", __func__);

	ret = dsim_write_hl_data(dsim, SEQ_DISPLAY_OFF, ARRAY_SIZE(SEQ_DISPLAY_OFF));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : DISPLAY_OFF\n", __func__);
		goto exit_err;
	}

	ret = dsim_write_hl_data(dsim, SEQ_SLEEP_IN, ARRAY_SIZE(SEQ_SLEEP_IN));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SLEEP_IN\n", __func__);
		goto exit_err;
	}

	msleep(120);

exit_err:
	return ret;
}

static int s6e3ha3_wqhd_init(struct dsim_device *dsim)
{
	int     ret = 0;

	dsim_info("MDD : %s was called\n", __func__);

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_FC, ARRAY_SIZE(SEQ_TEST_KEY_ON_FC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_FC\n", __func__);
		goto init_exit;
	}
	msleep(5);

	/* 7. Sleep Out(11h) */
	ret = dsim_write_hl_data(dsim, SEQ_SLEEP_OUT, ARRAY_SIZE(SEQ_SLEEP_OUT));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_SLEEP_OUT\n", __func__);
		goto init_exit;
	}
	msleep(5);

	/* 9. Interface Setting */
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_DISPLAY_TIMING1, ARRAY_SIZE(S6E3HA3_SEQ_DISPLAY_TIMING1));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_DISPLAY_TIMING1\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_DISPLAY_TIMING2, ARRAY_SIZE(S6E3HA3_SEQ_DISPLAY_TIMING2));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_DISPLAY_TIMING2\n", __func__);
		goto init_exit;
	}


	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_FREQ_CALIBRATION, ARRAY_SIZE(S6E3HA3_SEQ_FREQ_CALIBRATION));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_MIC\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_MIC, ARRAY_SIZE(S6E3HA3_SEQ_MIC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_MIC\n", __func__);
		goto init_exit;
	}
	if (hw_rev < 3) {
		ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_DCDC_GLOBAL, ARRAY_SIZE(S6E3HA3_SEQ_DCDC_GLOBAL));
		if (ret < 0) {
			dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_DCDC_GLOBAL\n", __func__);
			goto init_exit;
		}
		ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_DCDC, ARRAY_SIZE(S6E3HA3_SEQ_DCDC));
		if (ret < 0) {
			dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_DCDC\n", __func__);
			goto init_exit;
		}
		dsim_info("%s max77838 system rev : %d\n", __func__, hw_rev);
	}
#ifdef CONFIG_LCD_HMT
		if(dsim->priv.hmt_on != HMT_ON)
#endif
	msleep(120);

#ifdef CONFIG_ALWAYS_RELOAD_MTP_FACTORY_BUILD
	ret = lcd_reload_mtp(dynamic_lcd_type, dsim);
#endif

	/* Common Setting */
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_TE_RISING_TIMING, ARRAY_SIZE(S6E3HA3_SEQ_TE_RISING_TIMING));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TE_ON\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_TE_ON, ARRAY_SIZE(S6E3HA3_SEQ_TE_ON));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TE_ON\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_TSP_TE, ARRAY_SIZE(S6E3HA3_SEQ_TSP_TE));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TSP_TE\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_PCD, ARRAY_SIZE(S6E3HA3_SEQ_PCD));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_PCD\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_ERR_FG, ARRAY_SIZE(S6E3HA3_SEQ_ERR_FG));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_PCD\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_PENTILE_SETTING, ARRAY_SIZE(S6E3HA3_SEQ_PENTILE_SETTING));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_PCD\n", __func__);
		goto init_exit;
	}

#ifndef CONFIG_PANEL_AID_DIMMING
	/* Brightness Setting */
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_GAMMA_CONDITION_SET, ARRAY_SIZE(S6E3HA3_SEQ_GAMMA_CONDITION_SET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_GAMMA_CONDITION_SET\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_AOR, ARRAY_SIZE(S6E3HA3_SEQ_AOR));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HA3_SEQ_AOR_0\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_MPS_ELVSS_SET, ARRAY_SIZE(S6E3HA3_SEQ_MPS_ELVSS_SET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HA3_SEQ_MPS_ELVSS_SET\n", __func__);
		goto init_exit;
	}

	/* ACL Setting */
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_ACL_OFF, ARRAY_SIZE(S6E3HA3_SEQ_ACL_OFF));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_ACL_OFF\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_ACL_OFF_OPR, ARRAY_SIZE(S6E3HA3_SEQ_ACL_OFF_OPR));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_ACL_OFF_OPR\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, SEQ_GAMMA_UPDATE, ARRAY_SIZE(SEQ_GAMMA_UPDATE));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_GAMMA_UPDATE\n", __func__);
		goto init_exit;
	}

	/* elvss */
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_ELVSS_GLOBAL, ARRAY_SIZE(S6E3HA3_SEQ_ELVSS_GLOBAL));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HA3_SEQ_ELVSS_GLOBAL\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_ELVSS, ARRAY_SIZE(S6E3HA3_SEQ_ELVSS));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HA3_SEQ_ELVSS\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_TSET_GLOBAL, ARRAY_SIZE(S6E3HA3_SEQ_TSET_GLOBAL));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_TSET_GLOBAL\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_TSET, ARRAY_SIZE(S6E3HA3_SEQ_TSET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_TSET\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HA3_SEQ_VINT_SET, ARRAY_SIZE(S6E3HA3_SEQ_VINT_SET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HA3_SEQ_VINT_SET\n", __func__);
		goto init_exit;
	}
#endif
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_F0, ARRAY_SIZE(SEQ_TEST_KEY_OFF_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_F0\n", __func__);
		goto init_exit;
	}

init_exit:
	return ret;
}



struct dsim_panel_ops s6e3ha3_panel_ops = {
	.probe = s6e3ha3_wqhd_probe,
	.displayon = s6e3ha3_wqhd_displayon,
	.exit = s6e3ha3_wqhd_exit,
	.init = s6e3ha3_wqhd_init,
	.dump = s6e3ha3_wqhd_dump,
};

/******************** HF3 ********************/


static int s6e3hf3_read_init_info(struct dsim_device *dsim, unsigned char *mtp, unsigned char *hbm)
{
	int     i = 0;
	int     ret = 0;
	struct panel_private *panel = &dsim->priv;
	unsigned char buf[S6E3HF3_MTP_DATE_SIZE] = { 0, };
	unsigned char bufForCoordi[S6E3HF3_COORDINATE_LEN] = { 0, };
	unsigned char hbm_gamma[S6E3HF3_HBMGAMMA_LEN] = { 0, };
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
	}

	ret = dsim_read_hl_data(dsim, S6E3HF3_ID_REG, S6E3HF3_ID_LEN, dsim->priv.id);
	if (ret != S6E3HF3_ID_LEN) {
		dsim_err("%s : can't find connected panel. check panel connection\n", __func__);
		panel->lcdConnected = PANEL_DISCONNEDTED;
		goto read_exit;
	}

	dsim_info("READ ID : ");
	for (i = 0; i < S6E3HF3_ID_LEN; i++)
		dsim_info("%02x, ", dsim->priv.id[i]);
	dsim_info("\n");

	ret = dsim_read_hl_data(dsim, S6E3HF3_MTP_ADDR, S6E3HF3_MTP_DATE_SIZE, buf);
	if (ret != S6E3HF3_MTP_DATE_SIZE) {
		dsim_err("failed to read mtp, check panel connection\n");
		goto read_fail;
	}
	s6e3ha3_VT_RGB2GRB(buf + S6E3HF3_MTP_VT_ADDR);
	memcpy(mtp, buf, S6E3HF3_MTP_SIZE);
	memcpy(dsim->priv.date, &buf[40], min((int)(S6E3HF3_MTP_DATE_SIZE - 40), (int)ARRAY_SIZE(dsim->priv.date)));
	dsim_info("READ MTP SIZE : %d\n", S6E3HF3_MTP_SIZE);
	dsim_info("=========== MTP INFO =========== \n");
	for (i = 0; i < S6E3HF3_MTP_SIZE; i++)
		dsim_info("MTP[%2d] : %2d : %2x\n", i, mtp[i], mtp[i]);

	// coordinate
	ret = dsim_read_hl_data(dsim, S6E3HF3_COORDINATE_REG, S6E3HF3_COORDINATE_LEN, bufForCoordi);
	if (ret != S6E3HF3_COORDINATE_LEN) {
		dsim_err("fail to read coordinate on command.\n");
		goto read_fail;
	}
	dsim->priv.coordinate[0] = bufForCoordi[0] << 8 | bufForCoordi[1];      /* X */
	dsim->priv.coordinate[1] = bufForCoordi[2] << 8 | bufForCoordi[3];      /* Y */
	dsim_info("READ coordi : ");
	for (i = 0; i < 2; i++)
		dsim_info("%d, ", dsim->priv.coordinate[i]);
	dsim_info("\n");

	// code
	ret = dsim_read_hl_data(dsim, S6E3HF3_CODE_REG, S6E3HF3_CODE_LEN, dsim->priv.code);
	if (ret != S6E3HF3_CODE_LEN) {
		dsim_err("fail to read code on command.\n");
		goto read_fail;
	}
	dsim_info("READ code : ");
	for (i = 0; i < S6E3HF3_CODE_LEN; i++)
		dsim_info("%x, ", dsim->priv.code[i]);
	dsim_info("\n");

	// aid
	memcpy(dsim->priv.aid, &S6E3HF3_SEQ_AOR[1], ARRAY_SIZE(S6E3HF3_SEQ_AOR) - 1);
	dsim_info("READ aid : ");
	for (i = 0; i < S6E3HF3_AID_CMD_CNT - 1; i++)
		dsim_info("%x, ", dsim->priv.aid[i]);
	dsim_info("\n");

	// tset
	ret = dsim_read_hl_data(dsim, S6E3HF3_TSET_REG, S6E3HF3_TSET_LEN - 1, dsim->priv.tset);
	if (ret < S6E3HF3_TSET_LEN - 1) {
		dsim_err("fail to read code on command.\n");
		goto read_fail;
	}
	dsim_info("READ tset : ");
	for (i = 0; i < S6E3HF3_TSET_LEN - 1; i++)
		dsim_info("%x, ", dsim->priv.tset[i]);
	dsim_info("\n");

	// elvss
	ret = dsim_read_hl_data(dsim, S6E3HF3_ELVSS_REG, S6E3HF3_ELVSS_LEN - 1, dsim->priv.elvss_set);
	if (ret < S6E3HF3_ELVSS_LEN - 1) {
		dsim_err("fail to read elvss on command.\n");
		goto read_fail;
	}
	dsim_info("READ elvss : ");
	for (i = 0; i < S6E3HF3_ELVSS_LEN - 1; i++)
		dsim_info("%x \n", dsim->priv.elvss_set[i]);

	/* read hbm elvss for hbm interpolation */
	panel->hbm_elvss = dsim->priv.elvss_set[S6E3HF3_HBM_ELVSS_INDEX];

	ret = dsim_read_hl_data(dsim, S6E3HF3_HBMGAMMA_REG, S6E3HF3_HBMGAMMA_LEN, hbm_gamma);
	if (ret != S6E3HF3_HBMGAMMA_LEN) {
		dsim_err("fail to read elvss on command.\n");
		goto read_fail;
	}
	dsim_info("HBM Gamma : ");
	memcpy(hbm, hbm_gamma, S6E3HF3_HBMGAMMA_LEN);

	for (i = 0; i < S6E3HF3_HBMGAMMA_LEN; i++)
		dsim_info("hbm gamma[%d] : %x\n", i, hbm_gamma[i]);
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
		goto read_exit;
	}
	ret = 0;

read_exit:
	return 0;

read_fail:
	return -ENODEV;
}

static int s6e3hf3_wqhd_dump(struct dsim_device *dsim)
{
	int     ret = 0;
	int     i;
	unsigned char id[S6E3HF3_ID_LEN];
	unsigned char rddpm[4];
	unsigned char rddsm[4];
	unsigned char err_buf[4];

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
	}

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_FC, ARRAY_SIZE(SEQ_TEST_KEY_ON_FC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_FC\n", __func__);
	}

	ret = dsim_read_hl_data(dsim, 0xEA, 3, err_buf);
	if (ret != 3) {
		dsim_err("%s : can't read Panel's EA Reg\n", __func__);
		goto dump_exit;
	}

	dsim_info("=== Panel's 0xEA Reg Value ===\n");
	dsim_info("* 0xEA : buf[0] = %x\n", err_buf[0]);
	dsim_info("* 0xEA : buf[1] = %x\n", err_buf[1]);

	ret = dsim_read_hl_data(dsim, S6E3HF3_RDDPM_ADDR, 3, rddpm);
	if (ret != 3) {
		dsim_err("%s : can't read RDDPM Reg\n", __func__);
		goto dump_exit;
	}

	dsim_info("=== Panel's RDDPM Reg Value : %x ===\n", rddpm[0]);

	if (rddpm[0] & 0x80)
		dsim_info("* Booster Voltage Status : ON\n");
	else
		dsim_info("* Booster Voltage Status : OFF\n");

	if (rddpm[0] & 0x40)
		dsim_info("* Idle Mode : On\n");
	else
		dsim_info("* Idle Mode : OFF\n");

	if (rddpm[0] & 0x20)
		dsim_info("* Partial Mode : On\n");
	else
		dsim_info("* Partial Mode : OFF\n");

	if (rddpm[0] & 0x10)
		dsim_info("* Sleep OUT and Working Ok\n");
	else
		dsim_info("* Sleep IN\n");

	if (rddpm[0] & 0x08)
		dsim_info("* Normal Mode On and Working Ok\n");
	else
		dsim_info("* Sleep IN\n");

	if (rddpm[0] & 0x04)
		dsim_info("* Display On and Working Ok\n");
	else
		dsim_info("* Display Off\n");

	ret = dsim_read_hl_data(dsim, S6E3HF3_RDDSM_ADDR, 3, rddsm);
	if (ret != 3) {
		dsim_err("%s : can't read RDDSM Reg\n", __func__);
		goto dump_exit;
	}

	dsim_info("=== Panel's RDDSM Reg Value : %x ===\n", rddsm[0]);

	if (rddsm[0] & 0x80)
		dsim_info("* TE On\n");
	else
		dsim_info("* TE OFF\n");

	if (rddsm[0] & 0x02)
		dsim_info("* S_DSI_ERR : Found\n");

	if (rddsm[0] & 0x01)
		dsim_info("* DSI_ERR : Found\n");

	// id
	ret = dsim_read_hl_data(dsim, S6E3HF3_ID_REG, S6E3HF3_ID_LEN, id);
	if (ret != S6E3HF3_ID_LEN) {
		dsim_err("%s : can't read panel id\n", __func__);
		goto dump_exit;
	}

	dsim_info("READ ID : ");
	for (i = 0; i < S6E3HF3_ID_LEN; i++)
		dsim_info("%02x, ", id[i]);
	dsim_info("\n");

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_FC, ARRAY_SIZE(SEQ_TEST_KEY_OFF_FC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_FC\n", __func__);
	}

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_F0, ARRAY_SIZE(SEQ_TEST_KEY_OFF_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_F0\n", __func__);
	}
dump_exit:
	dsim_info(" - %s\n", __func__);
	return ret;

}

static int s6e3hf3_wqhd_probe(struct dsim_device *dsim)
{
	int     ret = 0;
	struct panel_private *panel = &dsim->priv;
	unsigned char mtp[S6E3HF3_MTP_SIZE] = { 0, };
	unsigned char hbm[S6E3HF3_HBMGAMMA_LEN] = { 0, };
	panel->dim_data = (void *)NULL;
	panel->lcdConnected = PANEL_CONNECTED;
	dsim->glide_display_size = 80;	// framebuffer of LCD is 1600x2560, display area is 1440x2560, glidesize = (1600-1440)/2
#ifdef CONFIG_LCD_ALPM
	panel->alpm = 0;
	panel->current_alpm = 0;
	mutex_init(&panel->alpm_lock);
	panel->alpm_support = 1;
#endif

	dsim_info(" +  : %s\n", __func__);

	ret = s6e3hf3_read_init_info(dsim, mtp, hbm);
	if (panel->lcdConnected == PANEL_DISCONNEDTED) {
		dsim_err("dsim : %s lcd was not connected\n", __func__);
		goto probe_exit;
	}

	if((panel->id[2] & 0x0F) < 5)	// under rev.E
		dsim->priv.esd_disable = 1;
	else
		dsim->priv.esd_disable = 0;

	panel->auto_brightness_level = AUTOBRIGHTNESS_LEVEL;

#ifdef CONFIG_PANEL_AID_DIMMING
	ret = init_dimming(dsim, mtp, hbm);
	if (ret) {
		dsim_err("%s : failed to generate gamma tablen\n", __func__);
	}
#ifdef CONFIG_PANEL_DUALIZATION
	if(panel->strDatFile) {
		/* support a3 line panel efs data start */
		pr_info("%s target file : %s\n", __func__, panel->strDatFile);
		INIT_DELAYED_WORK(&panel->octa_a3_read_data_work, (work_func_t)a3_line_read_work_fn);
		panel->octa_a3_read_data_work_q = create_singlethread_workqueue("octa_a3_workqueue");

		if (panel->octa_a3_read_data_work_q) {
			panel->octa_a3_read_cnt = 50;
			queue_delayed_work(panel->octa_a3_read_data_work_q,
				&panel->octa_a3_read_data_work, msecs_to_jiffies(500));
		}
		/* support a3 line panel efs data end */
	}
#endif

#endif
#ifdef CONFIG_LCD_HMT
	ret = hmt_init_dimming(dsim, mtp);
	if (ret) {
		dsim_err("%s : failed to generate gamma tablen\n", __func__);
	}
#endif
#ifdef CONFIG_EXYNOS_DECON_MDNIE_LITE
	panel->mdnie_support = 1;
#endif

probe_exit:
	return ret;

}


static int s6e3hf3_wqhd_displayon(struct dsim_device *dsim)
{
	int     ret = 0;

	dsim_info("MDD : %s was called\n", __func__);

	ret = dsim_write_hl_data(dsim, SEQ_DISPLAY_ON, ARRAY_SIZE(SEQ_DISPLAY_ON));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : DISPLAY_ON\n", __func__);
		goto displayon_err;
	}

displayon_err:
	return ret;

}

static int s6e3hf3_wqhd_exit(struct dsim_device *dsim)
{
	int ret = 0;
#ifdef CONFIG_LCD_ALPM
	struct panel_private *panel = &dsim->priv;
#endif
	dsim_info("MDD : %s was called\n", __func__);
#ifdef CONFIG_LCD_ALPM
	mutex_lock(&panel->alpm_lock);
	if (panel->current_alpm && panel->alpm) {
		dsim->alpm = 1;
		dsim_info("%s : ALPM mode\n", __func__);
	}
	else {
		ret = dsim_write_hl_data(dsim, SEQ_DISPLAY_OFF, ARRAY_SIZE(SEQ_DISPLAY_OFF));
		if (ret < 0) {
			dsim_err("%s : fail to write CMD : DISPLAY_OFF\n", __func__);
			goto exit_err;
		}

		ret = dsim_write_hl_data(dsim, SEQ_SLEEP_IN, ARRAY_SIZE(SEQ_SLEEP_IN));
		if (ret < 0) {
			dsim_err("%s : fail to write CMD : SLEEP_IN\n", __func__);
			goto exit_err;
		}
		msleep(120);
	}

	dsim_info("MDD : %s was called unlock\n", __func__);
#else
	ret = dsim_write_hl_data(dsim, SEQ_DISPLAY_OFF, ARRAY_SIZE(SEQ_DISPLAY_OFF));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : DISPLAY_OFF\n", __func__);
		goto exit_err;
	}

	ret = dsim_write_hl_data(dsim, SEQ_SLEEP_IN, ARRAY_SIZE(SEQ_SLEEP_IN));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SLEEP_IN\n", __func__);
		goto exit_err;
	}

	msleep(120);
#endif

exit_err:
#ifdef CONFIG_LCD_ALPM
	mutex_unlock(&panel->alpm_lock);
#endif
	return ret;
}


static int s6e3hf3_wqhd_init(struct dsim_device *dsim)
{
	int     ret = 0;

	dsim_info("MDD : %s was called\n", __func__);

	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_F0, ARRAY_SIZE(SEQ_TEST_KEY_ON_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_F0\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_ON_FC, ARRAY_SIZE(SEQ_TEST_KEY_ON_FC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_ON_FC\n", __func__);
		goto init_exit;
	}

	/* 7. Sleep Out(11h) */
	ret = dsim_write_hl_data(dsim, SEQ_SLEEP_OUT, ARRAY_SIZE(SEQ_SLEEP_OUT));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_SLEEP_OUT\n", __func__);
		goto init_exit;
	}
	msleep(5);

	/* 9. Interface Setting */
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_DISPLAY_TIMING1, ARRAY_SIZE(S6E3HF3_SEQ_DISPLAY_TIMING1));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_DISPLAY_TIMING1\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_DISPLAY_TIMING2, ARRAY_SIZE(S6E3HF3_SEQ_DISPLAY_TIMING2));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HA3_SEQ_DISPLAY_TIMING2\n", __func__);
		goto init_exit;
	}

	/* 9. Interface Setting */
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_MIC, ARRAY_SIZE(S6E3HF3_SEQ_MIC));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HF3_SEQ_MIC\n", __func__);
		goto init_exit;
	}
	if (hw_rev < 3) {
		ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_DCDC_GLOBAL, ARRAY_SIZE(S6E3HF3_SEQ_DCDC_GLOBAL));
		if (ret < 0) {
			dsim_err("%s : fail to write CMD : S6E3HF3_SEQ_DCDC_GLOBAL\n", __func__);
			goto init_exit;
		}
		ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_DCDC, ARRAY_SIZE(S6E3HF3_SEQ_DCDC));
		if (ret < 0) {
			dsim_err("%s : fail to write CMD : S6E3HF3_SEQ_DCDC\n", __func__);
			goto init_exit;
		}
		dsim_info("%s max77838 system rev : %d\n", __func__, hw_rev);
	}

	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_CASET_SET, ARRAY_SIZE(S6E3HF3_SEQ_CASET_SET));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HF3_SEQ_MIC\n", __func__);
		goto init_exit;
	}
#ifdef CONFIG_LCD_HMT
		if(dsim->priv.hmt_on != HMT_ON)
#endif
	msleep(120);

#ifdef CONFIG_ALWAYS_RELOAD_MTP_FACTORY_BUILD
	ret = lcd_reload_mtp(dynamic_lcd_type, dsim);
#endif

	/* Common Setting */
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_TE_RISING_TIMING, ARRAY_SIZE(S6E3HF3_SEQ_TE_RISING_TIMING));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TE_ON\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_TE_ON, ARRAY_SIZE(S6E3HF3_SEQ_TE_ON));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TE_ON\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_TSP_TE, ARRAY_SIZE(S6E3HF3_SEQ_TSP_TE));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TSP_TE\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_PCD, ARRAY_SIZE(S6E3HF3_SEQ_PCD));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HF3_SEQ_PCD\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_ERR_FG, ARRAY_SIZE(S6E3HF3_SEQ_ERR_FG));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HF3_SEQ_PCD\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_PENTILE_SETTING, ARRAY_SIZE(S6E3HF3_SEQ_PENTILE_SETTING));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : S6E3HF3_SEQ_PCD\n", __func__);
		goto init_exit;
	}

#ifndef CONFIG_PANEL_AID_DIMMING
	/* Brightness Setting */
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_GAMMA_CONDITION_SET, ARRAY_SIZE(S6E3HF3_SEQ_GAMMA_CONDITION_SET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_GAMMA_CONDITION_SET\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_AOR, ARRAY_SIZE(S6E3HF3_SEQ_AOR));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HF3_SEQ_AOR_0\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_MPS_ELVSS_SET, ARRAY_SIZE(S6E3HF3_SEQ_MPS_ELVSS_SET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HF3_SEQ_MPS_ELVSS_SET\n", __func__);
		goto init_exit;
	}

	/* ACL Setting */
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_ACL_OFF, ARRAY_SIZE(S6E3HF3_SEQ_ACL_OFF));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_ACL_OFF\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_ACL_OFF_OPR, ARRAY_SIZE(S6E3HF3_SEQ_ACL_OFF_OPR));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_ACL_OFF_OPR\n", __func__);
		goto init_exit;
	}

	ret = dsim_write_hl_data(dsim, SEQ_GAMMA_UPDATE, ARRAY_SIZE(SEQ_GAMMA_UPDATE));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_GAMMA_UPDATE\n", __func__);
		goto init_exit;
	}

	/* elvss */
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_ELVSS_GLOBAL, ARRAY_SIZE(S6E3HF3_SEQ_ELVSS_GLOBAL));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HF3_SEQ_ELVSS_GLOBAL\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_ELVSS, ARRAY_SIZE(S6E3HF3_SEQ_ELVSS));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HF3_SEQ_ELVSS\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_TSET_GLOBAL, ARRAY_SIZE(S6E3HF3_SEQ_TSET_GLOBAL));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_TSET_GLOBAL\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_TSET, ARRAY_SIZE(S6E3HF3_SEQ_TSET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : SEQ_TSET\n", __func__);
		goto init_exit;
	}
	ret = dsim_write_hl_data(dsim, S6E3HF3_SEQ_VINT_SET, ARRAY_SIZE(S6E3HF3_SEQ_VINT_SET));
	if (ret < 0) {
		dsim_err(":%s fail to write CMD : S6E3HF3_SEQ_VINT_SET\n", __func__);
		goto init_exit;
	}
#endif
	ret = dsim_write_hl_data(dsim, SEQ_TEST_KEY_OFF_F0, ARRAY_SIZE(SEQ_TEST_KEY_OFF_F0));
	if (ret < 0) {
		dsim_err("%s : fail to write CMD : SEQ_TEST_KEY_OFF_F0\n", __func__);
		goto init_exit;
	}

init_exit:
	return ret;
}




struct dsim_panel_ops s6e3hf3_panel_ops = {
	.probe = s6e3hf3_wqhd_probe,
	.displayon = s6e3hf3_wqhd_displayon,
	.exit = s6e3hf3_wqhd_exit,
	.init = s6e3hf3_wqhd_init,
	.dump = s6e3hf3_wqhd_dump,
};



/******************** COMMON ********************/

#ifdef CONFIG_ALWAYS_RELOAD_MTP_FACTORY_BUILD
static int lcd_reload_mtp(int lcd_type, struct dsim_device *dsim)
{
	int i, ret;
	unsigned char mtp[S6E3HA2_MTP_SIZE] = { 0, };
	unsigned char hbm[S6E3HA2_HBMGAMMA_LEN] = { 0, };

	// retry 3 times
	for( i=0; i <3; i++ ) {
		switch (lcd_type) {
			case LCD_TYPE_S6E3HA2_WQHD:
				ret = s6e3ha2_read_init_info(dsim, mtp, hbm);
				dsim_info( "%s : load MTP of s6e3ha2\n", __func__ );
				break;
			case LCD_TYPE_S6E3HA3_WQHD:
				ret = s6e3ha3_read_init_info(dsim, mtp, hbm);
				dsim_info( "%s : load MTP of s6e3ha3\n", __func__ );
				break;
			case LCD_TYPE_S6E3HF3_WQHD:
				ret = s6e3hf3_read_init_info(dsim, mtp, hbm);
				dsim_info( "%s : load MTP of s6e3hf3\n", __func__ );
				break;
			default:
				ret = s6e3ha3_read_init_info(dsim, mtp, hbm);
				dsim_info( "%s : load MTP of s6e3ha3(default)\n", __func__ );
				break;
		}
		if( ret == 0 ) break;
	}
	if( ret != 0 ) return -EIO;

	update_mdnie_coordinate( dsim->priv.coordinate[0], dsim->priv.coordinate[1] );

#ifdef CONFIG_PANEL_AID_DIMMING
	ret = init_dimming(dsim, mtp, hbm);
	if (ret) {
		dsim_err("%s : failed to generate gamma tablen\n", __func__);
	}
#endif

#ifdef CONFIG_LCD_HMT
	ret = hmt_init_dimming(dsim, mtp);
	if (ret) {
		dsim_err("%s : failed to generate gamma tablen\n", __func__);
	}
#endif

	return 0;	//success
}
#endif	// CONFIG_ALWAYS_RELOAD_MTP_FACTORY_BUILD


struct dsim_panel_ops *dsim_panel_get_priv_ops(struct dsim_device *dsim)
{
	switch (dynamic_lcd_type) {
	case LCD_TYPE_S6E3HA2_WQHD:
		return &s6e3ha2_panel_ops;
	case LCD_TYPE_S6E3HA3_WQHD:
		return &s6e3ha3_panel_ops;
	case LCD_TYPE_S6E3HF3_WQHD:
		return &s6e3hf3_panel_ops;
	default:
		return &s6e3ha2_panel_ops;
	}
}


static int __init get_lcd_type(char *arg)
{
	unsigned int lcdtype;

	get_option(&arg, &lcdtype);

	dsim_info("--- Parse LCD TYPE ---\n");
	dsim_info("LCDTYPE : %x\n", lcdtype);

#if 1	// kyNam_150430_ HF3-bare LCD
	if (lcdtype == 0x000010) lcdtype = LCD_TYPE_ID_HF3;
#endif

	switch (lcdtype & LCD_TYPE_MASK) {
	case LCD_TYPE_ID_HA2:
		dynamic_lcd_type = LCD_TYPE_S6E3HA2_WQHD;
		dsim_info("LCD TYPE : S6E3HA2 (WQHD) : %d\n", dynamic_lcd_type);
		aid_dimming_dynamic.elvss_len = S6E3HA2_ELVSS_LEN;
		aid_dimming_dynamic.elvss_reg = S6E3HA2_ELVSS_REG;
		aid_dimming_dynamic.elvss_cmd_cnt = S6E3HA2_ELVSS_CMD_CNT;
		aid_dimming_dynamic.aid_cmd_cnt = S6E3HA2_AID_CMD_CNT;
		aid_dimming_dynamic.aid_reg_offset = S6E3HA2_AID_REG_OFFSET;
		aid_dimming_dynamic.tset_len = S6E3HA2_TSET_LEN;
		aid_dimming_dynamic.tset_reg = S6E3HA2_TSET_REG;
		aid_dimming_dynamic.tset_minus_offset = S6E3HA2_TSET_MINUS_OFFSET;
		aid_dimming_dynamic.vint_reg2 = S6E3HA2_VINT_REG2;
		break;
	case LCD_TYPE_ID_HA3:
		dynamic_lcd_type = LCD_TYPE_S6E3HA3_WQHD;
		dsim_info("LCD TYPE : S6E3HA3 (WQHD) : %d\n", dynamic_lcd_type);
		aid_dimming_dynamic.elvss_len = S6E3HA3_ELVSS_LEN;
		aid_dimming_dynamic.elvss_reg = S6E3HA3_ELVSS_REG;
		aid_dimming_dynamic.elvss_cmd_cnt = S6E3HA3_ELVSS_CMD_CNT;
		aid_dimming_dynamic.aid_cmd_cnt = S6E3HA3_AID_CMD_CNT;
		aid_dimming_dynamic.aid_reg_offset = S6E3HA3_AID_REG_OFFSET;
		aid_dimming_dynamic.tset_len = S6E3HA3_TSET_LEN;
		aid_dimming_dynamic.tset_reg = S6E3HA3_TSET_REG;
		aid_dimming_dynamic.tset_minus_offset = S6E3HA3_TSET_MINUS_OFFSET;
		aid_dimming_dynamic.vint_reg2 = S6E3HA3_VINT_REG2;
		break;
	case LCD_TYPE_ID_HF3:
		dynamic_lcd_type = LCD_TYPE_S6E3HF3_WQHD;
		dsim_info("LCD TYPE : S6E3HF3 (WQHD) : %d\n", dynamic_lcd_type);
		aid_dimming_dynamic.elvss_len = S6E3HF3_ELVSS_LEN;
		aid_dimming_dynamic.elvss_reg = S6E3HF3_ELVSS_REG;
		aid_dimming_dynamic.elvss_cmd_cnt = S6E3HF3_ELVSS_CMD_CNT;
		aid_dimming_dynamic.aid_cmd_cnt = S6E3HF3_AID_CMD_CNT;
		aid_dimming_dynamic.aid_reg_offset = S6E3HF3_AID_REG_OFFSET;
		aid_dimming_dynamic.tset_len = S6E3HF3_TSET_LEN;
		aid_dimming_dynamic.tset_reg = S6E3HF3_TSET_REG;
		aid_dimming_dynamic.tset_minus_offset = S6E3HF3_TSET_MINUS_OFFSET;
		aid_dimming_dynamic.vint_reg2 = S6E3HF3_VINT_REG2;
		break;
	default:
		dynamic_lcd_type = LCD_TYPE_S6E3HA2_WQHD;
		dsim_info("LCD TYPE : [UNKNOWN] -> S6E3HA2 (WQHD) : %d\n", dynamic_lcd_type);
		aid_dimming_dynamic.elvss_len = S6E3HA2_ELVSS_LEN;
		aid_dimming_dynamic.elvss_reg = S6E3HA2_ELVSS_REG;
		aid_dimming_dynamic.elvss_cmd_cnt = S6E3HA2_ELVSS_CMD_CNT;
		aid_dimming_dynamic.aid_cmd_cnt = S6E3HA2_AID_CMD_CNT;
		aid_dimming_dynamic.aid_reg_offset = S6E3HA2_AID_REG_OFFSET;
		aid_dimming_dynamic.tset_len = S6E3HA2_TSET_LEN;
		aid_dimming_dynamic.tset_reg = S6E3HA2_TSET_REG;
		aid_dimming_dynamic.tset_minus_offset = S6E3HA2_TSET_MINUS_OFFSET;
		aid_dimming_dynamic.vint_reg2 = S6E3HA2_VINT_REG2;
		break;
	}
	return 0;
}
early_param("lcdtype", get_lcd_type);


static int __init get_hw_rev(char *arg)
{
	get_option(&arg, &hw_rev);
	dsim_info("hw_rev : %d\n", hw_rev);

	return 0;
}

early_param("androidboot.hw_rev", get_hw_rev);



