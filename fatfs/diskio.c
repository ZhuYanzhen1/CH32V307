#include "ff.h"
#include "diskio.h"
#include "w25qxx.h"

#define FLASH_SECTOR_SIZE   512
#define FLASH_BLOCK_SIZE    8

DSTATUS disk_status(
    BYTE pdrv        /* Physical drive nmuber to identify the drive */
) {
    return RES_OK;
}

DSTATUS disk_initialize(
    BYTE pdrv        /* Physical drive nmuber to identify the drive */
) {
    if (pdrv == 0)
        return RES_OK;
    else
        return STA_NOINIT;
}

DRESULT disk_read(
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    BYTE *buff,         /* Data buffer to store read data */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to read */
) {
    if (!count)
        return RES_PARERR;
    if (pdrv == 0) {
        for (; count > 0; count--) {
            w25qxx_read_bytes(sector * FLASH_SECTOR_SIZE, buff, FLASH_SECTOR_SIZE);
            sector++;
            buff += FLASH_SECTOR_SIZE;
        }
        return RES_OK;
    } else
        return RES_ERROR;
}

DRESULT disk_write(
    BYTE pdrv,              /* Physical drive nmuber to identify the drive */
    const BYTE *buff,       /* Data to be written */
    LBA_t sector,           /* Start sector in LBA */
    UINT count              /* Number of sectors to write */
) {
    if (!count)
        return RES_PARERR;
    if (pdrv == 0) {
        for (; count > 0; count--) {
            w25qxx_write_bytes(sector * FLASH_SECTOR_SIZE, (unsigned char *) buff, FLASH_SECTOR_SIZE);
            sector++;
            buff += FLASH_SECTOR_SIZE;
        }
        return RES_OK;
    } else
        return RES_ERROR;
}

DRESULT disk_ioctl(
    BYTE pdrv,          /* Physical drive nmuber (0..) */
    BYTE cmd,           /* Control code */
    void *buff          /* Buffer to send/receive control data */
) {
    DRESULT res;
    if (pdrv == 0) {
        switch (cmd) {
            case CTRL_SYNC:res = RES_OK;
                break;
            case GET_SECTOR_SIZE:*(WORD *) buff = FLASH_SECTOR_SIZE;
                res = RES_OK;
                break;
            case GET_BLOCK_SIZE:*(WORD *) buff = FLASH_BLOCK_SIZE;
                res = RES_OK;
                break;
            case GET_SECTOR_COUNT:*(DWORD *) buff = (w25qxx_get_size() / FLASH_SECTOR_SIZE);
                res = RES_OK;
                break;
            default:res = RES_PARERR;
                break;
        }
    } else res = RES_ERROR;
    return res;
}

DWORD get_fattime(void) {
    return 0;
}

void *ff_memalloc(      /* Returns pointer to the allocated memory block (null if not enough core) */
    UINT msize          /* Number of bytes to allocate */
) {
    return (void *) pvPortMalloc(msize);
}

void ff_memfree(
    void *mblock        /* Pointer to the memory block to free (nothing to do if null) */
) {
    vPortFree(mblock);
}

int ff_cre_syncobj(     /* 1:Function succeeded, 0:Could not create the sync object */
    BYTE vol,           /* Corresponding volume (logical drive number) */
    FF_SYNC_t *sobj     /* Pointer to return the created sync object */
) {
    *sobj = xSemaphoreCreateMutex();
    return (int) (*sobj != NULL);
}

int ff_del_syncobj(     /* 1:Function succeeded, 0:Could not delete due to an error */
    FF_SYNC_t sobj      /* Sync object tied to the logical drive to be deleted */
) {
    vSemaphoreDelete(sobj);
    return 1;
}

int ff_req_grant(       /* 1:Got a grant to access the volume, 0:Could not get a grant */
    FF_SYNC_t sobj      /* Sync object to wait */
) {
    return (int) (xSemaphoreTake(sobj, FF_FS_TIMEOUT) == pdTRUE);
}

void ff_rel_grant(
    FF_SYNC_t sobj      /* Sync object to be signaled */
) {
    xSemaphoreGive(sobj);
}
