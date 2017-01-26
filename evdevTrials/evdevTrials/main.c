//
//  main.c
//  evdevTrials
//
//  Created by Ahmed Moussa on 1/23/17.
//  Copyright © 2017 Ahmed Moussa. All rights reserved.
//

#include <stdio.h>
#include <libevdev.h>
#include <libevdev-uinput.h>

int main(int argc, char **argv)
{
    int err;
    struct libevdev *dev;
    struct libevdev_uinput *uidev;
    
    dev = libevdev_new();
    libevdev_set_name(dev, "fake keyboard device");
    
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, KEY_A, NULL);
    
    err = libevdev_uinput_create_from_device(dev,
                                             LIBEVDEV_UINPUT_OPEN_MANAGED,
                                             &uidev);
    
    if (err != 0)
        return err;
    
    libevdev_uinput_write_event(uidev, EV_KEY, KEY_A, 1);
    libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event(uidev, EV_KEY, KEY_A, 0);
    libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    
    libevdev_uinput_destroy(uidev);
    printf("Complete\n");
}
