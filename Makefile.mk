#====================================================================
# File automatically generated by ES3  
# This file regenerated each time you run EISC Studio, so save under a 
# new name if you hand edit, or it will be overwritten.
#				www.adc.co.kr
#====================================================================



ECHO	=	echo.exe
CC	=	ae32000-elf-gcc
AS	=	ae32000-elf-as
LD	=	ae32000-elf-ld
AR	=	ae32000-elf-ar
OBJCOPY	=	ae32000-elf-objcopy
OBJDUMP	=	ae32000-elf-objdump
CFLAGS	=	 -O2  -g -fno-strength-reduce -Wall 
ASFLAGS	=	 -gstabs 
LDFLAGS	=	-Wall -nostartfiles -lgcc -lc -lm -Wl,--warn-common 
DISFLAGS	=	-dzShxl
ARFLAGS	=	rcusv
#====== Directory =====
OUTPUTDIR	=	output/
OBJDIR	=	obj/
INCDIR	=	\
	 -I"../../../include"
LIBDIR	=	
LIBS	=	\
	../../../lib/libadLuna.a

SOURCES	 += main.c
SOURCES	 += user_task.c
SOURCES	 += ui_globals.c
SOURCES	 += NRC7394_wifi.c
SOURCES	 += w5500/Ethernet/W5500/w5500.c
SOURCES	 += w5500/Ethernet/socket.c
SOURCES	 += w5500/Ethernet/wizchip_conf.c
SOURCES	 += w5500/Internet/DHCP/dhcp.c
SOURCES	 += w5500/dhcp_client.c
SOURCES	 += w5500/loopback.c
SOURCES	 += w5500/w5500_spi.c
SOURCES	 += ../../../startup/adLunaInit.c
SOURCES	 += ../../../startup/startup_adLuna.s

OBJECTS	 += ${OBJDIR}main.o
OBJECTS	 += ${OBJDIR}user_task.o
OBJECTS	 += ${OBJDIR}ui_globals.o
OBJECTS	 += ${OBJDIR}NRC7394_wifi.o
OBJECTS	 += ${OBJDIR}w5500.o
OBJECTS	 += ${OBJDIR}socket.o
OBJECTS	 += ${OBJDIR}wizchip_conf.o
OBJECTS	 += ${OBJDIR}dhcp.o
OBJECTS	 += ${OBJDIR}dhcp_client.o
OBJECTS	 += ${OBJDIR}loopback.o
OBJECTS	 += ${OBJDIR}w5500_spi.o
OBJECTS	 += ${OBJDIR}adLunaInit.o
OBJECTS	 += ${OBJDIR}startup_adLuna.o

DEPS	 += ${OBJDIR}main.d
DEPS	 += ${OBJDIR}user_task.d
DEPS	 += ${OBJDIR}ui_globals.d
DEPS	 += ${OBJDIR}NRC7394_wifi.d
DEPS	 += ${OBJDIR}w5500.d
DEPS	 += ${OBJDIR}socket.d
DEPS	 += ${OBJDIR}wizchip_conf.d
DEPS	 += ${OBJDIR}dhcp.d
DEPS	 += ${OBJDIR}dhcp_client.d
DEPS	 += ${OBJDIR}loopback.d
DEPS	 += ${OBJDIR}w5500_spi.d
DEPS	 += ${OBJDIR}adLunaInit.d
DEPS	 += ${OBJDIR}startup_adLuna.d



#====== TAGET Define =====
TARGET	=	$(OUTPUTDIR)crane_master

BINARY	=	$(TARGET).bin

HEX	=	$(TARGET).hex

DISASSEMBLE	=	$(TARGET).dis

ALL	:	$(TARGET) $(BINARY) 

all	:	$(TARGET) $(BINARY) 

binary	:	$(BINARY)

bin	:	$(BINARY)

hex	:	$(HEX)

disassemble	:	$(DISASSEMBLE)

dis	:	$(DISASSEMBLE)

clean	:
	@$(ECHO) 'Deleting intermediate and output files... '
	rm -f $(OBJECTS) $(TARGET) $(DISASSEMBLE) $(BINARY) $(DEPS)
	@$(ECHO) ' '

cleanall	:
	@$(ECHO) 'Deleting intermediate and output files... '
	rm -f $(OBJECTS) $(TARGET) $(DISASSEMBLE) $(BINARY) $(DEPS)
	@$(ECHO) ' '

cleanobj	:
	@$(ECHO) 'Deleting intermediate files... '
	rm -f $(OBJECTS) 
	@$(ECHO) ' '

cleanbin	:
	rm -f $(BINARY)
	@$(ECHO) ' '

cleandis	:
	rm -f $(DISASSEMBLE)
	@$(ECHO) ' '

ifeq ($(FULLMSG),no)
ECHOMARK = @
else
ECHOMARK = 
endif

-include ${OBJDIR}main.d
-include ${OBJDIR}user_task.d
-include ${OBJDIR}ui_globals.d
-include ${OBJDIR}NRC7394_wifi.d
-include ${OBJDIR}w5500.d
-include ${OBJDIR}socket.d
-include ${OBJDIR}wizchip_conf.d
-include ${OBJDIR}dhcp.d
-include ${OBJDIR}dhcp_client.d
-include ${OBJDIR}loopback.d
-include ${OBJDIR}w5500_spi.d
-include ${OBJDIR}adLunaInit.d
-include ${OBJDIR}startup_adLuna.d

#============= TARGET ====================
${TARGET} : Makefile.mk ${OBJECTS} ${LIBS} ../../../startup/adLuna.ld
	@$(ECHO) 'Linking...'
	$(ECHOMARK)$(CC) -o $(TARGET) $(OBJECTS) $(LIBDIR) -Xlinker --start-group $(LIBS) -Xlinker --end-group $(LDFLAGS) -Xlinker -T"../../../startup/adLuna.ld"
	@$(ECHO) 'Finished building : $@ '
	@$(ECHO) ' '

$(BINARY): $(TARGET)
	@$(ECHO) 'Generating binary file...'
	$(ECHOMARK)$(OBJCOPY) -O binary $(TARGET) $@
	@$(ECHO) 'Finished building : $@ '
	@$(ECHO) ' '
	@$(ECHO) 'Program section information '
	$(ECHOMARK)size $(TARGET)
	@$(ECHO) ' '

$(DISASSEMBLE) : $(TARGET)
	@$(ECHO) 'Generating disassemble file...'
	$(ECHOMARK)$(OBJDUMP) $(DISFLAGS) $(TARGET) > $@ 
	@$(ECHO) 'Finished building : $@ '
	@$(ECHO) ' '

${OBJDIR}main.o : main.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}user_task.o : user_task.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}ui_globals.o : ui_globals.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}NRC7394_wifi.o : NRC7394_wifi.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}w5500.o : w5500/Ethernet/W5500/w5500.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}socket.o : w5500/Ethernet/socket.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}wizchip_conf.o : w5500/Ethernet/wizchip_conf.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}dhcp.o : w5500/Internet/DHCP/dhcp.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}dhcp_client.o : w5500/dhcp_client.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}loopback.o : w5500/loopback.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}w5500_spi.o : w5500/w5500_spi.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}adLunaInit.o : ../../../startup/adLunaInit.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

${OBJDIR}startup_adLuna.o : ../../../startup/startup_adLuna.s
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) -Wa,$(ASFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -o "$@" "$<"

