/**************************************************************************/ /**
    \file       fsrest.c
    \brief      fsrest function Header File
    \author     LaoZhu
    \version    V1.6.1
    \date       24. July 2022
******************************************************************************/

#ifndef MAIN_C_FATFS_FSTEST_H_
#define MAIN_C_FATFS_FSTEST_H_

unsigned char fs_get_free(char *drv);
unsigned char fs_scan_files(char *path);
void fs_config(void);

#endif //MAIN_C_FATFS_FSTEST_H_
