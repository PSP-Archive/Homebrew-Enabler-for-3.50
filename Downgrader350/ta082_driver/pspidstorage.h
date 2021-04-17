#ifndef __PSPIDSTORAGE_H__
#define __PSPIDSTORAGE_H__

/* sceIdStorageLookup() - Retrieves the value associated with a key
 * args: key    = id-storage key
 *       offset = offset within the 512-byte leaf
 *       buf    = buffer with enough storage
 *       len    = amount of data to retrieve (offset + len must be <= 512 bytes)
 */
int sceIdStorageLookup(u16 key, u32 offset, void *buf, u32 len);

/* sceIdStorageReadLeaf() - Retrieves the whole 512-byte container for the key
 * args: key = id-storage key
 *       buf = buffer with at last 512-bytes of storage
 */
int sceIdStorageReadLeaf(u16 key, void *buf); 

/* sceIdStorageUpdate() - Writes the value associated with a key
 * args: key    = id-storage key
 *       offset = offset within the 512-byte leaf
 *       buf    = buffer 
 *       len    = amount of data to write (offset + len must be <= 512 bytes)
 */
int sceIdStorageUpdate(u16 key, u32 offset, void *buf, u32 len);

/* sceIdStorageWriteLeaf() - Writes the whole 512-byte container for the key
 * args: key = id-storage key
 *       buf = buffer with at last 512-bytes of storage
 */
int sceIdStorageWriteLeaf(u16 key, void *buf); 

int sceIdStorageIsReadOnly();

#endif

