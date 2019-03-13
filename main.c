#include <stdio.h>

#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "enums.h"
#include "data.c"

double compute_elapsed_time(struct timeval start, struct timeval end)
{
    return (double) (end.tv_usec - start.tv_usec) / 1000. +
           (double) (end.tv_sec - start.tv_sec) * 1000.;
}

int main(int argc, char **argv) {
  struct timeval  start, end, global_start, global_end;
  gettimeofday(&global_start, NULL);

  if (argc < 2) {
    printf("Usage: %s mode modeparams...\n", argv[0]);
    return -1;
  }

  libusb_context *ctx = NULL;

  int r = libusb_init(&ctx);
  int exitcode = 0;
  if (r < 0) {
    printf("libusb_init error %d\n", r);
    return 1;
  }
  libusb_device_handle *handle = NULL;
  handle = libusb_open_device_with_vid_pid(ctx, 0x04d9, 0x8008);
  if (handle == NULL) {
    printf("Failed to open device!\n");
    libusb_exit(ctx);
    return 1;
  }
  if (libusb_set_auto_detach_kernel_driver(handle, 0) < 0) {
    printf("Kernel ctrl driver auto detach failed.\n");
    goto exit;
  }
  if (libusb_set_auto_detach_kernel_driver(handle, 3) < 0) {
    printf("Kernel driver auto detach failed.\n");
    goto exit;
  }

  gettimeofday(&start, NULL);
  r = libusb_claim_interface(handle, 0);
  if (r < 0) {
    printf("Failed to claim ctrl interface! %d\n", r);
    exitcode = 4;
    goto exit;
  }
  r = libusb_claim_interface(handle, 3);
  if (r < 0) {
    printf("Failed to claim interface! %d\n", r);
    exitcode = 2;
    goto exit;
  }
  gettimeofday(&end, NULL);
  printf ("Claiming duration = %.2f ms\n", 
  	compute_elapsed_time(start, end));

  char* mode = argv[1];
  if (strcmp(mode, "custom") == 0) {
    if (argc < 3) {
      printf("Usage: %s custom file\n", argv[0]);
      exitcode = -1;
      goto exit;
    }
    // Custom mode
    gettimeofday(&start, NULL);
    FILE *fd = fopen(argv[2], "rb");
    if (!fd) {
      printf("fopen(%s) failed: %s\n", argv[2], strerror(errno));
      exitcode = -1;
      goto exit;
    }
    fread(m_white_data, 512, 1, fd);
    fclose(fd);
    gettimeofday(&end, NULL);
    printf ("File reading duration = %.2f ms\n", 
  	compute_elapsed_time(start, end));

    gettimeofday(&start, NULL);
    r = set_custom_mode(handle, m_white_data);
    gettimeofday(&end, NULL);
    printf ("Set custom mode duration = %.2f ms\n", 
  	compute_elapsed_time(start, end));

    if (r < 0) {
      printf("Failed to set custom mode!\n");
      exitcode = -1;
      goto exit;
    }
    exitcode = -1;
    goto exit;
  }

exit:
  libusb_release_interface(handle, 0);
  libusb_release_interface(handle, 3);
  libusb_close(handle);
  libusb_exit(ctx);
  gettimeofday(&global_end, NULL);
  printf ("Global execution time = %.2f ms\n", 
  	compute_elapsed_time(global_start, global_end));


  return exitcode;
}
