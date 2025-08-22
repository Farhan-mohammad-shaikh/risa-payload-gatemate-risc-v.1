ICE40_IO_VIO_1
==============

Register Listing for ICE40_IO_VIO_1
-----------------------------------

+------------------------------------------------+----------------------------------------+
| Register                                       | Address                                |
+================================================+========================================+
| :ref:`ICE40_IO_VIO_1_OE <ICE40_IO_VIO_1_OE>`   | :ref:`0xf0008800 <ICE40_IO_VIO_1_OE>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_1_IN <ICE40_IO_VIO_1_IN>`   | :ref:`0xf0008804 <ICE40_IO_VIO_1_IN>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_1_OUT <ICE40_IO_VIO_1_OUT>` | :ref:`0xf0008808 <ICE40_IO_VIO_1_OUT>` |
+------------------------------------------------+----------------------------------------+

ICE40_IO_VIO_1_OE
^^^^^^^^^^^^^^^^^

`Address: 0xf0008800 + 0x0 = 0xf0008800`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_1_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_1_IN
^^^^^^^^^^^^^^^^^

`Address: 0xf0008800 + 0x4 = 0xf0008804`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: ICE40_IO_VIO_1_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_1_OUT
^^^^^^^^^^^^^^^^^^

`Address: 0xf0008800 + 0x8 = 0xf0008808`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_1_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


