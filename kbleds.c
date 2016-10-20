#include <linux/module.h>

#include <linux/kernel.h>

#include <linux/configfs.h>
#ifndef _LINUX_CONFIG_H
#define _LINUX_CONFIG_H

//#include <linux/autoconf.h>

#endif
#include <linux/init.h>

#include <linux/tty.h>   /* for fg_console, MAX_NR_CONSOLES */

#include <linux/kd.h>

#include <linux/vt.h>
#include <linux/vt_kern.h>

#include <linux/console_struct.h> /* for vc_cons */

MODULE_DESCRIPTION("using the keyboard LEDs ");

MODULE_AUTHOR("Amr Mohamed Awd ");

MODULE_LICENSE("GPL");

struct timer_list mytimer_list;
struct tty_driver *my_driver;
char kbledstatus = 0 ;


#define BLINK_DELAY HZ/5
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xff

static void my_timer_func(unsigned long ptr){

  int *pstatus =(int *) ptr;

  if(*pstatus==ALL_LEDS_ON)
    *pstatus=RESTORE_LEDS;
  else
    *pstatus=ALL_LEDS_ON;

  (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty,KDSETLED, *pstatus);

  mytimer_list.expires=jiffies+BLINK_DELAY;

  add_timer(&mytimer_list);

}

static int __init kbleds_init(void){

  int i=0;

  printk(KERN_INFO "kbleds: loading");

  printk(KERN_INFO "kbleds: fg_condole is %x ",fg_console);

  for(i=0; i<MAX_NR_CONSOLES;i++){

    if(!vc_cons[i].d)
      break;

    printk(KERN_INFO "LEDS: console[%i/%i] #%i,tty %lx ",i,MAX_NR_CONSOLES,vc_cons[i].d->vc_num,(unsigned long)vc_cons[i].d->port.tty);

  }

  printk(KERN_INFO "LEDS: finished scanning consoles ");


  my_driver=vc_cons[fg_console].d->port.tty->driver;

  printk(KERN_INFO "LEDS:tty driver magic %x ",my_driver->magic);

  init_timer(&mytimer_list);
  mytimer_list.function=my_timer_func;
  mytimer_list.data=(unsigned long)&kbledstatus;
  mytimer_list.expires=jiffies+BLINK_DELAY;
  add_timer(&mytimer_list);

  return 0;

}


static void __exit kbleds_exit(void){

  printk(KERN_INFO "LEDS: unloaaaading");

  del_timer(&mytimer_list);

  (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty,KDSETLED,RESTORE_LEDS);

}

module_init(kbleds_init);

module_exit(kbleds_exit);
