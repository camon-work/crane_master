//****************************************************************************
//
//! \file w5500_spi.c
//! \brief WIZnet W5500 EVB spi interface file
//!
//! Copyright (c)  2014, WIZnet Co., LTD.
//! All rights reserved.
//
//****************************************************************************

#include "sdk.h"
#include "w5500_spi.h"
#include "../ui_globals.h"

void SPI_Init()
{
	spi_master_init(WIZNET_SPI_CH);
	spi_set_freq(WIZNET_SPI_CH, SPI_MASTER, 15000000);	// 15MHz
	//*R_GPODIR(ETH_RST_PORT_NUM) |= (1 << ETH_RST_PORT_BIT  );
	// WIZNET_RST_LOW();
	// delayms(50);
	// WIZNET_RST_HIGH();
	// delayms(100);
	
}

void W5500_Init()
{
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };


	/* wizchip initialize*/
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		debugprintf("WIZCHIP Initialized fail.\r\n");
		while (1);
	}
	
/*
	do {
		if (ctlwizchip(CW_GET_PHYLINK, (void*) &tmp) == -1)
			debugprintf("Unknown PHY Link stauts.\r\n");
	} while (tmp == PHY_LINK_OFF);
*/
}
