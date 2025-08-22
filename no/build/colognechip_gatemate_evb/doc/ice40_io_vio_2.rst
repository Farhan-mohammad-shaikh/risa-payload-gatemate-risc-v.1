ICE40_IO_VIO_2
==============

Register Listing for ICE40_IO_VIO_2
-----------------------------------

+------------------------------------------------+----------------------------------------+
| Register                                       | Address                                |
+================================================+========================================+
| :ref:`ICE40_IO_VIO_2_OE <ICE40_IO_VIO_2_OE>`   | :ref:`0xf0009000 <ICE40_IO_VIO_2_OE>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_2_IN <ICE40_IO_VIO_2_IN>`   | :ref:`0xf0009004 <ICE40_IO_VIO_2_IN>`  |
+------------------------------------------------+----------------------------------------+
| :ref:`ICE40_IO_VIO_2_OUT <ICE40_IO_VIO_2_OUT>` | :ref:`0xf0009008 <ICE40_IO_VIO_2_OUT>` |
+------------------------------------------------+----------------------------------------+

ICE40_IO_VIO_2_OE
^^^^^^^^^^^^^^^^^

`Address: 0xf0009000 + 0x0 = 0xf0009000`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_2_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_2_IN
^^^^^^^^^^^^^^^^^

`Address: 0xf0009000 + 0x4 = 0xf0009004`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: ICE40_IO_VIO_2_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VIO_2_OUT
^^^^^^^^^^^^^^^^^^

`Address: 0xf0009000 + 0x8 = 0xf0009008`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VIO_2_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


