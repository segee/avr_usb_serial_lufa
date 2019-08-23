/*
             LUFA Library
     Copyright (C) Dean Camera, 2015.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2015  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/* This code is based on Dean Camera's "VirtualSerial.c" from the Demo code included */
/* with the LUFA library.  Unnecessary parts have been removed and the "High Low"     */
/* game has been dropped in largely unmodified.  This is meant to demonstrate a          */
/* minimal USB device on an AVR.  From the AVR perspective the USB is simply          */
/* stdin and stdout (with a CONNECTED flag).  From the PC's perspective the AVR is   */
/* a serial port.  It is a greatly simplified serial port in the sense that baud rate and         */
/* several other setting do not matter.  (The AVR can access these settings, but this is  */
/* not included in this code.)                                                                                             */


#include "VirtualSerial.h"

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;  //at the bottom of this file
// It is essential, but the documentation didn't give me the information I needed to create one
//so I used the one from the example, but I don't feel good about it.

#define CONNECTED (VirtualSerial_CDC_Interface.State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR)


static FILE USBSerialStream;  //This will become stdin and stdout


void do_high_low(void);  // This is from the 477 web site
void bruces_usb_init(void);

/* Below is a minimal main function for doing USB communication through a     */
/* Virtual serial port.  the function bruces_usb_init does the minimal initialization*/
/* then the program waits for a connection (PC opens port) before calling a       */
/* function to do whatever needs to be done.  In this case it is a high-low           */
/* guessing game from the 477 web site.  It **IS** important, however that        */
/* whatever program is run, it must spend the majority of its time blocked         */
/* waiting for I/O (e.g., in a call to scanf)                                           */

int main(void)
{
	bruces_usb_init();  // get the USB stuff going


	while(1)
	{
               while(!CONNECTED)
               {
                  USB_USBTask();
                  CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
               }

             do_high_low();
	} 
}

/* This code is essentially identical to the high_low_server code that  */
/* is one of the examples on the 477 page.  File I/O, stdin/stdout and  */
/* serial I/O are all incredibly similar and almost interchangeable           */

void do_high_low(void)
{
   FILE *fp, *fpr;
   int answer, guess;
 
   answer=rand()%10000;
   fp=stdout;
   fpr=stdin;
 
  fprintf(fp,"Welcome to the high-low guessing game \r\n");
  fprintf(fp,"please enter integer guesses between 0 and 10000\r\n");
 
  guess=answer+1;
  while((guess!=answer) )
  {   	while(fscanf(fpr,"%d",&guess)!=1) fscanf(fpr,"%*s");
 	if(guess<answer) fprintf(fp,"Too low, try again \r\n");
    	else if (guess>answer) fprintf(fp,"Too High, try again \r\n");
	else fprintf(fp,"Congratulations you got it right\r\n");
 
   }
   
}

/* Minimal configuration for USB I/O through stdin and stdout.  Disable watchdog timer, */
/* call USB_init in LUFA library, creating a blocking stream (note it is important to be       */
/* blocked most of the time for the USB stuff to work correctly), and set stdin and stdout  */
/* to point to it. Next it enables interrupts and waits for the host to open the serial port     */

void bruces_usb_init(void)
{                   
	wdt_disable();     //Make sure the Watchdog doesn't reset us
	USB_Init();  // Lufa library call to initialize USB
	/* Create a stream for the interface */
	CDC_Device_CreateBlockingStream(&VirtualSerial_CDC_Interface,&USBSerialStream);
            stdin=&USBSerialStream;  //By setting stdin and stdout to point to the stream we
            stdout=&USBSerialStream; //can use regular printf and scanf calls
	GlobalInterruptEnable();      // interrupts need to be enabled
 }

/* The following two event handlers are important.  When the USB interface  */
/* gets the event, we need to use the CDC routines to deal with them           */ 

/* When the USB library throws a Configuration Changed event we need to call */
/* CDC_Device_ConfigureEndpoints                                                                    */
void EVENT_USB_Device_ConfigurationChanged(void)
{    CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface); }

/* When we get a Control Request from the USB library we need to call         */
/* CDC_Device_ProcessControlRequest.                                                        */
void EVENT_USB_Device_ControlRequest(void)
{  CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface); }

/** This is taken directly from the VirtualSerial example.  Even with the aid of the  *
* Documentation I could not figure out all that is going on here.                             *
* It bothers me and I would *LOVE* to have someone find more information         *
*                                                                                                                               */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};



