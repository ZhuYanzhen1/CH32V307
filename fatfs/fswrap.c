/**************************************************************************/ /**
    \file       fstest.c
    \brief      fstest function Source File
    \author     LaoZhu
    \version    V1.6.1
    \date       24. July 2022
******************************************************************************/

#include "fswrap.h"
#include "ff.h"
#include "system.h"

/*! \brief FAT file system structure */
FATFS filesystem;

/*! \brief File system buffer arrays */
static unsigned char fs_buffer[FF_MAX_SS * 4];

unsigned char fs_get_free(char *drv) {
    FATFS *fs1;
    unsigned int fre_clust = 0, fre_sect, tot_sect, total, free;
    FRESULT result = f_getfree((const TCHAR *) drv, (DWORD *) &fre_clust, &fs1);
    if (result == 0) {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize;
        fre_sect = fre_clust * fs1->csize;
        total = tot_sect >> 1;     // Trans into KB
        free = fre_sect >> 1;      // Trans into KB
        PRINTF_LOGI("File system total size: %dKB, free size: %dKB\r\n", total, free)
    }
    return result;
}

static FILINFO fileinfo;
static DIR dir;
unsigned char fs_scan_files(char *path) {
    FRESULT res = f_opendir(&dir, (const TCHAR *) path);
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fileinfo);
            if (res != FR_OK || fileinfo.fname[0] == 0)
                break;
            PRINTF_LOGI("%s/%s\r\n", path, fileinfo.fname)
        }
    }
    return res;
}

void fs_config(void) {
    static unsigned char filesystem_test_data[32] = {0};
    static FIL newfile, readfile;
    static unsigned int bw, br, counter;
    FRESULT result = f_mount(&filesystem, "0:", 1);
    if (result == FR_OK) {
        test_file_system_access:
        for (counter = 0; counter < sizeof(filesystem_test_data); ++counter)
            filesystem_test_data[counter] = counter;
        FRESULT fr = f_open(&newfile, "0:/test_fs_file.txt", FA_WRITE | FA_CREATE_ALWAYS);
        if (fr) while (1);
        fr = f_write(&newfile, filesystem_test_data, sizeof(filesystem_test_data), &bw);
        if (fr) while (1);
        f_close(&newfile);
        for (counter = 0; counter < sizeof(filesystem_test_data); ++counter)
            filesystem_test_data[counter] = 0;
        fr = f_open(&readfile, "0:/test_fs_file.txt", FA_READ);
        if (fr) while (1);
        fr = f_read(&readfile, filesystem_test_data, sizeof(filesystem_test_data), &br);
        if (fr) while (1);
        f_close(&readfile);
        for (counter = 0; counter < sizeof(filesystem_test_data); ++counter)
            if (filesystem_test_data[counter] != counter)
                break;
        f_unlink("0:/test_fs_file.txt");

        if (counter != sizeof(filesystem_test_data)) {
            PRINTF_LOGW("File system mounted success, read write test failed, data:\r\n")
#if PRINT_DEBUG_LEVEL >= 2
            printf(LOG_COLOR_W);
            for (counter = 0; counter < sizeof(filesystem_test_data); ++counter) {
                if (counter % 8 == 0)
                    printf("\r\n\t");
                printf("0x%02x ", filesystem_test_data[counter]);
            }
            printf("\r\n\r\n");
            printf(LOG_RESET_COLOR);
#endif
        } else PRINTF_LOGI("File system mounted success, read write test success\r\n")
        fs_get_free("0:");
    } else if (result == FR_NO_FILESYSTEM) {
        if (f_mkfs("0:", 0, fs_buffer, sizeof(fs_buffer)) == FR_OK) {
            f_setlabel((const TCHAR *) "0:FLASH");
            f_mount(&filesystem, "0:", 1);
            PRINTF_LOGW("File system not detected, making file system success\r\n")
            goto test_file_system_access;
        } else {
            PRINTF_LOGE("File system not detected, making file system failed\r\n")
            while (1);
        }
    } else PRINTF_LOGE("Failed to mount file system, reason: %d\r\n", result)
}