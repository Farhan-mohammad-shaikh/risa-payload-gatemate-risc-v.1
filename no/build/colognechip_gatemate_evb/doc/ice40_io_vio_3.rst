ICE40_IO_VIO_3
==============

Register Listing for ICE40_IO_VIO_3
-----------------------------------

+------------------------------------------------+----------------------------------------+
| Register                                       | Address                                |
+================================================+========================================+
| :ref:`ICE40_IO_VIO_3_OE <ICE40_IO_VIO_3_OE>`   | :ref:`0xf0009800 <ICE40_IO_VIO_3_OE>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_3_IN <ICE40_IO_VIO_3_IN>`   | :ref:`0xf0009804 <ICE40_IO_VIO_3_IN>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_3_OUT <ICE40_IO_VIO_3_OUT>` | :ref:`0xf0009808 <ICE40_IO_VIO_3_OUT>` |
+------------------------------------------------+----------------------------------------+

ICE40_IO_VIO_3_OE
^^^^^^^^^^^^^^^^^

`Address: 0xf0009800 + 0x0 = 0xf0009800`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_3_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_3_IN
^^^^^^^^^^^^^^^^^

`Address: 0xf0009800 + 0x4 = 0xf0009804`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: ICE40_IO_VIO_3_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_3_OUT
^^^^^^^^^^^^^^^^^^

`Address: 0xf0009800 + 0x8 = 0xf0009808`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_3_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


