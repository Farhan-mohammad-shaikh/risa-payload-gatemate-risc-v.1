ICE40_IO_VCORE_0
================

Register Listing for ICE40_IO_VCORE_0
-------------------------------------

+----------------------------------------------------+------------------------------------------+
| Register                                           | Address                                  |
+====================================================+==========================================+
| :ref:`ICE40_IO_VCORE_0_OE <ICE40_IO_VCORE_0_OE>`   | :ref:`0xf0006000 <ICE40_IO_VCORE_0_OE>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`ICE40_IO_VCORE_0_IN <ICE40_IO_VCORE_0_IN>`   | :ref:`0xf0006004 <ICE40_IO_VCORE_0_IN>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`ICE40_IO_VCORE_0_OUT <ICE40_IO_VCORE_0_OUT>` | :ref:`0xf0006008 <ICE40_IO_VCORE_0_OUT>` |
+----------------------------------------------------+------------------------------------------+

ICE40_IO_VCORE_0_OE
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0006000 + 0x0 = 0xf0006000`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VCORE_0_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VCORE_0_IN
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0006000 + 0x4 = 0xf0006004`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: ICE40_IO_VCORE_0_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_IO_VCORE_0_OUT
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf0006000 + 0x8 = 0xf0006008`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: ICE40_IO_VCORE_0_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


