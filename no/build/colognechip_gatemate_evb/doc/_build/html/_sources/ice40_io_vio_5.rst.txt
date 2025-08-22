ICE40_IO_VIO_5
==============

Register Listing for ICE40_IO_VIO_5
-----------------------------------

+------------------------------------------------+----------------------------------------+
| Register                                       | Address                                |
+================================================+========================================+
| :ref:`ICE40_IO_VIO_5_OE <ICE40_IO_VIO_5_OE>`   | :ref:`0xf000a800 <ICE40_IO_VIO_5_OE>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_5_IN <ICE40_IO_VIO_5_IN>`   | :ref:`0xf000a804 <ICE40_IO_VIO_5_IN>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_5_OUT <ICE40_IO_VIO_5_OUT>` | :ref:`0xf000a808 <ICE40_IO_VIO_5_OUT>` |
+------------------------------------------------+----------------------------------------+

ICE40_IO_VIO_5_OE
^^^^^^^^^^^^^^^^^

`Address: 0xf000a800 + 0x0 = 0xf000a800`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_5_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_5_IN
^^^^^^^^^^^^^^^^^

`Address: 0xf000a800 + 0x4 = 0xf000a804`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: ICE40_IO_VIO_5_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_5_OUT
^^^^^^^^^^^^^^^^^^

`Address: 0xf000a800 + 0x8 = 0xf000a808`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_5_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


