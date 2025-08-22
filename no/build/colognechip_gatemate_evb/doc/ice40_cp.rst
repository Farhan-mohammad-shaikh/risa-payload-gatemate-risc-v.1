ICE40_CP
========

Register Listing for ICE40_CP
-----------------------------

+------------------------------------+----------------------------------+
| Register                           | Address                          |
+====================================+==================================+
| :ref:`ICE40_CP_OE <ICE40_CP_OE>`   | :ref:`0xf0005800 <ICE40_CP_OE>`  |
+------------------------------------+----------------------------------+
| :ref:`ICE40_CP_IN <ICE40_CP_IN>`   | :ref:`0xf0005804 <ICE40_CP_IN>`  |
+------------------------------------+----------------------------------+
| :ref:`ICE40_CP_OUT <ICE40_CP_OUT>` | :ref:`0xf0005808 <ICE40_CP_OUT>` |
+------------------------------------+----------------------------------+

ICE40_CP_OE
^^^^^^^^^^^

`Address: 0xf0005800 + 0x0 = 0xf0005800`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: ICE40_CP_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_CP_IN
^^^^^^^^^^^

`Address: 0xf0005800 + 0x4 = 0xf0005804`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: ICE40_CP_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


ICE40_CP_OUT
^^^^^^^^^^^^

`Address: 0xf0005800 + 0x8 = 0xf0005808`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: ICE40_CP_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


