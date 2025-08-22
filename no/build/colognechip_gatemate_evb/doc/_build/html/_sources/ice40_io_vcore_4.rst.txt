ICE40_IO_VCORE_4
================

Register Listing for ICE40_IO_VCORE_4
-------------------------------------

+----------------------------------------------------+------------------------------------------+
| Register                                           | Address                                  |
+====================================================+==========================================+
| :ref:`ICE40_IO_VCORE_4_OE <ICE40_IO_VCORE_4_OE>`   | :ref:`0xf0007800 <ICE40_IO_VCORE_4_OE>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`ICE40_IO_VCORE_4_IN <ICE40_IO_VCORE_4_IN>`   | :ref:`0xf0007804 <ICE40_IO_VCORE_4_IN>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`ICE40_IO_VCORE_4_OUT <ICE40_IO_VCORE_4_OUT>` | :ref:`0xf0007808 <ICE40_IO_VCORE_4_OUT>` |
+----------------------------------------------------+------------------------------------------+

ICE40_IO_VCORE_4_OE
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0007800 + 0x0 = 0xf0007800`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VCORE_4_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VCORE_4_IN
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0007800 + 0x4 = 0xf0007804`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: ICE40_IO_VCORE_4_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VCORE_4_OUT
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf0007800 + 0x8 = 0xf0007808`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VCORE_4_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


