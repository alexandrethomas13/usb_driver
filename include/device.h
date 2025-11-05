#ifndef DRIVER_H
#define DRIVER_H

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kref.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/usb.h>

// USB endpoint direction flags
#define USB_DIR_IN  0x80 // Endpoint direction: Device to Host (IN)
#define USB_DIR_OUT 0x00 // Endpoint direction: Host to Device (OUT)

// USB address ranges
#define USB_ENDPOINT_IN_START  0x80 // Start IN endpoint range
#define USB_ENDPOINT_IN_END    0x8F // End IN endpoint range
#define USB_ENDPOINT_OUT_START 0x00 // Start OUT endpoint range
#define USB_ENDPOINT_OUT_END   0x0F // End OUT endpoint range

// USB endpoint masks
#define USB_ENDPOINT_NUMBER_MASK   0x0F // Mask to extract endpoint number (bits 0–3)
#define USB_ENDPOINT_DIR_MASK      0x80 // Mask to extract direction bit (bit 7)
#define USB_ENDPOINT_XFERTYPE_MASK 0x03 // Mask to extract transfer type (bits 0–1)
// NOTE THAT BITS 4-6 ARE RESERVED

// USB transfer types
#define USB_ENDPOINT_XFER_CONTROL 0 // Control transfer (setup/configuration)
#define USB_ENDPOINT_XFER_ISOC    1 // Streaming transfer (audio/video, time-sensitive)
#define USB_ENDPOINT_XFER_BULK    2 // Bulk transfer (used for large data transfers)
#define USB_ENDPOINT_XFER_INT     3 // Interrupt transfer (used for low-latency data transfers)

#define USB_ENDPOINT_MAX 15 // Max number of endpoints (0–15)

#define USB_IS_VALID_ENDPOINT_ADDR(addr)                                                           \
    (((addr) >= USB_ENDPOINT_OUT_START && (addr) <= USB_ENDPOINT_OUT_END) ||                       \
     ((addr) >= USB_ENDPOINT_IN_START && (addr) <= USB_ENDPOINT_IN_END))

typedef _Bool bool;
#define true  1
#define false 0

typedef struct usb_endpoint_info
{
    __u8        address;       // Endpoint address
    __u8        number;        // Endpoint number (0–15)
    bool        is_in;         // true -> IN or false -> OUT
    __u8        transfer_type; // Transfer type: control, bulk, interrupt, streaming
    const char* purpose;       // Optional description of what this endpoint is used for
} usb_endpoint_info_t;

typedef struct usb_device_endpoints
{
    usb_endpoint_info_t endpoints[USB_ENDPOINT_MAX + 1]; // Array of endpoint info
    size_t              count; //  Number of valid endpoints found and stored
} usb_device_endpoints_t;

struct usb_device_data
{
    struct usb_device*     udev;           // Pointer to USB device
    struct usb_interface*  interface;      // Pointer to active interface
    unsigned char*         bulk_in_buffer; // Buffer for bulk IN transfers
    size_t                 bulk_in_size;   // Size of bulk IN buffer
    usb_device_endpoints_t bulk_endpoints; // Bulk endpoint information
    struct kref            kref;           // Reference counter for cleanup
    /* Use kref_get() to increment and kref_put() to decrement
     * Cleanup called when count reaches 0
     */
};

static int     usb_probe(struct usb_interface* interface, const struct usb_device_id* id);
static void    usb_disconnect(struct usb_interface* interface);
static int     usb_open(struct inode* inode, struct file* file);
static int     usb_release(struct inode* inode, struct file* file);
static ssize_t usb_read(struct file* file, char __user* buffer, size_t count, loff_t* ppos);
static ssize_t usb_write(struct file* file, const char __user* buffer, size_t count, loff_t* ppos);

#endif
