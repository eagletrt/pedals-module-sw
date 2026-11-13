Import("env")

import struct

# Must match the bootloader: flash_layout.c (first user sector) and
# BOOT_FLASH_VECTOR_TABLE_CS_OFFSET in the OpenBLT STM32C0 flash.c.
APP_BASE_ADDRESS = 0x08003000
CHECKSUM_OFFSET = 0xC0
CHECKSUM_PLACEHOLDER = 0x55AA11EE  # reserved word at the end of g_pfnVectors


def write_openblt_checksum(source, target, env):
    """
    Writes the OpenBLT signature checksum into firmware.bin and generates firmware.srec.

    The bootloader only starts the main firmware if the two's complement of the sum of the
    first 7 vector table entries is stored at CHECKSUM_OFFSET. It writes this value itself
    when flashing over CAN, but not when the image is flashed through SWD, so it is
    precomputed here. The .srec is the file format BootCommander/MicroBoot expect.
    """
    bin_path = target[0].get_abspath()
    with open(bin_path, "rb") as f:
        image = bytearray(f.read())

    (placeholder,) = struct.unpack_from("<I", image, CHECKSUM_OFFSET)
    if placeholder != CHECKSUM_PLACEHOLDER:
        raise RuntimeError(
            "OpenBLT checksum slot not found at offset 0x%X (read 0x%08X): the vector "
            "table in startup_stm32c092xx.s must end with .word 0x%08X"
            % (CHECKSUM_OFFSET, placeholder, CHECKSUM_PLACEHOLDER)
        )

    checksum = (-sum(struct.unpack_from("<7I", image, 0))) & 0xFFFFFFFF
    struct.pack_into("<I", image, CHECKSUM_OFFSET, checksum)
    with open(bin_path, "wb") as f:
        f.write(image)
    print("OpenBLT checksum 0x%08X written to %s" % (checksum, bin_path))

    srec_path = bin_path[: -len(".bin")] + ".srec"
    env.Execute(
        env.VerboseAction(
            '"$OBJCOPY" -I binary -O srec --srec-forceS3 --change-addresses 0x%08X "%s" "%s"'
            % (APP_BASE_ADDRESS, bin_path, srec_path),
            "Generating %s" % srec_path,
        )
    )


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", write_openblt_checksum)
