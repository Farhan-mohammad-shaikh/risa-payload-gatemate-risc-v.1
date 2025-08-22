GATEMATE_DEBUG_4
================

Register Listing for GATEMATE_DEBUG_4
-------------------------------------

+----------------------------------------------------+------------------------------------------+
| Register                                           | Address                                  |
+====================================================+==========================================+
| :ref:`GATEMATE_DEBUG_4_OE <GATEMATE_DEBUG_4_OE>`   | :ref:`0xf0001800 <GATEMATE_DEBUG_4_OE>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`GATEMATE_DEBUG_4_IN <GATEMATE_DEBUG_4_IN>`   | :ref:`0xf0001804 <GATEMATE_DEBUG_4_IN>`  |
+----------------------------------------------------+------------------------------------------+
| :ref:`GATEMATE_DEBUG_4_OUT <GATEMATE_DEBUG_4_OUT>` | :ref:`0xf0001808 <GATEMATE_DEBUG_4_OUT>` |
+----------------------------------------------------+------------------------------------------+

GATEMATE_DEBUG_4_OE
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0001800 + 0x0 = 0xf0001800`

    GPIO Tristate(s) Control.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_4_OE

        {
            "reg": [
                {"name": "oe", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


GATEMATE_DEBUG_4_IN
^^^^^^^^^^^^^^^^^^^

`Address: 0xf0001800 + 0x4 = 0xf0001804`

    GPIO Input(s) Status.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_4_IN

        {
            "reg": [
                {"name": "in", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


GATEMATE_DEBUG_4_OUT
^^^^^^^^^^^^^^^^^^^^

`Address: 0xf0001800 + 0x8 = 0xf0001808`

    GPIO Ouptut(s) Control.

    .. wavedrom::
        :caption: GATEMATE_DEBUG_4_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


